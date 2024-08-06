#include <tbpch.h>

#include <AL/al.h>
#include <AL/alext.h>

#include <Tabby/Audio/AudioEngine.h>
#include <utility>

#define CHECK_AL_ERRORS() \
    FetchALErrors(__FILE__, __LINE__)

#define CHECK_ALC_ERRORS(device) \
    FetchALCErrors(device, __FILE__, __LINE__)

namespace Tabby {
namespace Audio {
    Engine* Engine::s_Instance = nullptr;

    void FetchALErrors(const std::filesystem::path& file, int line)
    {
        bool error_found = false;
        std::stringstream err_msg_stream;

        ALenum error_flag = alGetError();
        while (error_flag != AL_NO_ERROR) {
            if (!error_found) {
                error_found = true;
                err_msg_stream << "OpenAL Flags: ";
            }

            switch (error_flag) {
            case AL_INVALID_NAME:
                err_msg_stream << "\"AL_INVALID_NAME\" ";
                break;
            case AL_INVALID_ENUM:
                err_msg_stream << "\"AL_INVALID_ENUM\" ";
                break;
            case AL_INVALID_VALUE:
                err_msg_stream << "\"AL_INVALID_VALUE\" ";
                break;
            case AL_INVALID_OPERATION:
                err_msg_stream << "\"AL_INVALID_OPERATION\" ";
                break;
            case AL_OUT_OF_MEMORY:
                err_msg_stream << "\"AL_OUT_OF_MEMORY\" ";
                break;
            default:
                err_msg_stream << "\"Flag not deducable\" ";
                break;
            }

            error_flag = alGetError();
        }

        if (error_found) {
            err_msg_stream << "File " << file << " @ Line " << line;
            throw std::runtime_error(err_msg_stream.str());
        }

        // TB_CORE_ASSERT_TAGGED(!error_found);
    }

    void FetchALCErrors(ALCdevice* device, const std::filesystem::path& file, int line)
    {
        bool error_found = false;
        std::stringstream err_msg_stream;

        ALCenum error_flag = alcGetError(device);
        while (error_flag != ALC_NO_ERROR) {
            if (!error_found) {
                error_found = true;
                err_msg_stream << "OpenAL Flags: ";
            }

            switch (error_flag) {
            case ALC_INVALID_DEVICE:
                err_msg_stream << "\"ALC_INVALID_DEVICE\" ";
                break;
            case ALC_INVALID_CONTEXT:
                err_msg_stream << "\"ALC_INVALID_CONTEXT\" ";
                break;
            case ALC_INVALID_ENUM:
                err_msg_stream << "\"ALC_INVALID_ENUM\" ";
                break;
            case ALC_INVALID_VALUE:
                err_msg_stream << "\"ALC_INVALID_VALUE\" ";
                break;
            case ALC_OUT_OF_MEMORY:
                err_msg_stream << "\"ALC_OUT_OF_MEMORY\" ";
                break;
            default:
                err_msg_stream << "\"Flag not deducable\" ";
                break;
            }

            error_flag = alcGetError(device);
        }

        if (error_found) {
            err_msg_stream << "File " << file << " @ Line " << line;
            throw std::runtime_error(err_msg_stream.str());
        }

        // TB_CORE_ASSERT_TAGGED(!error_found);
    }

    Engine::Engine()
    {
        TB_CORE_ASSERT_TAGGED(!s_Instance, "Audio Engine instance already created!");
        s_Instance = this;

        m_AlcDevice = alcOpenDevice(nullptr);
        CHECK_ALC_ERRORS(m_AlcDevice);
        TB_CORE_ASSERT_TAGGED(m_AlcDevice, "alcOpenDevice: Unable to create OpenAL device");

        m_AlcContext = alcCreateContext(m_AlcDevice, nullptr);
        CHECK_ALC_ERRORS(m_AlcDevice);
        TB_CORE_ASSERT_TAGGED(m_AlcContext, "alcCreateContext: Unable to create OpenAL context");

        if (!alcMakeContextCurrent(m_AlcContext))
            TB_CORE_ASSERT_TAGGED(false, "alcMakeCurrentContext: Could not set OpenAL context to current context");
        CHECK_ALC_ERRORS(m_AlcDevice);

        alListenerf(AL_GAIN, 0.50f);
        CHECK_AL_ERRORS();
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        CHECK_AL_ERRORS();

        s_Instance->m_MusicMixerLock.lock();
        for (auto& player : m_MusicMixer) {
            alGenSources(1, &player.sourceID);
            CHECK_AL_ERRORS();
            alSourcef(player.sourceID, AL_PITCH, 1);
            CHECK_AL_ERRORS();
            alSourcef(player.sourceID, AL_GAIN, 1.0f);
            CHECK_AL_ERRORS();
            alSource3f(player.sourceID, AL_POSITION, 0.0f, 0.0f, -1.0f);
            CHECK_AL_ERRORS();
            alSourcei(player.sourceID, AL_LOOPING, AL_FALSE);
            CHECK_AL_ERRORS();
            alSourcei(player.sourceID, AL_SOURCE_RELATIVE, AL_TRUE);
            CHECK_AL_ERRORS();
            alSourcei(player.sourceID, AL_ROLLOFF_FACTOR, 0);
            CHECK_AL_ERRORS();

            alGenBuffers(4, player.buffers);
        }
        s_Instance->m_MusicMixerLock.unlock();

        m_ShouldThreadClose = false;
        m_PollingThread = std::thread(&Engine::EnginePollingThread, this);
    }

    Engine::~Engine()
    {
        m_PollingThread.detach();

        m_MusicMixerLock.lock();
        for (auto& music : m_MusicMap) {
            delete music.second;
        }
        m_MusicMixerLock.unlock();
    }

    void Engine::Init()
    {
        if (!s_Instance)
            s_Instance = new Engine();
    }

    void Engine::Shutdown()
    {
        delete s_Instance;
    }

    void Engine::PlaySfx(const std::string& sfxFileName)
    {

        Sfx sfx;

        auto searchSfx = s_Instance->m_SfxMap.find(sfxFileName);
        if (searchSfx == s_Instance->m_SfxMap.end()) {
            sfx = Sfx(sfxFileName);
            s_Instance->m_SfxMap.insert(std::make_pair(sfxFileName, sfx));
        } else {
            sfx = searchSfx->second;
        }

        ALuint source_id;
        alGenSources(1, &source_id);
        CHECK_AL_ERRORS();

        alSourcef(source_id, AL_PITCH, 1);
        CHECK_AL_ERRORS();
        alSourcef(source_id, AL_GAIN, 1.0f);
        CHECK_AL_ERRORS();
        alSource3f(source_id, AL_POSITION, 0.0f, 0.0f, 0.0f);
        CHECK_AL_ERRORS();
        alSourcei(source_id, AL_LOOPING, AL_FALSE);
        CHECK_AL_ERRORS();

        ALuint buffer_id;
        alGenBuffers(1, &buffer_id);
        CHECK_AL_ERRORS();
        alBufferData(buffer_id, sfx.format, sfx.membuf, sfx.num_bytes, sfx.sampleRate);
        CHECK_AL_ERRORS();
        alSourcei(source_id, AL_BUFFER, buffer_id);
        CHECK_AL_ERRORS();

        alSourcePlay(source_id);
        CHECK_AL_ERRORS();

        s_Instance->m_SfxMixerLock.lock();
        s_Instance->m_SfxMixer.emplace_back(source_id, buffer_id);
        s_Instance->m_SfxMixerLock.unlock();
    }

    void Engine::PauseSfxMixer()
    {
        s_Instance->m_SfxMixerLock.lock();
        for (const SfxBuffers& sfx : s_Instance->m_SfxMixer)
            alSourcePause(sfx.GetSourceID());
        s_Instance->m_SfxMixerLock.unlock();
        CHECK_AL_ERRORS();
    }

    void Engine::UnpauseSfxMixer()
    {
        s_Instance->m_SfxMixerLock.lock();
        for (const SfxBuffers& sfx : s_Instance->m_SfxMixer)
            alSourcePlay(sfx.GetSourceID());
        s_Instance->m_SfxMixerLock.unlock();
        CHECK_AL_ERRORS();
    }

    void Engine::StopSfxMixer()
    {
        s_Instance->m_SfxMixerLock.lock();
        for (const SfxBuffers& sfx : s_Instance->m_SfxMixer)
            alSourceStop(sfx.GetSourceID());
        s_Instance->m_SfxMixerLock.unlock();
        CHECK_AL_ERRORS();
    }

    void Engine::PlayMusicPlayer(std::size_t index)
    {
        s_Instance->m_MusicMixerLock.lock();
        if (s_Instance->m_MusicMixer.at(index).musicFileName.empty())
            throw std::logic_error("audio::engine::play_music_player: music player has no music set (use audio::engine::set_player_music)");

        s_Instance->m_MusicMixer.at(index).playing = true;
        alSourcePlay(s_Instance->m_MusicMixer.at(index).sourceID);
        CHECK_AL_ERRORS();
        s_Instance->m_MusicMixerLock.unlock();
    }

    void Engine::PauseMusicPlayer(std::size_t index)
    {
        s_Instance->m_MusicMixerLock.lock();
        if (s_Instance->m_MusicMixer.at(index).musicFileName.empty())
            throw std::logic_error("audio::engine::play_music_player: music player has no music set (use audio::engine::set_player_music)");

        s_Instance->m_MusicMixer.at(index).playing = false;
        alSourcePause(s_Instance->m_MusicMixer.at(index).sourceID);
        CHECK_AL_ERRORS();
        s_Instance->m_MusicMixerLock.unlock();
    }

    void Engine::SetPlayerMusic(const std::string& musicFileName, std::size_t index)
    {

        s_Instance->m_MusicMixerLock.lock();
        MusicPlayer& player = s_Instance->m_MusicMixer.at(index);

        player.musicFileName = musicFileName;
        player.musicFile = std::ifstream(musicFileName);
        player.cursor = 0;

        Music* music = nullptr;
        auto searchMusic = s_Instance->m_MusicMap.find(musicFileName);

        if (searchMusic == s_Instance->m_MusicMap.end()) {
            music = new Music(musicFileName);
            s_Instance->m_MusicMap.insert(std::make_pair(musicFileName, music));
        } else {
            music = searchMusic->second;
        }

        player.SetMusic(music);
        s_Instance->m_MusicMixerLock.unlock();
    }

    void Engine::EnginePollingThread()
    {
        while (!s_Instance->m_ShouldThreadClose) {

            TB_PROFILE_SCOPE_NAME("AudioEngine::Update");
            s_Instance->m_SfxMixerLock.lock();

            auto isStopped = [](const SfxBuffers& sfx) {
                ALint sourceState;
                alGetSourcei(sfx.GetSourceID(), AL_SOURCE_STATE, &sourceState);
                CHECK_AL_ERRORS();

                if (sourceState == AL_STOPPED) {
                    alDeleteSources(1, sfx.GetSourceIDAddr());
                    CHECK_AL_ERRORS();
                    alDeleteBuffers(1, sfx.GetSourceIDAddr());
                    CHECK_AL_ERRORS();
                }

                return sourceState == AL_STOPPED;
            };

            s_Instance->m_SfxMixer.erase(
                std::remove_if(s_Instance->m_SfxMixer.begin(), s_Instance->m_SfxMixer.end(), isStopped),
                s_Instance->m_SfxMixer.end());

            s_Instance->m_SfxMixerLock.unlock();

            /* --------------------------------  */

            s_Instance->m_MusicMixerLock.lock();

            for (MusicPlayer& player : s_Instance->m_MusicMixer) {
                if (player.music)
                    player.UpdatePlayer();
            }

            s_Instance->m_MusicMixerLock.unlock();

            // s_Instance->m_MusicMixerLock.lock();
            // for (MusicPlayer& player : s_Instance->m_MusicMixer) {
            //     ALint source_state = AL_NONE;
            //     alGetSourcei(player.sourceID, AL_SOURCE_STATE, &source_state);
            //     CHECK_AL_ERRORS();
            //
            //     if (source_state != AL_PLAYING)
            //         continue;
            //
            //     ALint buffers_processed;
            //     alGetSourcei(player.sourceID, AL_BUFFERS_PROCESSED, &buffers_processed);
            //     CHECK_AL_ERRORS();
            //     while (buffers_processed > 0) {
            //         buffers_processed--;
            //
            //         ALuint buffer;
            //         alSourceUnqueueBuffers(player.sourceID, 1, &buffer);
            //         CHECK_AL_ERRORS();
            //
            //         const Music& music = s_Instance->m_MusicMap.at(player.musicFileName);
            //         if (player.cursor >= music.dataSize)
            //             continue;
            //
            //         ALsizei buffer_size = static_cast<ALsizei>(std::min(MUSIC_BUFFER_SIZE, music.dataSize - player.cursor));
            //         buffer_size -= buffer_size % 8;
            //
            //         std::vector<char> buffer_data(buffer_size);
            //         player.musicFile.seekg(music.dataStart + player.cursor);
            //         player.musicFile.read(buffer_data.data(), buffer_size);
            //
            //         alBufferData(buffer, music.format, buffer_data.data(), buffer_size, music.sampleRate);
            //         CHECK_AL_ERRORS();
            //         alSourceQueueBuffers(player.sourceID, 1, &buffer);
            //         CHECK_AL_ERRORS();
            //
            //         player.cursor += buffer_size;
            //         if (buffer_size < MUSIC_BUFFER_SIZE)
            //             player.cursor = music.dataSize;
            //     }
            // }
            // s_Instance->m_MusicMixerLock.unlock();

            static constexpr int UPDATES_PER_SECOND = 200;
            static constexpr int UPDATE_FRAME_MS = 1000 / UPDATES_PER_SECOND;
            std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_FRAME_MS));
        }
    }

    Engine::MusicPlayer::MusicPlayer()
        : music(nullptr)
        , playing(false)
    {
    }

    Engine::MusicPlayer::~MusicPlayer()
    {
        alDeleteSources(1, &sourceID);
        alDeleteBuffers(NumBuffers, buffers);
        CHECK_AL_ERRORS();
    }

    int Engine::MusicPlayer::SetMusic(Music* _music)
    {

        music = _music;

        ALsizei i;

        /* Rewind the source position and clear the buffer queue */
        alSourceRewind(sourceID);
        alSourcei(sourceID, AL_BUFFER, 0);

        /* Fill the buffer queue */
        for (i = 0; i < NumBuffers; i++) {
            sf_count_t slen;

            /* Get some data to give it to the buffer */
            if (music->sampleType == Int16) {
                slen = sf_readf_short(music->sndFile, static_cast<short*>(music->membuf),
                    (sf_count_t)music->blockCount * music->sampleblockalign);
                if (slen < 1)
                    break;
                slen *= music->byteblockalign;
            } else if (music->sampleType == Float) {
                slen = sf_readf_float(music->sndFile, static_cast<float*>(music->membuf),
                    (sf_count_t)music->blockCount * music->sampleblockalign);
                if (slen < 1)
                    break;
                slen *= music->byteblockalign;
            } else {
                slen = sf_read_raw(music->sndFile, music->membuf,
                    (sf_count_t)music->blockCount * music->byteblockalign);
                if (slen > 0)
                    slen -= slen % music->byteblockalign;
                if (slen < 1)
                    break;
            }

            if (music->sampleblockalign > 1)
                alBufferi(buffers[i], AL_UNPACK_BLOCK_ALIGNMENT_SOFT,
                    music->sampleblockalign);

            alBufferData(buffers[i], music->format, music->membuf, (ALsizei)slen,
                music->sfinfo.samplerate);
        }
        if (alGetError() != AL_NO_ERROR) {
            fprintf(stderr, "Error buffering for playback\n");
            return 0;
        }

        /* Now queue and start playback! */
        alSourceQueueBuffers(sourceID, i, buffers);
        alSourcePlay(sourceID);
        if (alGetError() != AL_NO_ERROR) {
            fprintf(stderr, "Error starting playback\n");
            return 0;
        }

        return 1;
    }

    int Engine::MusicPlayer::UpdatePlayer()
    {
        if (playing) {

            ALint processed, state;

            /* Get relevant source info */
            alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
            alGetSourcei(sourceID, AL_BUFFERS_PROCESSED, &processed);
            if (alGetError() != AL_NO_ERROR) {

                CHECK_AL_ERRORS();
                fprintf(stderr, "Error checking source state\n");
                return 0;
            }

            /* Unqueue and handle each processed buffer */
            while (processed > 0) {
                ALuint bufid;
                sf_count_t slen;

                alSourceUnqueueBuffers(sourceID, 1, &bufid);
                processed--;

                /* Read the next chunk of data, refill the buffer, and queue it
                 * back on the source */
                if (music->sampleType == Int16) {
                    slen = sf_readf_short(music->sndFile, static_cast<short*>(music->membuf),
                        (sf_count_t)music->blockCount * music->sampleblockalign);
                    if (slen > 0)
                        slen *= music->byteblockalign;
                } else if (music->sampleType == Float) {
                    slen = sf_readf_float(music->sndFile, static_cast<float*>(music->membuf),
                        (sf_count_t)music->blockCount * music->sampleblockalign);
                    if (slen > 0)
                        slen *= music->byteblockalign;
                } else {
                    slen = sf_read_raw(music->sndFile, music->membuf,
                        (sf_count_t)music->blockCount * music->byteblockalign);
                    if (slen > 0)
                        slen -= slen % music->byteblockalign;
                }

                if (slen > 0) {
                    alBufferData(bufid, music->format, music->membuf, (ALsizei)slen,
                        music->sfinfo.samplerate);
                    alSourceQueueBuffers(sourceID, 1, &bufid);
                }
                if (alGetError() != AL_NO_ERROR) {

                    CHECK_AL_ERRORS();
                    fprintf(stderr, "Error buffering data\n");
                    return 0;
                }
            }

            /* Make sure the source hasn't underrun */
            if (state != AL_PLAYING && state != AL_PAUSED) {
                ALint queued;

                /* If no buffers are queued, playback is finished */
                alGetSourcei(sourceID, AL_BUFFERS_QUEUED, &queued);
                if (queued == 0)
                    return 0;

                alSourcePlay(sourceID);
                if (alGetError() != AL_NO_ERROR) {

                    CHECK_AL_ERRORS();
                    fprintf(stderr, "Error restarting playback\n");
                    return 0;
                }
            }
        }
        return 1;
    }

    // ------------------------------------------------------------------- //
    // ENGINE::SFX

    Engine::Sfx::Sfx() { }

    Engine::Sfx::Sfx(const std::string& sfxFileName)
    {
        SNDFILE* sndFile;
        SF_INFO sfinfo;
        sndFile = sf_open(sfxFileName.c_str(), SFM_READ, &sfinfo);
        if (!sndFile) {

            TB_CORE_ASSERT_TAGGED(false, "Could not open audio file");
        }

        // verify file has valid data
        if (sfinfo.frames < 1 || sfinfo.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
            sf_close(sndFile);
            TB_CORE_ASSERT_TAGGED(false, "Audio file produced invalid data");
        }

        duration = float((float)sfinfo.frames / (float)sfinfo.samplerate);
        sampleRate = (float)sfinfo.samplerate;

        // get sound format and configure openal format
        format = AL_NONE;
        if (sfinfo.channels == 1)
            format = AL_FORMAT_MONO16;
        else if (sfinfo.channels == 2)
            format = AL_FORMAT_STEREO16;
        else if (sfinfo.channels == 3) {
            if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT)
                format = AL_FORMAT_BFORMAT2D_16; // 2d
        } else if (sfinfo.channels == 4) {
            if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, nullptr, 0) == SF_AMBISONIC_B_FORMAT)
                format = AL_FORMAT_BFORMAT3D_16; // 3d
        }

        if (!format) {

            sf_close(sndFile);
            TB_CORE_ASSERT_TAGGED(false, "Unsuported channel count/format");
        }

        // decode the audio file into a buffer
        membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

        num_frames = sf_readf_short(sndFile, membuf, sfinfo.frames);
        if (num_frames < 1) {
            free(membuf);
            sf_close(sndFile);
            TB_CORE_ASSERT_TAGGED(false, "Failed to read samples in audio file");
        }

        num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

        // clean up audio file now that we are done with it
        // free(membuf);
        sf_close(sndFile);
    }

    // ------------------------------------------------------------------- //
    // ENGINE::SFX_BUFFERS

    Engine::SfxBuffers::SfxBuffers(ALuint _sourceID, ALuint _bufferID)
        : sourceID(_sourceID)
        , bufferID(_bufferID)
    {
    }

    ALuint Engine::SfxBuffers::GetSourceID() const
    {
        return sourceID;
    }

    const ALuint* Engine::SfxBuffers::GetSourceIDAddr() const
    {
        return &sourceID;
    }

    const ALuint* Engine::SfxBuffers::GetBufferIDAddr() const
    {
        return &bufferID;
    }

    // ------------------------------------------------------------------- //
    // ENGINE::MUSIC_T

    Engine::Music::Music() { }

    Engine::Music::Music(const std::string& musicFileName)
    {

        int splblockalign = 0;

        /* Open the audio file and check that it's usable. */
        sndFile = sf_open(musicFileName.c_str(), SFM_READ, &sfinfo);
        if (!sndFile) {
            fprintf(stderr, "Could not open audio in %s: %s\n", musicFileName.c_str(), sf_strerror(NULL));
            return;
        }

        /* Detect a suitable format to load. Formats like Vorbis and Opus use float
         * natively, so load as float to avoid clipping when possible. Formats
         * larger than 16-bit can also use float to preserve a bit more precision.
         */
        switch ((sfinfo.format & SF_FORMAT_SUBMASK)) {
        case SF_FORMAT_PCM_24:
        case SF_FORMAT_PCM_32:
        case SF_FORMAT_FLOAT:
        case SF_FORMAT_DOUBLE:
        case SF_FORMAT_VORBIS:
        case SF_FORMAT_OPUS:
        case SF_FORMAT_ALAC_20:
        case SF_FORMAT_ALAC_24:
        case SF_FORMAT_ALAC_32:
        case 0x0080 /*SF_FORMAT_MPEG_LAYER_I*/:
        case 0x0081 /*SF_FORMAT_MPEG_LAYER_II*/:
        case 0x0082 /*SF_FORMAT_MPEG_LAYER_III*/:
            if (alIsExtensionPresent("AL_EXT_FLOAT32"))
                sampleType = Float;
            break;
        case SF_FORMAT_IMA_ADPCM:
            /* ADPCM formats require setting a block alignment as specified in the
             * file, which needs to be read from the wave 'fmt ' chunk manually
             * since libsndfile doesn't provide it in a format-agnostic way.
             */
            if (sfinfo.channels <= 2
                && (sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
                && alIsExtensionPresent("AL_EXT_IMA4")
                && alIsExtensionPresent("AL_SOFT_block_alignment"))
                sampleType = IMA4;
            break;
        case SF_FORMAT_MS_ADPCM:
            if (sfinfo.channels <= 2
                && (sfinfo.format & SF_FORMAT_TYPEMASK) == SF_FORMAT_WAV
                && alIsExtensionPresent("AL_SOFT_MSADPCM")
                && alIsExtensionPresent("AL_SOFT_block_alignment"))
                sampleType = MSADPCM;
            break;
        }

        if (sampleType == IMA4 || sampleType == MSADPCM) {
            /* For ADPCM, lookup the wave file's "fmt " chunk, which is a
             * WAVEFORMATEX-based structure for the audio format.
             */
            SF_CHUNK_INFO inf = { "fmt ", 4, 0, NULL };
            SF_CHUNK_ITERATOR* iter = sf_get_chunk_iterator(sndFile, &inf);

            /* If there's an issue getting the chunk or block alignment, load as
             * 16-bit and have libsndfile do the conversion.
             */
            if (!iter || sf_get_chunk_size(iter, &inf) != SF_ERR_NO_ERROR || inf.datalen < 14)
                sampleType = Int16;
            else {
                ALubyte* fmtbuf = (ALubyte*)calloc(inf.datalen, 1);
                inf.data = fmtbuf;
                if (sf_get_chunk_data(iter, &inf) != SF_ERR_NO_ERROR)
                    sampleType = Int16;
                else {
                    /* Read the nBlockAlign field, and convert from bytes- to
                     * samples-per-block (verifying it's valid by converting back
                     * and comparing to the original value).
                     */
                    byteblockalign = fmtbuf[12] | (fmtbuf[13] << 8);
                    if (sampleType == IMA4) {
                        splblockalign = (byteblockalign / sfinfo.channels - 4) / 4 * 8 + 1;
                        if (splblockalign < 1
                            || ((splblockalign - 1) / 2 + 4) * sfinfo.channels != byteblockalign)
                            sampleType = Int16;
                    } else {
                        splblockalign = (byteblockalign / sfinfo.channels - 7) * 2 + 2;
                        if (splblockalign < 2
                            || ((splblockalign - 2) / 2 + 7) * sfinfo.channels != byteblockalign)
                            sampleType = Int16;
                    }
                }
                free(fmtbuf);
            }
        }

        if (sampleType == Int16) {
            sampleblockalign = 1;
            byteblockalign = sfinfo.channels * 2;
        } else if (sampleType == Float) {
            sampleblockalign = 1;
            byteblockalign = sfinfo.channels * 4;
        } else {
            sampleblockalign = splblockalign;
            byteblockalign = byteblockalign;
        }

        /* Figure out the OpenAL format from the file and desired sample type. */
        format = AL_NONE;
        if (sfinfo.channels == 1) {
            if (sampleType == Int16)
                format = AL_FORMAT_MONO16;
            else if (sampleType == Float)
                format = AL_FORMAT_MONO_FLOAT32;
            else if (sampleType == IMA4)
                format = AL_FORMAT_MONO_IMA4;
            else if (sampleType == MSADPCM)
                format = AL_FORMAT_MONO_MSADPCM_SOFT;
        } else if (sfinfo.channels == 2) {
            if (sampleType == Int16)
                format = AL_FORMAT_STEREO16;
            else if (sampleType == Float)
                format = AL_FORMAT_STEREO_FLOAT32;
            else if (sampleType == IMA4)
                format = AL_FORMAT_STEREO_IMA4;
            else if (sampleType == MSADPCM)
                format = AL_FORMAT_STEREO_MSADPCM_SOFT;
        } else if (sfinfo.channels == 3) {
            if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                if (sampleType == Int16)
                    format = AL_FORMAT_BFORMAT2D_16;
                else if (sampleType == Float)
                    format = AL_FORMAT_BFORMAT2D_FLOAT32;
            }
        } else if (sfinfo.channels == 4) {
            if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                if (sampleType == Int16)
                    format = AL_FORMAT_BFORMAT3D_16;
                else if (sampleType == Float)
                    format = AL_FORMAT_BFORMAT3D_FLOAT32;
            }
        }
        if (!format) {
            fprintf(stderr, "Unsupported channel count: %d\n", sfinfo.channels);
            sf_close(sndFile);
            sndFile = NULL;
            return;
        }

        blockCount = sfinfo.samplerate / sampleblockalign;
        blockCount = blockCount * BufferMillisec / 1000;
        membuf = malloc((size_t)blockCount * (size_t)byteblockalign);

        return;
    }

    Engine::Music::~Music()
    {
        if (sndFile)
            sf_close(sndFile);
        sndFile = NULL;

        free(membuf);
        membuf = NULL;

        if (sampleblockalign > 1) {
            ALsizei i;
            sampleblockalign = 0;
            byteblockalign = 0;
        }
    }

}

}
