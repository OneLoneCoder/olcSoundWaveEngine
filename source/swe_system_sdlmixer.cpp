#include "swe_system_sdlmixer.h"

#include "swe_wave_engine.h"

///[OLC_HM] START SDLMIXER_CPP
#if defined(SOUNDWAVE_USING_SDLMIXER)

namespace olc::sound::driver
{

SDLMixer* SDLMixer::instance = nullptr;

SDLMixer::SDLMixer(olc::sound::WaveEngine* pHost)
    : Base(pHost)
{
    instance = this;
}

SDLMixer::~SDLMixer()
{
    Stop();
    Close();
}

bool SDLMixer::Open(const std::string& sOutputDevice, const std::string& sInputDevice)
{
    auto errc = Mix_OpenAudioDevice(static_cast<int>(m_pHost->GetSampleRate()),
                                    AUDIO_F32,
                                    static_cast<int>(m_pHost->GetChannels()),
                                    static_cast<int>(m_pHost->GetBlockSampleCount()),
                                    sOutputDevice == "DEFAULT" ? nullptr : sOutputDevice.c_str(),
                                    SDL_AUDIO_ALLOW_FORMAT_CHANGE);

    // Query the actual format of the audio device, as we have allowed it to be changed.
    if (errc || !Mix_QuerySpec(nullptr, &m_haveFormat, nullptr))
    {
        std::cerr << "Failed to open audio device '" << sOutputDevice << "'" << std::endl;
        return false;
    }

    // Compute the Mix_Chunk buffer's size according to the format of the audio device
    Uint32 bufferSize = 0;
    switch (m_haveFormat)
    {
        case AUDIO_F32:
        case AUDIO_S32:
            bufferSize = m_pHost->GetBlockSampleCount() * 4;
            break;
        case AUDIO_S16:
        case AUDIO_U16:
            bufferSize = m_pHost->GetBlockSampleCount() * 2;
            break;
        case AUDIO_S8:
        case AUDIO_U8:
            bufferSize = m_pHost->GetBlockSampleCount() * 1;
            break;
        default:
            std::cerr << "Audio format of device '" << sOutputDevice << "' is not supported" << std::endl;
            return false;
    }

    // Allocate the buffer once. The size will never change after this
    audioBuffer.resize(bufferSize);
    audioChunk = {
        0,                  // 0, as the chunk does not own the array
        audioBuffer.data(), // Pointer to data array
        bufferSize,         // Size in bytes
        128                 // Volume; max by default as it's not controlled by the driver.
    };

    return true;
}

template<typename Int>
void ConvertFloatTo(const std::vector<float>& fromArr, Int* toArr)
{
    static auto minVal = static_cast<float>(std::numeric_limits<Int>::min());
    static auto maxVal = static_cast<float>(std::numeric_limits<Int>::max());
    for (size_t i = 0; i != fromArr.size(); ++i)
    {
        toArr[i] = static_cast<Int>(std::clamp(fromArr[i] * maxVal, minVal, maxVal));
    }
}

void SDLMixer::FillChunkBuffer(const std::vector<float>& userData) const
{
    // Since the audio device might have changed the format we need to provide,
    // we convert the wave data from the user to that format.
    switch (m_haveFormat)
    {
        case AUDIO_F32:
            memcpy(audioChunk.abuf, userData.data(), audioChunk.alen);
            break;
        case AUDIO_S32:
            ConvertFloatTo<Sint32>(userData, reinterpret_cast<Sint32*>(audioChunk.abuf));
            break;
        case AUDIO_S16:
            ConvertFloatTo<Sint16>(userData, reinterpret_cast<Sint16*>(audioChunk.abuf));
            break;
        case AUDIO_U16:
            ConvertFloatTo<Uint16>(userData, reinterpret_cast<Uint16*>(audioChunk.abuf));
            break;
        case AUDIO_S8:
            ConvertFloatTo<Sint8>(userData, reinterpret_cast<Sint8*>(audioChunk.abuf));
            break;
        case AUDIO_U8:
            ConvertFloatTo<Uint8>(userData, audioChunk.abuf);
            break;
    }
}

void SDLMixer::SDLMixerCallback(int channel)
{
    static std::vector<float> userData(instance->m_pHost->GetBlockSampleCount());

    // Don't add another chunk if we should not keep running
    if (!instance->m_keepRunning)
        return;

    instance->GetFullOutputBlock(userData);
    instance->FillChunkBuffer(userData);

    if (Mix_PlayChannel(channel, &instance->audioChunk, 0) == -1)
    {
        std::cerr << "Error while playing Chunk" << std::endl;
    }
}

bool SDLMixer::Start()
{
    m_keepRunning = true;

    // Kickoff the audio driver
    SDLMixerCallback(0);

    // SDLMixer handles all other calls to reinsert user data
    Mix_ChannelFinished(SDLMixerCallback);
    return true;
}

void SDLMixer::Stop()
{
    m_keepRunning = false;

    // Stop might be called multiple times, so we check whether the device is already closed
    if (Mix_QuerySpec(nullptr, nullptr, nullptr))
    {
        for (int i = 0; i != m_pHost->GetChannels(); ++i)
        {
            if (Mix_Playing(i))
                Mix_HaltChannel(i);
        }
    }
}

void SDLMixer::Close()
{
    Mix_CloseAudio();
}
}

#endif // SOUNDWAVE_USING_SDLMIXER
///[OLC_HM] END SDLMIXER_CPP