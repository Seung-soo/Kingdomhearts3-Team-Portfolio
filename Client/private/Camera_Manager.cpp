#include "stdafx.h"
#include "..\public\Camera_Manager.h"
#include "GameInstance.h"
#include "Camera_Perspective.h"
#include "CameraPoint.h"
#include "GameManager.h"
#include "GameObject.h"
#include "Player.h"

READY_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{
}


void CCamera_Manager::Load_Camera(char* pLoadFileName)
{
	strcpy_s(m_LoadFileName, pLoadFileName);

	// 데이터 로드하기 위해 파일이름 입력받기
	char SaveFilePath[MAX_PATH] = "../SaveFiles/Camera/";
	strcat_s(SaveFilePath, m_LoadFileName);
	_bstr_t wSaveFilePath(SaveFilePath);

	HANDLE hFile = CreateFile(wSaveFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSGBOX("Check File Name!");
		return;
	}

	// 리셋
	{
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
			pPosCameraPoint->Set_Dead(true);
		m_Position_CameraPoint.clear();
		m_iPositionIndex = 0;

		for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			pLookAtCameraPoint->Set_Dead(true);
		m_LookAt_CameraPoint.clear();
		m_iLookAtIndex = 0;

		for (auto& pViaCameraPoint : m_Via_CameraPoint)
			pViaCameraPoint->Set_Dead(true);
		m_Via_CameraPoint.clear();
		m_iViaIndex = 0;
	}

	// 세이브데이터에서 구조체 값 받아오기
	DWORD dwBytes = 0;
	CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
	ZeroMemory(&CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC));

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	while (true)
	{
		ReadFile(hFile, &CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC), &dwBytes, nullptr);

		if (dwBytes == 0)
			break;

		// 받아온 구조체 값으로 카메라포인트 생성
		CGameObject* pNewCameraPoint = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);

		if (CameraPointDesc.eType == CCameraPoint::CAMERAPOINTTYPE_POS)
		{
			m_Position_CameraPoint.push_back(pNewCameraPoint);
			++m_iPositionIndex;
		}
		else if (CameraPointDesc.eType == CCameraPoint::CAMERAPOINTTYPE_LOOKAT)
		{
			m_LookAt_CameraPoint.push_back(pNewCameraPoint);
			++m_iLookAtIndex;
		}
		else
		{
			m_Via_CameraPoint.push_back(pNewCameraPoint);
			++m_iViaIndex;
		}
	}
	CloseHandle(hFile);
}

void CCamera_Manager::F_Load_Camera(char * pLoadFileName)
{
	strcpy_s(m_LoadFileName, pLoadFileName);

	// 실행중일 땐 패스
	if (m_bPlayOn == true)
	{
		return;
	}

	// 데이터 로드하기 위해 파일이름 입력받기
	char SaveFilePath[MAX_PATH] = "../SaveFiles/Camera/FlashMode/";
	strcat_s(SaveFilePath, m_LoadFileName);
	_bstr_t wSaveFilePath(SaveFilePath);

	HANDLE hFile = CreateFile(wSaveFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSGBOX("Check File Name!");
		return;
	}

	// 리셋
	{
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
			pPosCameraPoint->Set_Dead(true);
		m_Position_CameraPoint.clear();
		m_iPositionIndex = 0;

		for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			pLookAtCameraPoint->Set_Dead(true);
		m_LookAt_CameraPoint.clear();
		m_iLookAtIndex = 0;

		for (auto& pViaCameraPoint : m_Via_CameraPoint)
			pViaCameraPoint->Set_Dead(true);
		m_Via_CameraPoint.clear();
		m_iViaIndex = 0;

		m_bPlayOn = false;
		m_bIsPlayEnd = false;
		m_iPlayIndex = 0;
	}

	// 세이브데이터에서 구조체 값 받아오기
	DWORD dwBytes = 0;
	CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
	ZeroMemory(&CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC));

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	while (true)
	{
		ReadFile(hFile, &CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC), &dwBytes, nullptr);

		if (dwBytes == 0)
			break;

		// 받아온 구조체 값으로 카메라포인트 생성
		CGameObject* pNewCameraPoint = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);

		if (CameraPointDesc.eType == CCameraPoint::CAMERAPOINTTYPE_POS)
		{
			m_Position_CameraPoint.push_back(pNewCameraPoint);
			++m_iPositionIndex;
		}
		else if (CameraPointDesc.eType == CCameraPoint::CAMERAPOINTTYPE_LOOKAT)
		{
			m_LookAt_CameraPoint.push_back(pNewCameraPoint);
			++m_iLookAtIndex;
		}
		else
		{
			MSGBOX("ERROR : VIA_POINT ERROR");
		}
	}
	CloseHandle(hFile);
}

void CCamera_Manager::Play_Start()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_bPlayOn)
	{
		// 플레이버튼 누르면 카메라의 현재 월드행렬 기록해두고 m_bPlayOn 켜서 시작
		CGameObject* pCamera = pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();
		m_OriginWorldMatrix = pCameraTransform->Get_WorldFloat4x4();

		// 보간 시작 위치를 0 번 포지션 카메라포인트의 위치로 설정 / 0 번 Look 카메라포인트를 바라봄
		m_vLerpStartPos = m_Position_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();
		pCameraTransform->LookAt(m_LookAt_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position());

		if (pCamera == nullptr)
		{
			BREAKPOINT;
			return;
		}
		m_bIsFlash = false;
		m_bPlayOn = true;

		GM->Set_UIRender(false);
		GM->Set_bCameraMove(true);
	}
}

void CCamera_Manager::F_Play_Start(_float fWaitTimeAcc)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_bPlayOn)
	{
		if (m_Position_CameraPoint.size() == 0 || m_LookAt_CameraPoint.size() == 0 || m_Position_CameraPoint.size() != m_LookAt_CameraPoint.size() || m_bPlayOn == true)
		{
			return;
		}
		CGameObject* pCamera = pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();
		m_OriginWorldMatrix = pCameraTransform->Get_WorldFloat4x4();

		// 시작
		m_fMaxWaitTime = fWaitTimeAcc;
		m_bIsFlash = true;
		m_bPlayOn = true;
	}
}

_bool CCamera_Manager::Play(_double TimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	m_bFinish = false;

	if (false == m_bIsFlash && true == m_bPlayOn)
	{
		// 플레이버튼 누르면 카메라의 현재 월드행렬 기록해두고 m_bPlayOn 켜서 시작
		CGameObject* pCamera = pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();

		// 이동할 위치, 경유 위치, 바라볼 위치
		_vector vPosTarget = m_Position_CameraPoint[m_iPlayIndex + 1]->Get_Transform()->Get_Position();
		_vector vViaTarget = m_Via_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();
		_vector vLookAtTarget = m_LookAt_CameraPoint[m_iPlayIndex + 1]->Get_Transform()->Get_Position();

		// 이중선형보간으로 위치 보간
		_vector vLerpEndPosition = XMVectorSetW(BezierCurve(m_vLerpStartPos, vViaTarget, vPosTarget, (_float)m_LerpTimeAcc), 1.f);
		pCameraTransform->Set_Position(vLerpEndPosition);

		// 선형보간으로 LookAt 보간
		pCameraTransform->Look_Target_Lerp(vLookAtTarget, (_float)m_LerpTimeAcc, TimeDelta);
		m_LerpTimeAcc += TimeDelta * m_LerpSpeed;

		// m_LerpTimeAcc 이 1.0 을 넘으면 목표위치를 넘게 되므로 플레이 종료
		if (m_LerpTimeAcc >= 1.0)
		{
			m_bIsPlayEnd = true;
			m_vLerpStartPos = pCameraTransform->Get_Position();
		}

		// 한 포인트로의 이동이 끝났으면 인덱스 증가해서 다음포인트로 가도록
		if (m_bIsPlayEnd == true)
		{
			// 모든 포인트로의 이동이 끝났으면 초기화
			if (m_iPlayIndex == m_Position_CameraPoint.size() - 2)
			{
				Reset();

				return false;
			}

			m_LerpTimeAcc = 0.00;
			++m_iPlayIndex;
			m_bIsPlayEnd = false;
		}
	}
	

	return true;
}

_bool CCamera_Manager::F_Play(_double TimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	m_bFinish = false;

	CGameObject* pCamera = pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Camera"), 0);
	CTransform* pCameraTransform = pCamera->Get_Transform();

	// m_bPlayOn 켜지면 시작
	if (true == m_bIsFlash && m_bPlayOn == true)
	{
		// 대기 시간 증가 시작
		m_fWaitTimeAcc += (_float)TimeDelta;

		// 이동할 위치, 바라볼 위치
		_vector vPosTarget = m_Position_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();
		_vector vLookAtTarget = m_LookAt_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();

		// 이동
		pCameraTransform->Set_Position(vPosTarget);
		pCameraTransform->LookAt(vLookAtTarget);

		if (m_fWaitTimeAcc >= m_fMaxWaitTime)
			m_bIsPlayEnd = true;

		// 한 포인트로의 이동이 끝났으면 인덱스 증가해서 다음포인트로 가도록
		if (m_bIsPlayEnd == true)
		{
			// 모든 포인트로의 이동이 끝났으면 초기화
			if (m_iPlayIndex == m_Position_CameraPoint.size() - 1)
			{
				Reset();
				return false;
			}

			++m_iPlayIndex;
			m_fWaitTimeAcc = 0.0f;
			m_bIsPlayEnd = false;
		}
	}

	return true;
}

void CCamera_Manager::Reset()
{
	for (auto& pPosCameraPoint : m_Position_CameraPoint)
		pPosCameraPoint->Set_Dead(true);
	m_Position_CameraPoint.clear();
	m_iPositionIndex = 0;

	for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
		pLookAtCameraPoint->Set_Dead(true);
	m_LookAt_CameraPoint.clear();
	m_iLookAtIndex = 0;

	for (auto& pViaCameraPoint : m_Via_CameraPoint)
		pViaCameraPoint->Set_Dead(true);
	m_Via_CameraPoint.clear();
	m_iViaIndex = 0;

	m_bPlayOn = false;
	m_bIsPlayEnd = false;
	m_iPlayIndex = 0;
	m_LerpTimeAcc = 0.00;
	m_fWaitTimeAcc = 0.0f;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pCamera = pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Camera"), 0);
	CTransform* pCameraTransform = pCamera->Get_Transform();

	pCameraTransform->Set_WorldMatrix(m_OriginWorldMatrix);
	GM->Set_CamaeraSceneEnd(true);
	static_cast<CCamera_Perspective*>(pCamera)->Set_StartScene(false);
	GM->Set_bCameraMove(false);
	GM->Get_Player()->Set_ControlMode(true);
	GM->Set_UIRender(true);

	m_bFinish = true;
}

void CCamera_Manager::Free()
{
	for (auto& pPosCameraPoint : m_Position_CameraPoint)
		pPosCameraPoint->Set_Dead(true);
	m_Position_CameraPoint.clear();

	for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
		pLookAtCameraPoint->Set_Dead(true);
	m_LookAt_CameraPoint.clear();

	for (auto& pViaCameraPoint : m_Via_CameraPoint)
		pViaCameraPoint->Set_Dead(true);
	m_Via_CameraPoint.clear();
}


