﻿/**
Copyright 2022 Netflix, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "CoreMinimal.h"
#include "ValidationBase.h"
#include "Validation_PP_Project_Exposure.generated.h"

/**
* Validation which checks that the exposure default settings within the ue project are valid for ICVFX workflows.
*/
UCLASS()
class VALIDATIONFRAMEWORK_API UValidation_PP_Project_Exposure final : public UValidationBase
{
	GENERATED_BODY()

public:
	UValidation_PP_Project_Exposure();
	virtual FValidationResult Validation_Implementation() override;
	
	virtual FValidationFixResult Fix_Implementation() override;
};

