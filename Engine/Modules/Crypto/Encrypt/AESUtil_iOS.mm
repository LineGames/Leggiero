////////////////////////////////////////////////////////////////////////////////
// Encrypt/AESUtil_iOS.mm (Leggiero/Modules - Crypto)
//
// CommonCrypto based iOS AES encryption utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "AESUtil.h"

// Standard Library
#include <cstdio>
#include <vector>

// External Library
#import <CommonCrypto/CommonCryptor.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Encrypt
		{
			//////////////////////////////////////////////////////////////////////////////// AES256 ECB Mode

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Encrypt(const void *dataBuffer, size_t dataLength, const std::string &key)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t encryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding | kCCOptionECBMode,
					key.c_str(), kCCKeySizeAES256,
					nullptr, 
					dataBuffer, dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&encryptedSize);
					
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}

				return std::string((const char *)&resultBuffer[0], encryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Encrypt(const std::string &data, const std::string &key)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t encryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding | kCCOptionECBMode,
					key.c_str(), kCCKeySizeAES256,
					nullptr, 
					data.c_str(), dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&encryptedSize);
					
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}

				return std::string((const char *)&resultBuffer[0], encryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::ECB::EncryptInPlace(std::string &data, const std::string &key)
			{
				// Dummy Implementation
				data = Encrypt(data, key);
				return data.length();
			}

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Decrypt(const void *dataBuffer, size_t dataLength, const std::string &key)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t decryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding | kCCOptionECBMode,
					key.c_str(), kCCKeySizeAES256,
					nullptr, 
					dataBuffer, dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&decryptedSize);
					
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}

				return std::string((const char *)&resultBuffer[0], decryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::ECB::Decrypt(const std::string &data, const std::string &key)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t decryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding | kCCOptionECBMode,
					key.c_str(), kCCKeySizeAES256,
					nullptr, 
					data.c_str(), dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&decryptedSize);
				
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}

				return std::string((const char *)&resultBuffer[0], decryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::ECB::DecryptInPlace(void *dataBuffer, size_t dataLength, const std::string &key)
			{
				// Dummy Implementation
				std::string decryptedData(Decrypt(dataBuffer, dataLength, key));
				if (decryptedData.length() == 0)
				{
					return 0;
				}
				memcpy(dataBuffer, decryptedData.c_str(), decryptedData.length());
				return decryptedData.length();
			}

			//------------------------------------------------------------------------------
			size_t AES256::ECB::DecryptInPlace(std::string &data, const std::string &key)
			{
				// Dummy Implementation
				data = Decrypt(data, key);
				return data.length();
			}


			//////////////////////////////////////////////////////////////////////////////// AES256 CBC Mode

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Encrypt(const void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t encryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding,
					key.c_str(), kCCKeySizeAES256,
					iv.c_str(), 
					dataBuffer, dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&encryptedSize);
					
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}

				return std::string((const char *)&resultBuffer[0], encryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Encrypt(const std::string &data, const std::string &key, const std::string &iv)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t encryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCEncrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding,
					key.c_str(), kCCKeySizeAES256,
					iv.c_str(), 
					data.c_str(), dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&encryptedSize);
					
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}

				return std::string((const char *)&resultBuffer[0], encryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::CBC::EncryptInPlace(std::string &data, const std::string &key, const std::string &iv)
			{
				// Dummy Implementation
				data = Encrypt(data, key, iv);
				return data.length();
			}

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Decrypt(const void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv)
			{
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t decryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding,
					key.c_str(), kCCKeySizeAES256,
					iv.c_str(), 
					dataBuffer, dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&decryptedSize);
					
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}

				return std::string((const char *)&resultBuffer[0], decryptedSize);
			}

			//------------------------------------------------------------------------------
			std::string AES256::CBC::Decrypt(const std::string &data, const std::string &key, const std::string &iv)
			{
				size_t dataLength = data.length();
				size_t maximumDataSize = dataLength + 16 - (dataLength % 16);

				std::vector<unsigned char> resultBuffer;
				resultBuffer.resize(maximumDataSize);

				size_t decryptedSize = 0;
				CCCryptorStatus cryptStatus = CCCrypt(kCCDecrypt, 
					kCCAlgorithmAES128, 
					kCCOptionPKCS7Padding,
					key.c_str(), kCCKeySizeAES256,
					iv.c_str(), 
					data.c_str(), dataLength, 
					&resultBuffer[0], maximumDataSize, 
					&decryptedSize);
				
				if (cryptStatus != kCCSuccess)
				{
					// Error occured
					return "";
				}
				
				return std::string((const char *)&resultBuffer[0], decryptedSize);
			}

			//------------------------------------------------------------------------------
			size_t AES256::CBC::DecryptInPlace(void *dataBuffer, size_t dataLength, const std::string &key, const std::string &iv)
			{
				if (dataLength % 16 != 0)
				{
					// Invalid Size of Data
					return 0;
				}

				// Dummy Implementation
				std::string decryptedData(Decrypt(dataBuffer, dataLength, key, iv));
				if (decryptedData.length() == 0)
				{
					return 0;
				}
				memcpy(dataBuffer, decryptedData.c_str(), decryptedData.length());
				return decryptedData.length();
			}

			//------------------------------------------------------------------------------
			size_t AES256::CBC::DecryptInPlace(std::string &data, const std::string &key, const std::string &iv)
			{
				size_t dataLength = data.length();
				if (dataLength % 16 != 0)
				{
					// Invalid Size of Data
					return 0;
				}

				// Dummy Implementation
				data = Decrypt(data, key, iv);
				return data.length();
			}
		}
	}
}
