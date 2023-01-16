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


#include "Validation_Level_NDisplay_Mesh_UV_0_1.h"

#include "ValidationBPLibrary.h"


UValidation_Level_NDisplay_Mesh_UV_0_1::UValidation_Level_NDisplay_Mesh_UV_0_1()
{
	ValidationName = TEXT("NDisplay 메쉬의 UV 범위 0-1 확인");
	ValidationDescription = TEXT("nDisplay를 구축하는데 사용되는 메시의 UV는 0-1 공간에 있어야 합니다. 0-1 UV 공간 밖에 있는 버텍스는 nDisplay에 문제 및 아티팩트를 일으킵니다.");
	FixDescription = TEXT("아티스트가 직접 UV공간을 0-1 범위에 맞게 메쉬를 수정 후 다시 임포트해야합니다.");
	ValidationScope = EValidationScope::Level;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_Level_NDisplay_Mesh_UV_0_1::Validation_Implementation()
{
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	const UWorld* World = GetCorrectValidationWorld();
	const TFunction<EValidationStatus(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)> ProcessFunction =
		&UValidation_Level_NDisplay_Mesh_UV_0_1::ValidateUVs;
	
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

FValidationFixResult UValidation_Level_NDisplay_Mesh_UV_0_1::Fix_Implementation()
{
#if PLATFORM_WINDOWS || PLATFORM_LINUX
	FValidationFixResult Result = FValidationFixResult(EValidationFixStatus::Fixed, "");
	
	const UWorld* World = GetCorrectValidationWorld();
	const TFunction<EValidationStatus(UStaticMesh* StaticMesh, const int LodIndex, FString& Message)> ProcessFunction =
		&UValidation_Level_NDisplay_Mesh_UV_0_1::ValidateUVs;
	
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

EValidationStatus UValidation_Level_NDisplay_Mesh_UV_0_1::ValidateUVs(UStaticMesh* StaticMesh, const int LodIndex,
	FString& Message)
{

	EValidationStatus Result = EValidationStatus::Pass;
	const uint32 NumUVChannels = StaticMesh->GetNumUVChannels(LodIndex);
	const FStaticMeshLODResources& LODResource = StaticMesh->GetRenderData()->LODResources[LodIndex];
	const uint32 NumVerts = LODResource.VertexBuffers.StaticMeshVertexBuffer.GetNumVertices();
	for (uint32 UV = 0; UV < NumUVChannels; UV++)
	{
		for (uint32 i = 0; i < NumVerts; i++)
		{

			#if ENGINE_MAJOR_VERSION < 5
				const FVector2D UVPosition = LODResource.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, UV);
			#else
				const FVector2f UVPosition = LODResource.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, UV);
			#endif
			
			
			if (UVPosition.X > 1 || UVPosition.X < 0 || UVPosition.Y > 1 || UVPosition.Y < 0)
			{
				Result = EValidationStatus::Fail;
				Message += StaticMesh->GetPathName() + " LOD " + FString::FromInt(LodIndex) + TEXT(" 메쉬의 채널 ") + FString::FromInt(UV) + TEXT(" 번 UVs 범위가 0-1 밖입니다.\n");
				break;
			}
			
		}
	}

	return Result;
}
