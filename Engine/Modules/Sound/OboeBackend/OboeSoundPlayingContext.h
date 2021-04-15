////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeSoundPlayingContext.h (Leggiero/Modules - Sound)
//
// Base Sound Playing Context in Oboe Backend
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND__OBOE_SOUND_PLAYING_CONTEXT_H
#define __LM_SOUND__OBOE_BACKEND__OBOE_SOUND_PLAYING_CONTEXT_H


// Standard Library
#include <atomic>
#include <queue>
#include <memory>
#include <tuple>
#include <vector>

// External Library
#include <concurrentqueue/concurrentqueue.h>
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


			// Oboe Sound Playing Context
			class OboeSoundPlayingContext
				: public ISoundPlayingHandle
				, public IOboeStreamRendering
				, public std::enable_shared_from_this<OboeSoundPlayingContext>
				, private Utility::SyntacticSugar::NonCopyable
			{
			public:
				friend class OboeSoundMixer;
				using MixerClass = OboeSoundMixer;

			public:
				virtual ~OboeSoundPlayingContext();

			public:
				OboeSoundPlayingContext(std::shared_ptr<ISoundProvider> soundProvider, std::shared_ptr<Utility::Object::PointerHolder> mixerHolder, bool isStartImmediately = true, float volume = 1.0f, bool isPauseAfterFinish = false);

			private:
				OboeSoundPlayingContext() = delete;

			public:	// ISoundPlayingHandle
				virtual bool IsFinished() const override;
				virtual void Stop() override;

				virtual bool IsPaused() const override { return m_isPaused; }
				virtual void Pause() override;
				virtual void Resume() override;

				virtual SampleNumberType GetCurrentPosition() override { return m_lastSampleNumber; }
				virtual bool Seek(SampleNumberType samplePosition) override;

				virtual float GetVolume() const override { return m_volume.load(); }
				virtual void SetVolume(float volume) override;

			public:	// IOboeStreamRendering
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
				void _PrapareDataBuffer();

				size_t _FillBuffer(int bufferIndex, DataBufferType &buffer);

				std::shared_ptr<DataBufferType> _RetainBuffer();
				void _ReleaseBuffer(std::shared_ptr<DataBufferType> buffer);
				void _FlushFreeBuffers();

			protected:
				std::shared_ptr<ISoundProvider> m_soundProvider;
				SampleFormat					m_soundFormat;
				int32_t							m_soundChannels;
				int32_t							m_soundSampleFrequency;
				size_t							m_soundSampleByteSize;
				int								m_samplesPerDataBuffer;

				bool				m_isPaused;
				bool				m_isPlayFinished;
				bool				m_isAllDataQueued;
				bool				m_isAllDataStreamed;
				SampleNumberType	m_lastSampleNumber;

				bool m_isPauseAfterFinish;

				std::atomic<float>	m_volume;

				std::weak_ptr<Utility::Object::PointerHolder> m_mixerHolder;
				size_t m_bufferSize;
				moodycamel::ConcurrentQueue<std::shared_ptr<DataBufferType> > m_freedBuffers;
				
				std::shared_ptr<DataBufferType> m_currentBuffer;
				size_t							m_currentBufferFilledSize;
				int								m_currentBufferReadSample;

				std::vector<std::tuple<int, size_t, std::shared_ptr<DataBufferType> > > m_bufferQueue;
				int m_lastQueuedBufferIndex;

				Utility::Threading::SafePthreadLock m_bufferQueueTailLock;
				std::atomic_bool m_bufferLock;
				std::atomic_bool m_currentBufferLock;
			};
		}
	}
}

#endif
