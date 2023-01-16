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


#include "Validation_PP_Project_CorruptDefaultLevel.h"
#include "EditorAssetLibrary.h"
#include "GameMapsSettings.h"


UValidation_PP_Project_CorruptDefaultLevel::UValidation_PP_Project_CorruptDefaultLevel()
{
	ValidationName = TEXT("프로젝트 기본 레벨 설정 깨짐");

	ValidationDescription = TEXT("가끔 기본 레벨 설정이 사라져서 없는 경우 NDisplay가 터지는 문제가 있습니다.");

	FixDescription = TEXT("작업자가 직접 프로젝트 설정에 들어가 기본 레벨을 설정해 주어야 합니다.");
	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_PP_Project_CorruptDefaultLevel::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	FString Message = "";

	const UGameMapsSettings* Settings = GetMutableDefault<UGameMapsSettings>();
	const FString DefaultGameMap = Settings->GetGameDefaultMap();
	if (DefaultGameMap.Len() != 0)
	{
		const bool bGameMapExists = UEditorAssetLibrary::DoesAssetExist(DefaultGameMap);
		if(!bGameMapExists){
			ValidationResult.Result = EValidationStatus::Fail;
			Message += TEXT("기본 인게임 맵 설정이 프로젝트에 존재하지 않습니다.\n");
			
		}
	}

	const FSoftObjectPath EditorStartupMap = Settings->EditorStartupMap;
	const FString EditorStartupMapString = EditorStartupMap.GetAssetPathString();
	if (EditorStartupMapString.Len() != 0)
	{
		const bool bEditorMapExists = UEditorAssetLibrary::DoesAssetExist(EditorStartupMapString);
		if(!bEditorMapExists){
			ValidationResult.Result = EValidationStatus::Fail;
			Message += TEXT("기본 에디터 시작 맵 설정이 프로젝트에 존재하지 않습니다.\n");
			
		}
	}

	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		ValidationResult.Message = UValidationTranslation::Valid();
	}
	else
	{
		ValidationResult.Message = TEXT("에러 - 사용자가 직접 설정이 필요함\n") + Message;
	}
	return ValidationResult;
}

FValidationFixResult UValidation_PP_Project_CorruptDefaultLevel::Fix_Implementation()
{
	FValidationFixResult ValidationFixResult = FValidationFixResult();
	ValidationFixResult.Result = EValidationFixStatus::ManualFix;
	FString Message = TEXT("기본 레벨이 설정되어 있지 않아 직접 설정이 필요함");
	return ValidationFixResult;
}
