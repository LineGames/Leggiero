////////////////////////////////////////////////////////////////////////////////
// ProtectedKey/NotSecureKey.h (Leggiero/Modules - Crypto)
//
// Not-secure key that stores key in memory as plain text.
// Can use for development, but not recommended to use in production.
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_CRYPTO__PROTECTED_KEY__NOT_SECURE_KEY_H
#define __LM_CRYPTO__PROTECTED_KEY__NOT_SECURE_KEY_H


// Standard Library
#include <memory>
#include <string>

// Leggiero.Crypto
#include "IProtectedKey.h"


namespace Leggiero
{
	namespace Crypto
	{
		namespace Security
		{
			// Not secure key for developement
			class NotSecureKey
				: public IProtectedKey
			{
			public:
				NotSecureKey(const char *keyData, std::size_t keySize);
				NotSecureKey(const std::string &keyData);

				NotSecureKey(const NotSecureKey &other);
				NotSecureKey(NotSecureKey &&other);

				NotSecureKey &operator=(const NotSecureKey &other);
				NotSecureKey &operator=(NotSecureKey &&other);

				virtual ~NotSecureKey();

			public:
				static NotSecureKey FromBase64String(const std::string &base64Key);
				static std::shared_ptr<NotSecureKey> NewFromBase64String(const std::string &base64Key);

			private:
				NotSecureKey() = delete;

			public:	// IProtectedKey
				// Get length of the key
				virtual size_t GetLength() const override;

				// Get key byte value at given offset
				virtual uint8_t GetAt(size_t idx) const override;

				// Copy key to given buffer
				virtual void FillKey(void *buffer, size_t length, size_t keyOffset = 0) const override;

				// Do XOR on given data using the key
				virtual void XORKey(void *buffer, size_t length, size_t keyOffset = 0) const override;

			protected:
				uint8_t *m_keyBuffer;
				size_t m_keySize;
			};
		}
	}
}

#endif
