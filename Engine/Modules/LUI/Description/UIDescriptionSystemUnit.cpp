////////////////////////////////////////////////////////////////////////////////
// Description/UIDescriptionSystemUnit.cpp (Leggiero/Modules - LegacyUI)
//
// Implementation for System Unit of UI Description
////////////////////////////////////////////////////////////////////////////////

// My Header
#include "UIDescriptionSystemUnit.h"

// Leggiero.Application
#include <Application/ApplicationComponent.h>
#include <Application/IAppInformationProvider.h>
#include <Application/IDeviceInformationProvider.h>

// Leggiero.LegacyUI
#include "UIDescription.h"


namespace Leggiero
{
	namespace LUI
	{
		namespace Description
		{
			//////////////////////////////////////////////////////////////////////////////// SystemUnit

			//------------------------------------------------------------------------------
			const UnitNameType SystemUnit::kSystemUnitName("System");

			//------------------------------------------------------------------------------
			SystemUnit::SystemUnit(Application::ApplicationComponent *appComponent)
				: m_isValueLoaded(false)
				, m_appComponent(appComponent)
			{
				LoadSystemValuesFrom(appComponent);
			}

			//------------------------------------------------------------------------------
			SystemUnit::~SystemUnit()
			{
			}

			//------------------------------------------------------------------------------
			void SystemUnit::ReloadSystemValues()
			{
				LoadSystemValuesFrom(m_appComponent);
			}

			//------------------------------------------------------------------------------
			void SystemUnit::LoadSystemValuesFrom(Application::ApplicationComponent *appComponent)
			{
				if (appComponent == nullptr)
				{
					return;
				}

				AddObject(TypeUtility::ToVariableName("screenWidth"), std::make_shared<FloatVariable>(appComponent->AppInformation().GetPixelWidth()));
				AddObject(TypeUtility::ToVariableName("screenHeight"), std::make_shared<FloatVariable>(appComponent->AppInformation().GetPixelHeight()));
				AddObject(TypeUtility::ToVariableName("screenAspect"), std::make_shared<FloatVariable>(appComponent->AppInformation().GetPixelWidth() / appComponent->AppInformation().GetPixelHeight()));

				AddObject(TypeUtility::ToVariableName("screenPPIX"), std::make_shared<FloatVariable>(appComponent->DeviceInformation().GetApproximateScreenXPPI()));
				AddObject(TypeUtility::ToVariableName("screenPPIY"), std::make_shared<FloatVariable>(appComponent->DeviceInformation().GetApproximateScreenYPPI()));

				AddObject(TypeUtility::ToVariableName("isTablet"), std::make_shared<BoolVariable>(appComponent->DeviceInformation().IsTablet()));
				AddObject(TypeUtility::ToVariableName("isPhone"), std::make_shared<BoolVariable>(!appComponent->DeviceInformation().IsTablet()));

				AddObject(TypeUtility::ToVariableName("isIOS"), std::make_shared<BoolVariable>(appComponent->AppInformation().GetPlatformType() == Application::AppPlatformType::kIOS));
				AddObject(TypeUtility::ToVariableName("isAndroid"), std::make_shared<BoolVariable>(appComponent->AppInformation().GetPlatformType() == Application::AppPlatformType::kAndroid));

				Utility::Math::BasicRect<Application::DeviceScreenCoordType> safeArea = appComponent->DeviceInformation().GetSafeArea();
				AddObject(TypeUtility::ToVariableName("safeAreaWidth"), std::make_shared<FloatVariable>(safeArea.GetWidth()));
				AddObject(TypeUtility::ToVariableName("safeAreaHeight"), std::make_shared<FloatVariable>(safeArea.GetHeight()));

				AddObject(TypeUtility::ToVariableName("safeInsetLeft"), std::make_shared<FloatVariable>(safeArea.left));
				AddObject(TypeUtility::ToVariableName("safeInsetTop"), std::make_shared<FloatVariable>(safeArea.top));
				AddObject(TypeUtility::ToVariableName("safeInsetRight"), std::make_shared<FloatVariable>(safeArea.right));
				AddObject(TypeUtility::ToVariableName("safeInsetBottom"), std::make_shared<FloatVariable>(safeArea.bottom));

				m_isValueLoaded = true;
			}
		}
	}
}
