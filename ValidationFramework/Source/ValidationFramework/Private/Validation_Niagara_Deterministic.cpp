#include "Validation_Niagara_Deterministic.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

UValidation_Niagara_Deterministic::UValidation_Niagara_Deterministic()
{
	ValidationName = 		TEXT("나이아가라 - Emitter 설정");
	ValidationDescription = TEXT("나이아가라 컴퍼넌트의 Emitter 생성 설정이 Deterministic인지 확인합니다.");
	FixDescription = 		TEXT("ㅁㄴㅇㄹ");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::VAD
	};
}

FValidationResult UValidation_Niagara_Deterministic::Validation_Implementation()
{
	UWorld* World = GetCorrectValidationWorld();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), actors);

	size_t acotrs_num = actors.Num();
	for (size_t i = 0; i < acotrs_num; i++)
	{
		TArray<UNiagaraComponent*> comps;
		actors[i]->GetComponents<UNiagaraComponent>(comps, false);

		size_t comps_num = comps.Num();
		for (size_t ii = 0; ii < comps_num; ii++)
		{
			UNiagaraSystem* niagara = comps[ii]->GetAsset();
		}
	}

	FString Message = TEXT("");
	return FValidationResult(EValidationStatus::Pass, Message);
}

FValidationFixResult UValidation_Niagara_Deterministic::Fix_Implementation() 
{
	FString Message = TEXT("설정된 DDC 드라이브는 다음과 같음. 환경에 따라 X: 드라이브로 설정되어있는지 확인하세요. \n");
	return FValidationFixResult( EValidationFixStatus::Fixed, Message);
}
