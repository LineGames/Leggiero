////////////////////////////////////////////////////////////////////////////////
// Platform/WinPCBGMPlayer.cpp (Leggiero/Modules - Sound)
//
// WinPC Platform BGM Player Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "WinPCBGMPlayer.h"

// Standard Library
#include <tuple>
#include <unordered_set>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>
#include <Utility/String/AsciiStringUtility.h>
#include <Utility/Threading/ThreadSleep.h>

// Leggiero.FileSystem
#include <FileSystem/FileSystemUtility.h>
#include <FileSystem/BundleFileResourceComponent.h>

// Leggiero.Sound
#include "../SoundMixerComponent.h"
#include "../Provider/WavSoundProvider.h"
#include "../Provider/OggSoundProvider.h"
#include "../Provider/MP3SoundProvider.h"
#include "../OpenALBackend/OpenALSoundPlayingContext.h"


namespace Leggiero
{
	namespace Sound
	{
		//////////////////////////////////////////////////////////////////////////////// BGMPlayerComponent 

		//------------------------------------------------------------------------------
		// BGM Player Component Factory Function
		BGMPlayerComponent *BGMPlayerComponent::CreateComponentObject(bool isUseBundleFileSystem)
		{
			return new WinPC::WinPCBGMPlayer(isUseBundleFileSystem);
		}


		namespace WinPC
		{
			namespace _Internal
			{
				//////////////////////////////////////////////////////////////////////////////// WinPCBGMPlayingContext

				// BGM Playing Context in WinPC Platform
				// based on OpenAL Sound Playing Context
				class WinPCBGMPlayingContext
					: public IBGMPlayingHandle
				{
					friend class WinPCBGMPlayer;

				public:
					WinPCBGMPlayingContext(std::shared_ptr<OpenAL::OpenALSoundPlayingContext> playingContext, ISoundProvider &soundData);
					virtual ~WinPCBGMPlayingContext();

				public:	//IBGMPlayingHandle
					virtual void Prepare() override { }

					virtual void Play() override;
					virtual void Stop() override;
					virtual void Pause() override;

					virtual bool IsPlaying() override;

					virtual float GetLength() override { return m_musicLength; }
					virtual float GetCurrentPosition() override { return static_cast<float>(m_playingContext->GetCurrentPosition()) / m_frequency; }
					virtual bool Seek(float position) override { return m_playingContext->Seek(static_cast<SampleNumberType>(position * m_frequency)); }

					virtual bool IsLooping() override { return m_isLooping; }
					virtual void SetLooping(bool isLooping) override { m_isLooping = isLooping; }

					virtual float GetVolume() override { return m_playingContext->GetVolume(); }
					virtual void SetVolume(float volume) override { m_playingContext->SetVolume(volume); }

				protected:
					std::shared_ptr<OpenAL::OpenALSoundPlayingContext> m_playingContext;

					SamplingFrequencyType m_frequency;
					float m_musicLength;
					SampleNumberType	m_musicLengthInSamples;

					bool m_isPlayingByUser;
					bool m_isLooping;

					bool m_isLastPlaying;
				};

				//------------------------------------------------------------------------------
				WinPCBGMPlayingContext::WinPCBGMPlayingContext(std::shared_ptr<OpenAL::OpenALSoundPlayingContext> playingContext, ISoundProvider &soundData)
					: m_playingContext(playingContext)
					, m_frequency(soundData.GetFrequency()), m_musicLength(soundData.GetLengthInSecond()), m_musicLengthInSamples(soundData.GetSampleLength())
					, m_isPlayingByUser(false), m_isLastPlaying(false)
				{
				}

				//------------------------------------------------------------------------------
				WinPCBGMPlayingContext::~WinPCBGMPlayingContext()
				{
				}

				//------------------------------------------------------------------------------
				void WinPCBGMPlayingContext::Play()
				{
					m_isLastPlaying = true;
					m_isPlayingByUser = true;
					if (m_playingContext->IsPaused())
					{
						m_playingContext->Resume();
					}
				}

				//------------------------------------------------------------------------------
				void WinPCBGMPlayingContext::Stop()
				{
					m_isPlayingByUser = false;
					m_playingContext->Pause();
					m_playingContext->Seek(0);
				}

				//------------------------------------------------------------------------------
				void WinPCBGMPlayingContext::Pause()
				{
					m_isPlayingByUser = false;
					m_playingContext->Pause();
				}

				//------------------------------------------------------------------------------
				bool WinPCBGMPlayingContext::IsPlaying()
				{
					return (!m_isPlayingByUser && !m_playingContext->IsPaused());
				}
			}


			//////////////////////////////////////////////////////////////////////////////// WinPCBGMPlayer

			//------------------------------------------------------------------------------
			WinPCBGMPlayer::WinPCBGMPlayer(bool isUseBundleFileSystem)
				: m_isInitialized(false), m_isUpdateThreadExists(false)
				, m_isUseBundleFileSystem(isUseBundleFileSystem)
			{
				
			}

			//------------------------------------------------------------------------------
			WinPCBGMPlayer::~WinPCBGMPlayer()
			{
				ShutdownComponent();
			}

			//------------------------------------------------------------------------------
			// Initialize the Component
			void WinPCBGMPlayer::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
			{
				m_isShutdownRequested.store(false);

				if (pthread_create(&m_updateThread, NULL, _UpdatePlayThreadHelper, this) != 0)
				{
					// Update Thread Failed
					m_isUpdateThreadExists = false;
				}
				else
				{
					m_isUpdateThreadExists = true;
				}

				m_isInitialized.store(true);
			}

			//------------------------------------------------------------------------------
			// Safely Shutdown Component
			void WinPCBGMPlayer::ShutdownComponent()
			{
				bool shouldBeInitialized = true;
				if (!m_isInitialized.compare_exchange_strong(shouldBeInitialized, false))
				{
					// Not-Initialized
					return;
				}

				m_isShutdownRequested.store(true);
				if (m_isUpdateThreadExists)
				{
					pthread_join(m_updateThread, nullptr);
					m_isUpdateThreadExists = false;
				}
			}

			//------------------------------------------------------------------------------
			// Get type Id list of dependent components needed by this component
			const std::vector<EngineComponentIdType> &WinPCBGMPlayer::GetDependentComponents() const
			{
				static std::vector<EngineComponentIdType> dependentComponentListWithBundle{
					EngineComponentIdType::kBundleFileResource, 
					EngineComponentIdType::kSoundMixer
				};
				static std::vector<EngineComponentIdType> dependentComponentListNoBundle{
					EngineComponentIdType::kSoundMixer
				};
				return (m_isUseBundleFileSystem ? dependentComponentListWithBundle : dependentComponentListNoBundle);
			}

			//------------------------------------------------------------------------------
			// Inject Dependency to the Component.
			// All dependency injections will be done before the initialization.
			void WinPCBGMPlayer::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
			{
				switch (componentId)
				{
					case EngineComponentIdType::kBundleFileResource:
						{
							m_pBundleFileSys = dynamic_cast<FileSystem::BundleFileResourceComponent *>(dependentComponent);
						}
						break;

					case EngineComponentIdType::kSoundMixer:
						{
							m_pSoundMixer = dynamic_cast<SoundMixerComponent *>(dependentComponent);
						}
						break;
				}
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IBGMPlayingHandle> WinPCBGMPlayer::PlayFromFile(const std::string &filePath, bool isStartImmediately, float volume, bool isLooping, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler)
			{
				if (!m_isInitialized.load())
				{
					return nullptr;
				}

				SupportedSoundFormatType soundFormat = _GetFormatFromPath(filePath);
				if (soundFormat == SupportedSoundFormatType::kINVALID)
				{
					// Not supporting sound format
					return nullptr;
				}

				std::shared_ptr<ISoundProvider> dataProvider;
				switch (soundFormat)
				{
					case SupportedSoundFormatType::kWAV:
						dataProvider = std::make_shared<WavSoundProvider>(filePath);
						break;

					case SupportedSoundFormatType::kOGG:
						dataProvider = std::make_shared<OggSoundProvider>(filePath);
						break;

					case SupportedSoundFormatType::kMP3:
						dataProvider = std::make_shared<MP3SoundProvider>(filePath);
						break;
				}
				if (!dataProvider)
				{
					return nullptr;
				}

				std::shared_ptr<OpenAL::OpenALSoundPlayingContext> playingContext(m_pSoundMixer->Play(dataProvider, false, volume, true));
				if (!playingContext)
				{
					return nullptr;
				}
				
				std::shared_ptr<_Internal::WinPCBGMPlayingContext> bgmPlayingContext(std::make_shared<_Internal::WinPCBGMPlayingContext>(playingContext, *dataProvider));
				bgmPlayingContext->m_playFinishHandler = playFinishHandler;
				bgmPlayingContext->SetLooping(isLooping);

				{
					auto lockContext = m_contextListLock.Lock();
					m_contextHolders.push_back(bgmPlayingContext);
					m_processingContexts.push_back(bgmPlayingContext);
				}

				if (isStartImmediately)
				{
					bgmPlayingContext->Play();
				}

				return bgmPlayingContext;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IBGMPlayingHandle> WinPCBGMPlayer::PlayInBundle(const std::string &virtualPath, bool isStartImmediately, float volume, bool isLooping, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler)
			{
				if (!m_isInitialized.load())
				{
					return nullptr;
				}
				if (m_pBundleFileSys == nullptr)
				{
					return nullptr;
				}

				// Check File
				std::string realFilePath = m_pBundleFileSys->NPO_GetBundleFileRealPath(virtualPath);
				if (realFilePath.empty())
				{
					return nullptr;
				}
				if (!m_pBundleFileSys->IsBundleFileExists(virtualPath))
				{
					return nullptr;
				}

				return PlayFromFile(realFilePath, isStartImmediately, volume, isLooping, playFinishHandler);
			}

			//------------------------------------------------------------------------------
			WinPCBGMPlayer::SupportedSoundFormatType WinPCBGMPlayer::_GetFormatFromPath(const std::string &path)
			{
				std::string fileExtension = FileSystem::Utility::GetExtension(path);
				if (fileExtension.empty())
				{
					return SupportedSoundFormatType::kINVALID;
				}

				std::string lowerExtension = Utility::String::ASCIIStringUtility::ToLower(fileExtension);
				if (lowerExtension == "mp3")
				{
					return SupportedSoundFormatType::kMP3;
				}
				if (lowerExtension == "ogg")
				{
					return SupportedSoundFormatType::kOGG;
				}
				if (lowerExtension == "wav" || lowerExtension == "wave")
				{
					return SupportedSoundFormatType::kWAV;
				}

				return SupportedSoundFormatType::kINVALID;
			}

			//------------------------------------------------------------------------------
			// (Worker Thread) Update currently playing sounds
			void WinPCBGMPlayer::_UpdatePlay()
			{
				std::vector<std::tuple<std::shared_ptr<_Internal::WinPCBGMPlayingContext>, bool> > contextToNotifyFinish;
				std::unordered_set< _Internal::WinPCBGMPlayingContext *> inactiveStreams;
				std::unordered_set< _Internal::WinPCBGMPlayingContext *> registeredStreams;
				while (!m_isShutdownRequested.load())
				{
					// Update Contexts
					{
						auto lockContext = m_contextListLock.Lock();

						for (std::list<std::weak_ptr<_Internal::WinPCBGMPlayingContext> >::iterator it = m_processingContexts.begin(); it != m_processingContexts.end(); )
						{
							std::shared_ptr<_Internal::WinPCBGMPlayingContext> currentContext(it->lock());
							if (!currentContext)
							{
								it = m_processingContexts.erase(it);
								continue;
							}

							bool isStreamRestarted = false;
							bool isStreamToNotifyFinish = false;
							if (currentContext->m_playingContext->IsPaused())
							{
								if (currentContext->m_isPlayingByUser)
								{
									if (currentContext->m_isLooping)
									{
										if (currentContext->m_playingContext->GetCurrentPosition() + 1 >= currentContext->m_musicLengthInSamples)
										{
											currentContext->m_playingContext->Seek(0);
											currentContext->m_playingContext->Resume();
											currentContext->m_isLastPlaying = true;
											isStreamRestarted = true;
										}
									}
									else if (currentContext->m_isLastPlaying)
									{
										bool isFinishedStream = (currentContext->m_playingContext->GetCurrentPosition() + 1 >= currentContext->m_musicLengthInSamples);
										contextToNotifyFinish.push_back(std::make_tuple(currentContext, isFinishedStream));
										isStreamToNotifyFinish = true;
									}
								}

								if (!currentContext->m_isLastPlaying)
								{
									if (!isStreamToNotifyFinish)
									{
										inactiveStreams.insert(currentContext.get());
									}
								}

								if (!isStreamRestarted)
								{
									currentContext->m_isLastPlaying = false;
								}
							}
							else
							{
								currentContext->m_isLastPlaying = true;
							}
							++it;
						}
					}

					// Notify Stream Ends
					for (std::tuple<std::shared_ptr<_Internal::WinPCBGMPlayingContext>, bool> &currentToNotifyPair : contextToNotifyFinish)
					{
						std::get<0>(currentToNotifyPair)->_OnPlayFinish(std::get<1>(currentToNotifyPair));
					}
					contextToNotifyFinish.clear();

					// Check Inactive Holdings
					{
						auto lockContext = m_contextListLock.Lock();

						for (std::list<std::shared_ptr<_Internal::WinPCBGMPlayingContext> >::iterator it = m_contextHolders.begin(); it != m_contextHolders.end(); )
						{
							if (inactiveStreams.find(it->get()) != inactiveStreams.end())
							{
								it = m_contextHolders.erase(it);
								continue;
							}

							registeredStreams.insert(it->get());
							++it;
						}
					}
					inactiveStreams.clear();

					// Register Holdings
					{
						auto lockContext = m_contextListLock.Lock();

						for (std::list<std::weak_ptr<_Internal::WinPCBGMPlayingContext> >::iterator it = m_processingContexts.begin(); it != m_processingContexts.end(); )
						{
							std::shared_ptr<_Internal::WinPCBGMPlayingContext> currentContext(it->lock());
							if (!currentContext)
							{
								it = m_processingContexts.erase(it);
								continue;
							}

							if (registeredStreams.find(currentContext.get()) == registeredStreams.end())
							{
								m_contextHolders.push_back(currentContext);
							}
							++it;
						}
					}
					registeredStreams.clear();

					Utility::Threading::PthreadSleepMS(24);
				}
			}

			//------------------------------------------------------------------------------
			// Sound Play Thread Helper Function
			void *WinPCBGMPlayer::_UpdatePlayThreadHelper(void *threadThis)
			{
				((WinPCBGMPlayer *)threadThis)->_UpdatePlay();
				return nullptr;
			}
		}
	}
}
