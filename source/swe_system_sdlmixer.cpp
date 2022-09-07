#include "swe_system_sdlmixer.h"

#include "swe_wave_engine.h"

///[OLC_HM] START SDLMIXER_CPP
#if defined(SOUNDWAVE_USING_SDLMIXER)

namespace olc::sound::driver
{

#ifdef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
SDLMixer* SDLMixer::instance = nullptr;
#endif

SDLMixer::SDLMixer(olc::sound::WaveEngine* pHost)
    : Base(pHost)
{
#ifdef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
    instance = this;
#endif
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
        bufferSize = m_pHost->GetBlockSampleCount() * m_pHost->GetChannels() * 4;
        break;
    case AUDIO_S16:
    case AUDIO_U16:
        bufferSize = m_pHost->GetBlockSampleCount() * m_pHost->GetChannels() * 2;
        break;
    case AUDIO_S8:
    case AUDIO_U8:
        bufferSize = m_pHost->GetBlockSampleCount() * m_pHost->GetChannels() * 1;
        break;
    default:
        std::cerr << "Audio format of device '" << sOutputDevice << "' is not supported" << std::endl;
        return false;
    }

    // Allocate the buffer once. The size will never change after this
    m_audioBuffer.resize(bufferSize);
    m_audioChunk = {
        0,                    // 0, as the chunk does not own the array
        m_audioBuffer.data(), // Pointer to data array
        bufferSize,           // Size in bytes
        128                   // Volume; max by default as it's not controlled by the driver.
    };

    return true;
}

template<typename Int>
void ConvertFloatTo(const std::vector<float>& fromArr, Int* toArr)
{
    static auto minVal = static_cast<float>(std::numeric_limits<Int>::min());
    static auto maxVal = static_cast<float>(std::numeric_limits<Int>::max());

    for (size_t i = 0; i != fromArr.size(); ++i)
        toArr[i] = static_cast<Int>(std::clamp(fromArr[i] * maxVal, minVal, maxVal));
}

void SDLMixer::FillChunkBuffer(const std::vector<float>& userData)
{
    // Since the audio device might have changed the format we need to provide,
    // we convert the wave data from the user to that format.
    switch (m_haveFormat)
    {
    case AUDIO_F32:
        memcpy(m_audioChunk.abuf, userData.data(), m_audioChunk.alen);
        break;
    case AUDIO_S32:
        ConvertFloatTo<Sint32>(userData, reinterpret_cast<Sint32*>(m_audioChunk.abuf));
        break;
    case AUDIO_S16:
        ConvertFloatTo<Sint16>(userData, reinterpret_cast<Sint16*>(m_audioChunk.abuf));
        break;
    case AUDIO_U16:
        ConvertFloatTo<Uint16>(userData, reinterpret_cast<Uint16*>(m_audioChunk.abuf));
        break;
    case AUDIO_S8:
        ConvertFloatTo<Sint8>(userData, reinterpret_cast<Sint8*>(m_audioChunk.abuf));
        break;
    case AUDIO_U8:
        ConvertFloatTo<Uint8>(userData, m_audioChunk.abuf);
        break;
    }
}

#ifdef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
void SDLMixer::SDLMixerCallback(int channel)
{
    static std::vector<float> userData(instance->m_pHost->GetBlockSampleCount() * instance->m_pHost->GetChannels());

    if (channel != 0)
        std::cerr << "Unexpected channel number" << std::endl;

    // Don't add another chunk if we should not keep running
    if (!instance->m_keepRunning)
        return;

    instance->GetFullOutputBlock(userData);
    instance->FillChunkBuffer(userData);

    if (Mix_PlayChannel(0, &instance->m_audioChunk, 0) == -1)
        std::cerr << "Error while playing Chunk" << std::endl;
}
#endif

bool SDLMixer::Start()
{
    // Kickoff the audio driver
    m_keepRunning = true;

#ifdef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
    SDLMixerCallback(0);
    // SDLMixer handles all other calls to reinsert user data
    Mix_ChannelFinished(SDLMixerCallback);
#else
    m_driverThread = std::thread([this](){ DriverLoop(); });
#endif

    return true;
}

void SDLMixer::Stop()
{
    m_keepRunning = false;

    // Stop might be called multiple times, so we check whether the device is already closed
    if (Mix_QuerySpec(nullptr, nullptr, nullptr))
        if (Mix_Playing(0))
            Mix_HaltChannel(0);

#ifndef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
    if (m_driverThread.joinable())
        m_driverThread.join();
#endif
}

void SDLMixer::Close()
{
    Mix_CloseAudio();
}

#ifndef SOUNDWAVE_USING_SDLMIXER_SINGLETHREAD
void SDLMixer::DriverLoop()
{
    static std::vector<float> userData(m_pHost->GetBlockSampleCount() * m_pHost->GetChannels());

    while (m_keepRunning)
    {
        GetFullOutputBlock(userData);
        FillChunkBuffer(userData);

        // busy-wait for the channel to finish
        while (Mix_Playing(0)) {}
        if (Mix_PlayChannel(0, &m_audioChunk, 0) == -1)
            std::cerr << "Error while playing Chunk" << std::endl;
    }
}
#endif
}

#endif // SOUNDWAVE_USING_SDLMIXER
///[OLC_HM] END SDLMIXER_CPP