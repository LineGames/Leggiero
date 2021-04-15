////////////////////////////////////////////////////////////////////////////////
// ProtectedKey/NotSecureKey.cpp (Leggiero/Modules - Crypto)
//
// Not secure key implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "NotSecureKey.h"

// Standard Library
#include <cstdlib>

// Leggiero.Utility
#include <Utility/Encoding/Base64.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Security
		{
			//////////////////////////////////////////////////////////////////////////////// NotSecureKey

			//------------------------------------------------------------------------------
			NotSecureKey::NotSecureKey(const char *keyData, std::size_t keySize)
				: m_keySize(keySize)
			{
				m_keyBuffer = (uint8_t *)malloc(keySize);
				memcpy(m_keyBuffer, keyData, keySize);
			}

			//------------------------------------------------------------------------------
			NotSecureKey::NotSecureKey(const std::string &keyData)
				: m_keySize(keyData.length())
			{
				m_keyBuffer = (uint8_t *)malloc(m_keySize);
				memcpy(m_keyBuffer, keyData.c_str(), m_keySize);
			}

			//------------------------------------------------------------------------------
			NotSecureKey::NotSecureKey(const NotSecureKey &other)
				: m_keySize(other.m_keySize)
			{
				m_keyBuffer = (uint8_t *)malloc(m_keySize);
				memcpy(m_keyBuffer, other.m_keyBuffer, m_keySize);
			}

			//------------------------------------------------------------------------------
			NotSecureKey::NotSecureKey(NotSecureKey &&other)
				: m_keySize(other.m_keySize)
				, m_keyBuffer(other.m_keyBuffer)
			{
				other.m_keyBuffer = nullptr;
				other.m_keySize = 0;
			}

			//------------------------------------------------------------------------------
			NotSecureKey &NotSecureKey::operator=(const NotSecureKey &other)
			{
				if (m_keyBuffer == nullptr)
				{
					m_keySize = other.m_keySize;
					m_keyBuffer = (uint8_t *)malloc(m_keySize);
				}
				else
				{
					if (m_keySize != other.m_keySize)
					{
						m_keySize = other.m_keySize;
						free(m_keyBuffer);
						m_keyBuffer = (uint8_t *)malloc(m_keySize);
					}
				}

				memcpy(m_keyBuffer, other.m_keyBuffer, m_keySize);

				return *this;
			}

			//------------------------------------------------------------------------------
			NotSecureKey &NotSecureKey::operator=(NotSecureKey &&other)
			{
				if (m_keyBuffer != nullptr)
				{
					free(m_keyBuffer);
				}

				m_keySize = other.m_keySize;
				m_keyBuffer = other.m_keyBuffer;

				other.m_keyBuffer = nullptr;
				other.m_keySize = 0;

				return *this;
			}

			//------------------------------------------------------------------------------
			NotSecureKey::~NotSecureKey()
			{
				if (m_keyBuffer != nullptr)
				{
					free(m_keyBuffer);
				}
				m_keyBuffer = nullptr;
			}

			//------------------------------------------------------------------------------
			NotSecureKey NotSecureKey::FromBase64String(const std::string &base64Key)
			{
				return NotSecureKey(Utility::Encoding::Base64::Decode(base64Key));
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<NotSecureKey> NotSecureKey::NewFromBase64String(const std::string &base64Key)
			{
				return std::make_shared<NotSecureKey>(Utility::Encoding::Base64::Decode(base64Key));
			}

			//------------------------------------------------------------------------------
			// Get length of the key
			size_t NotSecureKey::GetLength() const
			{
				return m_keySize;
			}

			//------------------------------------------------------------------------------
			// Get key byte value at given offset
			uint8_t NotSecureKey::GetAt(size_t idx) const
			{
				if (idx < m_keySize)
				{
					return m_keyBuffer[idx];
				}
				if (m_keySize == 0)
				{
					return 0;
				}
				else if (m_keySize == 1)
				{
					return m_keyBuffer[0];
				}
				return m_keyBuffer[idx % m_keySize];
			}

			//------------------------------------------------------------------------------
			// Copy key to given buffer
			void NotSecureKey::FillKey(void *buffer, size_t length, size_t keyOffset) const
			{
				if (keyOffset + length <= m_keySize)
				{
					memcpy(buffer, m_keyBuffer + keyOffset, length);
					return;
				}

				uint8_t *byteBuffer = (uint8_t *)buffer;
				if (m_keySize == 0)
				{
					for (std::size_t i = 0; i < length; ++i)
					{
						byteBuffer[i] = 0;
					}
				}
				else if (m_keySize == 1)
				{
					uint8_t maskByte = m_keyBuffer[0];
					for (std::size_t i = 0; i < length; ++i)
					{
						byteBuffer[i] = maskByte;
					}
				}
				for (std::size_t i = 0; i < length; ++i)
				{
					byteBuffer[i] = m_keyBuffer[(keyOffset + i) % m_keySize];
				}
			}

			//------------------------------------------------------------------------------
			// Do XOR on given data using the key
			void NotSecureKey::XORKey(void *buffer, size_t length, size_t keyOffset) const
			{
				uint8_t *byteBuffer = (uint8_t *)buffer;
				if (m_keySize == 0)
				{
					// No key to XOR
					return;
				}
				else if (m_keySize == 1)
				{
					uint8_t maskByte = m_keyBuffer[0];
					for (std::size_t i = 0; i < length; ++i)
					{
						byteBuffer[i] ^= maskByte;
					}
				}
				for (std::size_t i = 0; i < length; ++i)
				{
					byteBuffer[i] ^= m_keyBuffer[(keyOffset + i) % m_keySize];
				}
			}
		}
	}
}
