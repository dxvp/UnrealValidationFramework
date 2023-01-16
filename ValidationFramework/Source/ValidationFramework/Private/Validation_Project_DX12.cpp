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


#include "Validation_Project_DX12.h"
#include "ValidationBPLibrary.h"


UValidation_Project_DX12::UValidation_Project_DX12()
{
	ValidationName = TEXT("다이렉트 12 체크");
	ValidationDescription = TEXT("필수는 아니지만 EXR 재생 등 일부 작업의 경우 다이렉트12로 실행되어야 합니다. 언리얼 5.1의 프로젝트는 기본적으로 다이렉트12로 설정되므로 주의하는것이 좋습니다.");
	FixDescription = TEXT("프로젝트의 RHI설정을 다이렉트 12로 설정합니다. 에디터 재시작이 필요합니다.");
	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_Project_DX12::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	FString Message = "";
	
#if PLATFORM_WINDOWS
	if (!UValidationBPLibrary::CheckDefaultRHIIsDirectX12())
	{
		ValidationResult.Result = EValidationStatus::Warning;
		Message += "프로젝트 설정이 다이렉트12를 사용중이지 않습니다. EXR 재생 등의 문제가 발생할 수 있습니다.\n";
		
	}
	
	if (ValidationResult.Result == EValidationStatus::Pass)
	{
		Message = UValidationTranslation::Valid();
	}
	
	ValidationResult.Message = Message;
	return ValidationResult;
#else
	ValidationResult.Result = EValidationStatus::Warning;
	ValidationResult.Message = TEXT("다이렉트X는 윈도우즈에서만 사용 가능합니다");
	return ValidationResult;
#endif
	
}


FValidationFixResult UValidation_Project_DX12::Fix_Implementation() 
{
#if PLATFORM_WINDOWS
	FString Message = "";

	if (!UValidationBPLibrary::CheckDefaultRHIIsDirectX12())
	{
		if(UValidationBPLibrary::SetProjectRHIDirectX12())
		{
			Message += "기본 그래픽 RHI 세팅을 다이렉트 12로 변경\n";	
		}
	}
	
	return FValidationFixResult( EValidationFixStatus::Fixed, Message);
#else
	return FValidationFixResult( EValidationFixStatus::NotFixed,  "다이렉트X는 윈도우즈에서만 사용 가능합니다");
#endif
	
}
