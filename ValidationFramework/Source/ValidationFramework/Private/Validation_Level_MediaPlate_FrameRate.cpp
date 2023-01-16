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

#include "Validation_Level_MediaPlate_FrameRate.h"

#include "MediaPlate.h"
#include "Kismet/GameplayStatics.h"
#include "MediaPlateComponent.h"
#include "MediaSource.h"
#include "MediaPlaylist.h"
#include "ImgMediaSource.h"
#include "ValidationBPLibrary.h"
#include "VFProjectSettingsBase.h"


UValidation_Level_MediaPlate_FrameRate::UValidation_Level_MediaPlate_FrameRate()
{
	ValidationName = TEXT("미디어플레이트 프레임레이트");
	ValidationDescription = TEXT("미디어플레이트 소스가 호환 가능한 촬영 속도로 설정되었는지 확인함");
	FixDescription = TEXT("프로젝트 설정과 일치하도록 ImgMediaSource의 프레임레이트를 설정하고 DX12를 사용하도록 설정합니다.");
	ValidationScope = EValidationScope::Level,
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX,
		EValidationWorkflow::VAD,
		EValidationWorkflow::VRScouting,
		EValidationWorkflow::SimulCam
	};
}

TArray<UImgMediaSource*> UValidation_Level_MediaPlate_FrameRate::GetAllMediaSourcesFromLevel() const
{
	const UWorld* World = GetCorrectValidationWorld();

	TArray<UImgMediaSource*> ImgMediaSourceArray;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AMediaPlate::StaticClass(), FoundActors);

	for (AActor* FoundActor : FoundActors)
	{
		const AMediaPlate* MediaPlateActor = Cast<AMediaPlate>(FoundActor);
		TInlineComponentArray<UMediaPlateComponent*> MediaPlateComponents;
		MediaPlateActor->GetComponents(MediaPlateComponents);
		for (const UMediaPlateComponent* MediaPlateComponent : MediaPlateComponents)
		{
			const TObjectPtr<UMediaPlaylist> MediaPlaylist = MediaPlateComponent->MediaPlaylist;
			for (int i = 0; i < MediaPlaylist->Num(); i++)
			{
				UMediaSource* MediaSource = MediaPlaylist->Get(i);
				UImgMediaSource* ImgMediaSource = Cast<UImgMediaSource>(MediaSource);
				ImgMediaSourceArray.AddUnique(ImgMediaSource);
				
			}
		}
	}
	return ImgMediaSourceArray;
}

FValidationResult UValidation_Level_MediaPlate_FrameRate::Validation_Implementation()
{
	FValidationResult ValidationResult = UValidation_Project_DX12::Validation_Implementation();
	FString Message = TEXT("");

	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		ValidationResult.Message = Message;
	}

	const UObject* Settings = UValidationBPLibrary::GetValidationFrameworkProjectSettings();
	if (Settings == nullptr)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message += TEXT("\n프로젝트에 유효성 검사 플러그인 설정파일이 존재하지 않음");
		return ValidationResult;
	}

	const UVFProjectSettingsBase* ProjectSettings = Cast<UVFProjectSettingsBase>(Settings);
	const FFrameRate ProjectFrameRate = ProjectSettings->ProjectFrameRate;

	TArray<UImgMediaSource*> ImgMediaSourceArray = GetAllMediaSourcesFromLevel();
	for (const UImgMediaSource* ImgMediaSource : ImgMediaSourceArray)
	{
		const FFrameRate MediaFrameRate = ImgMediaSource->FrameRateOverride;
		const EFrameRateComparisonStatus Compatible = UValidationBPLibrary::CompareFrameRateCompatability(
			 MediaFrameRate, ProjectFrameRate);

		if (Compatible == EFrameRateComparisonStatus::InValid)
		{
			ValidationResult.Result = EValidationStatus::Fail;
			ValidationResult.Message += TEXT("\nImageMediaSource 에셋에 설정된 프레임레이트와 프로젝트 설정이 일치하지 않음\n") + ImgMediaSource->GetPathName();
		}

		if (Compatible == EFrameRateComparisonStatus::ValidMultiple)
		{
			if (ValidationResult.Result > EValidationStatus::Warning)
			{
				ValidationResult.Result = EValidationStatus::Warning;
			}
			ValidationResult.Message += TEXT("\nImageMediaSource 에셋에 설정된 프레임레이트와 같지 않으나 호환되는 프레임레이트임. (24/48 or 30/60 같은) 의도한 설정일 수 있음") + ImgMediaSource->GetPathName();
		}
	}

	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		ValidationResult.Message = UValidationTranslation::Valid();
	}
	else
	{
		Message += TEXT("\n설정이 필요한 시퀀스의 프레임 속도를 자동으로 결정할 수 없음. 작업자의 직접 수정 필요\n");
		ValidationResult.Message += Message;
	}
	
	return ValidationResult;
}

FValidationFixResult UValidation_Level_MediaPlate_FrameRate::Fix_Implementation() 
{
	const FValidationResult ValidationResult = Validation_Implementation();
	FValidationFixResult ValidationFixResult = UValidation_Project_DX12::Fix_Implementation();
	

	if (ValidationResult.Result  != EValidationStatus::Pass)
	{
		ValidationFixResult.Result = EValidationFixStatus::ManualFix;
		ValidationFixResult.Message = TEXT("설정이 필요한 시퀀스의 프레임 속도를 자동으로 결정할 수 없음. 작업자의 직접 수정 필요\n");
	}
	
	
	return ValidationFixResult;
}



