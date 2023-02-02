#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"
#include "Validation_PP_Project_AntiAliasing.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_PP_Project_AntiAliasing : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_PP_Project_AntiAliasing();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
