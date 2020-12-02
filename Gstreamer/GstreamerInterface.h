#ifndef MediaPlayerPrivate_h
#define MediaPlayerPrivate_h
#include<string>
using namespace std;
namespace Streaming {
 
    class IStreamEventListener
    {
 
    public: 
        IStreamEventListener() {}
        virtual ~IStreamEventListener() {}
        virtual void OnError()                = 0;
        virtual void OnVedeoStarted()         = 0;
        virtual void OnVedeoStopped()         = 0;
        virtual void OnReachedEndofStream()   = 0;
        virtual void OnVedeoPaused()          = 0;
        virtual void OnRefreshGUI()           = 0;
        virtual void OnVedeoResume()          = 0;
         
    };
 
 
    class IStreamer {
    public:
        virtual ~IStreamer() { }
 
 
        virtual void Init(int argc, char *argv[])                 = 0;
        virtual void Load(const std::string& sUrl)                = 0;              
        virtual void Play()                                       = 0;
        virtual void Run()                                        = 0;
        virtual void Stop()                                       = 0;
        virtual void Pause()                                      = 0;
        virtual void Mute()                                       = 0;
        virtual void Seek(float time)                             = 0;
        virtual void SetRate(float)                               = 0;
        virtual void SetVolume(float)                             = 0;
        virtual void SetVedeoWindowHandle(HWND pHandle)           = 0;
        virtual void SetVisible(bool)                             = 0;
 
        virtual bool Playing()                                    = 0;
        virtual bool Seeking() const                              = 0;
        virtual bool Paused() const                               = 0;
        virtual bool HasVideo() const = 0;
         
     
        virtual float GetVolume()                                 = 0;
        virtual float Duration() const                            = 0;
        virtual float CurrentTime() const                         = 0;
        virtual int   GetRate() const                             = 0;
         
         
        //Call Backs
        virtual void AddStreamEventListener(IStreamEventListener* pCallBack) = 0;   
        virtual void SetTimer(unsigned int nTime)                            = 0;
 
        //virtual void paint(GraphicsContext*, const IntRect&) = 0;
         
    };
 
}
 
#endif