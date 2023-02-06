#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"

#include "Validation_Level_NDisplay_ZeroPoint.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_Level_NDisplay_ZeroPoint : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_Level_NDisplay_ZeroPoint();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
