cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_Graphics STATIC)
le_tgt_setCppOptions(LE_M_Graphics)


target_include_directories(LE_M_Graphics PUBLIC ..)


find_library(nlib-GLESv3 GLESv3)
find_library(nlib-EGL EGL)

target_link_libraries(LE_M_Graphics
    PRIVATE
        ${nlib-GLESv3} ${nlib-EGL}
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA LE_M_Application
        LE_M_FileSystem
)


target_sources(LE_M_Graphics
    PUBLIC
        GraphicsModuleInterface.h GraphicResourceManagerComponent.h GraphicsThreadContext.h
        Common/GraphicsTypes.h Common/IGLGraphicResource.h Common/GLColor.h Common/GraphicsReferenceState.h Common/GLFrameBufferBindContext.h
        Shader/CommonGLVertexType.h Shader/ProgramManager.h Shader/IGLProgramStateEventObserver.h Shader/GLVertexAttribEnabledContext.h
        Shader/GLShaderProgramResource.h Shader/CommonBaseShaderProgram.h Shader/TexturedShaderProgram.h
        Shader/Basic/ColorSimpleShader.h Shader/Basic/TexturedColorSimpleShader.h
        Texture/GLTextureResource.h Texture/TextureManager.h Texture/TextureHelper.h Texture/TextureCommitter.h Texture/TextureSection.h Texture/TextureAtlasTable.h
        Texture/ImageFormatType.h
        Texture/RuntimeTextureAtlas.h Texture/RuntimeTextureAtlasEntry.h Texture/RuntimeTextureAtlasManager.h
        Texture/DynamicTextureResource.h Texture/DynamicTextureRestore.h Texture/RenderTargetTextureResource.h
        
    PRIVATE
        GraphicsModuleInterface.cpp GraphicResourceManagerComponent.cpp
        _Internal/_DummyInterfaces.h _Internal/_InternalInitializer.h _Internal/_InternalUpdater.h _Internal/_GraphicsModuleInternalState.h _Internal/_GraphicsModuleInternalState.cpp
        Common/GLColor.cpp Common/GraphicsReferenceState.cpp Common/GLFrameBufferBindContext.cpp
        Shader/ProgramManager.cpp Shader/GLVertexAttribEnabledContext.cpp Shader/GLShaderProgramResource.cpp Shader/GLShaderProgramResource_Creation.cpp Shader/CommonBaseShaderProgram.cpp
        Shader/Basic/ColorSimpleShader.cpp Shader/Basic/ColorSimpleShader_Shader.cpp Shader/Basic/TexturedColorSimpleShader.cpp Shader/Basic/TexturedColorSimpleShader_Shader.cpp
        Texture/GLTextureResource.cpp Texture/GLTextureResource_Creation.cpp Texture/TextureManager.cpp Texture/TextureHelper.cpp Texture/TextureCommitter.cpp Texture/TextureSection.cpp Texture/TextureAtlasTable.cpp
        Texture/ImageFormatType.cpp Texture/TextureHelper_Loader.cpp Texture/TextureHelper_JPEG.cpp Texture/TextureHelper_PNG.cpp
        Texture/RuntimeTextureAtlas.cpp Texture/RuntimeTextureAtlasEntry.cpp Texture/RuntimeTextureAtlasManager.cpp
        Texture/DynamicTextureResource.cpp Texture/DynamicTextureResource_Creation.cpp Texture/RenderTargetTextureResource.cpp Texture/RenderTargetTextureResource_Creation.cpp
)
