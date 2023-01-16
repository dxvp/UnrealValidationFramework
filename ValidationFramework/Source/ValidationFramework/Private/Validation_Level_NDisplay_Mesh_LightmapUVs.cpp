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


#include "Validation_Level_NDisplay_Mesh_LightmapUVs.h"
#include "ValidationBPLibrary.h"


UValidation_Level_NDisplay_Mesh_LightmapUVs::UValidation_Level_NDisplay_Mesh_LightmapUVs()
{
	ValidationName = TEXT("NDisplay - 라이트맵 UV 유무 확인");
	ValidationDescription = TEXT("nDisplay를 구성하는 데 사용되는 모든 메시에는 라이트맵 UV 채널이 없어야 합니다. 메시에는 2개의 UV 채널만 있어야 하며 추가된 UV는 비주얼 상 문제를 일으킬 수 있습니다.");
	FixDescription = TEXT("작업자가 직접 라이트맵 UV가 비활성화된 메쉬로 다시 임포트해야 합니다.");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_Level_NDisplay_Mesh_LightmapUVs::Validation_Implementation()
{
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	const UWorld* World = GetCorrectValidationWorld();
	const TFunction<EValidationStatus(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)> ProcessFunction =
		&UValidation_Level_NDisplay_Mesh_LightmapUVs::ValidateLightmapUVs;
	
	FValidationResult Result =  UValidationBPLibrary::NDisplayMeshSettingsValidation(
		World, ProcessFunction);
	
	return Result;
#endif

#if PLATFORM_MAC
	FValidationResult ValidationResult = FValidationResult();
	ValidationResult.Result = EValidationStatus::Warning;
	ValidationResult.Message = UValidationTranslation::NoOSX();
	return ValidationResult;
#endif
}

FValidationFixResult UValidation_Level_NDisplay_Mesh_LightmapUVs::Fix_Implementation()
{
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	FValidationFixResult Result = FValidationFixResult(EValidationFixStatus::Fixed, "");
	
	const UWorld* World = GetCorrectValidationWorld();
	const TFunction<EValidationStatus(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)> ProcessFunction =
		&UValidation_Level_NDisplay_Mesh_LightmapUVs::ValidateLightmapUVs;
	
	FValidationResult VResult =  UValidationBPLibrary::NDisplayMeshSettingsValidation(
		World, ProcessFunction);

	if (VResult.Result != EValidationStatus::Pass)
	{
		Result.Result = EValidationFixStatus::ManualFix;
		Result.Message = VResult.Message;
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

EValidationStatus UValidation_Level_NDisplay_Mesh_LightmapUVs::ValidateLightmapUVs(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)
{
	const FStaticMeshSourceModel& LODModel = StaticMesh->GetSourceModel(LodIndex);
	if (LODModel.BuildSettings.bGenerateLightmapUVs)
	{
		Message = StaticMesh->GetPathName() + " LOD " + FString::FromInt(LodIndex) + TEXT(" 번 메쉬의 라이트맵 UV가 활성화되어 있습니다. 비활성화 후 다시 임포트가 필요합니다.\n");
		return EValidationStatus::Fail;
	}

	return EValidationStatus::Pass;
}