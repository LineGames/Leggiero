////////////////////////////////////////////////////////////////////////////////
// Platform/iOSBGMPlayer.mm (Leggiero/Modules - Sound)
//
// iOS Platform BGM Player Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "iOSBGMPlayer.h"

// Standard Library
#include <tuple>
#include <unordered_set>

// System Library
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>
#include <Utility/String/AsciiStringUtility.h>
#include <Utility/Threading/ThreadSleep.h>

// Leggiero.FileSystem
#include <FileSystem/FileSystemUtility.h>
#include <FileSystem/BundleFileResourceComponent.h>


// Forward Declaration
namespace Leggiero
{
	namespace Sound
	{
		namespace iOS
		{
			namespace _Internal
			{
				class iOSBGMPlayingContext;
			}
		}
	}
}


#pragma mark - Objective-C Interface
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Audio Player Delegate
@interface BGMPlayerDelegate : NSObject <AVAudioPlayerDelegate>
{
	std::weak_ptr<Leggiero::Sound::iOS::_Internal::iOSBGMPlayingContext> m_contextWeak;
    @public AVAudioPlayer *audioPlayer;
}

- (id)initWithContext:(std::shared_ptr<Leggiero::Sound::iOS::_Internal::iOSBGMPlayingContext>)playingContext musicUrl:(NSURL *)url;

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag;

@end


#pragma mark - iOS Playing Context
////////////////////////////////////////////////////////////////////////////////

namespace Leggiero
{
	namespace Sound
	{
		namespace iOS
		{
			namespace _Internal
			{
				// C++ BGM Playing Context in iOS Platform
				class iOSBGMPlayingContext
					: public IBGMPlayingHandle
				{
				public:
					iOSBGMPlayingContext(IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler);
					virtual ~iOSBGMPlayingContext();

				public:	//IBGMPlayingHandle
					virtual void Prepare() override;

					virtual void Play() override;
					virtual void Stop() override;
					virtual void Pause() override;

					virtual bool IsPlaying() override;

					virtual float GetLength() override;
					virtual float GetCurrentPosition() override;
					virtual bool Seek(float position) override;

					virtual bool IsLooping() override;
					virtual void SetLooping(bool isLooping) override;

					virtual float GetVolume() override;
					virtual void SetVolume(float volume) override;
					
				public:
					void InitializeWithPlayer(BGMPlayerDelegate *playerObject);
					void NotifyFinishPlayingEvent(bool successfully);

				protected:
					BGMPlayerDelegate *m_playerObject;
				};

				//------------------------------------------------------------------------------
				iOSBGMPlayingContext::iOSBGMPlayingContext(IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler)
					: m_playerObject(nil)
				{
                    m_playFinishHandler = playFinishHandler;
				}

				//------------------------------------------------------------------------------
				iOSBGMPlayingContext::~iOSBGMPlayingContext()
				{
				}
				
				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::Prepare()
				{
					[m_playerObject->audioPlayer prepareToPlay];
				}

				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::Play()
				{
                    [m_playerObject->audioPlayer play];
				}

				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::Stop()
				{
                    [m_playerObject->audioPlayer stop];
				}

				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::Pause()
				{
                    [m_playerObject->audioPlayer pause];
				}

				//------------------------------------------------------------------------------
				bool iOSBGMPlayingContext::IsPlaying()
				{
					return ([m_playerObject->audioPlayer isPlaying] == YES);
				}
				
				//------------------------------------------------------------------------------
				float iOSBGMPlayingContext::GetLength()
				{
                    NSTimeInterval length = m_playerObject->audioPlayer.duration;
					return static_cast<float>(length);
				}
				
				//------------------------------------------------------------------------------
				float iOSBGMPlayingContext::GetCurrentPosition()
				{
                    NSTimeInterval position = m_playerObject->audioPlayer.currentTime;
					return static_cast<float>(position);
				}
				
				//------------------------------------------------------------------------------
				bool iOSBGMPlayingContext::Seek(float position)
				{
                    NSTimeInterval timePosition = static_cast<NSTimeInterval>(position);
                    m_playerObject->audioPlayer.currentTime = timePosition;
					return true;
				}

				//------------------------------------------------------------------------------
				bool iOSBGMPlayingContext::IsLooping()
				{
					return (m_playerObject->audioPlayer.numberOfLoops < 0);
				}
				
				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::SetLooping(bool isLooping)
				{
                    if (isLooping)
                    {
                        m_playerObject->audioPlayer.numberOfLoops = -1;
                    }
                    else
                    {
                        m_playerObject->audioPlayer.numberOfLoops = 0;
                    }
				}

				//------------------------------------------------------------------------------
				float iOSBGMPlayingContext::GetVolume()
				{
					return m_playerObject->audioPlayer.volume;
				}
				
				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::SetVolume(float volume)
				{
                    m_playerObject->audioPlayer.volume = volume;
				}
				
				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::InitializeWithPlayer(BGMPlayerDelegate *playerObject)
				{
                    m_playerObject = playerObject;
				}
				
				//------------------------------------------------------------------------------
				void iOSBGMPlayingContext::NotifyFinishPlayingEvent(bool successfully)
				{
					_OnPlayFinish(successfully);
				}
			}
		}
	}
}


#pragma mark - Objective-C Implementation
////////////////////////////////////////////////////////////////////////////////

@implementation BGMPlayerDelegate

//------------------------------------------------------------------------------
- (id)initWithContext:(std::shared_ptr<Leggiero::Sound::iOS::_Internal::iOSBGMPlayingContext>)playingContext musicUrl:(NSURL *)url
{
    if (self = [super init])
    {
        m_contextWeak = playingContext;
		
		audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:nil];
		audioPlayer.delegate = self;
        
        return self;
    }
    else
    {
        return nil;
    }
}

//------------------------------------------------------------------------------
- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag;
{
	std::shared_ptr<Leggiero::Sound::iOS::_Internal::iOSBGMPlayingContext> context(m_contextWeak.lock());
	if (!context)
	{
		return;
	}
	
	context->NotifyFinishPlayingEvent(flag == YES);
}

@end


namespace Leggiero
{
	namespace Sound
	{
		#pragma mark - BGMPlayerComponent
		////////////////////////////////////////////////////////////////////////////////  

		//------------------------------------------------------------------------------
		// BGM Player Component Factory Function
		BGMPlayerComponent *BGMPlayerComponent::CreateComponentObject(bool isUseBundleFileSystem)
		{
			return new iOS::iOSBGMPlayer(isUseBundleFileSystem);
		}


		namespace iOS
		{
			#pragma mark - iOSBGMPlayer
			//////////////////////////////////////////////////////////////////////////////// 

			//------------------------------------------------------------------------------
			iOSBGMPlayer::iOSBGMPlayer(bool isUseBundleFileSystem)
				: m_isInitialized(false), m_isUpdateThreadExists(false)
				, m_isUseBundleFileSystem(isUseBundleFileSystem)
			{
				
			}

			//------------------------------------------------------------------------------
			iOSBGMPlayer::~iOSBGMPlayer()
			{
				ShutdownComponent();
			}

			//------------------------------------------------------------------------------
			// Initialize the Component
			void iOSBGMPlayer::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
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
			void iOSBGMPlayer::ShutdownComponent()
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
			const std::vector<EngineComponentIdType> &iOSBGMPlayer::GetDependentComponents() const
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
			void iOSBGMPlayer::InjectDependency(EngineComponentIdType componentId, EngineComponent *dependentComponent)
			{
				switch (componentId)
				{
					case EngineComponentIdType::kBundleFileResource:
						{
							m_pBundleFileSys = dynamic_cast<FileSystem::BundleFileResourceComponent *>(dependentComponent);
						}
						break;
				}
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IBGMPlayingHandle> iOSBGMPlayer::PlayFromFile(const std::string &filePath, bool isStartImmediately, float volume, bool isLooping, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler)
			{
				if (!m_isInitialized.load())
				{
					return nullptr;
				}
				
				std::shared_ptr<_Internal::iOSBGMPlayingContext> bgmPlayingContext(std::make_shared<_Internal::iOSBGMPlayingContext>(playFinishHandler));
				
				NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:[NSString stringWithUTF8String:filePath.c_str()]];
                BGMPlayerDelegate *player = [[BGMPlayerDelegate alloc] initWithContext:bgmPlayingContext musicUrl:(NSURL *)fileURL];
				
				bgmPlayingContext->InitializeWithPlayer(player);
				
				// Set properties
                bgmPlayingContext->SetVolume(volume);
				bgmPlayingContext->SetLooping(isLooping);

				{
					auto lockContext = m_contextListLock.Lock();
					m_contextHolders.push_back(bgmPlayingContext);
				}

				if (isStartImmediately)
				{
					bgmPlayingContext->Play();
				}

				return bgmPlayingContext;
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<IBGMPlayingHandle> iOSBGMPlayer::PlayInBundle(const std::string &virtualPath, bool isStartImmediately, float volume, bool isLooping, IBGMPlayingHandle::PlayFinishHandlerType playFinishHandler)
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
			// (Worker Thread) Update currently playing sounds
			void iOSBGMPlayer::_UpdatePlay()
			{
				std::vector<std::weak_ptr<_Internal::iOSBGMPlayingContext> > markedHolder;
				while (!m_isShutdownRequested.load())
				{
					// Check not playing contexts
					{
						auto lockContext = m_contextListLock.Lock();

						for (std::list<std::shared_ptr<_Internal::iOSBGMPlayingContext> >::iterator it = m_contextHolders.begin(); it != m_contextHolders.end(); )
						{
							if ((*it)->IsPlaying())
							{
								++it;
							}
							else
							{
								markedHolder.push_back(*it);
								it = m_contextHolders.erase(it);
							}
						}
					}
					
					// Re-hold not collected items
					{
						auto lockContext = m_contextListLock.Lock();
						
						for (std::weak_ptr<_Internal::iOSBGMPlayingContext> &currentMarked : markedHolder)
						{
							std::shared_ptr<_Internal::iOSBGMPlayingContext> currentContext(currentMarked.lock());
							if (currentContext)
							{
								m_contextHolders.push_back(currentContext);
							}
						}
					}
					markedHolder.clear();

					// This is a sort of GC without heavy usage
					Utility::Threading::PthreadSleepMS(900);
				}
			}

			//------------------------------------------------------------------------------
			// Sound Play Thread Helper Function
			void *iOSBGMPlayer::_UpdatePlayThreadHelper(void *threadThis)
			{
				((iOSBGMPlayer *)threadThis)->_UpdatePlay();
				return nullptr;
			}
		}
	}
}
