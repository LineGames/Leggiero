////////////////////////////////////////////////////////////////////////////////
// FileSystemUtility.cpp (Leggiero/Modules - FileSystem)
//
// Implementation of File System Utilities
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FileSystemUtility.h"

// Standard Library
#include <vector>
#include <unordered_set>

// Leggiero.Utility
#include <Utility/String/AsciiStringUtility.h>


namespace Leggiero
{
	namespace FileSystem
	{
		namespace Utility
		{
			//////////////////////////////////////////////////////////////////////////////// Internal Utility

			namespace _Internal
			{
				static std::unordered_set<char> kDelimeterMixed{ '/', '\\' };

				bool IsMixedDelimeter(char testChar) { return kDelimeterMixed.find(testChar) != kDelimeterMixed.end(); }

				static const std::string kSpecialPath_CurrentDir(".");
				static const std::string kSpecialPath_ParentDir("..");

				static const std::string kDelimiterString(1, kPathDelimiter);
			}


			//------------------------------------------------------------------------------
			std::string FilterDelimiter(const std::string &path, bool makeEndWithDelimeter)
			{
				std::vector<char> buffer;
				buffer.reserve(path.length() + 2);

				bool isLastDelimeter = false;
				for (int i = 0; i < path.length(); ++i)
				{
					char currentChar = path[i];
					if (currentChar == '\0')
					{
						break;
					}

					if (_Internal::IsMixedDelimeter(currentChar))
					{
						if (isLastDelimeter)
						{
							// Ignore successive delimeters
							continue;
						}
						isLastDelimeter = true;

						if (currentChar != kPathDelimiter)
						{
							currentChar = kPathDelimiter;
						}
					}
					else
					{
						isLastDelimeter = false;
					}

					buffer.push_back(currentChar);
				}

				if (makeEndWithDelimeter && !isLastDelimeter)
				{
					buffer.push_back(kPathDelimiter);
				}

				buffer.push_back('\0');
				return std::string(&buffer[0]);
			}

			//------------------------------------------------------------------------------
			std::string TrimTopAbsoluteDelimiter(const std::string &path)
			{
				const size_t sourceLength  = path.length();
				size_t startOffset = 0;
				for (size_t i = 0; i < sourceLength; ++i)
				{
					if (_Internal::IsMixedDelimeter(path[i]))
					{
						++startOffset;
					}
					else
					{
						break;
					}
				}
				return path.substr(startOffset);
			}

			//------------------------------------------------------------------------------
			// Combine two parts as a path
			std::string CombinePath(const std::string &path1, const std::string &path2)
			{
				std::vector<char> buffer;
				buffer.reserve(path1.length() + path2.length() + 2);

				bool isLastDelimeter = false;
				for (int i = 0; i < path1.length(); ++i)
				{
					char currentChar = path1[i];
					if (currentChar == '\0')
					{
						break;
					}

					if (_Internal::IsMixedDelimeter(currentChar))
					{
						if (isLastDelimeter)
						{
							// Ignore successive delimeters
							continue;
						}
						isLastDelimeter = true;

						if (currentChar != kPathDelimiter)
						{
							currentChar = kPathDelimiter;
						}
					}
					else
					{
						isLastDelimeter = false;
					}

					buffer.push_back(currentChar);
				}
				if (!isLastDelimeter && !buffer.empty())
				{
					buffer.push_back(kPathDelimiter);
					isLastDelimeter = true;
				}

				for (int i = 0; i < path2.length(); ++i)
				{
					char currentChar = path2[i];
					if (currentChar == '\0')
					{
						break;
					}

					if (_Internal::IsMixedDelimeter(currentChar))
					{
						if (isLastDelimeter)
						{
							// Ignore successive delimeters
							continue;
						}
						isLastDelimeter = true;

						if (currentChar != kPathDelimiter)
						{
							currentChar = kPathDelimiter;
						}
					}
					else
					{
						isLastDelimeter = false;
					}

					buffer.push_back(currentChar);
				}

				buffer.push_back('\0');
				return std::string(&buffer[0]);
			}

			//------------------------------------------------------------------------------
			std::string CombinePathWithResolvingParent(const std::string &path1, const std::string &path2, bool isGoBeyondTop)
			{
				std::string combinedPath(CombinePath(path1, path2));
				if (combinedPath.empty() || ((combinedPath.length() == 1) && (combinedPath[0] == kPathDelimiter)))
				{
					return combinedPath;
				}

				bool isStartWithAbsoluteDelimiter = (combinedPath[0] == kPathDelimiter);
				if (isStartWithAbsoluteDelimiter)
				{
					isGoBeyondTop = false;
				}

				std::vector<std::string> pathTokens = Leggiero::Utility::String::ASCIIStringUtility::Tokenize(combinedPath, { kPathDelimiter, }, true);
				
				std::vector<std::string> pathStack;
				pathStack.reserve(pathTokens.size());
				for (const std::string &currentToken : pathTokens)
				{
					if (currentToken == _Internal::kSpecialPath_CurrentDir)
					{
						// Ignore Current Dir
						continue;
					}
					else if (currentToken == _Internal::kSpecialPath_ParentDir)
					{
						if (pathStack.empty())
						{
							if (isGoBeyondTop)
							{
								pathStack.push_back(currentToken);
							}
						}
						else
						{
							if (pathStack.back() != _Internal::kSpecialPath_ParentDir)
							{
								pathStack.pop_back();
							}
							else
							{
								// Only case on isGoBeyondTop is true
								pathStack.push_back(currentToken);
							}
						}
					}
					else
					{
						pathStack.push_back(currentToken);
					}
				}

				bool isEndWithDelimiter = ((combinedPath.length()) > 1 && (combinedPath[combinedPath.length() - 1] == kPathDelimiter));

				if (pathStack.empty())
				{
					if (isStartWithAbsoluteDelimiter)
					{
						return _Internal::kDelimiterString;
					}
					return std::string("");
				}

				return Leggiero::Utility::String::ASCIIStringUtility::Join(pathStack, _Internal::kDelimiterString, isEndWithDelimiter, isStartWithAbsoluteDelimiter);
			}

			//------------------------------------------------------------------------------
			std::string GetExtension(const std::string &path)
			{
				// Find the last dot
				size_t lastDotPos = path.find_last_of('.');
				if (lastDotPos == std::string::npos)
				{
					// No dot
					return "";
				}

				// Find last delimiter
				size_t lastDelimiterPos = path.find_last_of('/');
				if (lastDelimiterPos == std::string::npos)
				{
					lastDelimiterPos = path.find_last_of('\\');
				}
				else
				{
					size_t anotherDelimiterPos = path.find_last_of('\\');
					if (anotherDelimiterPos != std::string::npos)
					{
						if (anotherDelimiterPos > lastDelimiterPos)
						{
							lastDelimiterPos = anotherDelimiterPos;
						}
					}
				}

				// Check dot position
				if (lastDelimiterPos != std::string::npos)
				{
					if (lastDotPos < lastDelimiterPos)
					{
						// No dot in file name
						return "";
					}
				}

				// Split
				return path.substr(lastDotPos + 1);
			}

			//------------------------------------------------------------------------------
			std::string GetParent(const std::string &path)
			{
				std::string filteredPath = FilterDelimiter(path, false);

				// Find last delimiter
				size_t lastDelimiterPos = path.find_last_of('/');
				if (lastDelimiterPos == std::string::npos)
				{
					lastDelimiterPos = path.find_last_of('\\');
				}
				else
				{
					size_t anotherDelimiterPos = path.find_last_of('\\');
					if (anotherDelimiterPos != std::string::npos)
					{
						if (anotherDelimiterPos > lastDelimiterPos)
						{
							lastDelimiterPos = anotherDelimiterPos;
						}
					}
				}

				if (lastDelimiterPos == std::string::npos)
				{
					// No Parent
					if (filteredPath.empty())
					{
						return path;
					}
					// root
					if (path[0] == '/')
					{
						return "/";
					}
					else if (path[0] == '\\')
					{
						return "\\";
					}
					return "";
				}

				return filteredPath.substr(0, lastDelimiterPos);
			}

			//------------------------------------------------------------------------------
			std::string GetName(const std::string &path, bool isIncludeExtension)
			{
				std::string filteredPath = FilterDelimiter(path, false);

				// Find last delimiter
				size_t lastDelimiterPos = path.find_last_of('/');
				if (lastDelimiterPos == std::string::npos)
				{
					lastDelimiterPos = path.find_last_of('\\');
				}
				else
				{
					size_t anotherDelimiterPos = path.find_last_of('\\');
					if (anotherDelimiterPos != std::string::npos)
					{
						if (anotherDelimiterPos > lastDelimiterPos)
						{
							lastDelimiterPos = anotherDelimiterPos;
						}
					}
				}

				if (lastDelimiterPos != std::string::npos)
				{
					filteredPath = filteredPath.substr(lastDelimiterPos);
				}
				if (filteredPath.empty())
				{
					// Top-level directory
					return filteredPath;
				}

				if (!isIncludeExtension)
				{
					size_t lastDotPos = filteredPath.find_last_of('.');
					if (lastDotPos == std::string::npos)
					{
						// No dot
						return filteredPath;
					}
					return filteredPath.substr(0, lastDotPos);
				}

				return filteredPath;
			}
		}
	}
}
