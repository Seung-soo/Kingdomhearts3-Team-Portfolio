#include "stdafx.h"
#include "Level_Test.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "MonsterGenerator.h"
#include "Camera_Dynamic.h"
#include "Map.h"
#include "UI_Parents.h"
#include "FileLoad_Manager.h"
#include "Navigation.h"
#include "MonsterSkill.h"
#include "GameManager.h"
#include "Camera_Perspective.h"
#include "Player.h"

#include "GameManager.h"
#include "Camera_Perspective.h"
#include "Player.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Test::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Lights()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_GameObjects()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance::GetInstance()->StopAll();

#ifdef _DEBUG
	// MonsterGenerator SetUp
	CMonsterGenerator::GetInstance()->CMonsterGenerator_SetUp();
#endif

	return S_OK;
}

_int CLevel_Test::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

#ifdef _DEBUG
	// MonsterGenerator Tick
	CMonsterGenerator::GetInstance()->CMonsterGenerator_Tick(TimeDelta);
#endif

	return RESULT_NOPROBLEM;
}

_int CLevel_Test::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	return RESULT_NOPROBLEM;
}

HRESULT CLevel_Test::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL; 

	if (CImGui_Manager::GetInstance()->show_levelcontrol_window)
	{
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::Begin("Level Control");

		if (ImGui::BeginTabBar("Level Control", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Separated"))
			{
				LIGHTDESC* pLight = CGameInstance::GetInstance()->Get_LightDesc(0);

				XMFLOAT4 vDiffuse   = pLight->vDiffuse;
				XMFLOAT4 vAmbient   = pLight->vAmbient;
				XMFLOAT4 vSpecualr  = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse[4]   = { vDiffuse.x, vDiffuse.y, vDiffuse.z, vDiffuse.w };
				_float Ambient[4]   = { vAmbient.x, vAmbient.y, vAmbient.z, vAmbient.w };
				_float Specualr[4]  = { vSpecualr.x, vSpecualr.y, vSpecualr.z, vSpecualr.w };
				_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };

				if (ImGui::DragFloat4("Diffuse", diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
				if (ImGui::DragFloat4("Ambient", Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient[0], Ambient[1], Ambient[2], Ambient[3]));
				if (ImGui::DragFloat4("Specular", Specualr, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vSpecular, XMVectorSet(Specualr[0], Specualr[1], Specualr[2], Specualr[3]));
				if (ImGui::DragFloat4("Direction", Direction, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDirection, XMVectorSet(Direction[0], Direction[1], Direction[2], Direction[3]));

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("United"))
			{
				LIGHTDESC* pLight = CGameInstance::GetInstance()->Get_LightDesc(0);

				XMFLOAT4 vDiffuse   = pLight->vDiffuse;
				XMFLOAT4 vAmbient   = pLight->vAmbient;
				XMFLOAT4 vSpecualr  = pLight->vSpecular;
				XMFLOAT4 vDirection = pLight->vDirection;

				_float diffuse      = vDiffuse.x;
				_float Ambient      = vAmbient.x;
				_float Specualr     = vSpecualr.x;
				_float Direction[4] = { vDirection.x, vDirection.y, vDirection.z, vDirection.w };

				if (ImGui::DragFloat("Diffuse", &diffuse, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDiffuse, XMVectorSet(diffuse, diffuse, diffuse, diffuse));
				if (ImGui::DragFloat("Ambient", &Ambient, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vAmbient, XMVectorSet(Ambient, Ambient, Ambient, Ambient));
				if (ImGui::DragFloat("Specular", &Specualr, 0.1f, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vSpecular, XMVectorSet(Specualr, Specualr, Specualr, Specualr));
				if (ImGui::DragFloat4("Direction", Direction, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
					XMStoreFloat4(&pLight->vDirection, XMVectorSet(Direction[0], Direction[1], Direction[2], Direction[3]));

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Load Map"))
			{
				CImGui_Manager::GetInstance()->ColoredButtonBegin(200, 50, 0);
				if (ImGui::Button("Reset", ImVec2(-FLT_MIN, 30.f)))
				{
					// 맵 리스트를 가져옴
					list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_TEST, TEXT("Layer_Map"));

					// 맵 레이어가 없거나 맵 리스트에 아무것도 없으면 패스
					if (MapList != nullptr && MapList->size() != 0)
					{
						// 전부 제거
						auto iter = (*MapList).begin();
						for (_uint i = 0; i < (*MapList).size(); i++)
						{
							(*iter)->Set_Dead(true);
							++iter;
						}
					}
				}
				CImGui_Manager::GetInstance()->ColoredButtonEnd();

				if (ImGui::Button("Frozen Map", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_TEST, TEXT("../SaveFiles/Map/SaveMap_FZ.dat"));
				if (ImGui::Button("KeyGrave Map", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_TEST, TEXT("../SaveFiles/Map/SaveMap_KG.dat"));
				if (ImGui::Button("FinalBoss Map", ImVec2(-FLT_MIN, 30.f)))
					CFileLoad_Manager::GetInstance()->Load_MapSaveFile(LEVEL_TEST, TEXT("../SaveFiles/Map/SaveMap_BT.dat"));

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::End();
	}

#ifdef _DEBUG
	// MonsterGenerator Render
	CMonsterGenerator::GetInstance()->CMonsterGenerator_Render();
#endif

	return S_OK;
}

HRESULT CLevel_Test::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse   = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient   = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular  = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -10.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_GameObjects()
{
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// Camera
	CCamera::CAMERADESC	CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Perspective"), &CameraDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Set_Camera(static_cast<CCamera_Perspective*>(m_pGameInstance->Get_Back(LEVEL_TEST, TEXT("Layer_Camera"))));

	// BackGround
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Player
	m_pPlayer = pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"));
	if (m_pPlayer == nullptr)
	{
		BREAKPOINT;
		return E_FAIL;
	}
	GM->Set_Player(static_cast<CPlayer*>(m_pPlayer));
	

	return S_OK;
}

CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();

#ifdef _DEBUG
	CMonsterGenerator::GetInstance()->DestroyInstance();
#endif
}