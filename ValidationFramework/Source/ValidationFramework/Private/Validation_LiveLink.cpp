#include "Validation_LiveLink.h"
#include "Kismet/GameplayStatics.h"
#include "ILiveLinkClient.h"
#include "LiveLinkSourceSettings.h"
#include "LiveLinkSettings.h"
#include "LiveLinkPreset.h"

UValidation_LiveLink::UValidation_LiveLink()
{
	ValidationName = 		TEXT("라이브링크 활성화 확인");
	ValidationDescription = TEXT("카메라 트래킹을 위한 라이브링크 소켓이 활성화되어있는지 확인합니다");
	FixDescription = 		TEXT("프로젝트 기본 라이브링크 설정과 연결이 활성화되는지 체크 후 수정합니다. 수동 IP 설정이 필요할 수 있습니다.");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_LiveLink::Validation_Implementation()
{
	FString Message = TEXT("");
	EValidationStatus Status = EValidationStatus::Pass;

	IModularFeatures& ModularFeatures = IModularFeatures::Get();
	ILiveLinkClient* const LiveLinkClient = &ModularFeatures.GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);
	if (LiveLinkClient == nullptr)
		return FValidationResult(EValidationStatus::Fail, *FString::Printf(TEXT("%hs"), __FUNCTION__));

	auto Subjects = LiveLinkClient->GetSubjects(false, false);
	for (auto& Sub : Subjects)
	{
		FString ConnectionSetting = LiveLinkClient->GetSourceSettings(Sub.Source)->ConnectionString;
		FString SubjectName = Sub.SubjectName.ToString();

		UE_LOG(LogTemp, Display, TEXT("%s : %s"), 
			*SubjectName,
			*ConnectionSetting
		);

		if (!LiveLinkClient->IsSubjectValid(Sub))
		{
			Status = EValidationStatus::Warning;
			Message += TEXT("LiveLink 연결이 되지 않은 상태입니다.") + ConnectionSetting + TEXT(" : ") + ConnectionSetting;
		}
	}
	
	const ULiveLinkSettings* Settings = GetMutableDefault<ULiveLinkSettings>();
	if (!Settings->DefaultLiveLinkPreset.IsValid())
	{
		Message += TEXT("프로젝트 기본 라이브링크 프리셋 설정이 없습니다.");
		Status = EValidationStatus::Fail;
	}

	return FValidationResult(Status, Message);
}

FValidationFixResult UValidation_LiveLink::Fix_Implementation() 
{
	const ULiveLinkSettings* Settings = GetMutableDefault<ULiveLinkSettings>();
	if (!Settings->DefaultLiveLinkPreset.IsValid())
	{
		return FValidationFixResult(EValidationFixStatus::ManualFix, TEXT("프로젝트 기본 라이브링크 프리셋의 수동 설정이 필요합니다"));
	}

	return FValidationFixResult(EValidationFixStatus::ManualFix, TEXT("라이브링크 연결이 되지 않은 상태인 경우 수동으로 설정 필요합니다."));
}
