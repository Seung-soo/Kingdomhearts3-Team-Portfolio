#include "stdafx.h"
#include "GoatWeapon.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "ImGui_Manager.h"
#include "GameManager.h"
#include "Goat.h"

CGoatWeapon::CGoatWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CGoatWeapon::CGoatWeapon(const CGoatWeapon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGoatWeapon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoatWeapon::NativeConstruct(void* pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec    = 0.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(0.f);
	tTransformDesc.fScalingPerSec  = 0.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	// m_pGoat, m_pGoatTransform, m_pGoatDead 초기화
	m_pGoat = static_cast<CGoat*>(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Goat"), 0));
	if (m_pGoat == nullptr)
	{
		BREAKPOINT;
	}
	m_pGoatTransform = m_pGoat->Get_Transform();
	m_pGoatDead      = m_pGoat->Get_DeadPointer();

	// 컴포넌트 초기화
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// 무기를 붙일 뼈 정보 초기화
	CModel* pGoatModelCom = static_cast<CModel*>(m_pGoat->Get_Component(TEXT("Com_Model")));
	m_BoneMatrixPtr       = pGoatModelCom->Get_BoneMatrixPtr("R_buki");
	m_SocketPivotmMatrix  = pGoatModelCom->Get_TransformMatrix();
	
	// 쉐이더패스 설정
	m_iPass = 44;

	return S_OK;
}

_int CGoatWeapon::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (*m_pGoatDead == true)
		return RESULT_DEAD;

	m_pTransformCom->Get_PositionFloat4_Pointer();

	// 칼을 붙일 위치 업데이트
	m_TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationX(XMConvertToRadians(180.f))
		* XMLoadFloat4x4(m_BoneMatrixPtr.pCombinedMatrix)
		* XMLoadFloat4x4(&m_SocketPivotmMatrix)
		* m_pGoatTransform->Get_WorldMatrix();

	// 칼 위치 업데이트
	m_pTransformCom->Set_WorldMatrix(m_TransformMatrix);

	// 콜라이더 위치 업데이트
	m_pAtkColliderCom->Update_Transform(m_TransformMatrix);

	// 콜리전매니저에 콜라이더 등록
	if (m_bIsAttacking == true)
		m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom);

	return RESULT_NOPROBLEM;
}

_int CGoatWeapon::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// 절두체 안에 있다면 + m_bDead 상태가 아니면 렌더그룹에 추가
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && false == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// 콜라이더 디버그 렌더링
	if (m_bIsAttacking == true && m_bReadyToDie == false && m_bDead == false)
		m_pRendererCom->Add_Debug(m_pAtkColliderCom);
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CGoatWeapon::Render()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

void CGoatWeapon::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	pImGui_Manager->SetGameObjectCollider(string("GW Col"), m_pAtkColliderCom);

	ImGui::End();
}

HRESULT CGoatWeapon::SetUp_Components()
{
	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = 99999.f;
	StatusDesc.fHP      = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = 5.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(-0.5f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Atk"), (CComponent**)&m_pAtkColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_GoatWeapon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoatWeapon::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	_float fZero = 1.f; 
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &fZero, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CGoatWeapon* CGoatWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGoatWeapon* pInstance = new CGoatWeapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGoatWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGoatWeapon::Clone(void* pArg)
{
	CGoatWeapon* pInstance = new CGoatWeapon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGoatWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGoatWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pAtkColliderCom);
}
