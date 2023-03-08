#include "Validation_Level_DuplicatedActors.h"
#include "Engine/PostProcessVolume.h"

UValidation_Level_DuplicatedActors::UValidation_Level_DuplicatedActors()
{
	ValidationName = TEXT("중복되는 액터들 확인");
	ValidationDescription = TEXT("작업 시 중복되면 안 되는 액터를 확인합니다. 포스트프로세스, NDisplay, Skylight 등이 대상입니다. 프로젝트 설정에서 중복되면 안되는 액터 종류를 추가할 수 있습니다.");
	FixDescription = TEXT("중복되는 액터를 확인 후 작업자가 수동으로 제거해주세요");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = 
	{
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_Level_DuplicatedActors::Validation_Implementation()
{
	UWorld* world = GetCorrectValidationWorld();
	FString message = TEXT("");
	EValidationStatus status = EValidationStatus::Pass;

	UObject* settings = UValidationBPLibrary::GetValidationFrameworkProjectSettings();
	if (settings == nullptr)
		return FValidationResult(EValidationStatus::Fail, TEXT("\n유효성 검사 플러그인의 사전 설정파일이 설정되지 않음"));

	const UVFProjectSettingsBase* project_settings = Cast<UVFProjectSettingsBase>(settings);

	for (auto actortype : project_settings->DuplicationCheckActors)
	{
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(world, actortype.Get(), actors);

		if (actors.Num() > 1)
		{
			if (actortype.Get() == APostProcessVolume::StaticClass())
			{
				int32 unbound_dup = 0;
				FString unbounded_actormsg;
				for (AActor* actor : actors)
				{
					if (CastChecked<APostProcessVolume>(actor)->GetProperties().bIsUnbound)
					{
						unbound_dup++;
						unbounded_actormsg += FString::Printf(TEXT("범위가 Unbound된 PostProcessVolume이 있습니다 : %s \n"), *actor->GetName());
					}
				}

				if (unbound_dup > 1)
				{
					message += unbounded_actormsg;
					status = EValidationStatus::Fail;
				}
			}
			else
			{
				status = EValidationStatus::Fail;

				for (AActor* actor : actors)
					message += FString::Printf(TEXT("%s \n"), *actor->GetName());
			}
		}
	}

	if (status == EValidationStatus::Fail)
	{
		message += FString::Printf(TEXT(" 가 레벨에 두개 이상 있습니다 \n"));
	}

	return FValidationResult(status, message);
}

FValidationFixResult UValidation_Level_DuplicatedActors::Fix_Implementation()
{
	return FValidationFixResult(EValidationFixStatus::ManualFix, TEXT("작업자가 직접 중복되는 액터들을 삭제 혹은 설정해주세요."));
}

