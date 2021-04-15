////////////////////////////////////////////////////////////////////////////////
// Platform/iOSPlatformSound.mm (Leggiero/Modules - Sound)
//
// Sound Implementation for iOS Platform Specific Features
////////////////////////////////////////////////////////////////////////////////

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// System Library
#import <Foundation/Foundation.h>
#import <AVFoundation/AVAudioSession.h>

// Leggiero.Utility
#include <Utility/Object/PointerHolder.h>
#include <Utility/Sugar/Finally.h>
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.Sound
#include "../SoundMixerComponent.h"
#include "../OpenALBackend/OpenALSoundMixer.h"


#pragma mark - iOS Platform Sound Mixer Definition
////////////////////////////////////////////////////////////////////////////////

namespace Leggiero
{
	namespace Sound
	{
        namespace IOS
        {
			// OpenAL Sound Mixer for iOS Platform
			class IOSOpenALSoundMixer
				: public OpenAL::OpenALSoundMixer
			{
			public:
				IOSOpenALSoundMixer();
				virtual ~IOSOpenALSoundMixer();
				
			public:
				void InitializeInterruptionObserver();
				void FinalizeInterruptionObserver();
				
				void HandleAudioSessionInterruptionBegin();
				void HandleAudioSessionInterruptionEnd();
				
			public:	// EngineComponent
				// Initialize the Component
				virtual void InitializeComponent(Engine::GameProcessAnchor *gameAnchor) override;

				// Safely Shutdown Component
				virtual void ShutdownComponent() override;
				
			public:	// IApplicationEventObserver
				virtual void OnPause() override;
				virtual void OnResume() override;
				
			protected:
				id notificationObserver;
				
				Utility::Threading::SafePthreadLock m_eventCheckMutex;
				
				std::atomic_bool m_isInAudioSessionInterruption;
				std::atomic_bool m_isInAppBackground;
			};
		}
	}
}


#pragma mark - Objective-C Implementation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Observer Class to handle Sound Notifications
@interface SoundNotificationObserver : NSObject
{
	std::weak_ptr<Leggiero::Utility::Object::PointerHolder> m_mixerHolder;
    bool m_isDetached;
}

- (id)initWithMixer:(std::shared_ptr<Leggiero::Utility::Object::PointerHolder>)mixerHolder;

- (void)detachObserver;

- (void)receiveAVAudioSessionInterruptionNofification:(NSNotification *)notification;

@end

@implementation SoundNotificationObserver

//------------------------------------------------------------------------------
- (id)initWithMixer:(std::shared_ptr<Leggiero::Utility::Object::PointerHolder>)mixerHolder
{
    if (self = [super init])
    {
        m_mixerHolder = mixerHolder;
        
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient
                                               error:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(receiveAVAudioSessionInterruptionNofification:)
                                                     name:AVAudioSessionInterruptionNotification
                                                   object:nil];
        m_isDetached = false;
        
        return self;
    }
    else
    {
        return nil;
    }
}

//------------------------------------------------------------------------------
- (void)dealloc
{
    [self detachObserver];
}

//------------------------------------------------------------------------------
- (void)detachObserver
{
    if (m_isDetached)
    {
        return;
    }
    
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    m_mixerHolder.reset();
    
    m_isDetached = true;
}

//------------------------------------------------------------------------------
- (void)receiveAVAudioSessionInterruptionNofification:(NSNotification *)notification
{
	std::shared_ptr<Leggiero::Utility::Object::PointerHolder> ownerMixerHolder(m_mixerHolder.lock());
	Leggiero::Sound::IOS::IOSOpenALSoundMixer *mixer = nullptr;
	std::unique_ptr<Leggiero::Utility::Object::PointerHolder::IHoldContext> lockContext;
	if (ownerMixerHolder)
	{
		lockContext = ownerMixerHolder->LockTarget();
		if (lockContext)
		{
			mixer = dynamic_cast<Leggiero::Sound::IOS::IOSOpenALSoundMixer *>(lockContext->GetTypedObject<Leggiero::Sound::OpenAL::OpenALSoundMixer>());
		}
	}
	if (mixer == nullptr)
	{
		return;
	}
    
    NSDictionary *userInfo = [notification userInfo];
    
    NSNumber *interruptionType = [userInfo objectForKey:AVAudioSessionInterruptionTypeKey];
    if (interruptionType == nullptr)
    {
        return;
    }
    switch ([interruptionType unsignedIntegerValue])
    {
        case AVAudioSessionInterruptionTypeBegan:
            {
                mixer->HandleAudioSessionInterruptionBegin();
            }
            break;
            
        case AVAudioSessionInterruptionTypeEnded:
            {
                mixer->HandleAudioSessionInterruptionEnd();
            }
            break;
    }
}

@end


namespace Leggiero
{
	namespace Sound
	{
        namespace IOS
        {
            #pragma mark - iOS Platform Sound Mixer Implementation
			////////////////////////////////////////////////////////////////////////////////
			
			//------------------------------------------------------------------------------
			IOSOpenALSoundMixer::IOSOpenALSoundMixer()
				: m_isInAudioSessionInterruption(false), m_isInAppBackground(false)
			{
				notificationObserver = nil;
			}

			//------------------------------------------------------------------------------
			IOSOpenALSoundMixer::~IOSOpenALSoundMixer()
			{
			}

			//------------------------------------------------------------------------------
			void IOSOpenALSoundMixer::InitializeInterruptionObserver()
			{
				notificationObserver = [[SoundNotificationObserver alloc] initWithMixer:m_myPointerHolder];
			}

			//------------------------------------------------------------------------------
			void IOSOpenALSoundMixer::FinalizeInterruptionObserver()
			{
				if (notificationObserver == nil)
				{
					return;
				}
				[notificationObserver detachObserver];
				notificationObserver = nil;
			}

			//------------------------------------------------------------------------------
			void IOSOpenALSoundMixer::HandleAudioSessionInterruptionBegin()
			{
				bool isNeedToSuspend = false;
				
				int lockResult = pthread_mutex_lock(&m_eventCheckMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_eventCheckMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);
					
					m_isInAudioSessionInterruption.store(true);
					if (!m_isInAppBackground.load())
					{
						isNeedToSuspend = true;
					}
				}
				else
				{
					m_isInAudioSessionInterruption.store(true);
					if (!m_isInAppBackground.load())
					{
						isNeedToSuspend = true;
					}
				}
				
				if (isNeedToSuspend)
				{
					_SuspendOpenAL();
				}
			}

			//------------------------------------------------------------------------------
			void IOSOpenALSoundMixer::HandleAudioSessionInterruptionEnd()
			{
				bool isNeedToRestore = false;
				
				int lockResult = pthread_mutex_lock(&m_eventCheckMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_eventCheckMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);
					
					m_isInAudioSessionInterruption.store(false);
					if (!m_isInAppBackground.load())
					{
						isNeedToRestore = true;
					}
				}
				else
				{
					m_isInAudioSessionInterruption.store(false);
					if (!m_isInAppBackground.load())
					{
						isNeedToRestore = true;
					}
				}
				
				if (isNeedToRestore)
				{
					_RestoreOpenAL();
				}
			}
			
			//------------------------------------------------------------------------------
			// Initialize the Component
			void IOSOpenALSoundMixer::InitializeComponent(Engine::GameProcessAnchor *gameAnchor)
			{
				OpenAL::OpenALSoundMixer::InitializeComponent(gameAnchor);
				InitializeInterruptionObserver();
			}

			//------------------------------------------------------------------------------
			// Safely Shutdown Component
			void IOSOpenALSoundMixer::ShutdownComponent()
			{
				FinalizeInterruptionObserver();
				OpenAL::OpenALSoundMixer::ShutdownComponent();
			}

			//------------------------------------------------------------------------------
			void IOSOpenALSoundMixer::OnPause()
			{
				bool isNeedToSuspend = false;
				
				int lockResult = pthread_mutex_lock(&m_eventCheckMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_eventCheckMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);
					
					m_isInAppBackground.store(true);
					if (!m_isInAudioSessionInterruption.load())
					{
						isNeedToSuspend = true;
					}
				}
				else
				{
					m_isInAppBackground.store(true);
					if (!m_isInAudioSessionInterruption.load())
					{
						isNeedToSuspend = true;
					}
				}
				
				if (isNeedToSuspend)
				{
					_SuspendOpenAL();
				}
			}

			//------------------------------------------------------------------------------
			void IOSOpenALSoundMixer::OnResume()
			{
				bool isNeedToRestore = false;
				
				int lockResult = pthread_mutex_lock(&m_eventCheckMutex.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &m_eventCheckMutex.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);
					
					m_isInAppBackground.store(false);
					if (!m_isInAudioSessionInterruption.load())
					{
						isNeedToRestore = true;
					}
				}
				else
				{
					m_isInAppBackground.store(false);
					if (!m_isInAudioSessionInterruption.load())
					{
						isNeedToRestore = true;
					}
				}
				
				if (isNeedToRestore)
				{
					_RestoreOpenAL();
				}
			}
        }
		
		
		#pragma mark - SoundMixerComponent
		////////////////////////////////////////////////////////////////////////////////

		//------------------------------------------------------------------------------
		// Sound Mixer Component Factory Function
		SoundMixerComponent *SoundMixerComponent::CreateComponentObject()
		{
			return new IOS::IOSOpenALSoundMixer();
		}

		
		#pragma mark - Platform Specific Settings
		////////////////////////////////////////////////////////////////////////////////

		namespace OpenAL
		{
			//------------------------------------------------------------------------------
			size_t OpenALSoundPlayingContext::g_bufferSize = 5120;

			//------------------------------------------------------------------------------
			double OpenALSoundMixer::GetMixerExpectedPlayDelayRatio()
			{
				return 0.75;
			}

			//------------------------------------------------------------------------------
			long OpenALSoundMixer::_GetUpdateThreadSleepTimeInNS() const
			{
				return 13 * 1000 * 1000;	// 13 ms
			}
		}
	}
}
