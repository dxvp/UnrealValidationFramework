#include "Validation_PP_Project_Raytracing.h"

#include "Engine/RendererSettings.h"

UValidation_PP_Project_Raytracing::UValidation_PP_Project_Raytracing()
{
	ValidationName = 		TEXT("프로젝트 - 레이트레이싱 설정");
	ValidationDescription = TEXT("프로젝트 설정에 하드웨어 레이트레이싱이 활성화되고, 그림자와 스카이라이트가 비활성화되어있는지 확인\n ");
	FixDescription = 		TEXT("각 라이트와 레벨마다 레이트레이싱 설정을 유동적으로 변경하기 쉽게 프로젝트 기본 설정을 비활성화함");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_PP_Project_Raytracing::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	FString Message = TEXT("");

	const URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	if (!Settings->bEnableRayTracing)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message = TEXT("하드웨어 레이트레이싱이 비활성화 되어 있음.\n");
	}
	else if (Settings->bEnableRayTracingShadows)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message = TEXT("레이트레이싱 그림자가 프로젝트 설정에서 활성화되어 있음.\n");
	}
	else if (Settings->bEnableRayTracingSkylight_DEPRECATED)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message = TEXT("레이트레이싱 스카이라이트가 프로젝트 설정에서 활성화되어 있음.\n");
	}

	ValidationResult.Message = Message;
	return ValidationResult;
}

FValidationFixResult UValidation_PP_Project_Raytracing::Fix_Implementation() 
{
	URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	Settings->bEnableRayTracing = true;
	Settings->bEnableRayTracingShadows = false;
	Settings->bEnableRayTracingSkylight_DEPRECATED = false;
	Settings->SaveConfig();

	FString Message = TEXT("프로젝트 레이트레이싱 설정을 변경함. 퍼포스에 서밋 필요.");
	return FValidationFixResult( EValidationFixStatus::Fixed, Message);
}
