#include "stdafx.h"
#include "..\public\UI_QusetNavi.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"

#include "Player.h"

CUI_QusetNavi::CUI_QusetNavi(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CUI_QusetNavi::CUI_QusetNavi(const CUI_QusetNavi & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_QusetNavi::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_QusetNavi::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));
	CHECK_FAILED(SetUp_Components());

	fScaleX = 1.4f;
	fScaleY = 1.4f;

	m_pTransformCom->Scaled(XMVectorSet(fScaleX, fScaleY, 1.f, 1.f));
	return S_OK;
}

_int CUI_QusetNavi::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	// center

	/* 플레이어 위치 */
	LEVEL current_level = GM->Get_CurrentLevel();
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(current_level, TEXT("Layer_Player")));
	Player_Origin_Pos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_vector Player_Y = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_UP);
	Player_Y = XMVector4Normalize(Player_Y);
	/* ============ */
	_vector	vObject_Right = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vObject_Right = XMVector4Normalize(vObject_Right);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Player_Origin_Pos + (Player_Y*0.5f));

	m_pTransformCom->LookAtXZ(m_vTargetPosition);
	m_pTransformCom->NowRotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
	
	return RESULT_NOPROBLEM;
}

_int CUI_QusetNavi::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_QusetNavi::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_QusetNavi::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestNave"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_QusetNavi::SetUp_ConstantTable()
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

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_fDistortion", &fShader_Zero, sizeof(_float)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_fTranslucency", &fShader_Zero, sizeof(_float)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &fShader_false, sizeof(_bool)));


	return S_OK;
}

CUI_QusetNavi * CUI_QusetNavi::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_QusetNavi*	pInstance = new CUI_QusetNavi(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_QusetNavi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_QusetNavi::Clone(void * pArg)
{
	CUI_QusetNavi*	pInstance = new CUI_QusetNavi(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_QusetNavi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_QusetNavi::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
