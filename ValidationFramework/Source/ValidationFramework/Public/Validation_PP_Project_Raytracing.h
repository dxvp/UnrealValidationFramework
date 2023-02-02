#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"
#include "Validation_PP_Project_Raytracing.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_PP_Project_Raytracing : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_PP_Project_Raytracing();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
