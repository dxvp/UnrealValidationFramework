#include "Validation_DDC.h"
#include "Engine/RendererSettings.h"
#include "DerivedDataCacheModule.h"
#include "DerivedDataCacheInterface.h"

UValidation_DDC::UValidation_DDC()
{
	ValidationName = 		TEXT("프로젝트 - DDC 설정");
	ValidationDescription = TEXT("언리얼 에디터에 DDC 설정이 올바르게 되어있는지 확인합니다. X: 드라이브 설정인지, 로컬 설정인지 확인합니다");
	FixDescription = 		TEXT("DDC 설정은 사용자가 수동으로 BaseEngine.ini 파일이나 X드라이브 설정이 필요합니다.");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::VAD
	};
}

TArray<FString> GetDDCDirectories()
{
	IDerivedDataCacheModule* Module = FModuleManager::LoadModulePtr<IDerivedDataCacheModule>("DerivedDataCache");	
	FDerivedDataCacheInterface* const* cache = Module->CreateOrGetCache();

	TArray<FString> dirs;
	(*cache)->GetDirectories(dirs);
	return dirs;
}

FValidationResult UValidation_DDC::Validation_Implementation()
{
	TArray<FString> dirs = GetDDCDirectories();
	if (dirs.Num() < 1)
	{
		return FValidationResult(EValidationStatus::Fail, TEXT("DDC 드라이브를 찾지 못했음. 쉐이더 컴파일에 실패함. 수동으로 PC의 드라이브 설정이 필요."));
	}

	FString Message = TEXT("DDC 드라이브가 설정됨 : \n");
	for (size_t i = 0; i < dirs.Num(); i++)
	{
		Message += dirs[i];
		Message += '\n';
	};
	return FValidationResult(EValidationStatus::Pass, Message);
}

FValidationFixResult UValidation_DDC::Fix_Implementation() 
{
	FString Message = TEXT("설정된 DDC 드라이브는 다음과 같음. 환경에 따라 X: 드라이브로 설정되어있는지 확인하세요. \n");

	TArray<FString> dirs = GetDDCDirectories();
	if (dirs.Num() < 1)
	{
		return FValidationFixResult(EValidationFixStatus::NotFixed, TEXT("DDC 드라이브를 찾지 못했음. 쉐이더 컴파일에 실패함. 수동으로 PC의 드라이브 설정이 필요."));;
	}
	
	return FValidationFixResult( EValidationFixStatus::Fixed, Message);
}
