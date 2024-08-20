#include "stdafx.h"
#include "Level_MapTool.h"
#include "GameInstance.h"
#include "MapTool.h"
#include "TerrainTool.h"
#include "CameraTool.h"
#include "ObjectPlaceTool.h"
#include "Camera_Dynamic.h"
#include "ImGui_Manager.h"

CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_MapTool::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	// 게임 속도 올리기
	g_fGameSpeed = 2.5;

	// 가이드용 피킹포인트 생성
	m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_GuidePoint"), TEXT("Prototype_GameObject_PickingPoint"));

#ifdef _DEBUG
	// 맵툴 초기화
	if (FAILED(CMapTool::GetInstance()->MapTool_SetUp()))
		return E_FAIL;

	// 터레인툴 초기화
	if (FAILED(CTerrainTool::GetInstance()->CTerrainTool_SetUp()))
		return E_FAIL;

	// 카메라툴 초기화
	if (FAILED(CCameraTool::GetInstance()->CCameraTool_SetUp()))
		return E_FAIL;

	// 오브젝트배치툴 초기화
	if (FAILED(CObjectPlaceTool::GetInstance()->ObjectPlaceTool_SetUp()))
		return E_FAIL;
#endif

	CGameInstance::GetInstance()->StopAll();

	return S_OK;
}

_int CLevel_MapTool::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

#ifdef _DEBUG
	// 맵툴 Tick
	if (0 > CMapTool::GetInstance()->MapTool_Tick(TimeDelta))
		return RESULT_ERROR;

	// 터레인툴 Tick
	if (0 > CTerrainTool::GetInstance()->CTerrainTool_Tick(TimeDelta))
		return RESULT_ERROR;

	// 카메라툴 Tick
	if (0 > CCameraTool::GetInstance()->CCameraTool_Tick(TimeDelta))
		return RESULT_ERROR;

	// 오브젝트툴 Tick 
	if (0 > CObjectPlaceTool::GetInstance()->ObjectPlaceTool_Tick(TimeDelta))
		return RESULT_ERROR;
#endif

	return RESULT_NOPROBLEM;
}

_int CLevel_MapTool::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	return RESULT_NOPROBLEM;
}

HRESULT CLevel_MapTool::Render()
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
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

#ifdef _DEBUG
	// 맵툴 Render
	if (FAILED(CMapTool::GetInstance()->MapTool_Render()))
		return E_FAIL;

	// 터레인툴 Render
	if (FAILED(CTerrainTool::GetInstance()->CTerrainTool_Render()))
		return E_FAIL;

	// 카메라툴 Render
	if (FAILED(CCameraTool::GetInstance()->CCameraTool_Render()))
		return E_FAIL;

	// 오브젝트툴 Render
	if (FAILED(CObjectPlaceTool::GetInstance()->ObjectPlaceTool_Render()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eLightType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDiffuse   = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient   = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular  = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapTool::Ready_GameObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Camera
	CCamera::CAMERADESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CameraDesc));

	CameraDesc.vEye    = _float3(0.f, 10.f, -7.f);
	CameraDesc.vAt     = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY  = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovy   = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear   = 0.2f;
	CameraDesc.fFar    = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec    = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Dynamic"), &CameraDesc)))
		return E_FAIL;

	// Axis
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Axis"), TEXT("Prototype_GameObject_Axis"))))
		return E_FAIL;

	// BackGround
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CLevel_MapTool* CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_MapTool* pInstance = new CLevel_MapTool(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_MapTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MapTool::Free()
{
	__super::Free();

	CMapTool::GetInstance()->DestroyInstance();
	CCameraTool::GetInstance()->DestroyInstance();
	CTerrainTool::GetInstance()->DestroyInstance();
	CObjectPlaceTool::GetInstance()->DestroyInstance();
}