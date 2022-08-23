#pragma once
#include "swe_system_base.h"

///[OLC_HM] START SDLMIXER_H
#if defined(SOUNDWAVE_USING_SDLMIXER)

#if defined(__EMSCRIPTEN__)
#include <SDL2/SDL_mixer.h>
#else
#include <SDL_mixer.h>
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
        void FillChunkBuffer(const std::vector<float>& userData) const;

        static void SDLMixerCallback(int channel);

    private:
        bool m_keepRunning = false;
        Uint16 m_haveFormat = AUDIO_F32SYS;
        std::vector<Uint8> audioBuffer;
        Mix_Chunk audioChunk;

        static SDLMixer* instance;
    };
}

#endif // SOUNDWAVE_USING_SDLMIXER
///[OLC_HM] END SDLMIXER_H