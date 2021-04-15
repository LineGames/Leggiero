////////////////////////////////////////////////////////////////////////////////
// Rendering/UIFontFaceManager.h (Leggiero/Modules - LegacyUI)
//
// Manager class to manage font faces in UI system
////////////////////////////////////////////////////////////////////////////////

#ifndef __LM_LUI__RENDERING__UI_FONT_FACE_MANAGER_H
#define __LM_LUI__RENDERING__UI_FONT_FACE_MANAGER_H


// Standard Library
#include <memory>
#include <unordered_map>

// Leggiero.Utility
#include <Utility/Threading/ManagedThreadPrimitives.h>

// Leggiero.LegacyUI
#include "../Common/UICommonTypes.h"


namespace Leggiero
{
	// Forward Declaration
	namespace Font
	{
		class IFontFace;
	}


	namespace LUI
	{
		// Forward Declaration
		class IUIAssetLoader;


		// Font Face Manager
		class UIFontFaceManager
		{
		public:
			UIFontFaceManager(IUIAssetLoader &assetLoader);
			virtual ~UIFontFaceManager();

		private:
			UIFontFaceManager() = delete;

		public:
			std::shared_ptr<Font::IFontFace> GetFont(const UIFontNameType &fontName);
			void PreLoadFont(const UIFontNameType &fontName);

			void RegisterExternalFont(const UIFontNameType &fontName, std::shared_ptr<Font::IFontFace> font);
			std::shared_ptr<Font::IFontFace> GetCachedFont(const UIFontNameType &fontName);

		public:
			void ClearCache();

		protected:
			std::shared_ptr<Font::IFontFace> _LoadFont(const UIFontNameType &fontName);

		protected:
			IUIAssetLoader &m_assetLoader;

			std::unordered_map<UIFontNameType, std::shared_ptr<Font::IFontFace> > m_fontCache;
			Utility::Threading::SafePthreadRWLock m_cacheLock;
		};
	}
}

#endif
