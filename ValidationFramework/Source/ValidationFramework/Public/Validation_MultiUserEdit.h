#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"

#include "Validation_MultiUserEdit.generated.h"

UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_MultiUserEdit : public UValidationBase
{
	GENERATED_BODY()
public:
	UValidation_MultiUserEdit();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
};
