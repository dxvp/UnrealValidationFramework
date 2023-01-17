/**
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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Validation_Translation.generated.h"

UCLASS(Blueprintable)
class VALIDATIONFRAMEWORK_API UValidationTranslation : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category="Validation Translation Support")
	static inline FString const Valid()
	{
		return TEXT("확인");
	}

	UFUNCTION(BlueprintPure, Category="Validation Translation Support")
	static inline FString const NoOSX()
	{
		return TEXT("NDisplay 유효성 검증은 OSX에서 동작하지 않습니다.");
	}
};
