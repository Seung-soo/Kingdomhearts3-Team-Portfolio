#include "stdafx.h"
#include "Axis.h"
#include "GameInstance.h"

CAxis::CAxis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CAxis::CAxis(const CAxis& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAxis::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxis::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(nullptr)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CAxis::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	return RESULT_NOPROBLEM;
}

_int CAxis::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return	RESULT_ERROR;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return RESULT_NOPROBLEM;
}

HRESULT CAxis::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxis::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPos"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Axis"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAxis::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4 viewMatrixTP = CGameInstance::GetInstance()->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	_float4x4 projMatrixTP = CGameInstance::GetInstance()->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

	// 전부 항등행렬
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &viewMatrixTP, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &projMatrixTP, sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}

CAxis* CAxis::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CAxis* pInstance = new CAxis(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CAxis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAxis::Clone(void* pArg)
{
	CAxis* pInstance = new CAxis(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CAxis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAxis::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}