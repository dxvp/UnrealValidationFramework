#include "Validation_Niagara_Deterministic.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

UValidation_Niagara_Deterministic::UValidation_Niagara_Deterministic()
{
	ValidationName = 		TEXT("나이아가라 - Emitter 설정");
	ValidationDescription = TEXT("나이아가라 컴퍼넌트의 Emitter 생성 설정이 Deterministic인지 확인합니다.");
	FixDescription = 		TEXT("모든 나이아가라 컴퍼넌트의 Emitter 생성 설정을 결정론적으로 수정함");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::VAD
	};
}

using EmitterRoots = std::tuple<AActor*, UNiagaraComponent*, FNiagaraEmitterHandle>;
TArray<EmitterRoots> GetDeterminismEmitters(UWorld* World)
{
	TArray<EmitterRoots> bad_emitters;
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), actors);

	const size_t actors_num = actors.Num();
	for (size_t i = 0; i < actors_num; i++)
	{
		TArray<UNiagaraComponent*> comps;
		actors[i]->GetComponents<UNiagaraComponent>(comps, false);

		const size_t comps_num = comps.Num();
		for (size_t ii = 0; ii < comps_num; ii++)
		{
			UNiagaraSystem* niagara = comps[ii]->GetAsset();
			TArray<FNiagaraEmitterHandle> emitters = niagara->GetEmitterHandles();

			const size_t emitters_num = emitters.Num();
			for (size_t iii = 0; iii < emitters_num; iii++)
			{
				FVersionedNiagaraEmitterData* emit = emitters[iii].GetEmitterData();
				if (!emit->bDeterminism)
				{
					bad_emitters.Add({actors[i], comps[ii], emitters[iii]});
				};
			}			
		}
	}

	return bad_emitters;
}

FValidationResult UValidation_Niagara_Deterministic::Validation_Implementation()
{
	TArray<EmitterRoots> emitters = GetDeterminismEmitters(GetCorrectValidationWorld());

	const size_t bademits_num = emitters.Num();
	if (bademits_num < 1)
	{
		return FValidationResult(EValidationStatus::Pass, TEXT("모든 나이아가라 Emitter 설정이 결정론적입니다."));
	}

	FString Message = TEXT("다음의 나이아가라 액터들의 RNG 설정이 결정론적이지 않습니다. : ");
	for (auto emit : emitters)
	{
		Message += FString::Printf(TEXT("%s->%s->%s\n"), 
			*std::get<AActor*>(emit)->GetHumanReadableName(),
			*std::get<UNiagaraComponent*>(emit)->GetReadableName(),
			*std::get<FNiagaraEmitterHandle>(emit).GetName().ToString());
	}

	return FValidationResult(EValidationStatus::Fail, Message);
}

FValidationFixResult UValidation_Niagara_Deterministic::Fix_Implementation() 
{
	TArray<EmitterRoots> emitters = GetDeterminismEmitters(GetCorrectValidationWorld());
	const size_t bademits_num = emitters.Num();
	if (bademits_num < 1)
	{
		return FValidationFixResult(EValidationFixStatus::Fixed, TEXT("모든 나이아가라 Emitter 설정이 결정론적입니다."));
	}

	FString Message = TEXT("다음 나이아가라 액터들의 Emitter 설정을 결정론적으로 바꿨습니다. 시드는 직접 설정 해 주세요. : ");
	for (auto emit : emitters)
	{
		Message += FString::Printf(TEXT("%s->%s->%s\n"), 
			*std::get<AActor*>(emit)->GetHumanReadableName(),
			*std::get<UNiagaraComponent*>(emit)->GetReadableName(),
			*std::get<FNiagaraEmitterHandle>(emit).GetName().ToString());

		std::get<FNiagaraEmitterHandle>(emit).GetEmitterData()->bDeterminism = true;
	}

	return FValidationFixResult(EValidationFixStatus::Fixed, Message);
}
