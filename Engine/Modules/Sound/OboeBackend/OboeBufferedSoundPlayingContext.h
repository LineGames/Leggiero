////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeBufferedSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// All data buffered Sound Playing Context in Oboe Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND__OBOE_BUFFERED_SOUND_PLAYING_CONTEXT_H
#define __LM_SOUND__OBOE_BACKEND__OBOE_BUFFERED_SOUND_PLAYING_CONTEXT_H


// Standard Library
#include <atomic>
#include <queue>
#include <memory>
#include <tuple>
#include <vector>

// External Library
#include <oboe/Oboe.h>

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>
#include <Utility/Object/VerySimpleObjectPool.h>
#include <Utility/Sugar/NonCopyable.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"
#include "../Common/ISoundPlayingHandle.h"
#include "OboeCommon.h"
#include "IOboeStreamRendering.h"


namespace Leggiero
{
	namespace Sound
	{
		// Forward Declaration
		class ISoundProvider;


		namespace Oboe
		{
			// Forward Declaration
			class OboeSoundMixer;


			// Buffered Sound Playing Context
			class OboeBufferedSoundPlayingContext
				: public ISoundPlayingHandle
				, public IOboeStreamRendering
				, public std::enable_shared_from_this<OboeBufferedSoundPlayingContext>
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				friend class OboeSoundMixer;
				using MixerClass = OboeSoundMixer;

			public:
				virtual ~OboeBufferedSoundPlayingContext();

			public:
				OboeBufferedSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, bool isStartImmediately = true, float volume = 1.0f, bool isLooping = false, bool isPauseAfterFinish = false);

			private:
				OboeBufferedSoundPlayingContext() = delete;

			public:	// ISoundPlayingHandle
				virtual bool IsFinished() const override;
				virtual void Stop() override;

				virtual bool IsPaused() const override { return m_isPaused; }
				virtual void Pause() override;
				virtual void Resume() override;

				virtual SampleNumberType GetCurrentPosition() override { return m_currentReadPosition; }
				virtual bool Seek(SampleNumberType samplePosition) override;

				virtual float GetVolume() const override { return m_volume.load(); }
				virtual void SetVolume(float volume) override;

			public:
				void SetIsLooping(bool isLooping);
				bool IsLooping();

				void Rewind();

			public:	// IOboeStreamRendering
				virtual bool IsHighPriorityObject() const override { return true; }
				virtual bool RenderAudio(void *audioData, int32_t numFrames, int32_t streamDataFrameBytes, oboe::AudioFormat streamDataFormat, int32_t streamChannelCount) override;

				virtual bool IsStoppedRendering() override { return IsFinished(); }
				virtual bool IsPausedRendering() override { return IsPaused(); }

			public:
				void SetWillPauseAfterFinish(bool isPauseAfterFinish) { m_isPauseAfterFinish = isPauseAfterFinish; }
				bool IsWillPauseAfterFinish() const { return m_isPauseAfterFinish; }

				void FillStreamData(void *dataBuffer, void *targetBuffer, int frameCount, bool isApplyVolume, int32_t streamDataFrameBytes, oboe::AudioFormat streamDataFormat, int32_t streamChannelCount);

			protected:
				void _Update();

			protected:
				void _FillBuffer(std::shared_ptr<ISoundProvider> soundProvider, Utility::Object::VerySimpleObjectPool<DataBufferType> &bufferPool);

			protected:
				SampleFormat		m_soundFormat;
				int32_t				m_soundChannels;
				int32_t				m_soundSampleFrequency;
				size_t				m_soundSampleByteSize;

				bool				m_isPaused;
				bool				m_isPlayFinished;
				bool				m_isAllDataStreamed;

				bool m_isPauseAfterFinish;

				bool				m_isLooping;
				SampleNumberType	m_currentReadPosition;

				std::atomic<float>	m_volume;

				uint8_t *m_dataBuffer;
				std::shared_ptr<DataBufferType> m_dataBufferHolder;
				SampleNumberType				m_bufferSampleSize;

				std::weak_ptr<Utility::Object::PointerHolder> m_mixerHolder;

				std::atomic_bool	m_currentReadHeaderLock;
			};
		}
	}
}

#endif
