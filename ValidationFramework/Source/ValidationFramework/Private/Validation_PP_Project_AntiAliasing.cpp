#include "Validation_PP_Project_AntiAliasing.h"

#include "Engine/RendererSettings.h"

UValidation_PP_Project_AntiAliasing::UValidation_PP_Project_AntiAliasing()
{
	ValidationName = 		TEXT("프로젝트 - 안티앨리어싱 설정");
	ValidationDescription = TEXT("프로젝트가 환경, 비주얼 의도에 따라 적절한 AA 설정이 적용되었는지 확인");
	FixDescription = 		TEXT("작업자가 비주얼 의도에 맞는 AA설정이 사용되고 있는지 직접 확인 후 FIX를 누를 것.");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_PP_Project_AntiAliasing::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	FString Message = TEXT("");

	const URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	if (Settings->DefaultFeatureAntiAliasing == EAntiAliasingMethod::AAM_FXAA)
	{
		Message = TEXT("FXAA를 사용 중임.\n");
	}
	else if (Settings->DefaultFeatureAntiAliasing == EAntiAliasingMethod::AAM_TemporalAA)
	{
		Message = TEXT("TAA를 사용 중임.\n");
	}
	else if (Settings->DefaultFeatureAntiAliasing == EAntiAliasingMethod::AAM_MSAA)
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message = TEXT("MSAA를 사용 중임. MSAA는 프로젝트에서 사용되면 안됨 \n");
	}
	else if (Settings->DefaultFeatureAntiAliasing == EAntiAliasingMethod::AAM_TSR)
	{
		Message = TEXT("TSR을 사용 중임. 기본 AA 설정임. \n");
	}

	ValidationResult.Message = Message;
	return ValidationResult;
}

FValidationFixResult UValidation_PP_Project_AntiAliasing::Fix_Implementation() 
{
	const URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	FString Message = TEXT("");
	if (Settings->DefaultFeatureAntiAliasing == EAntiAliasingMethod::AAM_MSAA)
	{
		Message = TEXT("프로젝트의 AA 설정이 MSAA임\n");
		return FValidationFixResult( EValidationFixStatus::NotFixed, Message);
	}
	else
	{
		Message = TEXT("프로젝트의 AA 설정이 의도한 대로 되어 있음.\n");
	}

	return FValidationFixResult( EValidationFixStatus::Fixed, Message);
}
