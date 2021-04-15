////////////////////////////////////////////////////////////////////////////////
// OboeBackend/OboeStream.h (Leggiero/Modules - Sound)
//
// Oboe Sound Playing Stream
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_SOUND__OBOE_BACKEND__OBOE_STREAM_H
#define __LM_SOUND__OBOE_BACKEND__OBOE_STREAM_H


// Standard Library
#include <atomic>
#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>

// External Library
#include <oboe/Oboe.h>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "../Common/SoundTypes.h"
#include "OboeCommon.h"


namespace Leggiero
{
	namespace Sound
	{
		namespace Oboe
		{
			// Forward Declaration
			class OboeSoundMixer;
			class IOboeStreamRendering;

			namespace _Internal
			{
				class RenderingQueueBuffer;
			}


			// Oboe Sound Stream
			class OboeStream
				: public oboe::AudioStreamDataCallback
				, public oboe::AudioStreamErrorCallback
			{
			public:
				friend class OboeSoundMixer;

			public:
				virtual ~OboeStream();

			public:
				OboeStream(SamplingFrequencyType samplingRate);

			private:
				OboeStream() = delete;

			public:	// Oboe Callbacks
				virtual oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override;
				virtual void onErrorAfterClose(oboe::AudioStream *audioStream, oboe::Result error) override;

			public:
				bool HasStream() const { return (bool)m_stream; }

				int32_t GetChannelCount() const { return m_streamChannelCount; }
				int32_t GetDataFrameBytes() const { return m_streamDataFrameBytes; }
				oboe::AudioFormat GetAudioFormat() const { return m_streamDataFormat; }

				void StartStream();
				void StopStream();

				std::shared_ptr<oboe::AudioStream> GetStream() { return m_stream; }

			protected:
				void _CreateStream(bool isForceTry);

			protected:
				std::shared_ptr<oboe::AudioStream>	m_stream;

				int32_t								m_streamChannelCount;
				int32_t								m_streamDataFrameBytes;
				oboe::AudioFormat					m_streamDataFormat;

				int									m_streamCreationError;

				bool	m_isSupposedToStart;

				int32_t	m_requestedSamplingRate;

				_Internal::RenderingQueueBuffer *m_oboeRenderingBuffer;

				void				*m_mixingTempBuffer;
				void				*m_hdrMixingBuffer;
				oboe::AudioFormat	m_currentMixingBufferFormat;

			protected:	// Data block for sound mixer usage
				std::atomic<_Internal::RenderingQueueBuffer *>	m_oboeReadyBuffer;
				_Internal::RenderingQueueBuffer					*m_renderingTargetBuffer;
				std::atomic<uint64_t>							m_currentQueueVersion;

				std::unordered_map<IOboeStreamRendering *, std::shared_ptr<IOboeStreamRendering> >	m_streamRenderings;
				std::list<std::weak_ptr<IOboeStreamRendering> >										m_weakHoldingRenderings;

				Leggiero::Utility::Threading::SafePthreadLock	m_bufferRenderingLock;
				std::atomic_bool								m_isBufferDirty;

				std::vector<IOboeStreamRendering *> m_renderingWorkingBuffer1;
				std::vector<IOboeStreamRendering *> m_renderingWorkingBuffer2;

				void _RegisterRendering(std::shared_ptr<IOboeStreamRendering> rendering);
				bool _MakeRenderQueue();
				void _MakeQueueIfNeeded();
			};
		}
	}
}

#endif
