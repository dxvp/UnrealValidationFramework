#include "Validation_Level_NDisplay_ZeroPoint.h"

UValidation_Level_NDisplay_ZeroPoint::UValidation_Level_NDisplay_ZeroPoint()
{
	ValidationName = TEXT("nDisplay 제로포인트 확인");
	ValidationDescription = TEXT("VP 프로젝트 작업 시 nDisplay 액터가 0, 0, 0 위치에 있어야 합니다.\n연출에 따라 그럴 수 없는 경우도 있으니 넘기세요.");
	FixDescription = TEXT("nDisplay 액터 위치를 0, 0, 0으로 설정");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX,
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_Level_NDisplay_ZeroPoint::Validation_Implementation()
{
	ADisplayClusterRootActor* const root = UValidationBPLibrary::GetCurrentLevelNDisplayActor(GetCorrectValidationWorld());
	
	if (!root)
		return FValidationResult(EValidationStatus::Fail, TEXT("레벨에 nDisplay 액터가 두개 이상 있습니다."));

	FTransform tf = root->GetActorTransform();
	if (tf.GetLocation().Equals(FVector::ZeroVector, 1.0f) && tf.GetRotation().Equals(FQuat::Identity, 1.0f))
	{
		return FValidationResult(EValidationStatus::Pass, TEXT("nDisplay 액터가 제로포인트에 있습니다."));
	}

	return FValidationResult(EValidationStatus::Fail, TEXT("nDisplay 액터가 제로포인트에 있지 않습니다."));
}

FValidationFixResult UValidation_Level_NDisplay_ZeroPoint::Fix_Implementation()
{
	ADisplayClusterRootActor* const root = UValidationBPLibrary::GetCurrentLevelNDisplayActor(GetCorrectValidationWorld());
	
	if (!root)
		return FValidationFixResult(EValidationFixStatus::ManualFix, TEXT("레벨에 nDisplay 액터가 두개 이상 있습니다."));

	FTransform tf;
	root->SetActorTransform(tf);

	UValidationBPLibrary::MarkCurrentLevelDirty();
	return FValidationFixResult(EValidationFixStatus::Fixed, TEXT("nDisplay 액터의 위치를 제로포인트로 변경함."));
}

