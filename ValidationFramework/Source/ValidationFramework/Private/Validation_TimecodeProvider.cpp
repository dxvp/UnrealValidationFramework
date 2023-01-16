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


#include "Validation_TimecodeProvider.h"

#include "ValidationBPLibrary.h"
#include "VFProjectSettingsBase.h"
#include "Engine/TimecodeProvider.h"


UValidation_TimecodeProvider::UValidation_TimecodeProvider()
{
	ValidationName = 		TEXT("타임코드 검증");
	ValidationDescription = TEXT("타임코드가 프로젝트 설정에 유효한지 검증합니다.");
	FixDescription = 		TEXT("물리적 하드웨어 설정 변경 (타임코드 장비)이 포함될 수 있는 사용자의 직접 설정이 필요합니다.");
	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::SimulCam,
		EValidationWorkflow::ICVFX
	};
}

FValidationResult UValidation_TimecodeProvider::Validation_Implementation()
{
	FValidationResult VFResult = FValidationResult();
	VFResult.Result = EValidationStatus::Pass;
	VFResult.Message = TEXT("");
	
	// Check that we have a timecode provider configured in the project
	const FSoftClassPath SoftClassPath = GetDefault<UEngine>()->TimecodeProviderClassName;
	if (SoftClassPath.ToString().Len() == 0)
	{
		VFResult.Result = EValidationStatus::Fail;
		VFResult.Message = TEXT("프로젝트에 타임코드 설정이 되어 있지 않음");
	}

	// Check that if we are generating default timecode that it matches the validation project settings
	const FFrameRate GeneratedFrameRate = GetDefault<UEngine>()->GenerateDefaultTimecodeFrameRate;

	// Ensure we have validation framework settings configured in the project before continuing 
	UObject* Settings = UValidationBPLibrary::GetValidationFrameworkProjectSettings();
	if (Settings == nullptr)
	{
		VFResult.Result = EValidationStatus::Fail;
		VFResult.Message += TEXT("\n프로젝트에 타임코드 관련 넷플릭스 검증 플러그인 사전 설정이 되어 있지 않음");
		return VFResult;
	}

	// Check That The Default Generated FrameRate Is Matching The FrameRate For The Project
	const UVFProjectSettingsBase* ProjectSettings = Cast<UVFProjectSettingsBase>(Settings);
	const FFrameRate ProjectFrameRate = ProjectSettings->ProjectFrameRate;

	const EFrameRateComparisonStatus GeneratedFrameRateComparison = UValidationBPLibrary::CompareFrameRateCompatability(
		GeneratedFrameRate, ProjectFrameRate);

	if (GeneratedFrameRateComparison == EFrameRateComparisonStatus::InValid)
	{
		VFResult.Result = EValidationStatus::Fail;
		VFResult.Message += TEXT("\n프로젝트의 프레임레이트 설정이 언리얼 기본 타임코드 프레임레이트와 일치하지 않음");
	}
	

	// If We Have A Timecode Provider Check Its Rate Matches The Validation Framework Project Rate
	const UTimecodeProvider* TimecodeProvider = GEngine->GetTimecodeProvider();
	if (TimecodeProvider)
	{
		const FFrameRate TimecodeProviderRate = TimecodeProvider->GetFrameRate();
		const EFrameRateComparisonStatus TimecodeProviderRateComparison = UValidationBPLibrary::CompareFrameRateCompatability(
			TimecodeProviderRate, ProjectFrameRate);

		if (TimecodeProviderRateComparison == EFrameRateComparisonStatus::InValid)
		{
			VFResult.Result = EValidationStatus::Fail;
			VFResult.Message += TEXT("\n프로젝트의 프레임레이트 설정이 타임코드 장비의 프레임레이트 설정과 일치하지 않음");
		}
	}
	
	// If We Have A Timecode Provider Check Its Not A System Timecode Generator & Advise Not To Use It 
	if (TimecodeProvider)
	{
		if (TimecodeProvider->IsA(TimecodeProvider->StaticClass()))
		{
			if (VFResult.Result > EValidationStatus::Warning)
			{
				VFResult.Result = EValidationStatus::Warning;	
			}
			
        	VFResult.Message += TEXT("\n현재 사용중인 타임코드 설정은 PC의 시스템 시간에서 생성됩니다.");
			VFResult.Message += TEXT("\n대신 카메라, 사운드 등과 동기화되는 타임코드 생성기(마스터클록) 하드웨어 장비 연결을 사용하는 것이 좋습니다.");
			VFResult.Message += TEXT("\n아니면 모든 하드웨어 장비의 시간이 PTP 시스템으로 동기화되고 있는지 확인하여");
			VFResult.Message += TEXT("\n각 하드웨어의 타임코드와 정확히 동기화되도록 보장이 필요합니다.");
		}
	}
	

	if (VFResult.Result == EValidationStatus::Pass)
	{
		VFResult.Message = UValidationTranslation::Valid();
	}
	
	return VFResult;
}

FValidationFixResult UValidation_TimecodeProvider::Fix_Implementation() 
{
	UE_LOG(LogTemp, Warning, TEXT("Running UValidation_TimecodeProvider Fix") );
	FValidationFixResult ValidationFixResult = FValidationFixResult(
		EValidationFixStatus::ManualFix,
		TEXT("해당 테스트는 사용자가 수동으로 프로젝트와 하드웨어 설정을 완료하고 언리얼을 재부팅해야합니다"));
	return ValidationFixResult;
}