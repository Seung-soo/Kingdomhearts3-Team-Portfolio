#include "stdafx.h"
#include "..\public\Nightmare_Blade.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "Effect_Particle.h"
#include "GameManager.h"

CNightmare_Blade::CNightmare_Blade(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CNightmare_Blade::CNightmare_Blade(const CNightmare_Blade & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNightmare_Blade::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNightmare_Blade::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 35.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	memcpy(&m_tBladeDesc, pArg, sizeof(BLADEDESC));

	m_pTransformCom->Set_WorldMatrix(m_tBladeDesc.LocalMatrix * m_tBladeDesc.pParentTransform->Get_WorldMatrix());

	if (FAILED(Make_Particle()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_fLifeTime = 3.f;

	m_iPass = 35;

	return S_OK;
}

_int CNightmare_Blade::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	m_fLifeTime -= (_float)TimeDelta;
	m_fMovingV += (_float)TimeDelta * m_fMovingUV_Power;
	if (2.f < m_fLifeTime && 0.f > m_fMovingV)
		m_fMovingV = 0.f;

	if (0.f > m_fLifeTime || -1.f >= m_fMovingV)
		m_bDead = true;

	m_pTransformCom->Set_WorldMatrix(m_tBladeDesc.LocalMatrix * m_tBladeDesc.pParentTransform->Get_WorldMatrix());

	m_vPosition = XMStoreFloat4RT(m_pTransformCom->Get_Position()/* + m_pTransformCom->Get_Look()*/);

	return _int();
}

_int CNightmare_Blade::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);


	return RESULT_NOPROBLEM;
}

HRESULT CNightmare_Blade::Render()
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
		//switch (i)
		//{
		//case 0:
		//	m_vColor = _float4(0.9f, 0.f, 0.f, 0.f);
		//	break;
		//case 1:
		//	m_vColor = _float4(0.f, 0.9f, 0.f, 0.f);
		//	break;
		//case 2:
		//	m_vColor = _float4(0.4f, 0.4f, 0.9f, 0.f);
		//	break;
		//case 3:
			m_vColor = _float4(0.9f, 0.6f, 0.1f, 0.f);
		//	break;
		//case 4:
		//	m_vColor = _float4(0.6f, 0.f, 0.9f, 0.f);
		//	break;
		//case 5:
		//	m_vColor = _float4(0.f, 0.9f, 0.6f, 0.f);
		//	break;
		//}

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

void CNightmare_Blade::PostRender(_double TimeDelta)
{
}

HRESULT CNightmare_Blade::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Blade06"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNightmare_Blade::SetUp_ConstantTable()
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

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 10)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	_bool	bIsPlayOnce = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_isPlayOnce", &bIsPlayOnce, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float	fMoveUVX = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMoveUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &m_fMovingV, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

	_bool	bDistortion = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &bDistortion, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float fTransparency = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &fTransparency, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &m_bBlur, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_iBlur)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNightmare_Blade::Make_Particle()
{
	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;

	_float	fAddColor[4] = { 1.f, -0.14f, -0.18f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 300.f;
	tParticleDesc.fMakeRange = 6.f;
	tParticleDesc.fMaxDistance = 300.f;
	tParticleDesc.fMinSpeed = 0.f;
	tParticleDesc.fMaxSpeed = 0.f;
	tParticleDesc.fScaleSpeed = 0.5f;
	tParticleDesc.fOriginScale = 0.25f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 2;
	tParticleDesc.pDead = &m_bDead;
	tParticleDesc.pParentPosition = &m_vPosition;
	tParticleDesc.vDir = __vector(0.f, -1.f, 0.f, 0.f);

	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float	fSecondAddColor[4] = { 1.f, -0.14f, -0.18f, 0.f };
	memcpy(tParticleDesc.fAddColor, fSecondAddColor, sizeof(_float) * 4);
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//_float	fThirdAddColor[4] = { 1.f, -0.14f, -0.18f, 0.f };
	//memcpy(tParticleDesc.fAddColor, fThirdAddColor, sizeof(_float) * 4);
	//if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	//_float	fFourthAddColor[4] = { 1.f, -0.14f, -0.18f, 0.f };
	//memcpy(tParticleDesc.fAddColor, fFourthAddColor, sizeof(_float) * 4);
	//if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	//_float	fFifthAddColor[4] = { 1.f, -0.14f, -0.18f, 0.f };
	//memcpy(tParticleDesc.fAddColor, fFifthAddColor, sizeof(_float) * 4);
	//if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	//_float	fSixthAddColor[4] = { 1.f, -0.14f, -0.18f, 0.f };
	//memcpy(tParticleDesc.fAddColor, fSixthAddColor, sizeof(_float) * 4);
	//if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	return S_OK;
}

CNightmare_Blade * CNightmare_Blade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CNightmare_Blade*	pInstance = new CNightmare_Blade(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CNightmare_Blade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNightmare_Blade::Clone(void * pArg)
{
	CNightmare_Blade*	pInstance = new CNightmare_Blade(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNightmare_Blade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNightmare_Blade::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
