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


#include "Validation_Level_SubLevelsAlwaysLoaded.h"
#include "EditorLevelUtils.h"
#include "Engine/LevelStreamingAlwaysLoaded.h"


UValidation_Level_SubLevelsAlwaysLoaded::UValidation_Level_SubLevelsAlwaysLoaded()
{
	ValidationName = TEXT("서브레벨 항상 로딩");
	ValidationDescription = TEXT("VP 프로젝트 작업 시 모든 서브레벨의 설정이 항상 로드됨으로 되어있어야 합니다.");
	FixDescription = TEXT("모든 서브레벨의 로딩 설정을 항상 로드됨으로 설정");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX,
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_Level_SubLevelsAlwaysLoaded::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	FString Message = "";

	const UWorld* CurrentWorld = GetCorrectValidationWorld();

	// Add models for each streaming level in the world
	for (const ULevelStreaming* StreamingLevel : CurrentWorld->GetStreamingLevels())
	{
		if (StreamingLevel)
		{
			const bool AlwaysLoaded = StreamingLevel->ShouldBeAlwaysLoaded();
			if (!AlwaysLoaded)
			{
				ValidationResult.Result = EValidationStatus::Fail;
				FName PackageName = StreamingLevel->GetWorldAssetPackageFName();
				Message += PackageName.ToString() + "\n";
			}
		}
	}
	

	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		ValidationResult.Message = UValidationTranslation::Valid();
	}
	else
	{
		ValidationResult.Message = TEXT("에러 - 다음 서브레벨들의 설정이 항상 로드됨으로 설정되어 있지 않음\n") + Message;
	}
	return ValidationResult;
}

FValidationFixResult UValidation_Level_SubLevelsAlwaysLoaded::Fix_Implementation()
{


	FValidationFixResult ValidationFixResult = FValidationFixResult();
	ValidationFixResult.Result = EValidationFixStatus::Fixed;
	FString Message = "";

	const UWorld* CurrentWorld = GetCorrectValidationWorld();
	
	// Add models for each streaming level in the world
	for (ULevelStreaming* StreamingLevel : CurrentWorld->GetStreamingLevels())
	{
		if (StreamingLevel)
		{
			const bool AlwaysLoaded = StreamingLevel->ShouldBeAlwaysLoaded();
			if (!AlwaysLoaded)
			{
				ValidationFixResult.Result = EValidationFixStatus::Fixed;
				UEditorLevelUtils::SetStreamingClassForLevel(
					StreamingLevel, ULevelStreamingAlwaysLoaded::StaticClass());
				FName PackageName = StreamingLevel->GetWorldAssetPackageFName();
				Message += PackageName.ToString() + TEXT(" 항상 로드됨으로 설정\n");
				
			}
		}
	}

	ValidationFixResult.Message = Message;

	return ValidationFixResult;
}

