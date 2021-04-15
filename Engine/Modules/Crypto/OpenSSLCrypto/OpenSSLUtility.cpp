////////////////////////////////////////////////////////////////////////////////
// OpenSSLCrypto/OpenSSLUtility.cpp (Leggiero/Modules - Crypto)
//
// OpenSSL utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "OpenSSLUtility.h"

// Leggiero.Basic
#include <Basic/LeggieroBasic.h>

// External Library
#include <pthread.h>
#include <openssl/crypto.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace OpenSSL
		{
			namespace _Internal
			{
				//////////////////////////////////////////////////////////////////////////////// OpenSSL Thread Process
				// From https://curl.haxx.se/libcurl/c/threaded-ssl.html

				//------------------------------------------------------------------------------
				static pthread_rwlock_t *lockarray;

				//------------------------------------------------------------------------------
				static void lock_callback(int mode, int type, char *file, int line)
				{
					(void)file;
					(void)line;
					if (mode & CRYPTO_READ) {
						if (mode & CRYPTO_LOCK) {
							pthread_rwlock_rdlock(&(lockarray[type]));
						}
						else {
							pthread_rwlock_unlock(&(lockarray[type]));
						}
					}
					else { /* CRYPTO_WRITE */
						if (mode & CRYPTO_LOCK) {
							pthread_rwlock_wrlock(&(lockarray[type]));
						}
						else {
							pthread_rwlock_unlock(&(lockarray[type]));
						}
					}
				}

				//------------------------------------------------------------------------------
				static unsigned long thread_id(void)
				{
					unsigned long ret;

					#if defined _LEGGIERO_ANDROID
						ret = (unsigned long)(pthread_self());
					#elif defined _LEGGIERO_WINPC
						ret = 0;
					#else
						#error The platform does not support OpenSSL in Leggeiro Engine
					#endif
					return ret;
				}

				//------------------------------------------------------------------------------
				static void init_locks(void)
				{
					int i;

					lockarray = (pthread_rwlock_t *)OPENSSL_malloc(CRYPTO_num_locks() *
						sizeof(pthread_rwlock_t));
					for (i = 0; i < CRYPTO_num_locks(); i++) {
						pthread_rwlock_init(&(lockarray[i]), NULL);
					}

					#if defined _LEGGIERO_ANDROID
						CRYPTO_set_id_callback((unsigned long(*)())thread_id);
						CRYPTO_set_locking_callback((void(*)(int, int, const char *, int))lock_callback);
					#elif defined _LEGGIERO_WINPC
						// Don't use callback
					#else
						#error The platform does not support OpenSSL in Leggeiro Engine
					#endif
				}

				//------------------------------------------------------------------------------
				static void kill_locks(void)
				{
					int i;

					CRYPTO_set_locking_callback(NULL);
					for (i = 0; i < CRYPTO_num_locks(); i++)
						pthread_rwlock_destroy(&(lockarray[i]));

					OPENSSL_free(lockarray);
				}
			}

			//////////////////////////////////////////////////////////////////////////////// Module Global Utility

			//------------------------------------------------------------------------------
			void InitializeOpenSSLCryptoSystem()
			{
				_Internal::init_locks();
			}

			//------------------------------------------------------------------------------
			void FinalizeOpenSSLCryptoSystem()
			{
				_Internal::kill_locks();
			}
		}
	}
}
