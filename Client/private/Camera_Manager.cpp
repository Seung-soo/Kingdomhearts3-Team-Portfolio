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

	// ������ �ε��ϱ� ���� �����̸� �Է¹ޱ�
	char SaveFilePath[MAX_PATH] = "../SaveFiles/Camera/";
	strcat_s(SaveFilePath, m_LoadFileName);
	_bstr_t wSaveFilePath(SaveFilePath);

	HANDLE hFile = CreateFile(wSaveFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSGBOX("Check File Name!");
		return;
	}

	// ����
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

	// ���̺굥���Ϳ��� ����ü �� �޾ƿ���
	DWORD dwBytes = 0;
	CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
	ZeroMemory(&CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC));

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	while (true)
	{
		ReadFile(hFile, &CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC), &dwBytes, nullptr);

		if (dwBytes == 0)
			break;

		// �޾ƿ� ����ü ������ ī�޶�����Ʈ ����
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

	// �������� �� �н�
	if (m_bPlayOn == true)
	{
		return;
	}

	// ������ �ε��ϱ� ���� �����̸� �Է¹ޱ�
	char SaveFilePath[MAX_PATH] = "../SaveFiles/Camera/FlashMode/";
	strcat_s(SaveFilePath, m_LoadFileName);
	_bstr_t wSaveFilePath(SaveFilePath);

	HANDLE hFile = CreateFile(wSaveFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSGBOX("Check File Name!");
		return;
	}

	// ����
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

	// ���̺굥���Ϳ��� ����ü �� �޾ƿ���
	DWORD dwBytes = 0;
	CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
	ZeroMemory(&CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC));

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	while (true)
	{
		ReadFile(hFile, &CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC), &dwBytes, nullptr);

		if (dwBytes == 0)
			break;

		// �޾ƿ� ����ü ������ ī�޶�����Ʈ ����
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
		// �÷��̹�ư ������ ī�޶��� ���� ������� ����صΰ� m_bPlayOn �Ѽ� ����
		CGameObject* pCamera = pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();
		m_OriginWorldMatrix = pCameraTransform->Get_WorldFloat4x4();

		// ���� ���� ��ġ�� 0 �� ������ ī�޶�����Ʈ�� ��ġ�� ���� / 0 �� Look ī�޶�����Ʈ�� �ٶ�
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

		// ����
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
		// �÷��̹�ư ������ ī�޶��� ���� ������� ����صΰ� m_bPlayOn �Ѽ� ����
		CGameObject* pCamera = pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();

		// �̵��� ��ġ, ���� ��ġ, �ٶ� ��ġ
		_vector vPosTarget = m_Position_CameraPoint[m_iPlayIndex + 1]->Get_Transform()->Get_Position();
		_vector vViaTarget = m_Via_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();
		_vector vLookAtTarget = m_LookAt_CameraPoint[m_iPlayIndex + 1]->Get_Transform()->Get_Position();

		// ���߼����������� ��ġ ����
		_vector vLerpEndPosition = XMVectorSetW(BezierCurve(m_vLerpStartPos, vViaTarget, vPosTarget, (_float)m_LerpTimeAcc), 1.f);
		pCameraTransform->Set_Position(vLerpEndPosition);

		// ������������ LookAt ����
		pCameraTransform->Look_Target_Lerp(vLookAtTarget, (_float)m_LerpTimeAcc, TimeDelta);
		m_LerpTimeAcc += TimeDelta * m_LerpSpeed;

		// m_LerpTimeAcc �� 1.0 �� ������ ��ǥ��ġ�� �Ѱ� �ǹǷ� �÷��� ����
		if (m_LerpTimeAcc >= 1.0)
		{
			m_bIsPlayEnd = true;
			m_vLerpStartPos = pCameraTransform->Get_Position();
		}

		// �� ����Ʈ���� �̵��� �������� �ε��� �����ؼ� ��������Ʈ�� ������
		if (m_bIsPlayEnd == true)
		{
			// ��� ����Ʈ���� �̵��� �������� �ʱ�ȭ
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

	// m_bPlayOn ������ ����
	if (true == m_bIsFlash && m_bPlayOn == true)
	{
		// ��� �ð� ���� ����
		m_fWaitTimeAcc += (_float)TimeDelta;

		// �̵��� ��ġ, �ٶ� ��ġ
		_vector vPosTarget = m_Position_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();
		_vector vLookAtTarget = m_LookAt_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();

		// �̵�
		pCameraTransform->Set_Position(vPosTarget);
		pCameraTransform->LookAt(vLookAtTarget);

		if (m_fWaitTimeAcc >= m_fMaxWaitTime)
			m_bIsPlayEnd = true;

		// �� ����Ʈ���� �̵��� �������� �ε��� �����ؼ� ��������Ʈ�� ������
		if (m_bIsPlayEnd == true)
		{
			// ��� ����Ʈ���� �̵��� �������� �ʱ�ȭ
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


