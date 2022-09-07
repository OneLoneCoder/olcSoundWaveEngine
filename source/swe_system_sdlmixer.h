#pragma once
#include "swe_system_base.h"

///[OLC_HM] START SDLMIXER_H
#if defined(SOUNDWAVE_USING_SDLMIXER)

#if defined(__EMSCRIPTEN__)
#include <SDL2/SDL_mixer.h>
#else
#include <SDL_mixer.h>
#endif

#if defined(__EMSCRIPTEN__) && !defined(__EMSCRIPTEN_PTHREADS__) && !defined(SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD)
#define SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
#endif

namespace olc::sound::driver
{
    class SDLMixer final : public Base
    {
    public:
        explicit SDLMixer(WaveEngine* pHost);
        ~SDLMixer() final;

    protected:
        bool Open(const std::string& sOutputDevice, const std::string& sInputDevice) final;
        bool Start() final;
        void Stop() final;
        void Close() final;

    private:
        void FillChunkBuffer(const std::vector<float>& userData);

#ifdef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
        static void SDLMixerCallback(int channel);
#else
        void DriverLoop();
#endif

    private:
#ifdef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
        static SDLMixer* instance;
#else
        std::thread m_driverThread{};
#endif
        Uint16 m_haveFormat = AUDIO_F32SYS;
        std::vector<Uint8> m_audioBuffer{};
        Mix_Chunk m_audioChunk{};
        std::atomic<bool> m_keepRunning = false;
    };
}

#endif // SOUNDWAVE_USING_SDLMIXER
///[OLC_HM] END SDLMIXER_H