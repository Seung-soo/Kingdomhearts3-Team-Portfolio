#include "stdafx.h"
#include "CameraTool.h"
#include "ImGui_Manager.h"
#include "CameraPoint.h"
#include "PickingPoint.h"

READY_SINGLETON(CCameraTool)

CCameraTool::CCameraTool()
{
}

#ifdef _DEBUG
HRESULT CCameraTool::CCameraTool_SetUp() 
{
	// 멤버변수 초기화
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = m_pGameInstance->Get_Device();
	m_pDeviceContext = m_pGameInstance->Get_Device_Context();
	m_pImGui_Manager = CImGui_Manager::GetInstance();
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pImGui_Manager);

	// m_pGuidePoint 에 가이드용 피킹포인트 저장 
	m_pGuidePoint = m_pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("Layer_GuidePoint"), 0);

	if (nullptr == m_pGameInstance || nullptr == m_pDevice ||
		nullptr == m_pDeviceContext || nullptr == m_pImGui_Manager)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CCameraTool::CCameraTool_Tick(_double TimeDelta)
{
	if (true == CImGui_Manager::GetInstance()->show_cameratool_window)
	{
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::Begin("CameraTool Window");

		// 가이드포인트 위치 업데이트
		UpdateGuidePoint();
		if (ImGui::BeginTabBar("Select Type", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Mode : Bezier"))
			{
				// 디버그 창 렌더링
				RenderDebug(TimeDelta);

				// 카메라포인트 설치 & 수정 & 제거
				SetCameraPoint(TimeDelta);
				ModifyCameraPoint(TimeDelta);
				DeleteCameraPoint();

				// 실행버튼
				Play(TimeDelta);

				// 리셋버튼
				Reset();

				// 세이브로드버튼
				SaveLoad();

				// 오토세이브
				AutoSave(TimeDelta);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Mode : Flash"))
			{
				// 디버그 창 렌더링
				F_RenderDebug(TimeDelta);

				// 한 포인트에서 멈춰있을 시간 설정
				F_SetWaitTime(TimeDelta);
				m_pImGui_Manager->SmallSeparator();

				// 카메라포인트 설치 & 수정 & 제거
				F_SetCameraPoint(TimeDelta);
				F_ModifyCameraPoint(TimeDelta);
				F_DeleteCameraPoint();

				// 실행버튼
				F_Play(TimeDelta);

				// 리셋버튼
				Reset();

				// 세이브로드버튼
				F_SaveLoad();

				// 오토세이브
				F_AutoSave(TimeDelta);

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
	return RESULT_NOPROBLEM;

}

HRESULT CCameraTool::CCameraTool_Render()
{
	return S_OK;
}

void CCameraTool::UpdateGuidePoint()
{
	// Ctrl + E 로 m_GuidePointY 올리기
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_E))
		m_GuidePointY += 0.1f;

	// Ctrl + Q 로 m_GuidePointY 내리기
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_Q))
		m_GuidePointY -= 0.1f;

	// Ctrl + W 로 m_GuidePointY 0 으로 초기화
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyUp(DIK_W))
		m_GuidePointY = 0;

	// 가이드포인트가 마우스 따라다니게 (y 위치는 m_GuidePointY 으로)
	dynamic_cast<CPickingPoint*>(m_pGuidePoint)->FollowMouse(m_GuidePointY);
}

void CCameraTool::RenderDebug(_double TimeDelta)
{
	if (m_iPositionIndex == m_iLookAtIndex && m_iLookAtIndex == (m_iViaIndex + 1))
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "POS(%d) == LOOKAT(%d) == VIA(%d)", m_iPositionIndex, m_iLookAtIndex, m_iViaIndex);
	else
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "POS(%d) != LOOKAT(%d) != VIA(%d)", m_iPositionIndex, m_iLookAtIndex, m_iViaIndex);

	_vector vGuidePos = m_pGuidePoint->Get_Transform()->Get_Position();
	ImGui::Text("Guide Position : %.1f, %.1f, %.1f", vecx(vGuidePos), vecy(vGuidePos), vecz(vGuidePos));

	m_pImGui_Manager->SmallSeparator();
}

void CCameraTool::SetCameraPoint(_double TimeDelta)
{
	if (m_bPlayOn == true)
		return;

	// Ctrl + 좌클릭으로 Position 카메라포인트 생성
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
	{
		if (m_iPositionIndex > m_iLookAtIndex)
			return;

		if (m_iPositionIndex != m_iViaIndex)
			return;

		pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_GuidePointY);
		if (CollisionCheck.first == true)
		{
			// 카메라포인트 속성 설정
			CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
			CameraPointDesc.eType = CCameraPoint::CAMERAPOINTTYPE_POS;
			CameraPointDesc.vPos = CollisionCheck.second;
			CameraPointDesc.iIndex = m_iPositionIndex;

			// Position 카메라포인트 생성하고 m_Position_CameraPoint 에 push_back 하고 인덱스 늘리기
			CGameObject* pCameraPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);
			if (pCameraPoint == nullptr)
			{
				BREAKPOINT;
				return;
			}
			m_Position_CameraPoint.push_back(pCameraPoint);
			++m_iPositionIndex;
		}
	}

	// Ctrl + 우클릭으로 LookAt 카메라포인트 생성
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_RBUTTON))
	{
		if (m_iLookAtIndex >= m_iPositionIndex)
			return;

		pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_GuidePointY);
		if (CollisionCheck.first == true)
		{
			// 카메라포인트 속성 설정
			CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
			CameraPointDesc.eType = CCameraPoint::CAMERAPOINTTYPE_LOOKAT;
			CameraPointDesc.vPos = CollisionCheck.second;
			CameraPointDesc.iIndex = m_iLookAtIndex;

			// LookAt 카메라포인트 생성하고 m_LookAt_CameraPoint 에 push_back 하고 인덱스 늘리기
			CGameObject* pCameraPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);
			if (pCameraPoint == nullptr)
			{
				BREAKPOINT;
				return;
			}
			m_LookAt_CameraPoint.push_back(pCameraPoint);
			++m_iLookAtIndex;
		}
	}

	// Ctrl + 휠클릭으로 Via 카메라포인트 생성
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_WHEEL))
	{
		if (m_iPositionIndex != m_iLookAtIndex)
			return;

		if (m_iViaIndex >= m_iPositionIndex)
			return;

		pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_GuidePointY);
		if (CollisionCheck.first == true)
		{
			// 카메라포인트 속성 설정
			CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
			CameraPointDesc.eType = CCameraPoint::CAMERAPOINTTYPE_VIA;
			CameraPointDesc.vPos = CollisionCheck.second;
			CameraPointDesc.iIndex = m_iViaIndex;

			// Via 카메라포인트 생성하고 m_Via_CameraPoint 에 push_back 하고 인덱스 늘리기
			CGameObject* pCameraPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);
			if (pCameraPoint == nullptr)
			{
				BREAKPOINT;
				return;
			}
			m_Via_CameraPoint.push_back(pCameraPoint);
			++m_iViaIndex;
		}
	}
}

void CCameraTool::ModifyCameraPoint(_double TimeDelta)
{
	if (m_bPlayOn == true)
		return;

	// 카메라 포인트 선택
	if (m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
	{
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
		{
			BoundingBox* BoundingBox = dynamic_cast<CCameraPoint*>(pPosCameraPoint)->GetCollider()->Get_AABB();
			_float fDist = 0.f;

			if (m_pGameInstance->Ray_Box_Collision(*BoundingBox, fDist) == true)
			{
				m_pSelectedCameraPoint = pPosCameraPoint;
				break;
			}
		}

		if (m_pSelectedCameraPoint == nullptr)
		{
			for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			{
				BoundingBox* BoundingBox = dynamic_cast<CCameraPoint*>(pLookAtCameraPoint)->GetCollider()->Get_AABB();
				_float fDist = 0.f;

				if (m_pGameInstance->Ray_Box_Collision(*BoundingBox, fDist) == true)
				{
					m_pSelectedCameraPoint = pLookAtCameraPoint;
					break;
				}
			}
		}

		if (m_pSelectedCameraPoint == nullptr)
		{
			for (auto& pViaCameraPoint : m_Via_CameraPoint)
			{
				BoundingBox* BoundingBox = dynamic_cast<CCameraPoint*>(pViaCameraPoint)->GetCollider()->Get_AABB();
				_float fDist = 0.f;

				if (m_pGameInstance->Ray_Box_Collision(*BoundingBox, fDist) == true)
				{
					m_pSelectedCameraPoint = pViaCameraPoint;
					break;
				}
			}
		}
	}

	// 카메라 포인트 이동
	if (m_pGameInstance->Mouse_Pressing(CInput_Device::MBS_LBUTTON))
	{
		if (m_pSelectedCameraPoint != nullptr)
		{
			// 위치 이동시키면 구조체 값도 수정(나중에 세이브데이터 할 때 필요)
			dynamic_cast<CCameraPoint*>(m_pSelectedCameraPoint)->FollowMouse(m_GuidePointY);
			dynamic_cast<CCameraPoint*>(m_pSelectedCameraPoint)->SetCameraPointDescPos(m_pSelectedCameraPoint->Get_Transform()->Get_Position());
		}
	}

	// 선택한 카메라 포인트 해제
	if (m_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
	{
		m_pSelectedCameraPoint = nullptr;
	}
}

void CCameraTool::DeleteCameraPoint()
{
	if (m_bPlayOn == true)
		return;

	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_Z))
	{
		if (m_iPositionIndex > 0 && m_iPositionIndex >= m_iLookAtIndex && m_iPositionIndex >= m_iViaIndex)
		{
			m_Position_CameraPoint[m_iPositionIndex - 1]->Set_Dead(true);
			m_Position_CameraPoint.pop_back();
			--m_iPositionIndex;
		}
	}

	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_X))
	{
		if (m_iLookAtIndex > 0 && m_iLookAtIndex >= m_iPositionIndex)
		{
			m_LookAt_CameraPoint[m_iLookAtIndex - 1]->Set_Dead(true);
			m_LookAt_CameraPoint.pop_back();
			--m_iLookAtIndex;
		}
	}

	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_C))
	{
		if (m_iViaIndex > 0 && m_iViaIndex >= m_iLookAtIndex)
		{
			m_Via_CameraPoint[m_iViaIndex - 1]->Set_Dead(true);
			m_Via_CameraPoint.pop_back();
			--m_iViaIndex;
		}
	}
}
void CCameraTool::Play(_double TimeDelta)
{
	m_pImGui_Manager->ColoredButtonBegin();
	if (ImGui::Button("PLAY", ImVec2(-FLT_MIN, 40.f)))
	{
		if (m_Position_CameraPoint.size() < 2 || m_LookAt_CameraPoint.size() < 2 || m_Via_CameraPoint.size() < 1)
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		if (m_bPlayOn == true)
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		// Position 인덱스랑 LookAt 인덱스가 다르거나 Position 인덱스랑 m_iViaIndex + 1 이 다르면 실행 안 함
		if (m_iPositionIndex != m_iLookAtIndex || m_iPositionIndex != (m_iViaIndex + 1))
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		// 플레이버튼 누르면 카메라의 현재 월드행렬 기록해두고 m_bPlayOn 켜서 시작
		CGameObject* pCamera = m_pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();
		m_OriginWorldMatrix = pCameraTransform->Get_WorldFloat4x4();

		// 보간 시작 위치를 0 번 포지션 카메라포인트의 위치로 설정 / 0 번 Look 카메라포인트를 바라봄
		m_vLerpStartPos = m_Position_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position();
		pCameraTransform->LookAt(m_LookAt_CameraPoint[m_iPlayIndex]->Get_Transform()->Get_Position());

		if (pCamera == nullptr)
		{
			BREAKPOINT;
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		m_bPlayOn = true;
	}
	m_pImGui_Manager->ColoredButtonEnd();

	// m_bPlayOn 켜지면 시작
	if (m_bPlayOn == true)
	{
		// 카메라의 Transform 을 가져옴
		CGameObject* pCamera = m_pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();

		if (pCamera == nullptr)
		{
			BREAKPOINT;
			return;
		}

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
				m_LerpTimeAcc = 0.00;
				m_iPlayIndex = 0;
				m_bIsPlayEnd = false;
				m_bPlayOn = false;
				pCameraTransform->Set_WorldMatrix(m_OriginWorldMatrix);
				return;
			}

			m_LerpTimeAcc = 0.00;
			++m_iPlayIndex;
			m_bIsPlayEnd = false;
		}
	}
}

void CCameraTool::Reset()
{
	m_pImGui_Manager->ColoredButtonBegin(200, 0, 0);
	if (ImGui::Button("RESET", ImVec2(-FLT_MIN, 40.f)))
	{
		// 실행중일 땐 패스
		if (m_bPlayOn == true)
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

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

		m_pSelectedCameraPoint = nullptr;
		m_bPlayOn = false;
		m_bIsPlayEnd = false;
		m_iPlayIndex = 0;
		m_TimeAccForAutoSave = 0.0;
	}
	m_pImGui_Manager->ColoredButtonEnd();
}

void CCameraTool::SaveLoad()
{
	m_pImGui_Manager->ColoredButtonBegin(160, 80, 160);
	if (ImGui::Button("Save", ImVec2(-FLT_MIN, 40.f)))
	{
		// 실행중일 땐 패스
		if (m_bPlayOn == true)
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		// 아무것도 없으면 패스
		if (m_Position_CameraPoint.size() == 0 || m_LookAt_CameraPoint.size() == 0)
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		// 포지션과 LookAt 사이즈가 같지 않으면 패스, LookAt 사이즈 - 1 과 Via 사이즈가 같지 않으면 패스
		if (m_Position_CameraPoint.size() == 0 != m_LookAt_CameraPoint.size() == 0 ||
			m_LookAt_CameraPoint.size() - 1 != m_Via_CameraPoint.size())
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		HANDLE hFile = CreateFile(TEXT("../SaveFiles/Camera/NewCameraData.dat"), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		// 카메라포인트 돌면서 저장
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
			pPosCameraPoint->Save_ObjectData(hFile);
		for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			pLookAtCameraPoint->Save_ObjectData(hFile);
		for (auto& pViaCameraPoint : m_Via_CameraPoint)
			pViaCameraPoint->Save_ObjectData(hFile);

		CloseHandle(hFile);
	}
	m_pImGui_Manager->ColoredButtonEnd();

	// =================================================================

	m_pImGui_Manager->SmallSeparator();

	// 로드할 파일 이름 입력받기 전 기본값 세팅
	ImGui::PushItemWidth(150.f);
	ImGui::InputText("Load File Name", m_LoadFileName, 128);
	ImGui::PopItemWidth();

	m_pImGui_Manager->ColoredButtonBegin(160, 80, 160);
	if (ImGui::Button("Load", ImVec2(-FLT_MIN, 40.f)))
	{
		// 실행중일 땐 패스
		if (m_bPlayOn == true)
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		// 데이터 로드하기 위해 파일이름 입력받기
		char SaveFilePath[MAX_PATH] = "../SaveFiles/Camera/";
		strcat_s(SaveFilePath, m_LoadFileName);
		_bstr_t wSaveFilePath(SaveFilePath);

		HANDLE hFile = CreateFile(wSaveFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			MSGBOX("Check File Name!");
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		// 기존 카메라포인트 리셋
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

		// 세이브데이터에서 구조체 값 받아오기
		DWORD dwBytes = 0;
		CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
		ZeroMemory(&CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC));

		while (true)
		{
			ReadFile(hFile, &CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC), &dwBytes, nullptr);

			if (dwBytes == 0)
				break;

			// 받아온 구조체 값으로 카메라포인트 생성
			CGameObject* pNewCameraPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);

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
	m_pImGui_Manager->ColoredButtonEnd();
}

void CCameraTool::AutoSave(_double TimeDelta)
{
	m_TimeAccForAutoSave += TimeDelta;

	if (m_TimeAccForAutoSave >= 90.0)
	{
		// 실행중일 땐 패스
		if (m_bPlayOn == true)
		{
			m_TimeAccForAutoSave = 0.0;
			return;
		}

		// 아무것도 없으면 패스
		if (m_Position_CameraPoint.size() == 0 || m_LookAt_CameraPoint.size() == 0)
		{
			m_TimeAccForAutoSave = 0.0;
			return;
		}

		// 포지션과 LookAt 사이즈가 같지 않으면 패스, LookAt 사이즈 - 1 과 Via 사이즈가 같지 않으면 패스
		if (m_Position_CameraPoint.size() == 0 != m_LookAt_CameraPoint.size() == 0 ||
			m_LookAt_CameraPoint.size() - 1 != m_Via_CameraPoint.size())
		{
			m_TimeAccForAutoSave = 0.0;
			return;
		}

		HANDLE hFile = CreateFile(TEXT("../SaveFiles/Camera/AutoSaveData.dat"), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		// 카메라포인트 돌면서 저장
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
			pPosCameraPoint->Save_ObjectData(hFile);
		for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			pLookAtCameraPoint->Save_ObjectData(hFile);
		for (auto& pViaCameraPoint : m_Via_CameraPoint)
			pViaCameraPoint->Save_ObjectData(hFile);

		CloseHandle(hFile);

		m_TimeAccForAutoSave = 0.0;
	}
}
void CCameraTool::F_RenderDebug(_double TimeDelta)
{
	if (m_iPositionIndex == m_iLookAtIndex)
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "POS(%d) == LOOKAT(%d)", m_iPositionIndex, m_iLookAtIndex);
	else
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "POS(%d) != LOOKAT(%d)", m_iPositionIndex, m_iLookAtIndex);

	_vector vGuidePos = m_pGuidePoint->Get_Transform()->Get_Position();
	ImGui::Text("Guide Position : %.1f, %.1f, %.1f", vecx(vGuidePos), vecy(vGuidePos), vecz(vGuidePos));

	m_pImGui_Manager->SmallSeparator();
}
void CCameraTool::F_SetWaitTime(_double TimeDelta)
{
	ImGui::DragFloat("WAIT", &m_fMaxWaitTime, 0.1f, 0.1f, 5.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
}

void CCameraTool::F_SetCameraPoint(_double TimeDelta)
{
	if (m_bPlayOn == true)
		return;

	// Ctrl + 좌클릭으로 Position 카메라포인트 생성
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
	{
		if (m_iPositionIndex > m_iLookAtIndex)
			return;

		pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_GuidePointY);
		if (CollisionCheck.first == true)
		{
			// 카메라포인트 속성 설정
			CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
			CameraPointDesc.eType = CCameraPoint::CAMERAPOINTTYPE_POS;
			CameraPointDesc.vPos = CollisionCheck.second;
			CameraPointDesc.iIndex = m_iPositionIndex;

			// Position 카메라포인트 생성하고 m_Position_CameraPoint 에 push_back 하고 인덱스 늘리기
			CGameObject* pCameraPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);
			if (pCameraPoint == nullptr)
			{
				BREAKPOINT;
				return;
			}
			m_Position_CameraPoint.push_back(pCameraPoint);
			++m_iPositionIndex;
		}
	}

	// Ctrl + 우클릭으로 LookAt 카메라포인트 생성
	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_RBUTTON))
	{
		if (m_iLookAtIndex >= m_iPositionIndex)
			return;

		pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_GuidePointY);
		if (CollisionCheck.first == true)
		{
			// 카메라포인트 속성 설정
			CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
			CameraPointDesc.eType = CCameraPoint::CAMERAPOINTTYPE_LOOKAT;
			CameraPointDesc.vPos = CollisionCheck.second;
			CameraPointDesc.iIndex = m_iLookAtIndex;

			// LookAt 카메라포인트 생성하고 m_LookAt_CameraPoint 에 push_back 하고 인덱스 늘리기
			CGameObject* pCameraPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);
			if (pCameraPoint == nullptr)
			{
				BREAKPOINT;
				return;
			}
			m_LookAt_CameraPoint.push_back(pCameraPoint);
			++m_iLookAtIndex;
		}
	}
}
void CCameraTool::F_ModifyCameraPoint(_double TimeDelta)
{
	if (m_bPlayOn == true)
		return;

	// 카메라 포인트 선택
	if (m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
	{
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
		{
			BoundingBox* BoundingBox = dynamic_cast<CCameraPoint*>(pPosCameraPoint)->GetCollider()->Get_AABB();
			_float fDist = 0.f;

			if (m_pGameInstance->Ray_Box_Collision(*BoundingBox, fDist) == true)
			{
				m_pSelectedCameraPoint = pPosCameraPoint;
				break;
			}
		}

		if (m_pSelectedCameraPoint == nullptr)
		{
			for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			{
				BoundingBox* BoundingBox = dynamic_cast<CCameraPoint*>(pLookAtCameraPoint)->GetCollider()->Get_AABB();
				_float fDist = 0.f;

				if (m_pGameInstance->Ray_Box_Collision(*BoundingBox, fDist) == true)
				{
					m_pSelectedCameraPoint = pLookAtCameraPoint;
					break;
				}
			}
		}
	}

	// 카메라 포인트 이동
	if (m_pGameInstance->Mouse_Pressing(CInput_Device::MBS_LBUTTON))
	{
		if (m_pSelectedCameraPoint != nullptr)
		{
			// 위치 이동시키면 구조체 값도 수정(나중에 세이브데이터 할 때 필요)
			dynamic_cast<CCameraPoint*>(m_pSelectedCameraPoint)->FollowMouse(m_GuidePointY);
			dynamic_cast<CCameraPoint*>(m_pSelectedCameraPoint)->SetCameraPointDescPos(m_pSelectedCameraPoint->Get_Transform()->Get_Position());
		}
	}

	// 선택한 카메라 포인트 해제
	if (m_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
	{
		m_pSelectedCameraPoint = nullptr;
	}
}
void CCameraTool::F_DeleteCameraPoint()
{
	if (m_bPlayOn == true)
		return;

	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_Z))
	{
		if (m_iPositionIndex > 0 && m_iPositionIndex >= m_iLookAtIndex)
		{
			m_Position_CameraPoint[m_iPositionIndex - 1]->Set_Dead(true);
			m_Position_CameraPoint.pop_back();
			--m_iPositionIndex;
		}
	}

	if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_X))
	{
		if (m_iLookAtIndex > 0 && m_iLookAtIndex >= m_iPositionIndex)
		{
			m_LookAt_CameraPoint[m_iLookAtIndex - 1]->Set_Dead(true);
			m_LookAt_CameraPoint.pop_back();
			--m_iLookAtIndex;
		}
	}
}

void CCameraTool::F_Play(_double TimeDelta)
{
	m_pImGui_Manager->ColoredButtonBegin();
	if (ImGui::Button("PLAY", ImVec2(-FLT_MIN, 40.f)))
	{
		if (m_Position_CameraPoint.size() == 0 || m_LookAt_CameraPoint.size() == 0 || m_Position_CameraPoint.size() != m_LookAt_CameraPoint.size() || m_bPlayOn == true)
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		// 플레이버튼 누르면 카메라의 현재 월드행렬 기록해두고 m_bPlayOn 켜서 시작
		CGameObject* pCamera = m_pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();
		m_OriginWorldMatrix = pCameraTransform->Get_WorldFloat4x4();

		// 시작
		m_bPlayOn = true;
	}
	m_pImGui_Manager->ColoredButtonEnd();

	// m_bPlayOn 켜지면 시작
	if (m_bPlayOn == true)
	{
		// 대기 시간 증가 시작
		m_fWaitTimeAcc += (_float)TimeDelta;

		// 카메라의 Transform 을 가져옴
		CGameObject* pCamera = m_pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Camera"), 0);
		CTransform* pCameraTransform = pCamera->Get_Transform();

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
				m_fWaitTimeAcc = 0.0f;
				m_iPlayIndex = 0;
				m_bIsPlayEnd = false;
				m_bPlayOn = false;
				pCameraTransform->Set_WorldMatrix(m_OriginWorldMatrix);
				return;
			}

			++m_iPlayIndex;
			m_fWaitTimeAcc = 0.0f;
			m_bIsPlayEnd = false;
		}
	}
}

void CCameraTool::F_SaveLoad()
{
	m_pImGui_Manager->ColoredButtonBegin(160, 80, 160);
	if (ImGui::Button("Save", ImVec2(-FLT_MIN, 40.f)))
	{
		if (m_bPlayOn == true || m_Position_CameraPoint.size() == 0 || m_LookAt_CameraPoint.size() == 0 || m_Position_CameraPoint.size() != m_LookAt_CameraPoint.size())
		{
			m_pImGui_Manager->ColoredButtonEnd();
			return;
		}

		HANDLE hFile = CreateFile(TEXT("../SaveFiles/Camera/FlashMode/NewCameraData.dat"), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		// 카메라포인트 돌면서 저장
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
			pPosCameraPoint->Save_ObjectData(hFile);
		for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			pLookAtCameraPoint->Save_ObjectData(hFile);

		CloseHandle(hFile);
	}
	m_pImGui_Manager->ColoredButtonEnd();

	// =================================================================

	m_pImGui_Manager->SmallSeparator();

	// 로드할 파일 이름 입력받기 전 기본값 세팅
	ImGui::PushItemWidth(150.f);
	ImGui::InputText("FileName.dat", m_LoadFileName, 128);
	ImGui::PopItemWidth();

	m_pImGui_Manager->ColoredButtonBegin(160, 80, 160);
	if (ImGui::Button("Load", ImVec2(-FLT_MIN, 40.f)))
	{
		// 실행중일 땐 패스
		if (m_bPlayOn == true)
		{
			m_pImGui_Manager->ColoredButtonEnd();
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
			m_pImGui_Manager->ColoredButtonEnd();
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

			m_pSelectedCameraPoint = nullptr;
			m_bPlayOn              = false;
			m_bIsPlayEnd           = false;
			m_iPlayIndex           = 0;
			m_TimeAccForAutoSave   = 0.0;
		}

		// 세이브데이터에서 구조체 값 받아오기
		DWORD dwBytes = 0;
		CCameraPoint::CAMERAPOINTDESC CameraPointDesc;
		ZeroMemory(&CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC));

		while (true)
		{
			ReadFile(hFile, &CameraPointDesc, sizeof(CCameraPoint::CAMERAPOINTDESC), &dwBytes, nullptr);

			if (dwBytes == 0)
				break;

			// 받아온 구조체 값으로 카메라포인트 생성
			CGameObject* pNewCameraPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_CameraPoint"), TEXT("Prototype_GameObject_CameraPoint"), &CameraPointDesc);

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
	m_pImGui_Manager->ColoredButtonEnd();
}

void CCameraTool::F_AutoSave(_double TimeDelta)
{
	m_TimeAccForAutoSave += TimeDelta;

	if (m_TimeAccForAutoSave >= 90.0)
	{
		if (m_bPlayOn == true || m_Position_CameraPoint.size() == 0 || m_LookAt_CameraPoint.size() == 0 || m_Position_CameraPoint.size() != m_LookAt_CameraPoint.size())
		{
			return;
		}

		HANDLE hFile = CreateFile(TEXT("../SaveFiles/Camera/FlashMode/AutoSaveData.dat"), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		// 카메라포인트 돌면서 저장
		for (auto& pPosCameraPoint : m_Position_CameraPoint)
			pPosCameraPoint->Save_ObjectData(hFile);
		for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
			pLookAtCameraPoint->Save_ObjectData(hFile);

		CloseHandle(hFile);

		m_TimeAccForAutoSave = 0.0;
	}
}
#endif

void CCameraTool::Free()
{
#ifdef _DEBUG
	// 멤버변수 Free
	if (nullptr != m_pDevice)
		Safe_Release(m_pDevice);
	if (nullptr != m_pDeviceContext)
		Safe_Release(m_pDeviceContext);
	if (nullptr != m_pGameInstance)
		Safe_Release(m_pGameInstance);
	if (nullptr != m_pImGui_Manager)
		Safe_Release(m_pImGui_Manager);

	for (auto& pPosCameraPoint : m_Position_CameraPoint)
		pPosCameraPoint->Set_Dead(true);
	m_Position_CameraPoint.clear();

	for (auto& pLookAtCameraPoint : m_LookAt_CameraPoint)
		pLookAtCameraPoint->Set_Dead(true);
	m_LookAt_CameraPoint.clear();

	for (auto& pViaCameraPoint : m_Via_CameraPoint)
		pViaCameraPoint->Set_Dead(true);
	m_Via_CameraPoint.clear();
#endif
}