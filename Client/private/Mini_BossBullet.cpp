#include "stdafx.h"
#include "..\public\MINI_BossBullet.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"
#include "SoundManager.h"

CMini_BossBullet::CMini_BossBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMini_BossBullet::CMini_BossBullet(const CMini_BossBullet & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMini_BossBullet::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMini_BossBullet::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg);
	CHECK_FAILED(SetUp_Components());

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	_float4 fBoss_Position = MINIGAME->Get_BossPosition();


	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::BOSS1_BULLET1:
		SizeX = 64.f;
		SizeY = 64.f;

		Random_Position = rand() % 2;
		Random_Frame = rand() % 5;

		if(0 == Random_Position)
			SetX = fBoss_Position.x + 100.f;
		if (1 == Random_Position)
			SetX = fBoss_Position.x - 100.f;

		SetY = fBoss_Position.y;

		SM->Sound_Command(L"MiniGame_1_1.wav", 0.2f);

		m_fFrame = (_float)Random_Frame;
		break;
	case Engine::CUI_Parents::BOSS2_BULLET1:
		SizeX = 32.f;
		SizeY = 32.f;
		SetX = -620.f;
		SetY = 0.f;


	
		break;
	case Engine::CUI_Parents::BOSS2_BULLET2:
		Bullet2_1_Position = MINIGAME->Get_Bullet2_Position();

		SizeX = 32.f;
		SizeY = 32.f;
		SetX = Bullet2_1_Position.x;
		SetY = Bullet2_1_Position.y;

		SM->Sound_Command(L"MiniGame_2_1.wav", 0.3f);

		Speed2_2Random = rand() % 6 + 3;
		break;
	case Engine::CUI_Parents::BOSS3_BULLET1:
		
		SizeX = 200.f;
		SizeY = 720.f;

		Bullet3_Random = rand() % 7;

		if (0 == Bullet3_Random)
		{
			SetX = -600.f;
			SetY = 0.f;
		}
		else if (1 == Bullet3_Random)
		{
			SetX = -400.f;
			SetY = 0.f;
		}
		else if (2 == Bullet3_Random)
		{
			SetX = -200.f;
			SetY = 0.f;
		}
		else if (3 == Bullet3_Random)
		{
			SetX = 0.f;
			SetY = 0.f;
		}
		else if (4 == Bullet3_Random)
		{
			SetX = 200.f;
			SetY = 0.f;

		}
		else if (5 == Bullet3_Random)
		{
			SetX = 400.f;
			SetY = 0.f;
		}
		else if (6 == Bullet3_Random)
		{
			SetX = 600.f;
			SetY = 0.f;
		}

		SM->Sound_Command(L"MiniGame_3_1.ogg", 0.3f);

		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::BOSS3_BULLET2:

		SizeX = 240.f;
		SizeY = 720.f;
		SetX = MINIGAME->Get_Bullet3_Position().x;
		SetY = 0.f;

		SM->Sound_Command(L"MiniGame_3_2.ogg", 0.5f);

		break;
	case Engine::CUI_Parents::BOSS3_BULLET3:
		break;
	case Engine::CUI_Parents::BOSS4_BULLET1:
		SizeX = 300.f;
		SizeY = 300.f;
		SetX = -600.f;
		SetY = -510.f;

		SM->Sound_Command(L"MiniGame_4_1.wav", 0.2f);
		break;
	case Engine::CUI_Parents::BOSS4_BULLET2:
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f));
		
		SizeX = 300.f;
		SizeY = 300.f;
		SetX = 600.f;
		SetY = 510.f;

		//SM->Sound_Command(L"MiniGame_4_1.wav", 0.2f);
		break;
	case Engine::CUI_Parents::BOSS5_BULLET1:
		SizeX = 64.f;
		SizeY = 64.f;

		Random_Position = rand() % 2;
		if (0 == Random_Position)
			SetX = fBoss_Position.x + 100.f;
		if (1 == Random_Position)
			SetX = fBoss_Position.x - 100.f;

		SM->Sound_Command(L"MiniGame_1_1.wav", 0.2f);

		SetY = fBoss_Position.y;
		break;

	default:
		break;
	}

	m_fDepth = 0.5f;

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SetX, SetY, 0.f, 1.f));

	return S_OK;
}

_int CMini_BossBullet::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	fPlayer_Position = MINIGAME->Get_PlayerPosition();
	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&fBossBullet_Position, vPosition);

	/* === */

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::BOSS1_BULLET1:
		Boss1_Bullet(TimeDelta);
		break;
	case Engine::CUI_Parents::BOSS2_BULLET1:
		Boss2_Bullet1(TimeDelta);
		break;
	case Engine::CUI_Parents::BOSS2_BULLET2:
		Boss2_Bullet2(TimeDelta);
		break;
	case Engine::CUI_Parents::BOSS3_BULLET1:
		Boss3_Bullet1(TimeDelta);
		break;
	case Engine::CUI_Parents::BOSS3_BULLET2:
		Boss3_Bullet2(TimeDelta);
		break;
	case Engine::CUI_Parents::BOSS3_BULLET3:
		break;
	case Engine::CUI_Parents::BOSS4_BULLET1:
		Boss4_Bullet1(TimeDelta);
		break;
	case Engine::CUI_Parents::BOSS4_BULLET2:
		Boss4_Bullet2(TimeDelta);
		break;
	case Engine::CUI_Parents::BOSS5_BULLET1:
		Boss1_Bullet(TimeDelta);
		break;

	default:
		break;
	}


	/* Dead */
	_vector	BossBulletPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4	fBossBulletPosition;
	XMStoreFloat4(&fBossBulletPosition, BossBulletPosition);

	if (-1000.f >= fBossBulletPosition.y
		|| -1200.f >= fBossBulletPosition.x
		|| 1200.f <= fBossBulletPosition.x)
		m_bDead = true;

	return _int();
}

_int CMini_BossBullet::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMini_BossBullet::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	if (Engine::CUI_Parents::BOSS3_BULLET2 == iCom_Texture)
	{
		m_pVIBufferCom->Render(m_pShaderCom, 10);
	}
	else
	{
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}
		
	return S_OK;
}

HRESULT CMini_BossBullet::SetUp_Components()
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
	case Engine::CUI_Parents::BOSS1_BULLET1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss1_Bullet1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::BOSS2_BULLET1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss2_Bullet1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::BOSS2_BULLET2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss2_Bullet2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::BOSS3_BULLET1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss3_Bullet1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::BOSS3_BULLET2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss3_Bullet2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::BOSS3_BULLET3:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss3_Bullet3"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::BOSS4_BULLET1:
	case Engine::CUI_Parents::BOSS4_BULLET2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss4_Bullet1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::BOSS5_BULLET1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Boss5_Bullet1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CMini_BossBullet::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::BOSS1_BULLET1:
	case Engine::CUI_Parents::BOSS3_BULLET1:
	case Engine::CUI_Parents::BOSS4_BULLET1:
	case Engine::CUI_Parents::BOSS4_BULLET2:
	case Engine::CUI_Parents::BOSS5_BULLET1:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

		break;
	case Engine::CUI_Parents::BOSS2_BULLET1:
	case Engine::CUI_Parents::BOSS2_BULLET2:
	case Engine::CUI_Parents::BOSS3_BULLET2:
	case Engine::CUI_Parents::BOSS3_BULLET3:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
		CHECK_FAILED(m_pShaderCom->Set_RawValue("FadeOut", &Bullet3_2_Fade, sizeof(_float)));

		break;

	default:
		break;
	}



	return S_OK;
}

void CMini_BossBullet::Boss1_Bullet(_double TimeDelta)
{
	m_fFrame = (_float)Random_Frame;

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

	if (0 == Move_Random)
		m_pTransformCom->Go_Right(TimeDelta * SpeedX);
	if (1 == Move_Random)
		m_pTransformCom->Go_Right(-TimeDelta * SpeedX);

	m_pTransformCom->Go_Down(TimeDelta * SpeedY);

	Bullet_Collider();
}

void CMini_BossBullet::Boss2_Bullet1(_double TimeDelta)
{
	_vector vBullet2_1 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4	fBullet2_1;
	XMStoreFloat4(&fBullet2_1, vBullet2_1);

	MINIGAME->Set_Bullet2_Position(fBullet2_1);

	/* === */

	if (-340.f >= fBossBullet_Position.y)
		MinusCnt = -1;
	if (200.f <= fBossBullet_Position.y)
		MinusCnt = 1;

	m_pTransformCom->Go_Down(TimeDelta * 60 * MinusCnt);

	/* === */
	Create2_Time += TimeDelta;
	if (0.1 < Create2_Time)
	{
		Create2_Random = rand() % 9;
		if (0 == Create2_Random
			|| 1 == Create2_Random
			|| 2 == Create2_Random)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS2_BULLET2;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
		}
		Create2_Time = 0;
	}
}

void CMini_BossBullet::Boss2_Bullet2(_double TimeDelta)
{
	_vector vBullet2_2 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4	fBullet2_2;
	XMStoreFloat4(&fBullet2_2, vBullet2_2);

	m_pTransformCom->Go_Right(TimeDelta * 10 * Speed2_2Random);

	if (630.f <= fBullet2_2.x)
		m_bDead = true;

	Bullet_Collider();
}

void CMini_BossBullet::Boss3_Bullet1(_double TimeDelta)
{
	_vector vBullet3_1 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4	fBullet3_1;
	XMStoreFloat4(&fBullet3_1, vBullet3_1);
	MINIGAME->Set_Bullet3_Position(fBullet3_1);

	m_fFrame += (_float)(9 * TimeDelta);
	if (3.f < m_fFrame)
		m_fFrame = 0.f;

	Create3_Time += TimeDelta;
	if (3 < Create3_Time)
	{
		iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS3_BULLET2;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
		{
			BREAKPOINT;
			return;
		}

		Create3_Time = 0;
		m_bDead = true;
	}

}

void CMini_BossBullet::Boss3_Bullet2(_double TimeDelta)
{
	Create3_Time2 += TimeDelta;
	if (0.01 < Create3_Time2)
	{
		Bullet3_2_Fade += 0.01f;
		Create3_Time2 = 0;
	}

	if (1.f <= Bullet3_2_Fade)
		m_bDead = true;

	Bullet_Collider();
}

void CMini_BossBullet::Boss3_Bullet3(_double TimeDelta)
{
}

void CMini_BossBullet::Boss4_Bullet1(_double TimeDelta)
{
	switch (Boss4Pattern)
	{
	case 0:
		m_pTransformCom->Go_Up(TimeDelta * 50);

		if (-210.f <= fBossBullet_Position.y)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-600.f, -210.f, 0.f, 1.f));
			Boss4Pattern = 1;
		}
		break;

	case 1:
		m_pTransformCom->Go_Right(TimeDelta * 50);

		if (800.f <= fBossBullet_Position.x)
			m_bDead = true;
		break;

	default:
		break;
	}

	Bullet_Collider();
}

void CMini_BossBullet::Boss4_Bullet2(_double TimeDelta)
{
	switch (Boss4Pattern)
	{
	case 0:
		m_pTransformCom->Go_Up(TimeDelta * 50);

		if (210.f >= fBossBullet_Position.y)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(600.f, 210.f, 0.f, 1.f));
			Boss4Pattern = 1;
		}
		break;

	case 1:
		m_pTransformCom->Go_Right(TimeDelta * 50);

		if (-800.f >= fBossBullet_Position.x)
			m_bDead = true;
		break;

	default:
		break;
	}

	Bullet_Collider();
}

void CMini_BossBullet::Bullet_Collider()
{
	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::BOSS1_BULLET1:

		if (fPlayer_Position.x <= fBossBullet_Position.x + 24.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 24.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 24.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 24.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}
	
		break;
	case Engine::CUI_Parents::BOSS2_BULLET2:

		if (fPlayer_Position.x <= fBossBullet_Position.x + 24.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 24.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 24.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 24.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}
		break;
	case Engine::CUI_Parents::BOSS3_BULLET2:

		if (fPlayer_Position.x <= fBossBullet_Position.x + 100.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 100.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 360.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 360.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}
		break;
	case Engine::CUI_Parents::BOSS3_BULLET3:
		break;
	case Engine::CUI_Parents::BOSS4_BULLET1:
		if (fPlayer_Position.x <= fBossBullet_Position.x + 100.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 100.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 150.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 150.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}

		if (fPlayer_Position.x <= fBossBullet_Position.x + 150.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 150.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 0.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 150.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}
		break;
	case Engine::CUI_Parents::BOSS4_BULLET2:
		if (fPlayer_Position.x <= fBossBullet_Position.x + 100.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 100.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 150.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 150.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}

		if (fPlayer_Position.x <= fBossBullet_Position.x + 150.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 150.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 150.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 0.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}
		break;
	case Engine::CUI_Parents::BOSS5_BULLET1:
		if (fPlayer_Position.x <= fBossBullet_Position.x + 24.f
			&& fPlayer_Position.x >= fBossBullet_Position.x - 24.f
			&& fPlayer_Position.y <= fBossBullet_Position.y + 24.f
			&& fPlayer_Position.y >= fBossBullet_Position.y - 24.f)
		{
			MINIGAME->Set_PlayerRender(false);
		}
		break;
	default:
		break;
	}

}

CMini_BossBullet * CMini_BossBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMini_BossBullet*	pInstance = new CMini_BossBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini_BossBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMini_BossBullet::Clone(void * pArg)
{
	CMini_BossBullet*	pInstance = new CMini_BossBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini_BossBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini_BossBullet::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
