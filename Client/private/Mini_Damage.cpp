#include "stdafx.h"
#include "..\public\Mini_Damage.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"


CMini_Damage::CMini_Damage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMini_Damage::CMini_Damage(const CMini_Damage & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMini_Damage::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMini_Damage::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	Mini_Set = (*(MINIGAME_SETTING*)pArg);

	iCom_Texture = *Mini_Set.pDamage_Texture;
	CHECK_FAILED(SetUp_Components());

	// UI ±âº» Setting ===
 
	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	Damage_Random = rand() % 2;

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	//SetX = 720.f;
	//SetY = 360.f;
	SetX = 640.f + Mini_Set.pPosition->x;
	SetY = 360.f + Mini_Set.pPosition->y;

	_float ReSize = 0.5f;

	m_fDepth = 0.5f;
	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MINI_DAMAGE5_0:/* 1 */
	case Engine::CUI_Parents::MINI_DAMAGE5_1:
		SizeX = 60.f * ReSize;
		SizeY = 70.f * ReSize;

		if (Engine::CUI_Parents::MINI_DAMAGE5_0 == iCom_Texture)
			m_fFrame = 0.f;
		if (Engine::CUI_Parents::MINI_DAMAGE5_1 == iCom_Texture)
			m_fFrame = 1.f;
		break;
	case Engine::CUI_Parents::MINI_DAMAGE1_1: /* 2 */
	case Engine::CUI_Parents::MINI_DAMAGE3_2:
	case Engine::CUI_Parents::MINI_DAMAGE4_0:
	case Engine::CUI_Parents::MINI_DAMAGE4_1:
		SizeX = 120.f * ReSize;
		SizeY = 70.f * ReSize;


		if (Engine::CUI_Parents::MINI_DAMAGE1_1 == iCom_Texture)
			m_fFrame = 1.f;
		if (Engine::CUI_Parents::MINI_DAMAGE3_2 == iCom_Texture)
			m_fFrame = 2.f;
		if (Engine::CUI_Parents::MINI_DAMAGE4_0 == iCom_Texture)
			m_fFrame = 0.f;
		if (Engine::CUI_Parents::MINI_DAMAGE4_1 == iCom_Texture)
			m_fFrame = 1.f;
		break;
	case Engine::CUI_Parents::MINI_DAMAGE1_3: /* 3 */
	case Engine::CUI_Parents::MINI_DAMAGE1_4:
	case Engine::CUI_Parents::MINI_DAMAGE3_0:
	case Engine::CUI_Parents::MINI_DAMAGE3_1:
	case Engine::CUI_Parents::MINI_DAMAGE4_2:
		SizeX = 180.f * ReSize;
		SizeY = 70.f * ReSize;

		if (Engine::CUI_Parents::MINI_DAMAGE1_3 == iCom_Texture)
			m_fFrame = 3.f;
		if (Engine::CUI_Parents::MINI_DAMAGE1_4 == iCom_Texture)
			m_fFrame = 4.f;
		if (Engine::CUI_Parents::MINI_DAMAGE3_0 == iCom_Texture)
			m_fFrame = 0.f;
		if (Engine::CUI_Parents::MINI_DAMAGE3_1 == iCom_Texture)
			m_fFrame = 1.f;
		if (Engine::CUI_Parents::MINI_DAMAGE4_2 == iCom_Texture)
			m_fFrame = 2.f;
		break;
	case Engine::CUI_Parents::MINI_DAMAGE1_5: /* 4 */
	case Engine::CUI_Parents::MINI_DAMAGE2_0:
	case Engine::CUI_Parents::MINI_DAMAGE2_2:
	case Engine::CUI_Parents::MINI_DAMAGE5_2:
		SizeX = 230.f * ReSize;
		SizeY = 70.f * ReSize;

		if (Engine::CUI_Parents::MINI_DAMAGE1_5 == iCom_Texture)
			m_fFrame = 5.f;
		if (Engine::CUI_Parents::MINI_DAMAGE2_0 == iCom_Texture)
			m_fFrame = 0.f;
		if (Engine::CUI_Parents::MINI_DAMAGE2_2 == iCom_Texture)
			m_fFrame = 2.f;
		if (Engine::CUI_Parents::MINI_DAMAGE5_2 == iCom_Texture)
			m_fFrame = 2.f;
		break;
	case Engine::CUI_Parents::MINI_DAMAGE1_0: /* 5 */
	case Engine::CUI_Parents::MINI_DAMAGE1_2:
	case Engine::CUI_Parents::MINI_DAMAGE2_1:
	case Engine::CUI_Parents::MINI_DAMAGE2_3:
		SizeX = 300.f * ReSize;
		SizeY = 70.f * ReSize;

		if (Engine::CUI_Parents::MINI_DAMAGE1_0 == iCom_Texture)
			m_fFrame = 0.f;
		if (Engine::CUI_Parents::MINI_DAMAGE1_2 == iCom_Texture)
			m_fFrame = 2.f;
		if (Engine::CUI_Parents::MINI_DAMAGE2_1 == iCom_Texture)
			m_fFrame = 1.f;
		if (Engine::CUI_Parents::MINI_DAMAGE2_3 == iCom_Texture)
			m_fFrame = 3.f;
		break;
	default:
		break;
	}
	

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	
	return S_OK;
}

_int CMini_Damage::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	Damage_Ani(TimeDelta);

	return _int();
}

_int CMini_Damage::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	_vector	vBulletPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4	fBulletPosition;
	XMStoreFloat4(&fBulletPosition, vBulletPosition); 

	if (-400.f >= fBulletPosition.y)
		m_bDead = true;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return _int();
}

HRESULT CMini_Damage::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CMini_Damage::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MINI_DAMAGE1_0:
	case Engine::CUI_Parents::MINI_DAMAGE1_1:
	case Engine::CUI_Parents::MINI_DAMAGE1_2:
	case Engine::CUI_Parents::MINI_DAMAGE1_3:
	case Engine::CUI_Parents::MINI_DAMAGE1_4:
	case Engine::CUI_Parents::MINI_DAMAGE1_5:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MINI_DAMAGE2_0:
	case Engine::CUI_Parents::MINI_DAMAGE2_1:
	case Engine::CUI_Parents::MINI_DAMAGE2_2:
	case Engine::CUI_Parents::MINI_DAMAGE2_3:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MINI_DAMAGE3_0:
	case Engine::CUI_Parents::MINI_DAMAGE3_1:
	case Engine::CUI_Parents::MINI_DAMAGE3_2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_3"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MINI_DAMAGE4_0:
	case Engine::CUI_Parents::MINI_DAMAGE4_1:
	case Engine::CUI_Parents::MINI_DAMAGE4_2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_4"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::MINI_DAMAGE5_0:
	case Engine::CUI_Parents::MINI_DAMAGE5_1:
	case Engine::CUI_Parents::MINI_DAMAGE5_2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Damage_5"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CMini_Damage::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));
	return S_OK;
}

void CMini_Damage::Damage_Ani(_double TimeDelta)
{
	SpeedXTime += TimeDelta;
	SpeedYTime += TimeDelta;

	if (0.01 < SpeedXTime)
	{
		SpeedX *= 1.07;
		SpeedXTime = 0;
	}
	if (0.01 < SpeedYTime)
	{
		SpeedY *= 1.1;
		SpeedYTime = 0;
	}

	if (0 == Damage_Random)
		m_pTransformCom->Go_Right(TimeDelta * SpeedX);
	if (1 == Damage_Random)
		m_pTransformCom->Go_Right(-TimeDelta * SpeedX);

	m_pTransformCom->Go_Down(TimeDelta * SpeedY);
}

CMini_Damage * CMini_Damage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMini_Damage*	pInstance = new CMini_Damage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini_Damage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMini_Damage::Clone(void * pArg)
{
	CMini_Damage*	pInstance = new CMini_Damage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini_Damage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini_Damage::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
