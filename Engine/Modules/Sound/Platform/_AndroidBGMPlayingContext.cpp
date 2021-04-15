////////////////////////////////////////////////////////////////////////////////
// Platform/_AndroidBGMPlayingContext.cpp (Leggiero/Modules - Sound)
//
// Android Platform BGM Playing Context Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "_AndroidBGMPlayingContext.h"

// Standard Library
#include <cmath>

// Leggiero.Platform.Android
#include <AndroidPlatform/AndroidJNISupport.h>

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.FileSystem
#include <FileSystem/FileSystemUtility.h>
#include <FileSystem/BundleFileResourceComponent.h>


namespace Leggiero
{
	namespace Sound
	{
		namespace Android
		{
			namespace _Internal
			{
				//////////////////////////////////////////////////////////////////////////////// AndroidBGMPlayingContext

				//------------------------------------------------------------------------------
				AndroidBGMPlayingContext::AndroidBGMPlayingContext(jobject javaPlayingContext)
					: m_javaPlayingContext(nullptr), m_javaContextClass(nullptr)
					, m_lastVolume(1.0f)
				{
					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return;
					}

					jclass javaContextClass = pEnv->GetObjectClass(javaPlayingContext);
					if (!javaContextClass)
					{
						return;
					}

					m_javaContextClass = reinterpret_cast<jclass>(pEnv->NewGlobalRef(javaContextClass));
					m_javaPlayingContext = pEnv->NewGlobalRef(javaPlayingContext);
				}

				//------------------------------------------------------------------------------
				AndroidBGMPlayingContext::~AndroidBGMPlayingContext()
				{
					if (m_javaPlayingContext != nullptr)
					{
						JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
						if (pEnv != nullptr)
						{
							jmethodID releaseMethod = pEnv->GetMethodID(m_javaContextClass, "ReleaseResource", "()V");
							if (releaseMethod)
							{
								pEnv->CallVoidMethod(m_javaPlayingContext, releaseMethod);
							}

							pEnv->DeleteGlobalRef(m_javaContextClass);
							pEnv->DeleteGlobalRef(m_javaPlayingContext);
						}
						m_javaPlayingContext = nullptr;
					}
				}

				//------------------------------------------------------------------------------
				void AndroidBGMPlayingContext::Prepare()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "DoPrepare", "()V");
					if (!callingMethod)
					{
						// Failed to Find Method
						return;
					}

					// Do Prepare
					pEnv->CallVoidMethod(m_javaPlayingContext, callingMethod);
				}

				//------------------------------------------------------------------------------
				void AndroidBGMPlayingContext::Play()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "DoPlay", "()V");
					if (!callingMethod)
					{
						// Failed to Find Method
						return;
					}

					// Do Play
					pEnv->CallVoidMethod(m_javaPlayingContext, callingMethod);
				}

				//------------------------------------------------------------------------------
				void AndroidBGMPlayingContext::Stop()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "DoStop", "()V");
					if (!callingMethod)
					{
						// Failed to Find Method
						return;
					}

					// Do Stop
					pEnv->CallVoidMethod(m_javaPlayingContext, callingMethod);
				}

				//------------------------------------------------------------------------------
				void AndroidBGMPlayingContext::Pause()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "DoPause", "()V");
					if (!callingMethod)
					{
						// Failed to Find Method
						return;
					}

					// Do Pause
					pEnv->CallVoidMethod(m_javaPlayingContext, callingMethod);
				}

				//------------------------------------------------------------------------------
				bool AndroidBGMPlayingContext::IsPlaying()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return false;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "IsPlaying", "()I");
					if (!callingMethod)
					{
						// Failed to Find Method
						return false;
					}

					// Check is playing
					int resValue = (int)pEnv->CallIntMethod(m_javaPlayingContext, callingMethod);

					return (resValue == 1);
				}
				
				//------------------------------------------------------------------------------
				float AndroidBGMPlayingContext::GetLength()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return -1.0f;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "GetLengthInMS", "()I");
					if (!callingMethod)
					{
						// Failed to Find Method
						return -1.0f;
					}

					// Get duration value
					int resValue = (int)pEnv->CallIntMethod(m_javaPlayingContext, callingMethod);
					if (resValue < 0)
					{
						return -1.0f;
					}

					return (resValue * 0.001f);
				}
				
				//------------------------------------------------------------------------------
				float AndroidBGMPlayingContext::GetCurrentPosition()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return 0.0f;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "GetCurrentPositionInMS", "()I");
					if (!callingMethod)
					{
						// Failed to Find Method
						return 0.0f;
					}

					// Get current position value
					int resValue = (int)pEnv->CallIntMethod(m_javaPlayingContext, callingMethod);

					return (resValue * 0.001f);
				}
				
				//------------------------------------------------------------------------------
				bool AndroidBGMPlayingContext::Seek(float position)
				{
					int positionInMS = static_cast<int>(position * 1000.0f);

					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return false;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "DoSeek", "(I)V");
					if (!callingMethod)
					{
						// Failed to Find Method
						return false;
					}

					// Do Seek
					pEnv->CallVoidMethod(m_javaPlayingContext, callingMethod, (jint)positionInMS);

					return true;
				}

				//------------------------------------------------------------------------------
				bool AndroidBGMPlayingContext::IsLooping()
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return false;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "GetIsLooping", "()I");
					if (!callingMethod)
					{
						// Failed to Find Method
						return false;
					}

					// Check is looping
					int resValue = (int)pEnv->CallIntMethod(m_javaPlayingContext, callingMethod);

					return (resValue == 1);
				}
				
				//------------------------------------------------------------------------------
				void AndroidBGMPlayingContext::SetLooping(bool isLooping)
				{
					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "DoSetLooping", "(I)V");
					if (!callingMethod)
					{
						// Failed to Find Method
						return;
					}

					// Set Is Looping
					pEnv->CallVoidMethod(m_javaPlayingContext, callingMethod, (jint)(isLooping ? 1 : 0));
				}

				//------------------------------------------------------------------------------
				float AndroidBGMPlayingContext::GetVolume()
				{
					return m_lastVolume;
				}
				
				//------------------------------------------------------------------------------
				void AndroidBGMPlayingContext::SetVolume(float volume)
				{
					if (!isfinite(volume))
					{
						volume = 0.0f;
					}
					if (volume < 0.0f)
					{
						volume = 0.0f;
					}
					else if (volume > 1.0f)
					{
						volume = 1.0f;
					}

					auto lockContext = m_contextLock.Lock();

					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return;
					}

					jmethodID callingMethod = pEnv->GetMethodID(m_javaContextClass, "DoSetVolume", "(F)V");
					if (!callingMethod)
					{
						// Failed to Find Method
						return;
					}

					m_lastVolume = volume;

					// Set Volume
					pEnv->CallVoidMethod(m_javaPlayingContext, callingMethod, (jfloat)volume);
				}
				
				//------------------------------------------------------------------------------
				void AndroidBGMPlayingContext::NotifyFinishPlayingEvent(bool successfully)
				{
					_OnPlayFinish(successfully);
				}


				//////////////////////////////////////////////////////////////////////////////// AndroidBGMPlayingContext - Factory

				//------------------------------------------------------------------------------
				std::shared_ptr<AndroidBGMPlayingContext> AndroidBGMPlayingContext::CreateContextWithFileResource(const std::string &filePath, int contextId)
				{
					// Call Java Method to Create Context
					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return nullptr;
					}

					jclass bgmHelperClass = Leggiero::Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/Sound/BGMHelper");
					if (!bgmHelperClass)
					{
						return nullptr;
					}

					jmethodID creatingMethod = pEnv->GetStaticMethodID(bgmHelperClass, "CreatePlayingContextFromFile", "(Ljava/lang/String;I)Lcom/leggiero/Engine/Modules/Sound/BGMPlayingContext;");
					if (!creatingMethod)
					{
						pEnv->DeleteLocalRef(bgmHelperClass);
						return nullptr;
					}

					jstring filePathString = pEnv->NewStringUTF(filePath.c_str());

					jobject javaPlayingContext = pEnv->CallStaticObjectMethod(bgmHelperClass, creatingMethod, filePathString, (jint)contextId);

					pEnv->DeleteLocalRef(filePathString);
					pEnv->DeleteLocalRef(bgmHelperClass);

					if (!javaPlayingContext)
					{
						// Java Context Creation Failed
						return nullptr;
					}

					// Create C++ Context
					std::shared_ptr<AndroidBGMPlayingContext> createdContext(std::make_shared<AndroidBGMPlayingContext>(javaPlayingContext));
					if (createdContext->m_javaPlayingContext == nullptr)
					{
						// Something Wrong
						return nullptr;
					}

					return createdContext;
				}

				//------------------------------------------------------------------------------
				std::shared_ptr<AndroidBGMPlayingContext> AndroidBGMPlayingContext::CreateContextWithBundleResource(const std::string &virtualPath, int contextId)
				{
					// Call Java Method to Create Context
					JNIEnv *pEnv = Leggiero::Platform::Android::GetJNIEnv();
					if (pEnv == nullptr)
					{
						return nullptr;
					}

					jclass bgmHelperClass = Leggiero::Platform::Android::FindJavaClass(pEnv, "com/leggiero/Engine/Modules/Sound/BGMHelper");
					if (!bgmHelperClass)
					{
						return nullptr;
					}

					jmethodID creatingMethod = pEnv->GetStaticMethodID(bgmHelperClass, "CreatePlayingContextInBundle", "(Ljava/lang/String;I)Lcom/leggiero/Engine/Modules/Sound/BGMPlayingContext;");
					if (!creatingMethod)
					{
						pEnv->DeleteLocalRef(bgmHelperClass);
						return nullptr;
					}

					jstring virtualPathString = pEnv->NewStringUTF(virtualPath.c_str());

					jobject javaPlayingContext = pEnv->CallStaticObjectMethod(bgmHelperClass, creatingMethod, virtualPathString, (jint)contextId);

					pEnv->DeleteLocalRef(virtualPathString);
					pEnv->DeleteLocalRef(bgmHelperClass);

					if (!javaPlayingContext)
					{
						// Java Context Creation Failed
						return nullptr;
					}

					// Create C++ Context
					std::shared_ptr<AndroidBGMPlayingContext> createdContext(std::make_shared<AndroidBGMPlayingContext>(javaPlayingContext));
					if (createdContext->m_javaPlayingContext == nullptr)
					{
						// Something Wrong
						return nullptr;
					}

					return createdContext;
				}
			}
		}
	}
}
