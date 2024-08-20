#include "stdafx.h"
#include "..\public\TT_Guide.h"
#include "GameInstance.h"

CTT_Guide::CTT_Guide(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CTT_Guide::CTT_Guide(const CTT_Guide & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTT_Guide::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CTT_Guide::NativeConstruct(void * pArg)
{

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));
	CHECK_FAILED(SetUp_Components());

	_vector	vPosition = (*(_vector*)pArg);

	_vector vObject_Scale = XMVectorSet(1.f, 1.f, 1.f, 1.f);

	m_pTransformCom->Scaled(vObject_Scale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

_int CTT_Guide::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	dDelete_Time += TimeDelta;

	if (0.6 < dDelete_Time)
		m_bDead = true;

	return RESULT_NOPROBLEM;
}

_int CTT_Guide::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CTT_Guide::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CTT_Guide::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Model */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Item_Object"), TEXT("m_pModelCom"), (CComponent**)&m_pModelCom));

	return S_OK;
}

HRESULT CTT_Guide::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	// 쉐이더에 던져야 하는 변수들
	_bool		bBlur = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float		fAlpha = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &fAlpha, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_bool		bDistortion = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &bDistortion, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CTT_Guide * CTT_Guide::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTT_Guide*	pInstance = new CTT_Guide(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTT_Guide");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTT_Guide::Clone(void * pArg)
{
	CTT_Guide*	pInstance = new CTT_Guide(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTT_Guide");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTT_Guide::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
