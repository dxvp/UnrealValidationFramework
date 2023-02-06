#include "Validation_MultiUserEdit.h"
#include "Kismet/GameplayStatics.h"

#include "IMultiUserClientModule.h"
#include "IConcertSyncClient.h"
#include "IConcertClient.h"

UValidation_MultiUserEdit::UValidation_MultiUserEdit()
{
	ValidationName = 		TEXT("멀티 유저 에디팅 확인");
	ValidationDescription = TEXT("멀티 유저 서버가 접속 가능한지, 접속된 상태인지 확인합니다.");
	FixDescription = 		TEXT("수동으로 서버 상태 확인과, 서버 접속이 가능하게 레벨을 다시 로딩하는 작업이 필요합니다.");

	ValidationScope = EValidationScope::Project;
	ValidationApplicableWorkflows = {
		EValidationWorkflow::ICVFX
	};
}

TSharedPtr<IConcertClientSession> GetClientSession(IMultiUserClientModule& MUServer)
{
	const TSharedPtr<IConcertSyncClient> SessionSyncClient = MUServer.GetClient();
	if (!SessionSyncClient)
		return nullptr;

	return SessionSyncClient->GetConcertClient()->GetCurrentSession();
}

FValidationResult UValidation_MultiUserEdit::Validation_Implementation()
{
	IMultiUserClientModule& MUServer = IMultiUserClientModule::Get();
	TSharedPtr<IConcertClientSession> Session = GetClientSession(MUServer);

	if (!Session.IsValid())
		return FValidationResult(EValidationStatus::Fail, *FString::Printf(TEXT("%hs: The ConcertSyncClient could not be found. Please check the output log for errors and try again."), __FUNCTION__));

	if (Session->GetConnectionStatus() == EConcertConnectionStatus::Connected)
	{
		return FValidationResult(EValidationStatus::Pass, TEXT("멀티유저서버에 접속된 상태입니다."));
	}
	else
	{
		if (MUServer.IsConcertServerRunning())
		{
			return FValidationResult(EValidationStatus::Fail, TEXT("멀티유저 서버가 로컬에 존재하나 접속되지 않은 상태입니다."));
		}
		else
		{
			return FValidationResult(EValidationStatus::Fail, TEXT("멀티유저 서버가 로컬에 실행된 상태가 아니고, 접속되지 않습니다."));
		}
	}
}

FValidationFixResult UValidation_MultiUserEdit::Fix_Implementation() 
{
	IMultiUserClientModule& MUServer = IMultiUserClientModule::Get();
	TSharedPtr<IConcertClientSession> Session = GetClientSession(MUServer);

	return FValidationFixResult(EValidationFixStatus::Fixed, TEXT("수동으로 서버 설정 변경이 필요합니다. 현재 멀티유저 서버 접속 정보 :\n%s"));
}
