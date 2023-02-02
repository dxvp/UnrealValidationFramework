#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"
#include "Validation_Lens_Presets.generated.h"

/**
* Validation which checks that unreal is using DX12 for ICVFX workflows, whilst not critical for all workflows
* It is a hard dependency for the EXR plate playback
*/
UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_Lens_Presets : public UValidationBase
{
	GENERATED_BODY()

public:
	UValidation_Lens_Presets();
	virtual FValidationResult Validation_Implementation() override;
	virtual FValidationFixResult Fix_Implementation() override;
	
};


