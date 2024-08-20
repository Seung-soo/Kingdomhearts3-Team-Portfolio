#include "stdafx.h"
#include "..\public\Mini_Reward.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"
#include "SoundManager.h"

CMini_Reward::CMini_Reward(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMini_Reward::CMini_Reward(const CMini_Reward & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMini_Reward::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMini_Reward::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg);
	CHECK_FAILED(SetUp_Components());

	// UI ±âº» Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========
	_float qq = 0.8f;

	//switch (iCom_Texture)
	//{
	//case Engine::CUI_Parents::MINI_BOSS1:
	//	SizeX = 600.f * qq;
	//	SizeY = 130.f * qq;
	//	SetX = 0.f;
	//	SetY = -400.f; //-275.f;

	//	m_fFrame = 4.f;
	//	break;
	//case Engine::CUI_Parents::MINI_BOSS2:
	//	SizeX = 600.f * qq;
	//	SizeY = 130.f * qq;
	//	SetX = 0.f;
	//	SetY = -400.f; //-145.f;

	//	m_fFrame = 3.f;
	//	break;
	//case Engine::CUI_Parents::MINI_BOSS3:
	//	SizeX = 600.f * qq;
	//	SizeY = 130.f * qq;
	//	SetX = 0.f;
	//	SetY = -400.f; //-15.f;
	//	
	//	m_fFrame = 2.f;
	//	break;
	//case Engine::CUI_Parents::MINI_BOSS4:
	//	SizeX = 860.f * qq;
	//	SizeY = 130.f * qq;
	//	SetX = 0.f;
	//	SetY = -400.f; //115.f;

	//	m_fFrame = 1.f;
	//	break;
	//case Engine::CUI_Parents::MINI_BOSS5:
	//	SizeX = 1100.f * qq;
	//	SizeY = 180.f * qq;
	//	SetX = 0.f;
	//	SetY = -400.f; //255.f;

	//	m_fFrame = 0.f;
	//	break;

	//case Engine::CUI_Parents::MINI_GAMEEND:
	//	
	//	END_Size = 1200.f;

	//	SizeX = END_Size;
	//	SizeY = END_Size;

	//	SetX = 460.f;
	//	SetY = -200.f;

	//	m_fFrame = 5.f;
	//	break;
	//default:
	//	break;
	//}


	if (iCom_Texture == Engine::CUI_Parents::MINI_BOSS1)
	{
		SizeX = 1280.f;
		SizeY = 720.f;
		SetX = 1280.f;
		SetY = 0.f;


		m_fFrame = 0.f;
	}

	if (iCom_Texture == Engine::CUI_Parents::MINI_GAMEEND)
	{
		END_Size = 1200.f;
	
		SizeX = END_Size;
		SizeY = END_Size;
	
		SetX = 460.f;
		SetY = -200.f;
	
		m_fFrame = 5.f;
	}
	m_fDepth = 0.5f;


	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SetX, SetY, 0.f, 1.f));


	return S_OK;
}

_int CMini_Reward::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	_vector vRewardPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 fRewardPosition;
	XMStoreFloat4(&fRewardPosition, vRewardPosition);
	
	if (iCom_Texture == Engine::CUI_Parents::MINI_BOSS1)
	{
		if (0 < fRewardPosition.x)
		{
			m_pTransformCom->Go_Left(TimeDelta * 50);
		}

		if (0 > fRewardPosition.x)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
			MINIGAME->Set_EndTexture(true);
		}
	}

	if (iCom_Texture == Engine::CUI_Parents::MINI_GAMEEND)
	{

		END_Time += TimeDelta;
		if (0.005 < END_Time && 300 <= END_Size)
		{
			END_Size -= 100;
			END_Time = 0;
		}

		if (END_Size >= 300 && false == SoundCheck)
		{
			SM->Sound_Command(L"END.ogg", 0.8f);
			SoundCheck = true;
		}

		m_pTransformCom->Scaled(XMVectorSet(END_Size, END_Size, 0.f, 1.f));
	}

			


	/*switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MINI_BOSS1:
		
		if (-275.f > fRewardPosition.y)
			m_pTransformCom->Go_Up(TimeDelta * 70);
		
		if (-275.f < fRewardPosition.y)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -275.f, 0.f, 1.f));
		
		break;
	case Engine::CUI_Parents::MINI_BOSS2:
		if (-145.f > fRewardPosition.y)
			m_pTransformCom->Go_Up(TimeDelta * 70);

		if (-145.f < fRewardPosition.y)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -145.f, 0.f, 1.f));

		break;
	case Engine::CUI_Parents::MINI_BOSS3:
		if (-15.f > fRewardPosition.y)
			m_pTransformCom->Go_Up(TimeDelta * 70);

		if (-15.f < fRewardPosition.y)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -15.f, 0.f, 1.f));

		break;
	case Engine::CUI_Parents::MINI_BOSS4:
		if (115.f > fRewardPosition.y)
			m_pTransformCom->Go_Up(TimeDelta * 70);

		if (115.f < fRewardPosition.y)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 115.f, 0.f, 1.f));

		break;
	case Engine::CUI_Parents::MINI_BOSS5:
		if (255.f > fRewardPosition.y)
			m_pTransformCom->Go_Up(TimeDelta * 70);

		if (255.f < fRewardPosition.y)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 255.f, 0.f, 1.f));
			MINIGAME->Set_EndTexture(true);
		}
		
		break;
	case Engine::CUI_Parents::MINI_GAMEEND:

		END_Time += TimeDelta;
		if (0.005 < END_Time && 300 <= END_Size)
		{
			END_Size -= 100;
			END_Time = 0;
		}

		if (END_Size >= 300 && false == SoundCheck)
		{
			SM->Sound_Command(L"END.ogg", 0.8f);
			SoundCheck = true;
		}
		
		m_pTransformCom->Scaled(XMVectorSet(END_Size, END_Size, 0.f, 1.f));
		break;
	default:
		break;
	}*/

	/*===*/





	return _int();
}

_int CMini_Reward::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMini_Reward::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CMini_Reward::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Reward"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CMini_Reward::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	return S_OK;
}

CMini_Reward * CMini_Reward::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMini_Reward*	pInstance = new CMini_Reward(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini_Reward");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMini_Reward::Clone(void * pArg)
{
	CMini_Reward*	pInstance = new CMini_Reward(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini_Reward");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini_Reward::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
