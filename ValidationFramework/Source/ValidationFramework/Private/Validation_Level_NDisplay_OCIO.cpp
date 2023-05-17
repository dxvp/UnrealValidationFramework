/**
Copyright 2022 Netflix, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include "Validation_Level_NDisplay_OCIO.h"
#include "Kismet/GameplayStatics.h"

#if PLATFORM_WINDOWS || PLATFORM_LINUX
#include "DisplayClusterRootActor.h"
#include "DisplayClusterConfigurationTypes.h"
#include "Components/DisplayClusterICVFXCameraComponent.h"
#endif

#include "ValidationBPLibrary.h"
#include "VFProjectSettingsBase.h"

UValidation_Level_NDisplay_OCIO::UValidation_Level_NDisplay_OCIO()
{
	ValidationName = TEXT("NDisplay - OCIO 설정");
	ValidationDescription = TEXT("OCIO는 색상 관리 워크플로우를 보장합니다. OCIO 설정이 맞지 않거나 누락되면 콘텐츠의 색이 다르게 표현됩니다.");
	FixDescription = TEXT("LED와 스튜디오 장비 구성에 따라 원인과 해결법이 명확하지 않아 사용자가 직접 언리얼 엔진의 설정이나 LED 프로세서의 설정 수정이 필요하기도 합니다.");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX,
		EValidationWorkflow::VAD
	};
}
#if PLATFORM_WINDOWS || PLATFORM_LINUX
void UValidation_Level_NDisplay_OCIO::ValidateOCIOColorConversionSettings(
	FValidationResult& ValidationResult,
	const FOpenColorIOColorConversionSettings OCIOSettings1,
	FOpenColorIOColorConversionSettings OCIOSettings2, FString OCIOObjectName, bool HardFail
	)
{

	EValidationStatus Status = EValidationStatus::Fail;
	if (!HardFail)
	{
		Status = EValidationStatus::Warning;
	}
	if (OCIOSettings2.ConfigurationSource != OCIOSettings1.ConfigurationSource)
	{
		if (ValidationResult.Result > Status)
		{
			ValidationResult.Result = Status;	
		}
		ValidationResult.Message += OCIOObjectName + TEXT(" OCIO 설정이 프로젝트 유효성 검사 플러그인에 설정된 OCIO세팅과 일치하지 않음\n");
	}

	if (OCIOSettings2.SourceColorSpace != OCIOSettings1.SourceColorSpace)
	{
		if (ValidationResult.Result > Status)
		{
			ValidationResult.Result = Status;	
		}
		ValidationResult.Message += OCIOObjectName + TEXT(" 소스 컬러스페이스 설정이 프로젝트 유효성 검사 플러그인에 설정된 컬러스페이스와 일치하지 않음\n");
		ValidationResult.Message += OCIOSettings2.SourceColorSpace.ToString() + " v " + OCIOSettings1.SourceColorSpace.ToString() + "\n";
	}

	if (OCIOSettings2.DestinationColorSpace != OCIOSettings1.DestinationColorSpace)
	{
		if (ValidationResult.Result > Status)
		{
			ValidationResult.Result = Status;	
		}
		ValidationResult.Message += OCIOObjectName + TEXT(" 출력 컬러스페이스 설정이 프로젝트 유효성 검사 플러그인에 설정된 컬러스페이스와 일치하지 않음\n");
		ValidationResult.Message += OCIOSettings2.DestinationColorSpace.ToString() + " v " + OCIOSettings1.DestinationColorSpace.ToString() + "\n";
	}
			
	if (OCIOSettings2.DestinationDisplayView != OCIOSettings1.DestinationDisplayView)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += OCIOObjectName + TEXT(" 디스플레이 뷰 설정이 프로젝트 유효성 검사 플러그인에 설정된 디스플레이 뷰 설정과 일치하지 않음\n");
		ValidationResult.Message += OCIOSettings2.DestinationDisplayView.ToString() + " v " + OCIOSettings1.DestinationDisplayView.ToString() + "\n";
	}
}

void UValidation_Level_NDisplay_OCIO::ValidateAllViewportOCIOSetups(
	FValidationResult& ValidationResult,
	const FOpenColorIOColorConversionSettings ProjectOCIOSettings,
	FString OCIOObjectName, const FDisplayClusterConfigurationICVFX_StageSettings StageSettings)
{
	if (!StageSettings.ViewportOCIO.AllViewportsOCIOConfiguration.bIsEnabled)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += OCIOObjectName + TEXT("\n모든 NDisplay 뷰포트에 OCIO를 사용하도록 설정되어 있지 않음\n");
	}
		
	FOpenColorIOColorConversionSettings AllViewPortsOCIOSettings = StageSettings.
	                                                                ViewportOCIO.
	                                                                AllViewportsOCIOConfiguration.
	                                                                ColorConfiguration;
		
	if (!AllViewPortsOCIOSettings.IsValid())
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += OCIOObjectName + TEXT("\n모든 뷰포트에 OCIO설정이 되어 있지 않거나 설정 파일에 소스나 디스플레이 컬러가 설정되어 있지 않음\n");
	
	}
	else
	{
		ValidateOCIOColorConversionSettings(
			ValidationResult, ProjectOCIOSettings, AllViewPortsOCIOSettings, OCIOObjectName);
	}
		
	
}

void UValidation_Level_NDisplay_OCIO::ValidatePerViewportOCIOOverrideSetups(
	FValidationResult& ValidationResult, const FOpenColorIOColorConversionSettings ProjectOCIOSettings,
	FString OCIOObjectName, const FDisplayClusterConfigurationICVFX_StageSettings StageSettings)
{
	for (int Viewport_Idx = 0; Viewport_Idx < StageSettings.ViewportOCIO.PerViewportOCIOProfiles.Num(); Viewport_Idx++)
	{
		
		if (StageSettings.ViewportOCIO.PerViewportOCIOProfiles[Viewport_Idx].bIsEnabled)
		{
				
			FOpenColorIOColorConversionSettings PerViewPortOCIOSettings = StageSettings.
																			ViewportOCIO.
																			PerViewportOCIOProfiles[Viewport_Idx].
																			ColorConfiguration;

			if (!PerViewPortOCIOSettings.IsValid())
			{
					
				ValidationResult.Result = EValidationStatus::Fail;
				ValidationResult.Message += OCIOObjectName + TEXT("\n뷰포트 ") +
					FString::FromInt(Viewport_Idx) + 
					TEXT(" 번의 개별 OCIO 설정파일이 존재하지 않거나 소스/디스플레이 컬러스페이스 설정이 되어 있지 않습니다\n");
	
			}
			else
			{
				ValidateOCIOColorConversionSettings(ValidationResult, ProjectOCIOSettings,
					PerViewPortOCIOSettings, OCIOObjectName, false);
			}
		}
		else
		{
			if (ValidationResult.Result > EValidationStatus::Warning)
			{
				ValidationResult.Result = EValidationStatus::Warning;
			}
			ValidationResult.Message += OCIOObjectName + TEXT("\n뷰포트 ") +
				FString::FromInt(Viewport_Idx) + 
				TEXT(" 번의 OCIO 설정이 되어있지만 활성화되지 않았습니다\n");
				
		}
	}
}

void UValidation_Level_NDisplay_OCIO::ValidateInnerFrustumOCIOSetups(
	FValidationResult& ValidationResult, const FOpenColorIOColorConversionSettings ProjectOCIOSettings,
	FString ComponentName, FDisplayClusterConfigurationICVFX_CameraSettings Icvfx_CameraSettings)
{
	if (!Icvfx_CameraSettings.CameraOCIO.AllNodesOCIOConfiguration.bIsEnabled)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += ComponentName + TEXT("\n이너프러스텀의 OCIO 설정이 되어있지 않음\n");
	}
	FOpenColorIOColorConversionSettings InnerFrustumOCIOConfig = Icvfx_CameraSettings
		.CameraOCIO
		.AllNodesOCIOConfiguration
		.ColorConfiguration;
			
	if (!InnerFrustumOCIOConfig.IsValid())
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += ComponentName + TEXT("\n이너프러스텀의 OCIO 설정이 되어 있지 않거나, OCIO설정 파일에 소스 혹은 디스플레이 컬러스페이스 설정이 존재하지 않음\n");
	
	}
	else
	{
		ValidateOCIOColorConversionSettings(ValidationResult, ProjectOCIOSettings,
			InnerFrustumOCIOConfig, ComponentName);
	}
}

void UValidation_Level_NDisplay_OCIO::ValidateInnerFrustumOCIOPerNodeSetups(
	FValidationResult& ValidationResult,
	const FOpenColorIOColorConversionSettings ProjectOCIOSettings,
	FString ComponentName, FDisplayClusterConfigurationICVFX_CameraSettings Icvfx_CameraSettings)
{
	for (int PerNodeIndex = 0; PerNodeIndex < Icvfx_CameraSettings.CameraOCIO.PerNodeOCIOProfiles.Num(); PerNodeIndex++)
	{
		if(!Icvfx_CameraSettings.CameraOCIO.PerNodeOCIOProfiles[PerNodeIndex].bIsEnabled)
		{
			if(ValidationResult.Result > EValidationStatus::Warning)
			{
				ValidationResult.Result = EValidationStatus::Warning;	
			}
			ValidationResult.Message += ComponentName + TEXT("\n렌더 노드 ") +
				FString::FromInt(PerNodeIndex) + 
				TEXT(" 번의 노드당 개별 이너프러스텀 OCIO 설정이 존재하지만 적용되지 않았습니다.\n");
		}
		else
		{
			FOpenColorIOColorConversionSettings InnerFrustumPerNodeOCIOConfig = Icvfx_CameraSettings
			.CameraOCIO
			.PerNodeOCIOProfiles[PerNodeIndex]
			.ColorConfiguration;

			if (!InnerFrustumPerNodeOCIOConfig.IsValid())
			{
				ValidationResult.Result = EValidationStatus::Fail;
				ValidationResult.Message += ComponentName +	TEXT("\n렌더 노드 ")
					+ FString::FromInt(PerNodeIndex)
					+ TEXT("번의 노드당 개별 이너프러스텀 OCIO 설정의 소스/디스플레이 컬러스페이스가 설정되어 있지 않거나 OCIO 파일이 존재하지 않음\n");
	
			}
			else
			{
				ValidateOCIOColorConversionSettings(
				ValidationResult,
				ProjectOCIOSettings,
				InnerFrustumPerNodeOCIOConfig,
				ComponentName, false);
			}
		}
		
	}

}
#endif

FValidationResult UValidation_Level_NDisplay_OCIO::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	// Ensure we have validation framework settings configured in the project before continuing
	// TODO: Refactor into function as used in a number of places
	UObject* Settings = UValidationBPLibrary::GetValidationFrameworkProjectSettings();
	if (Settings == nullptr)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += "\n유효성 검사 플러그인의 사전 설정파일이 설정되지 않음";
		return ValidationResult;
	}

	const UVFProjectSettingsBase* ProjectSettings = Cast<UVFProjectSettingsBase>(Settings);
	const FOpenColorIOColorConversionSettings ProjectOCIOSettings = ProjectSettings->ProjectOpenIOColorConfig;

	if (!ProjectOCIOSettings.IsValid())
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += TEXT("\n유효성 검사 플러그인 설정에 OCIO 파일이 설정되지 않았거나 소스/사전 컬러스페이스가 설정되지 않았거나,\nOCIO 설정파일이 존재하지 않아 유효성 검증 진행과 수정이 불가능함.");
		return ValidationResult;
		
	}
	
	const UWorld* World = GetCorrectValidationWorld();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ADisplayClusterRootActor::StaticClass(), FoundActors);
	for (AActor* FoundActor : FoundActors)
	{
		const ADisplayClusterRootActor* MyActor = Cast<ADisplayClusterRootActor>(FoundActor);
		const UDisplayClusterConfigurationData* ConfigData = MyActor->GetConfigData();
		const FDisplayClusterConfigurationICVFX_StageSettings StageSettings = ConfigData->StageSettings;
		ValidateAllViewportOCIOSetups(ValidationResult, ProjectOCIOSettings, FoundActor->GetName(), StageSettings);
		ValidatePerViewportOCIOOverrideSetups(ValidationResult, ProjectOCIOSettings, FoundActor->GetName(), StageSettings);
		
		TInlineComponentArray<UDisplayClusterICVFXCameraComponent*> IcvfxCameraComponents;
		MyActor->GetComponents(IcvfxCameraComponents);
		for (const UDisplayClusterICVFXCameraComponent* IcvfxCameraComponent : IcvfxCameraComponents)
		{

			FDisplayClusterConfigurationICVFX_CameraSettings Icvfx_CameraSettings = IcvfxCameraComponent->GetCameraSettingsICVFX();
			if(Icvfx_CameraSettings.bEnable)
			{
				const FString ComponentName = FoundActor->GetName() + " -> " + IcvfxCameraComponent->GetName(); 
				ValidateInnerFrustumOCIOSetups(ValidationResult, ProjectOCIOSettings, ComponentName,Icvfx_CameraSettings);
				ValidateInnerFrustumOCIOPerNodeSetups(ValidationResult, ProjectOCIOSettings, ComponentName,Icvfx_CameraSettings);
			}
		}
	}

	if (ValidationResult.Result ==  EValidationStatus::Pass)
	{
		ValidationResult.Message = UValidationTranslation::Valid();
	}
#else
	ValidationResult.Result = EValidationStatus::Warning;
	ValidationResult.Message = TEXT("NDisplay는 리눅스 혹은 윈도우에서만 가능합니다");
#endif

	return ValidationResult;
}

FValidationFixResult UValidation_Level_NDisplay_OCIO::Fix_Implementation()
{

	FValidationFixResult ValidationFixResult = FValidationFixResult();
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	FValidationResult ValidationResult = Validation_Implementation();
	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		ValidationFixResult.Result = EValidationFixStatus::Fixed;
		ValidationFixResult.Message = TEXT("설정이 완료됨");
	}
	else
	{
		ValidationFixResult.Result = EValidationFixStatus::ManualFix;
		ValidationFixResult.Message = TEXT("사용자의 수동 설정이 필요함");
	}
#else
	ValidationFixResult.Result = EValidationFixStatus::Fixed;
	ValidationFixResult.Message = TEXT("설정이 완료됨");
#endif
	
	return ValidationFixResult;
}




