#include "Validation_PP_Project_StaticLighting.h"

#include "Engine/RendererSettings.h"

UValidation_PP_Project_StaticLighting::UValidation_PP_Project_StaticLighting()
{
	ValidationName = 		TEXT("프로젝트 - 스태틱 라이팅 설정");
	ValidationDescription = TEXT("프로젝트가 Baked된 라이트맵을 사용해야 하는 경우 프로젝트 설정에서 Static Lighting이 활성화되어 있어야 함.");
	FixDescription = 		TEXT("프로젝트 의도에 따라 프로젝트 설정에서 Static Lighting을 활성화해야 되는지 작업자가 직접 확인");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_PP_Project_StaticLighting::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Fail, "");
	FString Message = TEXT("");

	const URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	if (Settings->bAllowStaticLighting)
	{
		ValidationResult.Result = EValidationStatus::Pass;
		Message = TEXT("프로젝트 설정에서 스태틱 라이팅이 활성화 되어 있음.\n");
	}
	else
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message = TEXT("프로젝트 설정에서 스태틱 라이팅이 비활성화 되어 있음.\n");
	}
	
	ValidationResult.Message = Message;
	return ValidationResult;
}

FValidationFixResult UValidation_PP_Project_StaticLighting::Fix_Implementation() 
{
	URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	FString Message = TEXT("");
	if (!Settings->bAllowStaticLighting)
	{
		Settings->bAllowStaticLighting = true;
		Message = TEXT("프로젝트 설정에서 스태틱 라이팅을 활성화했음. 퍼포스 서밋 후 다시 확인 필요.\n");
		Settings->SaveConfig();
	}

	return FValidationFixResult( EValidationFixStatus::Fixed, Message);
}
