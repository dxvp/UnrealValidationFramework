#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"

#include "Validation_DDC.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_DDC : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_DDC();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
