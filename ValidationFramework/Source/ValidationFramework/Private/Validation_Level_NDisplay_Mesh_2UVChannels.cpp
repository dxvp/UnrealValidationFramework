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

#include "Validation_Level_NDisplay_Mesh_2UVChannels.h"
#include "ValidationBPLibrary.h"


UValidation_Level_NDisplay_Mesh_2UVChannels::UValidation_Level_NDisplay_Mesh_2UVChannels()
{
	ValidationName = TEXT("NDisplay - UV채널 2개 확인");
	ValidationDescription = TEXT("nDisplay를 구성하는데 사용되는 모든 메시에는 두개의 UV 채널이 있어야 합니다. UV가 정확한지 확인하지는 않지만 최소한 누락되지는 않았는지 확인합니다.");
	FixDescription = TEXT("작업자가 직접 메쉬를 수정 후 다시 임포트해야 합니다");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_Level_NDisplay_Mesh_2UVChannels::Validation_Implementation()
{
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	const UWorld* World = GetCorrectValidationWorld();
	const TFunction<EValidationStatus(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)> ProcessFunction =
		&UValidation_Level_NDisplay_Mesh_2UVChannels::ValidateMeshesWithout2UVChannels;
	
	FValidationResult Result =  UValidationBPLibrary::NDisplayMeshSettingsValidation(
		World, ProcessFunction);

	if (Result.Result == EValidationStatus::Fail)
	{
		Result.Message += TEXT("메시에 2개의 UV 채널만 있는지 확인 필요");
	}
	
	return Result;
#endif

#if PLATFORM_MAC
	FValidationResult ValidationResult = FValidationResult();
	ValidationResult.Result = EValidationStatus::Warning;
	ValidationResult.Message = UValidationTranslation::NoOSX();
	return ValidationResult;
#endif
}

FValidationFixResult UValidation_Level_NDisplay_Mesh_2UVChannels::Fix_Implementation()
{
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	FValidationFixResult Result = FValidationFixResult(EValidationFixStatus::Fixed, "");
	
	const UWorld* World = GetCorrectValidationWorld();
	const TFunction<EValidationStatus(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)> ProcessFunction =
		&UValidation_Level_NDisplay_Mesh_2UVChannels::ValidateMeshesWithout2UVChannels;
	
	FValidationResult VResult =  UValidationBPLibrary::NDisplayMeshSettingsValidation(
		World, ProcessFunction);

	if (VResult.Result != EValidationStatus::Pass)
	{
		Result.Result = EValidationFixStatus::ManualFix;
		Result.Message = VResult.Message;
		Result.Message += TEXT("LightMass UV 자동 생성이 꺼져 있고, 메시에 2개의 UV 채널만 있는지 확인");
	}
	return Result;
#endif

#if PLATFORM_MAC
	FValidationFixResult ValidationFixResult = FValidationFixResult();
	ValidationFixResult.Result = EValidationFixStatus::NotFixed;
	ValidationFixResult.Message = UValidationTranslation::NoOSX();
	return ValidationFixResult;
#endif
}

EValidationStatus UValidation_Level_NDisplay_Mesh_2UVChannels::ValidateMeshesWithout2UVChannels(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)
{
	const int NumUVChannels = StaticMesh->GetNumUVChannels(LodIndex);
	if (NumUVChannels != 2)
	{
		Message = StaticMesh->GetPathName() + " LOD " + FString::FromInt(LodIndex) + TEXT(" 메시에 UV 채널이 두 개가 아닌 ") + FString::FromInt(NumUVChannels) + TEXT(" 개가 있음\n");
		return EValidationStatus::Fail;
	}

	return EValidationStatus::Pass;
}

