#include "stdafx.h"
#include "..\public\Level_ColliderTool.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "ImGui_Manager.h"
#include "HierarchyNode.h"
#include "ColliderTool_Model.h"

CLevel_ColliderTool::CLevel_ColliderTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_ColliderTool::NativeConstruct()
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

	if (FAILED(Ready_GameObjects()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance::GetInstance()->StopAll();

	return S_OK;
}

_int CLevel_ColliderTool::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Collider_Tool");

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		//Texture_Tab ==========================================================================================================================================================
		if (ImGui::BeginTabItem("Collider"))
		{
			if (nullptr == m_pColliderModel)
				Collider_Create();
			else
			{
				Model_Pick();
				Collider_BoneList();
				Animation_List();
				AnimModelTransform();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();

	return RESULT_NOPROBLEM;
}

_int CLevel_ColliderTool::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;


	return RESULT_NOPROBLEM;
}

HRESULT CLevel_ColliderTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_ColliderTool::Ready_Lights()
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

HRESULT CLevel_ColliderTool::Ready_GameObjects()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Camera
	CCamera::CAMERADESC CameraDesc;
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

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_COLLIDERTOOL, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_Dynamic"), &CameraDesc)))
		return E_FAIL;

	// Axis
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_COLLIDERTOOL, TEXT("Layer_Axis"), TEXT("Prototype_GameObject_Axis"))))
		return E_FAIL;

	// Sky
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_COLLIDERTOOL, TEXT("Layer_Sky"), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;


	return S_OK;
}

void CLevel_ColliderTool::Collider_Create()
{
	if (ImGui::Button("Create"))
	{
		// Create 누르면 매쉬 생성
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_COLLIDERTOOL, TEXT("Layer_AnimationModel"), TEXT("Prototype_GameObject_ColliderTool_Model"))))
		{
			BREAKPOINT;
			return;
		}

		m_pColliderModel = static_cast<CColliderTool_Model*>(m_pGameInstance->Get_Back(LEVEL_COLLIDERTOOL, TEXT("Layer_AnimationModel")));

		m_pColliderModel->Set_BoneName(m_pColliderModel->Get_Model()->Get_RootName());
	}
}

void CLevel_ColliderTool::Animation_List()
{
	ImGui::Text("Animation List");
	ImGui::BeginChild("AnimationList", ImVec2(500.f, 150.f), true);

	vector<CAnimation*>* pAnimations = m_pColliderModel->Get_Model()->Get_Animations();

	for (_uint i = 0; i < m_pColliderModel->Get_Model()->Get_NumberOfAnimations(); i++)
	{
		if (ImGui::Selectable((*pAnimations)[i]->Get_Name(), !strcmp(m_szAnimationFileName, (*pAnimations)[i]->Get_Name())))
		{
			m_iAnimationSelect = i;
			strcpy_s(m_szAnimationFileName, (*pAnimations)[i]->Get_Name());

			m_pColliderModel->Get_Model()->Set_TempIndex(i);
		}
	}
	ImGui::EndChild();
}

void CLevel_ColliderTool::Model_Pick()
{
	ImGui::InputText("Prototype Name", m_szModelPrototypeName, sizeof(char) * MAX_PATH);

	if (ImGui::Button("Change_Model"))
	{
		_tchar wszPrototype[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, m_szModelPrototypeName, sizeof(char) * MAX_PATH, wszPrototype, sizeof(char) * MAX_PATH);

		_bool bExist = false;

		if (nullptr != m_pGameInstance->Find_Component(LEVEL_COLLIDERTOOL, wszPrototype))
			bExist = true;
		if (true == bExist)
		{
			m_pColliderModel->Set_Model(wszPrototype);
		}
	}
	
}

void CLevel_ColliderTool::Collider_BoneList()
{
	ImGui::Text("Bone List");
	ImGui::BeginChild("BoneList", ImVec2(500.f, 150.f), true);


	vector<CHierarchyNode*>* pNodes = m_pColliderModel->Get_Model()->Get_Nodes();

	for (size_t i = 0; i < pNodes->size(); i++)
	{
		if (ImGui::Selectable((*pNodes)[i]->Get_Name(), m_iBoneSelect == i))
		{
			m_iBoneSelect = (_uint)i;

			m_pColliderModel->Set_BoneName(((*pNodes)[i]->Get_Name()));
		}

	}
	ImGui::EndChild();

	ImGui::DragFloat("Collider Radius", &m_fColliderRadius, 0.01f, 0.01f, 120.f);
	m_pColliderModel->Get_Collider()->Set_Scale(_float3(m_fColliderRadius, m_fColliderRadius, m_fColliderRadius));

	ImGui::DragFloat3("Collider LocalPosition", m_fColliderLocalPos, 0.01f, -200.f, 200.f);
	m_pColliderModel->Get_Collider()->Set_Translation(_float4(m_fColliderLocalPos[0], m_fColliderLocalPos[1], m_fColliderLocalPos[2], 1.f));
}

void CLevel_ColliderTool::AnimModelTransform()
{
	CTransform* pTransform = static_cast<CTransform*>(m_pColliderModel->Get_Component(TEXT("Com_Transform")));

	ImGui::DragFloat3("Scale", m_fAnimModelScale, 0.01f, 0.01f, 10.f);
	ImGui::DragFloat3("Rotation", m_fAnimModelRotation, 0.1f, -180.f, 180.f);
	ImGui::DragFloat3("Position", m_fAnimModelPosition, 0.01f, -200.f, 200.f);

	pTransform->Scaled(XMVectorSet(m_fAnimModelScale[0], m_fAnimModelScale[1], m_fAnimModelScale[2], 0.f));
	pTransform->AllRotation(XMVectorSet(XMConvertToRadians(m_fAnimModelRotation[0]), XMConvertToRadians(m_fAnimModelRotation[1]), XMConvertToRadians(m_fAnimModelRotation[2]), 0.f));
	pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fAnimModelPosition[0], m_fAnimModelPosition[1], m_fAnimModelPosition[2], 1.f));
}

CLevel_ColliderTool* CLevel_ColliderTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_ColliderTool* pInstance = new CLevel_ColliderTool(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_ColliderTool::Free()
{
	__super::Free();

}