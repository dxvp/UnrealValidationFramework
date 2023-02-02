#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"

#include "Validation_Niagara_Deterministic.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_Niagara_Deterministic : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_Niagara_Deterministic();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
