#pragma once
#include <string>
 
#include "gst\gstelement.h"
#include "glib\gmain.h"
#include "gst\gstbus.h"
#include <glib.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gst/audio/streamvolume.h>
#include "IStreamer.h"
 
using namespace std;
namespace Streaming
{
    class CGstreamerImpl : public IStreamer
    {
 
    public:
        CGstreamerImpl();
 
        virtual ~CGstreamerImpl();
 
        virtual void Load(const std::string& sUrl);
 
        virtual void Play();
 
        virtual void Pause();
 
        virtual void Mute();
 
        virtual void SetVolume(float);
 
        virtual void Seek(float time);
 
        virtual bool Seeking() const;
 
        virtual void SetRate(float);
 
        virtual bool Paused() const;
 
        virtual void SetVedeoWindowHandle(HWND pHandle);
         
 
        virtual bool HasVideo() const;
 
        virtual void SetVisible(bool);
 
        virtual float Duration() const;
 
        virtual float CurrentTime() const;      
 
        virtual int GetRate() const;                
 
        virtual void AddStreamEventListener(IStreamEventListener* pCallBack);
 
        virtual void Stop();
 
        virtual void SetTimer(unsigned int nTime);
 
        virtual bool Playing();
 
    protected:
 
        void ClearPipeLine();
        void CreatePipeLine(const std::string& sUrl);
        static gboolean StreamEventCallBack(GstBus *bus, GstMessage *message, gpointer data);
        /* This function is called periodically to refresh the GUI */
        static gboolean RefreshGUI(CGstreamerImpl *data);
        IStreamEventListener* GetEventListener() { return m_pEventListener; }
        static gboolean FilterVisFeatures(GstPluginFeature *feature, gpointer data);
 
        virtual void Run();     
 
        virtual float GetVolume();
 
        virtual void Init(int argc, char *argv[]);
 
    private:
 
        /* playbin2 flags */
        typedef enum {
            GST_PLAY_FLAG_VIS = (1 << 3) /* Enable rendering of visualizations when there is no video stream. */
        } GstPlayFlags;
 
        IStreamEventListener* m_pEventListener;
        GstElement *m_pPipeline;
        guint m_Bus_watch_id;
        GstBus *m_Bus;
        GstState m_State;                  /* Current state of the pipeline */
        GMainLoop *m_pLoop;
        GstElement *m_pPlaybin;
        mutable gint64 m_nDuration;                /* Duration of the clip, in nanoseconds */
        std::string m_sUrl;
        HWND m_WindowHandle;
        bool m_bPaused;
        bool m_bPlaying;
        bool m_bMuted;
        bool m_Visible;
        float m_flVolume;
        float m_rate;
    };
}