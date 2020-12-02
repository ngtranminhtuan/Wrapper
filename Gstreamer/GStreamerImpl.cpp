#include "stdafx.h"
#include "GStreamerImpl.h"
#include "IStreamer.h"
 
namespace Streaming
{
 
    CGstreamerImpl::CGstreamerImpl() :m_bPaused(false), m_pPlaybin(nullptr), m_pPipeline(nullptr), m_pLoop(nullptr),
                                      m_bPlaying(false), m_bMuted(false), m_pEventListener(nullptr), m_sUrl(""), 
                                      m_WindowHandle(nullptr), m_flVolume(0.5f), m_rate(1.0f), m_Visible(true)
    {
         
 
    }
 
 
    CGstreamerImpl::~CGstreamerImpl()
    {
 
    }
 
 
    /****************************************************************************
    *       call back
    *
    ****************************************************************************/
    gboolean CGstreamerImpl::StreamEventCallBack(GstBus *bus, GstMessage *message, gpointer data)
    {
        g_print("Got %s message\n", GST_MESSAGE_TYPE_NAME(message));
 
        CGstreamerImpl* pStreamObj = static_cast<CGstreamerImpl*>(data);
 
        IStreamEventListener* pListener = pStreamObj->GetEventListener();
 
        switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug;
            gst_message_parse_error(message, &err, &debug);
            g_print("Error: %s\n", err->message);
            g_error_free(err);
            g_free(debug);
            g_main_loop_quit(pStreamObj->m_pLoop);
             
            if (pListener != nullptr)
                pListener->OnError();
             
            break;
        }
        case GST_MESSAGE_EOS:
            /* end-of-stream */
            g_main_loop_quit(pStreamObj->m_pLoop);
            pStreamObj->m_bPlaying = false;
            pStreamObj->m_bPaused = false;
            if (pListener != nullptr)
                pListener->OnReachedEndofStream();
            //::PostMessage(pThis->GetSafeHwnd(), VEDEO_PLAY_STOPPED, (WPARAM)0, (LPARAM)0);
            break;
        case GST_MESSAGE_STATE_CHANGED:
            if (GST_MESSAGE_SRC(message) == GST_OBJECT(pStreamObj->m_pPlaybin)) {
                GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed(message, &old_state, &new_state, &pending_state);
                //float duration = pStreamObj->Duration();
                if (old_state == GST_STATE_READY && new_state == GST_STATE_PAUSED)
                {
                     
                    if (pListener != nullptr)
                        pListener->OnVedeoStarted();
                }
                else if (old_state == GST_STATE_PAUSED && new_state == GST_STATE_PLAYING)
                {
                    g_timeout_add_seconds(1, (GSourceFunc)RefreshGUI, pStreamObj);
                    if (pListener != nullptr)
                        pListener->OnVedeoResume();                  
                }
                else if ((old_state == GST_STATE_PLAYING || old_state == GST_STATE_PAUSED) && new_state == GST_STATE_READY)
                {
                    g_main_loop_quit(pStreamObj->m_pLoop);
 
                    if (pListener != nullptr)
                        pListener->OnVedeoStopped();                 
                }
                else if (old_state == GST_STATE_PLAYING && new_state == GST_STATE_PAUSED)
                {               
                    if (pListener != nullptr)
                        pListener->OnVedeoPaused();                  
                }
            }
            break;
 
        default:
            /* unhandled message */
            break;
        }
        return true;
    }
 
 
    gboolean CGstreamerImpl::RefreshGUI(CGstreamerImpl *data)
    {
         
        CGstreamerImpl* pStreamObj = static_cast<CGstreamerImpl*>(data);
        IStreamEventListener* pListener = pStreamObj->GetEventListener();
 
        if (!pStreamObj->Playing() || pStreamObj->Paused())
            return false;
        else if (pListener != nullptr)
            pListener->OnRefreshGUI();
 
        return true;
 
    }
 
    /* Return TRUE if this is a Visualization element */
    gboolean CGstreamerImpl::FilterVisFeatures(GstPluginFeature *feature, gpointer data) {
        GstElementFactory *factory;
 
        if (!GST_IS_ELEMENT_FACTORY(feature))
            return FALSE;
        factory = GST_ELEMENT_FACTORY(feature);
        if (!g_strrstr(gst_element_factory_get_klass(factory), "Visualization"))
            return FALSE;
 
        return TRUE;
    }
 
    void CGstreamerImpl::Load(const std::string& sUrl)
    {
        CreatePipeLine(sUrl);
    }
 
     
 
    void CGstreamerImpl::Play()
    {
        m_nDuration = GST_CLOCK_TIME_NONE;
        GstStateChangeReturn ret = gst_element_set_state(m_pPipeline, GST_STATE_PLAYING);
 
        if (ret == GST_STATE_CHANGE_FAILURE) {
            ClearPipeLine();
            return;
        }
        /* Iterate */
        if (!m_bPaused)
        {
            Run();
        }
 
        m_bPlaying = true;
        m_bPaused = false;
 
    }
 
    void CGstreamerImpl::Pause()
    {
        /* wait until it’s up and running or failed */
        GstState state;
        gst_element_get_state(m_pPipeline, &state, 0, 0);
        if (state == GST_STATE_PLAYING) {
            gst_element_set_state(m_pPipeline, GST_STATE_PAUSED);
            m_bPaused = TRUE;
 
        }
    }
 
    void CGstreamerImpl::Mute()
    {
        m_bMuted = !m_bMuted;
 
        if (m_bMuted) {
            gst_stream_volume_set_volume(GST_STREAM_VOLUME(m_pPlaybin), GST_STREAM_VOLUME_FORMAT_CUBIC, 0.0);
        }
        else {
            gst_stream_volume_set_volume(GST_STREAM_VOLUME(m_pPlaybin), GST_STREAM_VOLUME_FORMAT_CUBIC,
                m_flVolume);
        }
 
    }
 
    void CGstreamerImpl::SetVolume(float flVolume)
    {
        if (m_bPlaying)
            gst_stream_volume_set_volume(GST_STREAM_VOLUME(m_pPlaybin), GST_STREAM_VOLUME_FORMAT_CUBIC, flVolume);
 
        m_flVolume = flVolume;
 
    }
 
    void CGstreamerImpl::Seek(float time)
    {
        GstClockTime sec = (GstClockTime)(time * GST_SECOND);
        if (m_bPlaying)
        {
            if (!gst_element_seek(m_pPipeline, m_rate,
                GST_FORMAT_TIME,
                (GstSeekFlags)(GST_SEEK_FLAG_FLUSH),
                GST_SEEK_TYPE_SET, sec,
                GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
            {
                //if (!gst_element_seek(m_pPipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                //GST_SEEK_TYPE_SET, (gint64)(value * 1000000000),
                //GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
                g_print("Seek failed!\n");
            }
        }
         
    }
 
    bool CGstreamerImpl::Seeking() const
    {
        return true;
    }
 
    void CGstreamerImpl::SetRate(float rate)
    {
        /*if (m_bPlaying)
        {
            if (!gst_element_seek(m_pPipeline, rate,
                GST_FORMAT_TIME,
                (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                GST_SEEK_TYPE_SET, (GstClockTime)(CurrentTime() * GST_SECOND),
                GST_SEEK_TYPE_SET, (GstClockTime)(Duration() )));
        }   */
 
        gint64 position;
        GstFormat format = GST_FORMAT_TIME;
//      GstEvent *seek_event;
 
        /* Obtain the current position, needed for the seek event */
        if (!gst_element_query_position(m_pPipeline, format, &position)) {
            g_printerr("Unable to retrieve current position.\n");
            return;
        }
        /* Create the seek event */
        if (rate > 0) {
             gst_element_seek(m_pPipeline, rate, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                GST_SEEK_TYPE_SET, position, GST_SEEK_TYPE_SET, -1);
        }
        else {
             gst_element_seek(m_pPipeline, rate, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE),
                GST_SEEK_TYPE_SET, 0, GST_SEEK_TYPE_SET, position);
        }
 
    }
 
    bool CGstreamerImpl::Paused() const
    {
        return m_bPaused;
    }
 
    void CGstreamerImpl::SetVedeoWindowHandle(HWND pHandle)
    {
        m_WindowHandle = pHandle;
    }
 
    bool CGstreamerImpl::HasVideo() const
    {
        gint currentVideo = -1;
        if (m_pPlaybin)
            g_object_get(G_OBJECT(m_pPlaybin), "current-video", &currentVideo, NULL);
        return currentVideo > -1;
    }
 
    void CGstreamerImpl::SetVisible(bool)
    {
        throw std::logic_error("The method or operation is not implemented.");
    }
 
    float CGstreamerImpl::Duration() const
    {
        if (m_bPlaying){
 
            GstFormat fmt = GST_FORMAT_TIME;
            if (!gst_element_query_duration(m_pPipeline, fmt, &m_nDuration)) {
                m_nDuration = 0;
                g_printerr("Could not query current duration.\n");
            }
            return (float)m_nDuration;
        }
 
        else
            return 0.0f;
    }
 
    float CGstreamerImpl::CurrentTime() const
    {
        float ret = 0.0f;
 
        if (m_bPlaying)
        {
            GstQuery* query = gst_query_new_position(GST_FORMAT_TIME);
            if (gst_element_query(m_pPipeline, query)) {
                gint64 position;
                gst_query_parse_position(query, NULL, &position);
                ret = (float)(position / 1000000000);
            }
            else {
                ret = 0.0f;
            }
            gst_query_unref(query);
        }
         
        return ret;
    }
         
    int CGstreamerImpl::GetRate() const
    {
        throw std::logic_error("The method or operation is not implemented.");
    }
     
    void CGstreamerImpl::AddStreamEventListener(IStreamEventListener* pCallBack)
    {
        m_pEventListener = pCallBack;
    }
 
    void CGstreamerImpl::Stop()
    {
        gst_element_set_state(m_pPipeline, GST_STATE_READY);
        m_bPaused = false;
        m_bPlaying = false;
    }
 
    bool CGstreamerImpl::Playing()
    {
        return m_bPlaying;
    }
 
 
 
    void CGstreamerImpl::ClearPipeLine()
    {
        if (m_pPipeline != nullptr){
            gst_element_set_state(m_pPipeline, GST_STATE_NULL);
            gst_object_unref(GST_OBJECT(m_pPipeline));
            gst_object_unref(m_Bus);
            g_source_remove(m_Bus_watch_id);
            g_main_loop_unref(m_pLoop);
        }
 
    }
 
    void CGstreamerImpl::CreatePipeLine(const std::string& sUrl)
    {
        m_nDuration = GST_CLOCK_TIME_NONE;
        m_sUrl = sUrl;
        ClearPipeLine();
        m_bPaused = FALSE;
        m_pPipeline = gst_pipeline_new(NULL);
 
        /* Create gstreamer elements */
        m_pPlaybin = gst_element_factory_make("playbin", NULL);
        gst_bin_add(GST_BIN(m_pPipeline), m_pPlaybin);
        g_object_set(G_OBJECT(m_pPlaybin), "uri", m_sUrl.c_str(), NULL);
        //  CWnd* pDlg = GetDlgItem(IDC_STATIC);
        if (m_Visible && m_WindowHandle != nullptr)
            gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(m_pPlaybin), (guintptr)(m_WindowHandle));
        /* bus callbacks */
 
        m_Bus = gst_pipeline_get_bus(GST_PIPELINE(m_pPipeline));
 
        m_Bus_watch_id = gst_bus_add_watch(m_Bus, StreamEventCallBack, this);
 
        gst_object_unref(m_Bus);
 
        m_bPlaying = TRUE;
 
        GstElementFactory *selected_factory = NULL;
        GstElement *vis_plugin;
        GList *list, *walk;
        /* Get a list of all visualization plugins */
        list = gst_registry_feature_filter(gst_registry_get(), FilterVisFeatures, FALSE, NULL);
        for (walk = list; walk != NULL; walk = g_list_next(walk)) {
            const gchar *name;
            GstElementFactory *factory;
 
            factory = GST_ELEMENT_FACTORY(walk->data);
            name = gst_element_factory_get_longname(factory);
            g_print("  %s\n", name);
 
            if (selected_factory == NULL || g_str_has_prefix(name, "goom")) {
                selected_factory = factory;
            }
        }
 
        vis_plugin = gst_element_factory_create(selected_factory, NULL);
        guint flags;
        /*Set the visualization flag*/
        g_object_get(m_pPlaybin, "flags", &flags, NULL);
        flags |= GST_PLAY_FLAG_VIS;
        g_object_set(m_pPlaybin, "flags", flags, NULL);
 
        /* set vis plugin for playbin2 */
        g_object_set(G_OBJECT(m_pPlaybin), "vis-plugin", vis_plugin, NULL);
 
        SetVolume(m_flVolume);
 
        /* Register a function that GLib will call every second */
        g_timeout_add_seconds(1, (GSourceFunc)RefreshGUI, this);
    }
 
    void CGstreamerImpl::SetTimer(unsigned int nTime)
    {
        throw std::logic_error("The method or operation is not implemented.");
    }
 
    void CGstreamerImpl::Run()
    {
        /* Iterate */
        if (!m_bPaused)
        {
            m_pLoop = g_main_loop_new(NULL, FALSE);
            g_main_loop_run(m_pLoop);
        }
 
    }   
 
    float CGstreamerImpl::GetVolume()
    {
        return m_flVolume;
    }
 
    void CGstreamerImpl::Init(int argc, char *argv[])
    {
        static int nCount = 0;      
        gst_init(&argc, &argv);
        nCount++;
    }
 
}