#include "stdafx.h"
#include "CameraPoint.h"
#include "GameInstance.h"
#include "GameManager.h"

CCameraPoint::CCameraPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CCameraPoint::CCameraPoint(const CCameraPoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCameraPoint::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraPoint::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(nullptr)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_CameraPointDesc, pArg, sizeof(CAMERAPOINTDESC));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_CameraPointDesc.vPos);
	}

	m_pTransformCom->Scaled(XMVectorSet(0.75f, 0.75f, 0.75f, 0.f));
	m_pTransformCom->Set_Speed(5.f);
	m_pColliderCom->Set_Scale(_float3(1.3f, 1.3f, 1.3f));

	return S_OK;
}

_int CCameraPoint::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_bDead)
		return RESULT_DEAD;

	// 콜라이더 위치 업데이트
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return RESULT_NOPROBLEM;
}

_int CCameraPoint::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return	RESULT_ERROR;

	if (GM->Get_CurrentLevel() == LEVEL_MAPTOOL)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return RESULT_NOPROBLEM;
}

HRESULT CCameraPoint::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 2)))
		return E_FAIL;

#ifdef _DEBUG
	// 콜라이더 강제 렌더링
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CCameraPoint::FollowMouse(_float fPosY)
{
	pair<_bool, _vector> CollisionCheck = CGameInstance::GetInstance()->Check_Ray_VirtualPlane_Collision(fPosY);

	if (TRUE == CollisionCheck.first)
	{
		// 피킹이 제대로 됐다면 그 위치로 이동
		_vector vFollowPosition = XMVectorSetY(CollisionCheck.second, fPosY);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vFollowPosition);
	}
}

HRESULT CCameraPoint::Save_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 구조체 저장
	DWORD dwBytes = 0;
	WriteFile(hFile, &m_CameraPointDesc, sizeof(CAMERAPOINTDESC), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CCameraPoint::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(0.8f, 0.8f, 0.8f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCameraPoint::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (m_CameraPointDesc.eType == CAMERAPOINTTYPE_POS)
		m_vColor = XMVectorSet(0.5f, 0.f, 0.f, 1.f);
	else if (m_CameraPointDesc.eType == CAMERAPOINTTYPE_LOOKAT)
		m_vColor = XMVectorSet(0.f, 1.f, 0.5f, 1.f);
	else
		m_vColor = XMVectorSet(0.5f, 0.f, 1.f, 1.f);

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CCameraPoint* CCameraPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCameraPoint* pInstance = new CCameraPoint(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCameraPoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCameraPoint::Clone(void* pArg)
{
	CCameraPoint* pInstance = new CCameraPoint(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCameraPoint");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCameraPoint::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}