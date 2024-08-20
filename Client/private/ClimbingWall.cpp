#include "stdafx.h"
#include "..\public\ClimbingWall.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Level_Loading.h"
#include "GameManager.h"

CClimbingWall::CClimbingWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CClimbingWall::CClimbingWall(const CClimbingWall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CClimbingWall::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CClimbingWall::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(*(_float4x4*)pArg);

	m_pPlayerTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform")));

	m_vPrevPlayerPos = XMVector3TransformCoord(m_pPlayerTransform->Get_Position(), m_pTransformCom->Get_WorldMatrixInverse());

	return S_OK;
}

_int CClimbingWall::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	return _int();
}

_int CClimbingWall::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	_vector	vCurrentPlayerPos = m_pPlayerTransform->Get_Position();

	// 두 지점 사이에 선이 렉트에 레이 충돌하면 플레이어를 이전 포지션으로 돌려줌

	// 벽의 월드 매트릭스 역행렬을 플레이어 포지션에 곱해서 z로 비교
	// z + 플레이어 사이즈 절반 만큼 띄어져 있어야함

	// 벽의 로컬로 이동한 플레이어 포지션
	vCurrentPlayerPos = XMVector3TransformCoord(vCurrentPlayerPos, m_pTransformCom->Get_WorldMatrixInverse());

	// 이전 로컬Z
	_float fPrevZ = vecz(m_vPrevPlayerPos) + m_fPlayerHalfSize;

	// 현재 로컬Z
	_float fCurrentZ = vecz(vCurrentPlayerPos) + m_fPlayerHalfSize;


	if (fPrevZ <= 0 && fCurrentZ > 0)
	{ 
		m_pPlayerTransform->Set_Position(XMVector3TransformCoord(m_vPrevPlayerPos, m_pTransformCom->Get_WorldMatrix()));
	}


	m_vPrevPlayerPos = vCurrentPlayerPos;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return RESULT_NOPROBLEM;
}




HRESULT CClimbingWall::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 16)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CClimbingWall::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CClimbingWall::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

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

	return S_OK;
}

CClimbingWall * CClimbingWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CClimbingWall*	pInstance = new CClimbingWall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CClimbingWall::Clone(void * pArg)
{
	CClimbingWall*	pInstance = new CClimbingWall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CClimbingWall::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
