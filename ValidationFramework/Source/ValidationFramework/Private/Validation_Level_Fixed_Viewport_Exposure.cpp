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


#include "Validation_Level_Fixed_Viewport_Exposure.h"



UValidation_Level_Fixed_Viewport_Exposure::UValidation_Level_Fixed_Viewport_Exposure()
{
	ValidationName = TEXT("에디터 뷰포트 노출 고정 설정 해제");
	ValidationDescription = TEXT("에디터 뷰포트 노출 설정값이 있으면 레벨의 포스트프로세스의 노출값을 덮어쓰는 문제가 있음.");
	FixDescription = TEXT("EV100 값을 1.0으로 설정하고 설정을 비활성화하여 인게임 로직을 사용하도록 되돌립니다.");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX,
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_Level_Fixed_Viewport_Exposure::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	
	const FEditorViewportClient* Client = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
	if (Client->ExposureSettings.bFixed)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		ValidationResult.Message = TEXT("뷰포트에 설정된 고정 노출값이 있습니다.");
	}
	

	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		ValidationResult.Message = UValidationTranslation::Valid();
	}
	return ValidationResult;
}

FValidationFixResult UValidation_Level_Fixed_Viewport_Exposure::Fix_Implementation()
{


	FValidationFixResult ValidationFixResult = FValidationFixResult();
	ValidationFixResult.Result = EValidationFixStatus::Fixed;
	FString Message = "";

	FEditorViewportClient* Client = static_cast<FEditorViewportClient*>(GEditor->GetActiveViewport()->GetClient());
	Client->ExposureSettings.bFixed = false;
	Client->ExposureSettings.FixedEV100 = 1.0f;

	ValidationFixResult.Message = Message;

	return ValidationFixResult;
}


