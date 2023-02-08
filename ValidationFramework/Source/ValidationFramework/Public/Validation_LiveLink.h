#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"

#include "Validation_LiveLink.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_LiveLink : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_LiveLink();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
