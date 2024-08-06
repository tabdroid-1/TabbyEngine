#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>

namespace Tabby {

namespace Audio {

    class Engine {
    public:
        static void Init();
        static void Shutdown();

        static float GetVolume();
        static void SetVolume(float newVolume);

        static void PlaySfx(const std::string& sfxFileName);
        static void PauseSfxMixer();
        static void UnpauseSfxMixer();
        static void StopSfxMixer();

        static void SetPlayerMusic(const std::string& musicFileName, std::size_t index = 0);
        static void UnsetPlayerMusic(std::size_t index = 0);

        static void PlayMusicPlayer(std::size_t index = 0);
        static void PauseMusicPlayer(std::size_t index = 0);
        static bool IsMusicPlaying(std::size_t index = 0);

        static float GetMusicDuration(const std::string& music_file_name);
        static void SetPlaybackTime(float time, std::size_t index = 0);

    private:
        Engine();
        ~Engine();

        void EnginePollingThread();

        typedef enum SampleType {
            Int16,
            Float,
            IMA4,
            MSADPCM
        } SampleType;

        class Music;
        class Sfx;
        class SfxBuffers;
        class MusicPlayer {
        public:
            MusicPlayer();
            ~MusicPlayer();

            int SetMusic(Music* music);
            int UpdatePlayer();

            /* These are the buffers and source to play out through OpenAL with. */
            ALuint buffers[4];
            ALuint sourceID;

            Music* music;
            std::string musicFileName;
            std::ifstream musicFile;
            std::size_t cursor;

            bool playing;

        private:
            const int NumBuffers = 4;
        };

    private:
        static Engine* s_Instance;

        ALCdevice* m_AlcDevice;
        ALCcontext* m_AlcContext;

        std::atomic_bool m_ShouldThreadClose;
        std::thread m_PollingThread;

        std::unordered_map<std::string, Sfx> m_SfxMap;
        std::unordered_map<std::string, Music*> m_MusicMap;

        std::vector<SfxBuffers> m_SfxMixer;
        std::mutex m_SfxMixerLock;

        std::array<MusicPlayer, 4> m_MusicMixer;
        std::mutex m_MusicMixerLock;

        static constexpr std::size_t MUSIC_BUFFER_SIZE = 65536;
    };

    class Engine::Music {
    public:
        Music();
        Music(const std::string& musicFileName);

        ~Music();

        SNDFILE* sndFile;
        SF_INFO sfinfo;
        void* membuf;

        SampleType sampleType = Int16;
        int byteblockalign;
        int sampleblockalign;
        int blockCount;

        ALenum format;

    private:
        enum { BufferMillisec = 200 };
    };

    class Engine::Sfx {
    public:
        Sfx(const std::string& sfxFileName);
        Sfx();

        int sampleRate;
        ALenum format;
        float duration;

        short* membuf;
        ALsizei num_bytes;
        sf_count_t num_frames;
    };

    class Engine::SfxBuffers {
    public:
        SfxBuffers(ALuint _sourceID, ALuint _bufferID);

        ALuint GetSourceID() const;
        const ALuint* GetSourceIDAddr() const;
        const ALuint* GetBufferIDAddr() const;

    private:
        ALuint sourceID;
        ALuint bufferID;
    };

} // namespace Audio

} // namespace Tabby
