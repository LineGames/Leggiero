cmake_minimum_required(VERSION 3.18.1)

include("../../Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LE_M_LegacyUI STATIC)
le_tgt_setCppOptions(LE_M_LegacyUI)


target_include_directories(LE_M_LegacyUI PUBLIC ..)

target_link_libraries(LE_M_LegacyUI
    PRIVATE
        LE_Libraries
        LE_Basic LE_Utility LE_Engine
        LE_PlatformA LE_M_Application
        LE_M_FileSystem LE_M_Graphics LE_M_Input LE_M_Task LE_M_Font
)


target_sources(LE_M_LegacyUI
    PUBLIC
        UIObject.h UIManager.h
        Common/UICommonTypes.h Common/UICommonResources.h Common/IUIArea.h Common/UICommonArea.h Common/UITransform.h Common/UISizeSettable.h
        Scene/UISceneBase.h
        Component/UIComponentType.h Component/IUIComponent.h Component/UILayoutComponent.h Component/UITransformComponent.h Component/UIClippingComponent.h Component/UISizeSubComponent.h
        Touch/UITouch.h Touch/UITouchComponent.h Touch/ValuedTouchComponent.h Touch/UITouchNegotiator.h
        Rendering/UIRenderer.h Rendering/UIRenderingUtility.h Rendering/UIClipping.h Rendering/UIShaders.h Rendering/IUIRenderingShape.h Rendering/UIShapeRect.h Rendering/UITexture.h Rendering/UITextureManager.h Rendering/UIFontFaceManager.h
        Rendering/UIRenderingComponent.h Rendering/ShapeSetRenderingComponent.h Rendering/TextureRenderingComponent.h Rendering/PatchedTextureRenderingComponent.h Rendering/TextRenderingComponent.h Rendering/CachedTextRenderingComponent.h
        Loader/IUIAssetLoader.h Loader/DummyUIAssetLoader.h Loader/UIChainAssetLoader.h Loader/UITableAssetLoader.h Loader/UIFileAssetLoader.h Loader/UIBundleAssetLoader.h
        Controller/CommonControllerDefines.h Controller/CommonButtonRenderingController.h Controller/OnOffButtonController.h Controller/SimpleSliderRenderingController.h Controller/TextInputBoxRenderingController.h
        Element/UIElementSingleSimpleButton.h Element/UIElementTestButton.h Element/UIElementSimpleSlideBar.h Element/UIElementSlideBar.h Element/UIElementSimpleHorizontalScrollPanel.h Element/UIElementSimpleVerticalScrollPanel.h Element/UIElementCancelPanelArea.h Element/UIElementBlurredBarrier.h Element/UIElementFixedText.h Element/UIElementVariantText.h Element/UIElementCachedVariantText.h Element/UIElementTextInputBox.h
        Prefab/UIPrefabCommonObjects.h Prefab/UIPrefabPlaceProcessor.h Prefab/UIFabricatedComponent.h
        Prefab/UIComponentPrefab.h Prefab/UILayoutComponentPrefab.h Prefab/UITransformComponentPrefab.h Prefab/UIShapeRenderingComponentPrefab.h Prefab/UITextureRenderingComponentPrefab.h Prefab/UIPatchedTextureRenderingComponentPrefab.h
        Prefab/UIObjectPrefab.h Prefab/UIObjectButtonPrefab.h Prefab/UIObjectScrollPanelPrefab.h Prefab/UIObjectSliderPrefab.h Prefab/UITextObjectPrefab.h Prefab/UITextInputBoxPrefab.h

        Description/UIDescription.h Description/UIDescriptionManager.h Description/UIDescriptionTypes.h Description/UIDescriptionProcessingContext.h
        Description/UIDescriptionUnit.h Description/UIDescriptionSystemUnit.h
        Description/UIDescriptionReader.h Description/UIDescriptionParserUtility.h
        Description/UIDescriptionColor.h Description/UIDescriptionFont.h Description/UIDescriptionTexture.h Description/UIDescriptionSourcedString.h
        Description/UIDescriptionExpression.h Description/UIDescriptionExpression_Nodes.h Description/UIDescriptionCommonExpression.h
        Description/UIPrefabDescription.h Description/UIPrefabDescription_Reader.h

    PRIVATE
        UIObject.cpp UIManager.cpp
        Common/UICommonResources.cpp Common/IUIArea.cpp Common/UICommonArea.cpp Common/UITransform.cpp
        Scene/UISceneBase.cpp
        Component/UILayoutComponent.cpp Component/UITransformComponent.cpp Component/UIClippingComponent.cpp Component/UISizeSubComponent.cpp
        Touch/UITouch.cpp Touch/UITouchComponent.cpp Touch/ValuedTouchComponent.cpp Touch/UITouchNegotiator.cpp
        Rendering/UIRenderer.cpp Rendering/UIClipping.cpp Rendering/UIShaders.cpp Rendering/UIShaders_Shader.cpp Rendering/UIShapeRect.cpp Rendering/UITextureManager.cpp Rendering/UIFontFaceManager.cpp
        Rendering/ShapeSetRenderingComponent.cpp Rendering/TextureRenderingComponent.cpp Rendering/PatchedTextureRenderingComponent.cpp Rendering/TextRenderingComponent.cpp Rendering/CachedTextRenderingComponent.cpp
        Loader/DummyUIAssetLoader.cpp Loader/UIChainAssetLoader.cpp Loader/UITableAssetLoader.cpp Loader/UIFileAssetLoader.cpp Loader/UIBundleAssetLoader.cpp
        Controller/CommonControllerDefines.cpp Controller/CommonButtonRenderingController.cpp Controller/OnOffButtonController.cpp Controller/SimpleSliderRenderingController.cpp Controller/TextInputBoxRenderingController.cpp
        Element/UIElementSingleSimpleButton.cpp Element/UIElementTestButton.cpp Element/UIElementSimpleSlideBar.cpp Element/UIElementSlideBar.cpp Element/UIElementSimpleHorizontalScrollPanel.cpp Element/UIElementSimpleVerticalScrollPanel.cpp Element/UIElementCancelPanelArea.cpp Element/UIElementBlurredBarrier.cpp Element/UIElementFixedText.cpp Element/UIElementVariantText.cpp Element/UIElementCachedVariantText.cpp Element/UIElementTextInputBox.cpp
        Prefab/UIPrefabCommonObjects.cpp Prefab/UIFabricatedComponent.cpp
        Prefab/UIComponentPrefab.cpp Prefab/UILayoutComponentPrefab.cpp Prefab/UITransformComponentPrefab.cpp Prefab/UIShapeRenderingComponentPrefab.cpp Prefab/UITextureRenderingComponentPrefab.cpp Prefab/UIPatchedTextureRenderingComponentPrefab.cpp
        Prefab/UIObjectPrefab.cpp Prefab/UIObjectButtonPrefab.cpp Prefab/UIObjectScrollPanelPrefab.cpp Prefab/UIObjectSliderPrefab.cpp Prefab/UITextObjectPrefab.cpp Prefab/UITextInputBoxPrefab.cpp

        Description/UIDescription.cpp Description/UIDescriptionManager.cpp Description/UIDescriptionTypes.cpp Description/UIDescriptionProcessingContext.cpp
        Description/UIDescriptionUnit.cpp Description/UIDescriptionSystemUnit.cpp
        Description/UIDescriptionReader.cpp Description/UIDescriptionParserUtility.cpp
        Description/UIDescriptionColor.cpp Description/UIDescriptionFont.cpp Description/UIDescriptionTexture.cpp Description/UIDescriptionSourcedString.cpp
        Description/UIDescriptionExpression.cpp Description/UIDescriptionCommonExpression.cpp
        Description/UIPrefabDescription.cpp Description/UIPrefabDescription_Reader.cpp
)
