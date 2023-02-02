#include "Validation_Lens_Presets.h"
#include "CineCameraComponent.h"

UValidation_Lens_Presets::UValidation_Lens_Presets()
{
	ValidationName = 		TEXT("프로젝트 - 렌즈 프리셋 확인");
	ValidationDescription = TEXT("언리얼 에디터의 BaseEngine.ini 파일에 추가 렌즈 프리셋이 적용되었는지 확인");
	FixDescription = 		TEXT("추가된 렌즈 프리셋을 확인 후, 없으면 사용자가 직접 BaseEngine.ini 업데이트가 필요.");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::VAD
	};
}

bool CheckLensPresets()
{	
	TArray<FNamedLensPreset> const& Presets = UCineCameraSettings::GetLensPresets();

	// stupid hack. currently we have 89 lens presets.
	bool has_sample = Presets.ContainsByPredicate([&](const FNamedLensPreset& preset)
	{
		if (preset.Name.Equals(TEXT("SuperPrime_35mm(S)")))
		{
			return true;
		}
		return false;
	});

	if (Presets.Num() < 20 || !has_sample)
	{
		return false;
	}

	return true;
}

FValidationResult UValidation_Lens_Presets::Validation_Implementation()
{
	FValidationResult ValidationResult = FValidationResult(EValidationStatus::Pass, "");
	FString Message = TEXT("");
	
	if (!CheckLensPresets())
	{
		ValidationResult.Result = EValidationStatus::Fail;
		Message += TEXT("엔진의 BaseEngine.ini 파일에 렌즈 프리셋이 없음.\n");
	}

	ValidationResult.Message = Message;
	return ValidationResult;
}

FValidationFixResult UValidation_Lens_Presets::Fix_Implementation() 
{
	if (!CheckLensPresets())
	{
		FString Message = TEXT("BaseEngine.ini 파일 업데이트 필요\n");
		return FValidationFixResult( EValidationFixStatus::NotFixed, Message);
	}
	return FValidationFixResult( EValidationFixStatus::Fixed, UValidationTranslation::Valid());
}
