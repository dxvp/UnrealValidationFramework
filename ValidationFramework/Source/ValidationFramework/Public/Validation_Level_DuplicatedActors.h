#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"

#include "Validation_Level_DuplicatedActors.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_Level_DuplicatedActors : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_Level_DuplicatedActors();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
