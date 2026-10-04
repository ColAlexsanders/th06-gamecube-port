#include "SoundPlayer.hpp"

#include "FileSystem.hpp"
#include "Supervisor.hpp"
#include "i18n.hpp"
#include "utils.hpp"

#include <array>
#include <cmath>
#include <cstring>
#include <new>
#include <vector>
#include "platform.h"

#define BACKGROUND_MUSIC_WAV_NUM_CHANNELS 2
#define BACKGROUND_MUSIC_WAV_SAMPLE_RATE 44100
#define BACKGROUND_MUSIC_WAV_BITS_PER_SAMPLE 16
#define BACKGROUND_MUSIC_WAV_BLOCK_ALIGN (BACKGROUND_MUSIC_WAV_BITS_PER_SAMPLE / 8 * BACKGROUND_MUSIC_WAV_NUM_CHANNELS)
#define BACKGROUND_MUSIC_WAV_BYTE_RATE (BACKGROUND_MUSIC_WAV_BLOCK_ALIGN * BACKGROUND_MUSIC_WAV_SAMPLE_RATE)

static const SoundBufferIdxVolume g_SoundBufferIdxVol[32] = {
    {0, -1500}, {0, -2000}, {1, -1200}, {1, -1400}, {2, -1000},  {3, -500},   {4, -500},   {5, -1700},
    {6, -1700}, {7, -1700}, {8, -1000}, {9, -1000}, {10, -1900}, {11, -1200}, {12, -900},  {5, -1500},
    {13, -900}, {14, -900}, {15, -600}, {16, -400}, {17, -1100}, {18, -900},  {5, -1800},  {6, -1800},
    {7, -1800}, {19, -300}, {20, -600}, {21, -800}, {22, -100},  {23, -500},  {24, -1000}, {25, -1000},
};
static const char *const g_SFXList[26] = {
    "data/wav/plst00.wav", "data/wav/enep00.wav",   "data/wav/pldead00.wav", "data/wav/power0.wav",
    "data/wav/power1.wav", "data/wav/tan00.wav",    "data/wav/tan01.wav",    "data/wav/tan02.wav",
    "data/wav/ok00.wav",   "data/wav/cancel00.wav", "data/wav/select00.wav", "data/wav/gun00.wav",
    "data/wav/cat00.wav",  "data/wav/lazer00.wav",  "data/wav/lazer01.wav",  "data/wav/enep01.wav",
    "data/wav/nep00.wav",  "data/wav/damage00.wav", "data/wav/item00.wav",   "data/wav/kira00.wav",
    "data/wav/kira01.wav", "data/wav/kira02.wav",   "data/wav/extend.wav",   "data/wav/timeout.wav",
    "data/wav/graze.wav",  "data/wav/powerup.wav",
};
SoundPlayer g_SoundPlayer;

SoundPlayer::SoundPlayer()
{
    // Note: memset of an std::mutex crashes on windows
    // std::memset(this, 0, sizeof(SoundPlayer));
}

ZunResult SoundPlayer::InitializeDSound()
{
    // Audio initialization stub - GameCube audio not yet implemented
    // Just set up a dummy audio device
    this->audioDev = 1; // Non-zero dummy value
    
    // Start the background music thread for compatibility
    this->backgroundMusicThreadHandle = std::thread(&SoundPlayer::BackgroundMusicPlayerThread, this);
    
    g_GameErrorContext.Log(TH_DBG_SOUNDPLAYER_INIT_SUCCESS);
    return ZUN_SUCCESS;
}

ZunResult SoundPlayer::Release(void)
{
    this->terminateFlag = true;
    this->backgroundMusicThreadHandle.join();
    this->terminateFlag = false;

    StopBGM();

    for (int i = 0; i < ARRAY_SIZE_SIGNED(this->soundBuffers); i++)
    {
        if (this->soundBuffers[i].samples != NULL)
        {
            delete[] this->soundBuffers[i].samples;
            this->soundBuffers[i].samples = NULL;
            this->soundBuffers[i].isPlaying = false;
        }
    }

    // Audio device cleanup stub
    this->audioDev = 0;

    return ZUN_SUCCESS;
}

void SoundPlayer::StopBGM()
{
    // BGM stop stub - no-op for now
    if (this->backgroundMusic.srcWav.fileStream != NULL)
    {
        // Already null in stub mode
        this->backgroundMusic.srcWav.fileStream = NULL;
        utils::DebugPrint2("stop BGM\n");
    }
}

void SoundPlayer::FadeOut(f32 seconds)
{
    if (this->backgroundMusic.srcWav.fileStream != NULL)
    {
        this->backgroundMusic.fadeoutLen = seconds * 44100;
        this->backgroundMusic.fadeoutProgress = 0;
    }
}

ZunResult SoundPlayer::LoadWav(const char *path)
{
    // WAV loading stub - audio not yet implemented
    // Just return success to let the game continue
    utils::DebugPrint2("load BGM (stub) %s\n", path);
    return ZUN_SUCCESS;
}

ZunResult SoundPlayer::LoadPos(const char *path)
{
    // Load loop points stub
    if (this->audioDev == 0 || g_Supervisor.cfg.playSounds == 0 || backgroundMusic.srcWav.fileStream == NULL)
    {
        return ZUN_ERROR;
    }

    u8 *fileData = FileSystem::OpenPath(path, 0);

    if (fileData == NULL)
    {
        return ZUN_ERROR;
    }

    // Stub: just read dummy values
    // this->backgroundMusic.loopStart = SDL_SwapLE32(*((u32 *)fileData));
    // this->backgroundMusic.loopEnd = SDL_SwapLE32(*(u32 *)(fileData + 4));
    
    free(fileData);

    if (this->backgroundMusic.loopStart >= this->backgroundMusic.loopEnd ||
        this->backgroundMusic.loopEnd > this->backgroundMusic.srcWav.samples)
    {
        this->backgroundMusic.loopStart = 0;
        this->backgroundMusic.loopEnd = this->backgroundMusic.srcWav.samples;

        return ZUN_ERROR;
    }

    return ZUN_SUCCESS;
}

ZunResult SoundPlayer::InitSoundBuffers()
{
    if (this->audioDev == 0)
    {
        return ZUN_ERROR;
    }

    std::fill_n(this->soundBuffersToPlay, ARRAY_SIZE(this->soundBuffersToPlay), -1);

    for (int idx = 0; idx < ARRAY_SIZE_SIGNED(g_SoundBufferIdxVol); idx++)
    {
        if (this->LoadSound(idx, g_SFXList[g_SoundBufferIdxVol[idx].bufferIdx],
                            1.0f / ZUN_POWF(10.0f, (float)g_SoundBufferIdxVol[idx].volume / -2000)) != ZUN_SUCCESS)
        {
            g_GameErrorContext.Log(TH_ERR_SOUNDPLAYER_FAILED_TO_LOAD_SOUND_FILE, g_SFXList[idx]);
            return ZUN_ERROR;
        }

        this->soundBuffers[idx].isPlaying = false;
        this->soundBuffers[idx].pos = 0;
    }

    return ZUN_SUCCESS;
}

ZunResult SoundPlayer::LoadSound(i32 idx, const char *path, f32 volumeMultiplier)
{
    // Sound loading stub - audio not yet implemented
    u8 *wavRawData;
    u8 *wavRawSamples;
    u32 wavRawSampleByteCount;

    soundBufMutex.lock();

    if (this->soundBuffers[idx].samples != NULL)
    {
        delete[] this->soundBuffers[idx].samples;
        this->soundBuffers[idx].samples = NULL;
    }

    wavRawData = (u8 *)FileSystem::OpenPath(path, 0);

    if (wavRawData == NULL)
    {
        goto fail;
    }

    // Stub: just use dummy values for sound loading
    // In real implementation, this would load WAV data using SDL_RW functions
    wavRawSamples = wavRawData; // Placeholder
    wavRawSampleByteCount = g_LastFileSize;

    // Convert audio if needed - stubbed for now
    if (wavRawSampleByteCount > 0)
    {
        this->soundBuffers[idx].len = wavRawSampleByteCount / 2;
        this->soundBuffers[idx].samples = new i16[this->soundBuffers[idx].len];
        std::memcpy(this->soundBuffers[idx].samples, wavRawSamples, wavRawSampleByteCount);
    }

    for (u32 i = 0; i < this->soundBuffers[idx].len; i++)
    {
        this->soundBuffers[idx].samples[i] *= volumeMultiplier;
    }

    this->soundBuffers[idx].pos = 0;
    this->soundBuffers[idx].isPlaying = false;

    soundBufMutex.unlock();
    return ZUN_SUCCESS;

fail:
    soundBufMutex.unlock();
    return ZUN_ERROR;
}

ZunResult SoundPlayer::PlayBGM(bool isLooping)
{
    utils::DebugPrint2("play BGM (stub)\n");

    if (this->backgroundMusic.srcWav.fileStream == NULL)
    {
        return ZUN_ERROR;
    }

    this->isLooping = isLooping;
    return ZUN_SUCCESS;
}

void SoundPlayer::PlaySounds()
{
    i32 idx;
    i32 sndBufIdx;

    if (this->audioDev == 0 || !g_Supervisor.cfg.playSounds)
    {
        return;
    }

    soundBufMutex.lock();

    for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->soundBuffersToPlay); idx++)
    {
        if (this->soundBuffersToPlay[idx] < 0)
        {
            break;
        }

        sndBufIdx = this->soundBuffersToPlay[idx];
        this->soundBuffersToPlay[idx] = -1;

        if (this->soundBuffers[sndBufIdx].samples == NULL)
        {
            continue;
        }

        this->soundBuffers[sndBufIdx].pos = 0;
        this->soundBuffers[sndBufIdx].isPlaying = true;
    }

    soundBufMutex.unlock();
}

void SoundPlayer::PlaySoundByIdx(SoundIdx idx)
{
    u32 i;

    for (i = 0; i < ARRAY_SIZE(this->soundBuffersToPlay); i++)
    {
        if (this->soundBuffersToPlay[i] < 0)
        {
            break;
        }

        if (this->soundBuffersToPlay[i] == idx)
        {
            return;
        }
    }

    if (i >= 3)
    {
        return;
    }

    this->soundBuffersToPlay[i] = idx;
}

void SoundPlayer::MixAudio(u32 samples)
{
    // Audio mixing stub - no-op for now
    // Real implementation would mix SFX and BGM samples
    std::vector<i16> finalBuffer(samples);
    std::vector<i32> mixBuffer(samples);
    u8 playingChannels = 0;

    this->soundBufMutex.lock();

    for (int i = 0; i < ARRAY_SIZE_SIGNED(this->soundBuffers); i++)
    {
        if (!this->soundBuffers[i].isPlaying)
        {
            continue;
        }

        playingChannels++;
        const u32 samplesToMix = std::min(samples / 2, this->soundBuffers[i].len - this->soundBuffers[i].pos);

        for (u32 j = 0; j < samplesToMix; j++)
        {
            mixBuffer[j * 2] += this->soundBuffers[i].samples[this->soundBuffers[i].pos + j];
            mixBuffer[j * 2 + 1] += this->soundBuffers[i].samples[this->soundBuffers[i].pos + j];
        }

        this->soundBuffers[i].pos += samplesToMix;

        if (this->soundBuffers[i].pos == this->soundBuffers[i].len)
        {
            this->soundBuffers[i].isPlaying = false;
        }
    }

    if (this->backgroundMusic.srcWav.fileStream != NULL)
    {
        u32 samplesMixed = 0;
        f32 fadeoutMult = 1.0f;

        while (samplesMixed < samples / 2)
        {
            // BGM playback stub
            this->backgroundMusic.pos += 1;
            samplesMixed += 1;

            if (this->backgroundMusic.pos >= this->backgroundMusic.loopEnd)
            {
                this->backgroundMusic.pos = this->backgroundMusic.loopStart;
            }
        }
        playingChannels++;
    }

    this->soundBufMutex.unlock();

    // Audio output stub - queue to audio device
    // SDL_QueueAudio is replaced with no-op
    // This would be replaced with real audio output when implemented
}

void SoundPlayer::BackgroundMusicPlayerThread()
{
    // Audio thread stub - just run a simple loop
    u32 latencyLimit = 14'700; // ~5 frames
    u64 samplesSent = 0;
    
    // Use platform timing instead of SDL_GetTicks64
    // u64 startTick = SDL_GetTicks64();
    
    while (1)
    {
        // Platform timing stub
        // u64 curTicks = SDL_GetTicks64();
        
        // Keep slightly more than 1 frame's worth of samples in the audio buffer at all times
        i32 targetSamples = 1024; // Simplified

        if (targetSamples > 0)
        {
            this->MixAudio(targetSamples * 2);
            samplesSent += targetSamples;
        }

        if (this->terminateFlag)
        {
            return;
        }

        // Use platform delay instead of SDL_Delay
        platform_delay(5);
    }
}
