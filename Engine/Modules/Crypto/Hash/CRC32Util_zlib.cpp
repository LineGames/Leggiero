////////////////////////////////////////////////////////////////////////////////
// Hash/CRC32Util_zlib.cpp (Leggiero/Modules - Crypto)
//
// zlib based CRC32 checksum utility implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "CRC32Util.h"

// Standard Library
#include <cstdio>

// External Library
#include <zlib/zlib.h>

// Leggiero.Utility
#include <Utility/Encoding/HexString.h>


namespace Leggiero
{
	namespace Crypto
	{
		namespace Hash
		{
			//////////////////////////////////////////////////////////////////////////////// CRC32 Checksum Utility

			//------------------------------------------------------------------------------
			// Calculate CRC32 checksum of given data
			uint32_t CRC32Checksum::CalculateCRC32(const void *dataBufferToHash, size_t dataLength)
			{
				return (uint32_t)(::crc32((uLong)0Lu, (const Bytef *)dataBufferToHash, (uInt)dataLength));
			}

			//------------------------------------------------------------------------------
			// Calculate CRC32 checksum of given data
			uint32_t CRC32Checksum::CalculateCRC32(const std::string &dataToHash)
			{
				return (uint32_t)(::crc32((uLong)0Lu, (const Bytef *)dataToHash.c_str(), (uInt)dataToHash.length()));
			}

			//------------------------------------------------------------------------------
			// Calculate CRC32 checksum of a given file
			std::tuple<bool, uint32_t> CRC32Checksum::CalculateFileCRC32(const std::string &filePath)
			{
				constexpr size_t kBufferSize = 4096;

				FILE *targetFile = fopen(filePath.c_str(), "rb");
				if (targetFile == nullptr)
				{
					// File Open Failed
					return std::make_tuple(false, 0);
				}

				unsigned char buf[kBufferSize];
				size_t readBytes;

				uint32_t currentChecksum = static_cast<uint32_t>(0Lu);
				while ((readBytes = fread(buf, 1, kBufferSize, targetFile)) != 0)
				{
					currentChecksum = (uint32_t)(::crc32(currentChecksum, (const Bytef *)buf, (uInt)readBytes));
				}
				fclose(targetFile);

				return std::make_tuple(true, currentChecksum);
			}

			//------------------------------------------------------------------------------
			// Calculate binary CRC32 checksum of given data
			std::string CRC32Checksum::CalculateHash(const void *dataBufferToHash, size_t dataLength)
			{
				uint32_t resultBuffer = CalculateCRC32(dataBufferToHash, dataLength);
				return std::string((const char *)&resultBuffer, 4);
			}

			//------------------------------------------------------------------------------
			// Calculate binary CRC32 checksum of given data
			std::string CRC32Checksum::CalculateHash(const std::string &dataToHash)
			{
				uint32_t resultBuffer = CalculateCRC32(dataToHash);
				return std::string((const char *)&resultBuffer, 4);
			}

			//------------------------------------------------------------------------------
			// Get CRC32 checksum as hex string
			std::string CRC32Checksum::GetHashString(const void *dataBufferToHash, size_t dataLength, bool isCapital)
			{
				uint32_t dataBuffer = CalculateCRC32(dataBufferToHash, dataLength);
				return Utility::Encoding::HexString::GetFixedLengthHexString<4>(&dataBuffer, isCapital);
			}

			//------------------------------------------------------------------------------
			// Get CRC32 checksum as hex string
			std::string CRC32Checksum::GetHashString(const std::string &dataToHash, bool isCapital)
			{
				uint32_t dataBuffer = CalculateCRC32(dataToHash);
				return Utility::Encoding::HexString::GetFixedLengthHexString<4>(&dataBuffer, isCapital);
			}

			//------------------------------------------------------------------------------
			// Get CRC32 checksum as hex string
			std::string CRC32Checksum::GetHashString(const char *dataCStringToHash, bool isCapital)
			{
				size_t dataLength = strlen(dataCStringToHash);
				uint32_t dataBuffer = CalculateCRC32(dataCStringToHash, dataLength);
				return Utility::Encoding::HexString::GetFixedLengthHexString<4>(&dataBuffer, isCapital);
			}

			//------------------------------------------------------------------------------
			// Calculate CRC32 checksum of a given file
			std::string CRC32Checksum::CalculateFileHash(const std::string &filePath)
			{
				std::tuple<bool, uint32_t> fileChecksumResult = CalculateFileCRC32(filePath);
				if (!std::get<0>(fileChecksumResult))
				{
					return std::string();
				}

				uint32_t dataBuffer = std::get<1>(fileChecksumResult);
				return std::string((const char *)&dataBuffer, 4);
			}


			//////////////////////////////////////////////////////////////////////////////// CRC32 Summing Context

			namespace _Internal
			{
				//------------------------------------------------------------------------------
				class ConcreteCRC32Context
				{
				public:
					ConcreteCRC32Context()
						: m_currentChecksum(static_cast<uint32_t>(0Lu))
					{ }

					virtual ~ConcreteCRC32Context() { }

				public:
					void Append(const void *dataBufferToAppend, size_t dataLength)
					{
						m_currentChecksum = (uint32_t)(::crc32(m_currentChecksum, (const Bytef *)dataBufferToAppend, (uInt)dataLength));
					}

					uint32_t GetChecksum() { return m_currentChecksum; }
					void Clear() { m_currentChecksum = (uint32_t)0Lu; }

				protected:
					uint32_t m_currentChecksum;
				};
			}

			//------------------------------------------------------------------------------
			CRC32Context::CRC32Context()
				: m_concreteObject(std::make_shared<_Internal::ConcreteCRC32Context>())
			{
			}

			//------------------------------------------------------------------------------
			CRC32Context::CRC32Context(const void *initialDataBuffer, size_t dataLength)
				: m_concreteObject(std::make_shared<_Internal::ConcreteCRC32Context>())
			{
				m_concreteObject->Append(initialDataBuffer, dataLength);
			}

			//------------------------------------------------------------------------------
			CRC32Context::CRC32Context(const std::string &initialData)
				: m_concreteObject(std::make_shared<_Internal::ConcreteCRC32Context>())
			{
				m_concreteObject->Append((const void *)initialData.c_str(), initialData.size());
			}

			//------------------------------------------------------------------------------
			void CRC32Context::Append(const void *dataBufferToAppend, size_t dataLength)
			{
				m_concreteObject->Append(dataBufferToAppend, dataLength);
			}

			//------------------------------------------------------------------------------
			void CRC32Context::Append(const std::string &dataToAppend)
			{
				m_concreteObject->Append((const void *)dataToAppend.c_str(), dataToAppend.size());
			}

			//------------------------------------------------------------------------------
			std::string CRC32Context::GetHash()
			{
				uint32_t checksum = m_concreteObject->GetChecksum();
				return std::string((const char *)(&checksum), 4);
			}

			//------------------------------------------------------------------------------
			uint32_t CRC32Context::GetChecksum()
			{
				return m_concreteObject->GetChecksum();
			}

			//------------------------------------------------------------------------------
			std::string CRC32Context::GetHashString(bool isCapital)
			{
				uint32_t dataBuffer = m_concreteObject->GetChecksum();
				return Utility::Encoding::HexString::GetFixedLengthHexString<4>(&dataBuffer, isCapital);
			}

			//------------------------------------------------------------------------------
			void CRC32Context::Clear()
			{
				m_concreteObject->Clear();
			}
		}
	}
}
