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


#include "Validation_PP_Project_Exposure.h"

#include "Engine/RendererSettings.h"

UValidation_PP_Project_Exposure::UValidation_PP_Project_Exposure()
{
	ValidationName = 		TEXT("포스트 프로세스 - 자동 노출 끄기");
	ValidationDescription = TEXT("자동 노출을 비활성화하고 컬러 파이프라인을 방해하지 않도록 ICVFX에서 노출이 수동으로 설정되었는지 확인하기 위한 것입니다");
	FixDescription = 		TEXT("자동 노출을 비활성화하고 수동으로 설정, 바이어스를 0으로 설정합니다.");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_PP_Project_Exposure::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	FString Message = TEXT("");

	const URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	if (Settings->bDefaultFeatureAutoExposure)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message += "프로젝트 설정에서 자동 노출이 활성화 되어 있음.\n";
		
	}

	if (Settings->DefaultFeatureAutoExposure != EAutoExposureMethodUI::AEM_Manual)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message += "프로젝트 설정에서 자동 노출이 수동으로 설정되지 않음\n";
		
	}
	
	if (Settings->DefaultFeatureAutoExposureBias != 0.0)
    {
    	ValidationResult.Result = EValidationStatus::Fail;
    	Message += "자동 노출의 값은 0으로 설정되어 있어야 합니다.\n";
    	
    }

	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		Message = UValidationTranslation::Valid();
	}
	
	ValidationResult.Message = Message;
	return ValidationResult;
}

FValidationFixResult UValidation_PP_Project_Exposure::Fix_Implementation() 
{
	FString Message = TEXT("");

	URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	if (Settings->bDefaultFeatureAutoExposure)
	{
		Settings->bDefaultFeatureAutoExposure = false;
		Message += "자동 노출이 프로젝트 설정에서 꺼져 있음\n";
		
	}

	if (Settings->DefaultFeatureAutoExposure != EAutoExposureMethodUI::AEM_Manual)
	{
		Settings->DefaultFeatureAutoExposure = EAutoExposureMethodUI::AEM_Manual;
		Message += "자동 노출이 프로젝트 설정에서 수동으로 설정되 있음\n";
		
	}
	
	if (Settings->DefaultFeatureAutoExposureBias != 0.0)
	{
		Settings->DefaultFeatureAutoExposureBias = 0.0;
		Message += "자동 노출의 값이 0으로 설정되어 있음\n";
    	
	}
	Settings->SaveConfig();
	
	return FValidationFixResult( EValidationFixStatus::Fixed, Message);
}


