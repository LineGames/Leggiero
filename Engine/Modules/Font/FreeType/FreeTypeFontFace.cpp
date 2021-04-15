////////////////////////////////////////////////////////////////////////////////
// FreeType/FreeTypeFontFace.cpp (Leggiero/Modules - Font)
//
// FreeType based Font Face Implementation
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "FreeTypeFontFace.h"

// Standard Library
#include <cstdio>

// External Library
#include FT_GLYPH_H
#include FT_STROKER_H

// Leggiero.Utility
#include <Utility/Sugar/Finally.h>

// Leggiero.Graphics
#include <Graphics/Common/GraphicsReferenceState.h>

// Leggiero.Font
#include "FreeTypeFontCommon.h"
#include "FreeTypeNormalGlyphDrawer.h"


namespace Leggiero
{
	namespace Font
	{
		namespace FreeType
		{
			//////////////////////////////////////////////////////////////////////////////// Creator Functions

			//------------------------------------------------------------------------------
			std::shared_ptr<FontFace> CreateFontFaceFromFile(std::string filePath, FT_Long faceIndex)
			{
				FT_Face loadedFace;
				FT_Error loadError;

				int lockResult = pthread_mutex_lock(&_Internal::g_libraryFaceGenerateLock.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &_Internal::g_libraryFaceGenerateLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					loadError = FT_New_Face(_Internal::g_libraryInstance, filePath.c_str(), faceIndex, &loadedFace);
				}
				else
				{
					// Something Wrong
					return nullptr;
				}

				if (loadError != FT_Err_Ok)
				{
					// Font Load Error
					return nullptr;
				}
				return std::shared_ptr<FontFace>(new FontFace(loadedFace, []() {}, [filePath, faceIndex](FT_Face &faceHandleBuffer) {
					int freetypeLockResult = pthread_mutex_lock(&_Internal::g_libraryFaceGenerateLock.GetLock());
					if (freetypeLockResult == 0)
					{
						pthread_mutex_t *lockCopy = &_Internal::g_libraryFaceGenerateLock.GetLock();
						auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						return FT_New_Face(_Internal::g_libraryInstance, filePath.c_str(), faceIndex, &faceHandleBuffer);
					}
					else
					{
						return (FT_Error)FT_Err_Cannot_Open_Resource;
					}
				}));
			}

			//------------------------------------------------------------------------------
			std::shared_ptr<FontFace> CreateFontFaceFromMemory(const char *buffer, size_t size, FT_Long faceIndex)
			{
				char *copiedBuffer = (char *)malloc(size);
				if (copiedBuffer == nullptr)
				{
					// Out of Memory
					return nullptr;
				}
				memcpy(copiedBuffer, buffer, size);

				FT_Face loadedFace;
				FT_Error loadError;

				int lockResult = pthread_mutex_lock(&_Internal::g_libraryFaceGenerateLock.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &_Internal::g_libraryFaceGenerateLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					loadError = FT_New_Memory_Face(_Internal::g_libraryInstance, (FT_Byte *)copiedBuffer, (FT_Long)size, faceIndex, &loadedFace);
				}
				else
				{
					// Something Wrong
					free(copiedBuffer);
					return nullptr;
				}

				if (loadError != FT_Err_Ok)
				{
					// Font Load Error
					free(copiedBuffer);
					return nullptr;
				}
				return std::shared_ptr<FontFace>(new FontFace(loadedFace, [copiedBuffer]() { free(copiedBuffer); }, [copiedBuffer, size, faceIndex](FT_Face &faceHandleBuffer) { 
					int freetypeLockResult = pthread_mutex_lock(&_Internal::g_libraryFaceGenerateLock.GetLock());
					if (freetypeLockResult == 0)
					{
						pthread_mutex_t *lockCopy = &_Internal::g_libraryFaceGenerateLock.GetLock();
						auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						return FT_New_Memory_Face(_Internal::g_libraryInstance, (FT_Byte *)copiedBuffer, (FT_Long)size, faceIndex, &faceHandleBuffer);
					}
					else
					{
						return (FT_Error)FT_Err_Cannot_Open_Resource;
					}
				}));
			}

			//------------------------------------------------------------------------------
			// Load font face from the given memory stream data
			// if size is 0, use the entire stream (stream must be seekable)
			std::shared_ptr<FontFace> CreateFontFaceFromMemory(std::istream &dataStream, std::streamsize size, FT_Long faceIndex)
			{
				if (size == 0)
				{
					dataStream.seekg(0, std::ios::end);
                    size = (std::streamsize)dataStream.tellg();
					dataStream.seekg(0, std::ios::beg);
				}

				char *copiedBuffer = (char *)malloc(size);
				if (copiedBuffer == nullptr)
				{
					// Out of Memory
					return nullptr;
				}
				dataStream.read(copiedBuffer, size);

				FT_Face loadedFace;
				FT_Error loadError;

				int lockResult = pthread_mutex_lock(&_Internal::g_libraryFaceGenerateLock.GetLock());
				if (lockResult == 0)
				{
					pthread_mutex_t *lockCopy = &_Internal::g_libraryFaceGenerateLock.GetLock();
					auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
					FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

					loadError = FT_New_Memory_Face(_Internal::g_libraryInstance, (FT_Byte *)copiedBuffer, (FT_Long)size, faceIndex, &loadedFace);
				}
				else
				{
					// Something Wrong
					free(copiedBuffer);
					return nullptr;
				}

				if (loadError != FT_Err_Ok)
				{
					// Font Load Error
					free(copiedBuffer);
					return nullptr;
				}
				return std::shared_ptr<FontFace>(new FontFace(loadedFace, [copiedBuffer]() { free(copiedBuffer); }, [copiedBuffer, size, faceIndex](FT_Face &faceHandleBuffer) {
					int freetypeLockResult = pthread_mutex_lock(&_Internal::g_libraryFaceGenerateLock.GetLock());
					if (freetypeLockResult == 0)
					{
						pthread_mutex_t *lockCopy = &_Internal::g_libraryFaceGenerateLock.GetLock();
						auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						return FT_New_Memory_Face(_Internal::g_libraryInstance, (FT_Byte *)copiedBuffer, (FT_Long)size, faceIndex, &faceHandleBuffer);
					}
					else
					{
						return (FT_Error)FT_Err_Cannot_Open_Resource;
					}
				}));
			}


			//////////////////////////////////////////////////////////////////////////////// FontFace

			//------------------------------------------------------------------------------
			// Basic Test Glyph Drawer for FreeType based Implementation
			static NormalGlyphDrawer s_freeTypeBasicGlyphDrawer;

			//------------------------------------------------------------------------------
			FontFace::FontFace(FT_Face faceHandle, std::function<void()> resourceCollector, std::function<FT_Error(FT_Face &)> faceCreator)
				: m_resourceCollector(resourceCollector), m_faceCreator(faceCreator)
			{
				m_faceHandles.enqueue(faceHandle);
			}

			//------------------------------------------------------------------------------
			FontFace::~FontFace()
			{
				FT_Face dequeuedFace;
				while (m_faceHandles.try_dequeue(dequeuedFace))
				{
					int lockResult = pthread_mutex_lock(&_Internal::g_libraryFaceGenerateLock.GetLock());
					if (lockResult == 0)
					{
						pthread_mutex_t *lockCopy = &_Internal::g_libraryFaceGenerateLock.GetLock();
						auto releaseLockFunc = [lockCopy]() mutable { pthread_mutex_unlock(lockCopy); };
						FINALLY_OF_BLOCK(_RELEASE_LOCK, releaseLockFunc);

						FT_Done_Face(dequeuedFace);
					}
					else
					{
						// Something Wrong
						continue;
					}
				}
				m_resourceCollector();
			}

			//------------------------------------------------------------------------------
			GlyphCodeInFont FontFace::FindGlyphCodeInFont(GlyphCodePointType glyph)
			{
				std::shared_ptr<FaceInstance> face = UseFace();
				if (!face)
				{
					return kGlyphCode_INVALID;
				}

				FT_UInt foundCharacterIndex = FT_Get_Char_Index(face->GetFaceHandle(), (FT_ULong)glyph);
				if (foundCharacterIndex == 0)
				{
					return kGlyphCode_INVALID;
				}

				return static_cast<GlyphCodeInFont>(foundCharacterIndex);
			}

			//------------------------------------------------------------------------------
			Graphics::CoordVector2D FontFace::GetKerningAmount(GlyphCodeInFont lastGlyphIndex, GlyphCodeInFont currentGlyphIndex, GlyphSizeType size, float horizontalRatio, bool isManualBold, bool isManualItalic)
			{
				if (lastGlyphIndex == kGlyphCode_INVALID || currentGlyphIndex == kGlyphCode_INVALID)
				{
					return Graphics::CoordVector2D::kZero;
				}

				std::shared_ptr<FaceInstance> face = UseFace();
				if (!face)
				{
					// Unavailable Font
					return Graphics::CoordVector2D::kZero;
				}

				if (!FT_HAS_KERNING(face->GetFaceHandle()))
				{
					// No Kerning Font
					return Graphics::CoordVector2D::kZero;
				}

				// Set Size
				FT_UInt horizontalDPI = (FT_UInt)round(Graphics::ReferenceState::GetApproxScreenXPPI());
				FT_UInt verticalDPI = (FT_UInt)round(Graphics::ReferenceState::GetApproxScreenYPPI());
				GlyphSizeType apiSize = 64.0f * (size * 72.0f / Graphics::ReferenceState::GetApproxScreenYPPI());
				FT_F26Dot6 apiFontHeight = (FT_F26Dot6)round(apiSize);
				FT_F26Dot6 apiFontWidth = 0;
				if (horizontalRatio > 1.0f + _Internal::kFloatEpsilon || horizontalRatio < 1.0f - _Internal::kFloatEpsilon)
				{
					apiFontWidth = (FT_F26Dot6)round(horizontalRatio * apiSize);
				}
				if (FT_Set_Char_Size(face->GetFaceHandle(), apiFontWidth, apiFontHeight, horizontalDPI, verticalDPI) != FT_Err_Ok)
				{
					// Failed to set font size
				}

				// Set Options
				if (isManualBold)
				{
					FT_Outline_Embolden(&face->GetFaceHandle()->glyph->outline, (FT_Pos)((face->GetFaceHandle()->size->metrics.x_ppem * _Internal::kBoldAmount) * 64.0f));
				}
				if (isManualItalic)
				{
					FT_Matrix transform = { 0x10000, (FT_Fixed)(65536.0f * _Internal::kItalicAmount), 0x00000, 0x10000 };
					FT_Outline_Transform(&face->GetFaceHandle()->glyph->outline, &transform);
				}

				FT_Vector delta;
				if (FT_Get_Kerning(face->GetFaceHandle(), lastGlyphIndex, currentGlyphIndex, FT_KERNING_DEFAULT, &delta) != FT_Err_Ok)
				{
					return Graphics::CoordVector2D::kZero;
				}

				return Graphics::CoordVector2D(static_cast<Graphics::BasicCoordType>(delta.x >> 6), static_cast<Graphics::BasicCoordType>(0.0f));
			}

			//------------------------------------------------------------------------------
			IGlyphDrawer *FontFace::GetBasicTestDrawer()
			{
				return &s_freeTypeBasicGlyphDrawer;
			}

			//------------------------------------------------------------------------------
			// Get Face Instance to use font face thread-safely
			std::shared_ptr<FontFace::FaceInstance> FontFace::UseFace()
			{
				FT_Face faceHandleBuffer;
				if (m_faceHandles.try_dequeue(faceHandleBuffer))
				{
					return std::shared_ptr<FaceInstance>(new FaceInstance(faceHandleBuffer, shared_from_this()));
				}

				FT_Error loadError = m_faceCreator(faceHandleBuffer);
				if (loadError == FT_Err_Ok)
				{
					return std::shared_ptr<FaceInstance>(new FaceInstance(faceHandleBuffer, shared_from_this()));
				}

				return nullptr;
			}

			//------------------------------------------------------------------------------
			// Interface for return resource for a used font instance
			void FontFace::_ReturnFaceResource(FT_Face faceHandle)
			{
				m_faceHandles.enqueue(faceHandle);
			}


			//////////////////////////////////////////////////////////////////////////////// FaceInstance

			//------------------------------------------------------------------------------
			FontFace::FaceInstance::FaceInstance(FT_Face faceHandle, std::shared_ptr<FontFace> ownerFace)
				: m_faceHandle(faceHandle), m_ownerFace(ownerFace)
			{
			}

			//------------------------------------------------------------------------------
			FontFace::FaceInstance::~FaceInstance()
			{
				m_ownerFace->_ReturnFaceResource(m_faceHandle);
			}
		}
	}
}
