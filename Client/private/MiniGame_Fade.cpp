#include "stdafx.h"
#include "..\public\MiniGame_Fade.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Camera_Perspective.h"
#include "Progress_Manager.h"

CMiniGame_Fade::CMiniGame_Fade(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMiniGame_Fade::CMiniGame_Fade(const CMiniGame_Fade & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMiniGame_Fade::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMiniGame_Fade::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	//iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());
	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));
	// Setting =========

	SizeX = 1280.f;
	SizeY = 720.f;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SetX = 640.f;
	SetY = 360.f;

	m_fDepth = 0.f; // 깊이
					/*====*/

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CMiniGame_Fade::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	m_fFrame += (_float)(80 * TimeDelta);

	if (20.f < m_fFrame)
	{
		PM->Set_Minigame(true);

		_int iCom_Texture_Set = 0;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Minigame"), &iCom_Texture_Set)))
		{
			BREAKPOINT;
			return 0;
		}

		iCom_Texture_Set = 1;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_Minigame"), &iCom_Texture_Set)))
		{
			BREAKPOINT;
			return 0;
		}

		m_fFrame = 0.f;
		m_bDead = true;

	}




	return RESULT_NOPROBLEM;
}

_int CMiniGame_Fade::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CMiniGame_Fade::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	//m_pVIBufferCom->Render(m_pShaderCom, 10);
	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CMiniGame_Fade::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Minigame_Fade"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CMiniGame_Fade::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	//CHECK_FAILED(m_pShaderCom->Set_RawValue("FadeOut", &fFadeIn, sizeof(_float)));
	
	return S_OK;
}

CMiniGame_Fade * CMiniGame_Fade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMiniGame_Fade*	pInstance = new CMiniGame_Fade(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMiniGame_Fade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMiniGame_Fade::Clone(void * pArg)
{
	CMiniGame_Fade*	pInstance = new CMiniGame_Fade(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMiniGame_Fade");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMiniGame_Fade::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
