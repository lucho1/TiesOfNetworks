#ifndef _MODULE_SOUND_H
#define _MODULE_SOUND_H


struct sound_output;
struct sound_output_buffer;

struct AudioClip
{
	void *samples = nullptr;
	uint32 sampleCount = 0;
	uint32 samplingRate = 0;
	uint16 bitsPerSample = 0;
	uint16 channelCount = 0;
};


class ModuleSound : public Module
{
public:

	// Module methods
	bool Init() override;
	bool PostUpdate() override;
	bool CleanUp() override;


	// ModuleSound methods
	AudioClip* LoadAudioClip (const char *filename);
	void FreeAudioClip(AudioClip *audioClip);
	void PlayAudioClip(AudioClip *audioClip);


private:

	void RenderAudio(sound_output_buffer *Buffer, uint32 advancedSamples);
	void OutputAudio(sound_output *SoundOutput, sound_output_buffer *SourceBuffer, DWORD ByteToLock, DWORD BytesToWrite);


private:	

	enum audio_source_flags {
		AUDIO_SOURCE_START_BIT = 1<<0,
		AUDIO_SOURCE_LOOPS_BIT = 1<<1
	};

	struct audio_source
	{
		AudioClip *clip = nullptr;
		uint32 lastWriteSampleIndex = 0;
		uint8 flags = 0;
	};

	int16 m_Samples[48000 * 2];
	AudioClip m_AudioClips[2] = {};
	audio_source m_AudioSources[2] = {};
};

#endif // _MODULE_SOUND_H
