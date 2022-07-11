cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Font STATIC)
le_tgt_setCppOptions(LE_M_Font)


target_include_directories(LE_M_Font PUBLIC ..)

target_link_libraries(LE_M_Font
    PRIVATE
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA
        LE_M_Graphics
)


target_sources(LE_M_Font
    PUBLIC
        FontModuleInterface.h GlyphManagerComponent.h
        Common/FontTypes.h Common/FontBackend.h Common/IFontFace.h Common/IGlyphDrawer.h Common/MultiPageFontSetting.h
        Common/CachedGlyph.h Common/GlyphCache.h Common/GlyphSearchCache.h
        Common/IFontSet.h Common/SingleFontSet.h Common/StyledFontSet.h
        Common/Typeset.h Common/Typesetting.h
        FreeType/FreeTypeFontCommon.h FreeType/FreeTypeFontFace.h FreeType/FreeTypeNormalGlyphDrawer.h
        
    PRIVATE
        FontModuleInterface.cpp GlyphManagerComponent.cpp
        _Internal/_FontInternalUtility.h _Internal/_FontInternalUtility.cpp
        Common/IFontFace.cpp Common/MultiPageFontSetting.cpp
        Common/CachedGlyph.cpp Common/GlyphCache.cpp Common/GlyphSearchCache.cpp
        Common/IFontSet.cpp Common/SingleFontSet.cpp Common/StyledFontSet.cpp
        Common/Typeset.cpp
        FreeType/FreeTypeFontCommon.cpp FreeType/FreeTypeFontFace.cpp FreeType/FreeTypeNormalGlyphDrawer.cpp
)
