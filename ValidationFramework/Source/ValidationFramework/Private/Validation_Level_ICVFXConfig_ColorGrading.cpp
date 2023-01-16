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

#include "Validation_Level_ICVFXConfig_ColorGrading.h"
#include "Kismet/GameplayStatics.h"
#if PLATFORM_WINDOWS
#include "DisplayClusterRootActor.h"
#include "DisplayClusterConfigurationTypes.h"
#include "Components/DisplayClusterICVFXCameraComponent.h"
#endif



UValidation_Level_ICVFXConfig_ColorGrading::UValidation_Level_ICVFXConfig_ColorGrading()
{
	ValidationName = TEXT("NDisplay - 컬러그레이딩");
	ValidationDescription = TEXT("ICVFX의 컬러 그레이딩 파이프라인에 잘못된 설정이 없도록 전체 클러스터 및 뷰포트별, ICVFX 카메라에 잘못된 포스트프로세싱 설정이 있는지 확인합니다.");
	FixDescription = TEXT("Blue Correction을 비활성화하고 클러스터/뷰포트의 색역 확장(Expand Gamut)을 비활성화함.");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

#if PLATFORM_WINDOWS
void UValidation_Level_ICVFXConfig_ColorGrading::ValidateEntireClusterColorGrading(
	FValidationResult& Result, FString& ActorMessages, const FDisplayClusterConfigurationICVFX_StageSettings StageSettings)
{
	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.bOverride_AutoExposureBias)
	{
		if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.AutoExposureBias != 0.0)
		{
			Result.Result = EValidationStatus::Warning;
			ActorMessages += TEXT("전체 뷰포트의 노출 보정 활성화 및 0.0이 아님. 작업자가 의도한 것일 수 있음 \n");
		}
	}
	
	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.bOverride_BlueCorrection)
	{
		Result.Result = EValidationStatus::Fail;
		ActorMessages += TEXT("전체 뷰포트의 Blue Correction이 활성화 되어 있음\n");
			
	}

	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.BlueCorrection != 0.0)
	{
		Result.Result = EValidationStatus::Fail;
		ActorMessages += TEXT("전체 뷰포트의 Blue Correction이 0.0으로 설정되어 있지 않음\n");
	}

	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.bOverride_ExpandGamut)
	{
		Result.Result = EValidationStatus::Fail;
		ActorMessages += TEXT("색역 확장이 활성화 되어 있음\n");
			
	}

	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.ExpandGamut != 0.0)
	{
		Result.Result = EValidationStatus::Fail;
		ActorMessages += TEXT("색역 확장 설정값이 0.0이 아님\n");
			
	}
}

void UValidation_Level_ICVFXConfig_ColorGrading::ValidatePerViewPortColorGrading(
	FValidationResult& Result, FString& ActorMessages, const FDisplayClusterConfigurationICVFX_StageSettings StageSettings) const
{
	for (int x=0; x<StageSettings.PerViewportColorGrading.Num(); x++)
	{
		TArray< FStringFormatArg > Args;
		Args.Add( FStringFormatArg( x ) );

		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.bOverride_AutoExposureBias)
		{
			if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.AutoExposureBias != 0.0)
			{
				Result.Result = EValidationStatus::Warning;
				ActorMessages += FString::Format
				(
					TEXT("뷰포트 {0}번의 노출 보정이 활성화되어 있거나 값이 0.0이 아님. 작업자가 의도한 것일 수 있음\n" ), Args
				);
				
			}
		}
		
		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.bOverride_BlueCorrection)
		{
			Result.Result = EValidationStatus::Fail;
			ActorMessages += FString::Format
			(
				TEXT("뷰포트 {0} 번의 Blue Correction이 활성화 되어 있음\n" ), Args
			);
		}

		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.BlueCorrection != 0.0)
		{
			Result.Result = EValidationStatus::Fail;
			ActorMessages += FString::Format
			(
				TEXT( "뷰포트 {0} 번의 Blue Correction값이 0.0이 아님\n" ), Args
			);
		}

		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.bOverride_ExpandGamut)
		{
			Result.Result = EValidationStatus::Fail;
			ActorMessages += FString::Format
			(
				TEXT( "뷰포트 {0} 번의 색역 확장이 활성화 되어 있음\n" ), Args
			);
		}

		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.ExpandGamut != 0.0)
		{
			Result.Result = EValidationStatus::Fail;
			ActorMessages += FString::Format
			(
				TEXT( "뷰포트 {0} 번의 색역 확장 설정값이 0.0이 아님\n" ), Args
			);
		}
	}
}

void UValidation_Level_ICVFXConfig_ColorGrading::ValidateInnerFrustumColorGrading(
	FValidationResult& Result, FString& ActorMessages, const ADisplayClusterRootActor* Actor) const
{
	TInlineComponentArray<UDisplayClusterICVFXCameraComponent*> IcvfxCameraComponents;
	Actor->GetComponents(IcvfxCameraComponents);
	for (const UDisplayClusterICVFXCameraComponent* IcvfxCameraComponent : IcvfxCameraComponents)
	{
		const bool BlueCorrectionEnabled = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.bOverride_BlueCorrection;
		const float BlueCorrection = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.BlueCorrection;

		const float ExpandGamutEnabled = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.bOverride_ExpandGamut;
		const float ExpandGamut = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.ExpandGamut;

		TArray< FStringFormatArg > Args;
		Args.Add( FStringFormatArg( IcvfxCameraComponent->GetName() ) );
			
		if (BlueCorrectionEnabled && BlueCorrection != 0.0)
		{
			Result.Result = EValidationStatus::Fail;
			ActorMessages += FString::Format(
				TEXT("{0}\n카메라의 이너프러스텀 Blue Correction이 활성화 되어 있음. 0.0으로 설정되어야 함.\n"),
				Args
			);
		}

		if (ExpandGamutEnabled && ExpandGamut != 0.0)
		{
			Result.Result = EValidationStatus::Fail;
			ActorMessages += FString::Format(
				TEXT("{0}\n카메라의 이너프러스텀 색역 확장이 활성화 되어 있음. 0.0으로 설정되어야 함.\n"),
				Args
			);
		}
	}
}

void UValidation_Level_ICVFXConfig_ColorGrading::FixEntireClusterColorGrading(
	FString& ActorMessages, FDisplayClusterConfigurationICVFX_StageSettings& StageSettings)
{
	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.bOverride_BlueCorrection)
	{
		StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.bOverride_BlueCorrection = false;
		ActorMessages += TEXT("전체 뷰포트의 Blue Correction이 비활성화됨\n");
			
	}

	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.BlueCorrection != 0.0)
	{
		StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.BlueCorrection = 0.0;
		ActorMessages += TEXT("전체 뷰포트의 Blue Correction을 0.0으로 설정함\n");
	}

	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.bOverride_ExpandGamut)
	{
		StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.bOverride_ExpandGamut = false;
		ActorMessages += TEXT("전체 뷰포트의 색역 확장을 비활성화홤\n");
			
	}

	if (StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.ExpandGamut != 0.0)
	{
		StageSettings.EntireClusterColorGrading.ColorGradingSettings.Misc.ExpandGamut = 0.0;
		ActorMessages += TEXT("전체 뷰포트의 색역 확장값을 0.0으로 설정함\n");
	}
}

void UValidation_Level_ICVFXConfig_ColorGrading::FixPerViewportColorGrading(FString& ActorMessages, FDisplayClusterConfigurationICVFX_StageSettings& StageSettings) const
{
	for (int x=0; x<StageSettings.PerViewportColorGrading.Num(); x++)
	{
		TArray< FStringFormatArg > Args;
		Args.Add( FStringFormatArg( x ) );
			
		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.bOverride_BlueCorrection)
		{
			StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.bOverride_BlueCorrection = false;
			ActorMessages += FString::Format
			(
				TEXT( "뷰포트 {0} 번의 Blue Correction을 비활성화함\n" ), Args
			);
				
		}

		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.BlueCorrection != 0.0)
		{
			StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.BlueCorrection = 0.0;
			ActorMessages += FString::Format
			(
				TEXT( "뷰포트 {0} 번의 Blue Correction값을 0.0으로 설정함\n" ), Args
			);
				
		}

		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.bOverride_ExpandGamut)
		{
			StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.bOverride_ExpandGamut = false;
			ActorMessages += FString::Format
			(
				TEXT( "뷰포트 {0} 번의 색역 확장을 비활성화함\n" ), Args
			);
				
		}

		if (StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.ExpandGamut != 0.0)
		{
			StageSettings.PerViewportColorGrading[x].ColorGradingSettings.Misc.ExpandGamut = 0.0;
			ActorMessages += FString::Format
			(
				TEXT( "뷰포트 {0} 번의 색역 확장값을 0.0으로 설정함\n" ), Args
			);
		}
			
	}
}

void UValidation_Level_ICVFXConfig_ColorGrading::FixInnerFrustumColorGrading(
	FValidationFixResult& Result, FString& ActorMessages, const ADisplayClusterRootActor* Actor) const
{
	TInlineComponentArray<UDisplayClusterICVFXCameraComponent*> IcvfxCameraComponents;
	Actor->GetComponents(IcvfxCameraComponents);
	for (UDisplayClusterICVFXCameraComponent* IcvfxCameraComponent : IcvfxCameraComponents)
	{
		const bool BlueCorrectionEnabled = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.bOverride_BlueCorrection;
		const float BlueCorrection = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.BlueCorrection;

		const float ExpandGamutEnabled = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.bOverride_ExpandGamut;
		const float ExpandGamut = IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.ExpandGamut;

		TArray< FStringFormatArg > Args;
		Args.Add( FStringFormatArg( IcvfxCameraComponent->GetName() ) );
			
		if (BlueCorrectionEnabled || BlueCorrection != 0.0)
		{
			Result.Result = EValidationFixStatus::Fixed;
			IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.bOverride_BlueCorrection = false;
			IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.BlueCorrection = 0.0;
			ActorMessages += FString::Format(
				TEXT("{0}\n 카메라의 이너프러스텀 Blue Correction을 비활성화하고 0.0으로 설정함\n"),
				Args
			);
		}

		if (ExpandGamutEnabled || ExpandGamut != 0.0)
		{
			Result.Result = EValidationFixStatus::Fixed;
			IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.bOverride_ExpandGamut = false;
			IcvfxCameraComponent->CameraSettings.AllNodesColorGrading.ColorGradingSettings.Misc.ExpandGamut = 0.0;
			ActorMessages += FString::Format(
				TEXT("{0}\n 카메라의 이너프러스텀 색역 확장을 비활성화하고 0.0으로 설정함\n"),
				Args
			);
		}
	}
}

#endif


FValidationResult UValidation_Level_ICVFXConfig_ColorGrading::Validation_Implementation()
{
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "Valid");
	FString Message = "";
	
	const UWorld* World = GetCorrectValidationWorld();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ADisplayClusterRootActor::StaticClass(), FoundActors);

	for (AActor* FoundActor : FoundActors)
	{
		FString ActorMessages = "";
		const ADisplayClusterRootActor* MyActor = Cast<ADisplayClusterRootActor>(FoundActor);
		
		const UDisplayClusterConfigurationData* ConfigData = MyActor->GetConfigData();
		const FDisplayClusterConfigurationICVFX_StageSettings StageSettings = ConfigData->StageSettings;
		ValidateEntireClusterColorGrading(ValidationResult, ActorMessages, StageSettings);
		ValidatePerViewPortColorGrading(ValidationResult, ActorMessages, StageSettings);
		ValidateInnerFrustumColorGrading(ValidationResult, ActorMessages, MyActor);
		
		if (ActorMessages.Len())
		{
			Message +=  MyActor->GetName();
			Message += "\n";
			Message += ActorMessages;
		}
		
	}

	if (ValidationResult.Result != EValidationStatus::Pass)
	{
		ValidationResult.Message = Message;
	}
	
	return ValidationResult;
#endif

#if PLATFORM_MAC
	FValidationResult ValidationResult = FValidationResult();
	ValidationResult.Result = EValidationStatus::Warning;
	ValidationResult.Message = UValidationTranslation::NoOSX();
	return ValidationResult;
#endif
}

FValidationFixResult UValidation_Level_ICVFXConfig_ColorGrading::Fix_Implementation()
{
	#if PLATFORM_WINDOWS || PLATFORM_LINUX
	FValidationFixResult ValidationFixResult = FValidationFixResult(EValidationFixStatus::Fixed, "");
	FString Message = "";
	
	const UWorld* World = GetCorrectValidationWorld();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ADisplayClusterRootActor::StaticClass(), FoundActors);

	for (AActor* FoundActor : FoundActors)
	{
		FString ActorMessages = "";
		const ADisplayClusterRootActor* MyActor = Cast<ADisplayClusterRootActor>(FoundActor);
		
		UDisplayClusterConfigurationData* ConfigData = MyActor->GetConfigData();
		
		FDisplayClusterConfigurationICVFX_StageSettings StageSettings = ConfigData->StageSettings;
		FixEntireClusterColorGrading(ActorMessages, StageSettings);
		FixPerViewportColorGrading(ActorMessages, StageSettings);
		FixInnerFrustumColorGrading(ValidationFixResult, ActorMessages, MyActor);
		
		ConfigData->StageSettings = StageSettings;
		
		if (ActorMessages.Len())
		{
			Message +=  MyActor->GetName();
			Message += "\n";
			Message += ActorMessages;
		}
		
	}
	ValidationFixResult.Message = Message;
	return ValidationFixResult;
	#endif

	#if PLATFORM_MAC
		FValidationFixResult ValidationFixResult = FValidationFixResult();
		ValidationFixResult.Result = EValidationFixStatus::NotFixed;
		ValidationFixResult.Message = UValidationTranslation::NoOSX();
		return ValidationFixResult;
	#endif
}

