#include "stdafx.h"
#include "..\public\Mini_Boss.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"
#include "SoundManager.h"


CMini_Boss::CMini_Boss(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMini_Boss::CMini_Boss(const CMini_Boss & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMini_Boss::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMini_Boss::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg);
	CHECK_FAILED(SetUp_Components());

	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MINI_BOSS1:
		SizeX = 320.f;
		SizeY = 130.f;
		SetX = 640.f;
		SetY = 800.f;

		m_fFrame = 4.f;
		MINIGAME->Set_Boss(1);
		break;
	case Engine::CUI_Parents::MINI_BOSS2:
		SizeX = 320.f;
		SizeY = 130.f;
		SetX = 640.f;
		SetY = 800.f;

		m_fFrame = 3.f;
		MINIGAME->Set_Boss(2);
		break;
	case Engine::CUI_Parents::MINI_BOSS3:
		SizeX = 320.f;
		SizeY = 130.f;
		SetX = 640.f;
		SetY = 800.f;

		m_fFrame = 2.f;
		MINIGAME->Set_Boss(3);
		break;
	case Engine::CUI_Parents::MINI_BOSS4:
		SizeX = 320.f;
		SizeY = 130.f;
		SetX = 640.f;
		SetY = 800.f;

		m_fFrame = 1.f;
		MINIGAME->Set_Boss(4);
		break;
	case Engine::CUI_Parents::MINI_BOSS5:
		SizeX = 320.f;
		SizeY = 180.f;
		SetX = 640.f;
		SetY = 800.f;

		m_fFrame = 0.f;
		MINIGAME->Set_Boss(5);
		break;
	default:
		break;
	}


	m_fDepth = 0.0f;
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	
	DeathPosition = _float4{ 1000.f, 1000.f, 0.f,1.f };

	Position_Check();
	return S_OK;
}

_int CMini_Boss::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MINI_BOSS1:

		Boss1_Pattern(TimeDelta);

		break;
	case Engine::CUI_Parents::MINI_BOSS2:

		Boss2_Pattern(TimeDelta);

		break;
	case Engine::CUI_Parents::MINI_BOSS3:

		Boss3_Pattern(TimeDelta);

		break;
	case Engine::CUI_Parents::MINI_BOSS4:
		
		Boss4_Pattern(TimeDelta);

		break;
	case Engine::CUI_Parents::MINI_BOSS5:

		Boss5_Pattern(TimeDelta);

		break;
	default:
		break;
	}

	return _int();
}

_int CMini_Boss::LateTick(_double TimeDelta)
{

	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MINI_BOSS1:

		break;
	case Engine::CUI_Parents::MINI_BOSS2:

		break;
	case Engine::CUI_Parents::MINI_BOSS3:

		break;
	case Engine::CUI_Parents::MINI_BOSS4:

		break;
	case Engine::CUI_Parents::MINI_BOSS5:

		break;
	default:
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMini_Boss::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 10);

	return S_OK;
}

HRESULT CMini_Boss::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Boss"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CMini_Boss::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));
	
	CHECK_FAILED(m_pShaderCom->Set_RawValue("FadeOut", &Fade, sizeof(_float)));

	return S_OK;
}

void CMini_Boss::Boss1_Pattern(_double TimeDelta)
{
	Position_Check();
	switch (Boss_Cnt)
	{
	case 0:
		m_pTransformCom->Go_Up(-TimeDelta * 10);

		if (200.f >= fBossPosition.y)
			Boss_Cnt = 1;
		
		break;
	case 1:
		dMoveTIme += TimeDelta;
		if (0.001 < dMoveTIme)
		{
			dMoveSpeed_X *= 1.05f;
			dMoveTIme = 0;
		}

		/* ================== */

		if (-500.f >= fBossPosition.x)
		{
			dMoveSpeed_X = 20.f;
			Position_Cnt_X = 1;
		}
		if (500.f <= fBossPosition.x)
		{
			dMoveSpeed_X = 20.f;
			Position_Cnt_X = -1;
		}
		
		if (300.f <= fBossPosition.y)
		{
			Position_Cnt_Y = -1;
		}
		if (150.f >= fBossPosition.y)
		{
			Position_Cnt_Y = 1;
		}

		/* ================== */

		m_pTransformCom->Go_Right(TimeDelta * dMoveSpeed_X * Position_Cnt_X);
		m_pTransformCom->Go_Up(TimeDelta * 10 * Position_Cnt_Y);

		/* 공격 */
		
		BossAttackTime += TimeDelta;
		if (0.3 <= BossAttackTime)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS1_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
			BossAttackTime = 0;
		}


		/* ==== */

		if (0 >= MINIGAME->Get_BOSS1_HP())
			Boss_Cnt = 2;
			
		if (m_pGameInstance->Get_KeyEnter(DIK_1))
			Boss_Cnt = 2;

		break;
	case 2: /* DEAD */
		Boss_Dead(TimeDelta);
		dMoveSpeed_X = 1.f;
		dMoveSpeed_Y = 1.f;
		break;
	default:
		break;
	}

}

void CMini_Boss::Boss2_Pattern(_double TimeDelta)
{
	Position_Check();
	switch (Boss_Cnt)
	{
	case 0:
		m_pTransformCom->Go_Up(-TimeDelta * 10);

		if (200.f >= fBossPosition.y)
		{
			dMoveSpeed_X = 1.f;
			dMoveSpeed_Y = 1.f;
			Boss_Cnt = 1;
		}
			
		break;
	case 1:
		dMoveTIme += TimeDelta;
		if (0.001 < dMoveTIme)
		{
			dMoveSpeed_X *= 1.05f;
			dMoveSpeed_Y *= 1.03f;
			dMoveTIme = 0;
		}
		/* ================== */
		if(-400.f < fBossPosition.x) 
			m_pTransformCom->Go_Left(TimeDelta * dMoveSpeed_X);
		if (270.f > fBossPosition.y)
			m_pTransformCom->Go_Up(TimeDelta * dMoveSpeed_Y);

		/* ================== */

		//m_pTransformCom->Go_Right(TimeDelta * dMoveSpeed_X * Position_Cnt_X);
		//m_pTransformCom->Go_Up(TimeDelta * 10 * Position_Cnt_Y);

		if (-400.f >= fBossPosition.x && 270.f <= fBossPosition.y)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-400.f, 270.f, 0.f, 1.f));
			Boss_Cnt = 2;
		}

		/* 공격 */
		if (false == Create_2)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS2_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
			Create_2 = true;
		}

		break;
	case 2:
		if (-500.f >= fBossPosition.x)
		{
			dMoveSpeed_X = 60.f;
			Position_Cnt_X = 1;
		}
		if (500.f <= fBossPosition.x)
		{
			dMoveSpeed_X = 60.f;
			Position_Cnt_X = -1;
		}

		m_pTransformCom->Go_Right(TimeDelta * dMoveSpeed_X * Position_Cnt_X);

		/* === */

		if (0 >= MINIGAME->Get_BOSS2_HP())
			Boss_Cnt = 3;


		if (m_pGameInstance->Get_KeyEnter(DIK_1))
			Boss_Cnt = 3;

		break;
	case 3:
		Boss_Dead(TimeDelta);
		dMoveSpeed_X = 1.f;
		dMoveSpeed_Y = 1.f;
		break;
	default:
		break;
	}
}

void CMini_Boss::Boss3_Pattern(_double TimeDelta)
{
	Position_Check();
	switch (Boss_Cnt)
	{
	case 0:
		m_pTransformCom->Go_Up(-TimeDelta * 10);

		if (200.f >= fBossPosition.y)
			Boss_Cnt = 1;
		break;
	case 1:
		dMoveTIme += TimeDelta;
		if (0.001 < dMoveTIme)
		{
			dMoveSpeed_Y *= 1.05f;
			dMoveTIme = 0;
		}

		/* ================== */

		m_pTransformCom->Go_Up(TimeDelta * dMoveSpeed_Y);

		/* ================== */

		if (290.f <= fBossPosition.y)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet( 0.f, 270.f, 0.f, 1.f));
			Boss_Cnt = 2;
		}
		break;
	case 2:

		dMoveTIme += TimeDelta;
		if (0.001 < dMoveTIme)
		{
			dMoveSpeed_X *= 1.05f;
			dMoveTIme = 0;
		}

		if (450.f <= fBossPosition.x)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(450.f, 270.f, 0.f, 1.f));
			dMoveSpeed_X = 20.f;
			Position_Cnt_X = -1;

			Boss_Cnt = 3;
		}

		m_pTransformCom->Go_Right(TimeDelta * dMoveSpeed_X * Position_Cnt_X);

		break;
	case 3:
		if (false == bBoss3_Time)
		{
			dMoveTIme += TimeDelta;
			if (0.001 < dMoveTIme)
			{
				dMoveSpeed_X *= 1.05f;
				dMoveTIme = 0;
			}
		}

		/* ================== */

		if (false == bBoss3_Pattern)
		{
			if (-450.f >= fBossPosition.x)
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-450.f, 270.f, 0.f, 1.f));
				dMoveSpeed_X = 20.f;
				Position_Cnt_X = 1;

				bBoss3_Pattern = true;
				bBoss3_Time = true;
			}

		}
		else
		{
			if (450.f <= fBossPosition.x)
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(450.f, 270.f, 0.f, 1.f));
				dMoveSpeed_X = 20.f;
				Position_Cnt_X = -1;

				bBoss3_Pattern = false;
				bBoss3_Time = true;
			}

		}

		if (true == bBoss3_Time)
		{
			dBoss3_Time += TimeDelta;
			if (5.0 <= dBoss3_Time)
			{
				bBoss3_Time = false;
				dBoss3_Time = 0;
			}

		}

		if (false == bBoss3_Time)
			m_pTransformCom->Go_Right(TimeDelta * dMoveSpeed_X * Position_Cnt_X);

		/* ================== */

		dCreateTime_3 += TimeDelta;
		if (2 <= dCreateTime_3)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS3_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
			dCreateTime_3 = 0;
		}


		/* ================== */

		if (0 >= MINIGAME->Get_BOSS3_HP())
			Boss_Cnt = 4;


		if (m_pGameInstance->Get_KeyEnter(DIK_1))
			Boss_Cnt = 4;
		break;
	case 4:
		Boss_Dead(TimeDelta);
		dMoveSpeed_X = 1.f;
		dMoveSpeed_Y = 1.f;
		break;
	default:
		break;
	}
}

void CMini_Boss::Boss4_Pattern(_double TimeDelta)
{
	Position_Check();
	switch (Boss_Cnt)
	{
	case 0:
		m_pTransformCom->Go_Up(-TimeDelta * 10);

		if (200.f >= fBossPosition.y)
			Boss_Cnt = 1;
		break;
	case 1:
		dMoveTIme += TimeDelta;
		{
			dMoveSpeed_Y *= 1.05f;
			dMoveTIme = 0;
		}

		m_pTransformCom->Go_Down(TimeDelta * dMoveSpeed_Y);

		if (0 >= fBossPosition.y)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
			Boss_Cnt = 2;
		}

		break;
	case 2:
		dCreateTime_4 += TimeDelta;

		if (2.0 < dCreateTime_4)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS4_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}

			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS4_BULLET2;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}

			dCreateTime_4 = 0;
		}


		if (0 >= MINIGAME->Get_BOSS4_HP())
			Boss_Cnt = 3;

		if (m_pGameInstance->Get_KeyEnter(DIK_1))
			Boss_Cnt = 3;
		break;
	case 3:
		Boss_Dead(TimeDelta);
		break;

	default:
		break;
	}

}

void CMini_Boss::Boss5_Pattern(_double TimeDelta)
{
	Position_Check();
	switch (Boss_Cnt)
	{
	case 0:
		m_pTransformCom->Go_Up(-TimeDelta * 10);

		if (200.f >= fBossPosition.y)
			Boss_Cnt = 1;
		break;
	case 1:
		
		dMoveTIme += TimeDelta;
		if (0.001 < dMoveTIme)
		{
			dMoveSpeed_Y *= 1.05f;
			dMoveTIme = 0;
		}

		/* ================== */

		m_pTransformCom->Go_Up(TimeDelta * dMoveSpeed_Y);

		/* ================== */

		if (290.f <= fBossPosition.y)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 270.f, 0.f, 1.f));
			Boss_Cnt = 2;
		}

		break;
	case 2:

		/* 공격 */

		BossAttackTime += TimeDelta;
		if (0.8 <= BossAttackTime)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS5_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
			BossAttackTime = 0;
		}

		/* ======================================== */

		if (false == Create_2)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS2_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
			Create_2 = true;
		}

		

		if (false == Create_2)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS2_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
			Create_2 = true;
		}

		/* ======================================== */

		dCreateTime_3 += TimeDelta;
		if (2 <= dCreateTime_3)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS3_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}
			dCreateTime_3 = 0;
		}

		/* ======================================== */

		dCreateTime_4 += TimeDelta;
		if (2.0 < dCreateTime_4)
		{
			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS4_BULLET1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}

			iBullet_Texture = CUI_Parents::BOSS_BULLET::BOSS4_BULLET2;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"), TEXT("Prototype_GameObject_Minigame_BossBullet"), &iBullet_Texture)))
			{
				BREAKPOINT;
				return;
			}

			dCreateTime_4 = 0;
		}

		/* ======================================== */

		/* ======================================== */


		if (0 >= MINIGAME->Get_BOSS5_HP())
			Boss_Cnt = 3;


		if (m_pGameInstance->Get_KeyEnter(DIK_1))
			Boss_Cnt = 3;
		break;
	case 3:
		Boss_Dead(TimeDelta);
		break;
	case 4:

		break;
	default:
		break;
	}

}

void CMini_Boss::Position_Check()
{
	vBossPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&fBossPosition, vBossPosition);
	MINIGAME->Set_BossPosition(fBossPosition);
}

void CMini_Boss::Boss_Dead(_double TimeDelta)
{
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f));
	m_pTransformCom->Go_Down(TimeDelta * 10);

	dTime += TimeDelta;
	if (0.05 < dTime)
	{
		Fade_Cnt += 1;
		dTime = 0;
	}

	if (0 == Fade_Cnt)
	{
		Fade = 0.f;
	}
	else if (1 == Fade_Cnt)
	{
		Fade = 0.5f;
	}
	else if (2 == Fade_Cnt)
	{
		Fade = 1.f;
		Fade_Cnt = 0;
	}

	/* === */

	Boss_DeathTime += TimeDelta;

	if (4.0 < Boss_DeathTime)
	{
		Render_Reward();

		MINIGAME->Set_BossPosition(DeathPosition);
		Boss_Cnt = 0;
		m_bDead = true;
	
		if (iCom_Texture == Engine::CUI_Parents::MINI_BOSS5)
			m_pGameInstance->Release_LayerObject(LEVEL_ENDING, TEXT("Layer_MINI_Player"));

		Boss_DeathTime = 0;
	}

	/* === */

	DeadSoundTime += TimeDelta;
	if (0.5 < DeadSoundTime)
	{
		if (0 == DeadSoundCnt)
		{
			SM->Sound_Command(L"MiniGame_Dead1.wav", 0.3f);
			DeadSoundCnt = 1;
		}
		else
		{
			SM->Sound_Command(L"MiniGame_Dead2.wav", 0.3f);
			DeadSoundCnt = 0;
		}

	
		DeadSoundTime = 0;
	}

	m_pGameInstance->Release_LayerObject(LEVEL_ENDING, TEXT("Layer_MINI_BossBullet"));
}

void CMini_Boss::Render_Reward()
{
	if (Engine::CUI_Parents::MINI_BOSS1 == iCom_Texture)
	{
		MINIGAME->Set_Boss(2);
	}
	else if (Engine::CUI_Parents::MINI_BOSS2 == iCom_Texture)
	{
		MINIGAME->Set_Boss(3);
	}
	else if (Engine::CUI_Parents::MINI_BOSS3 == iCom_Texture)
	{
		MINIGAME->Set_Boss(4);
	}
	else if (Engine::CUI_Parents::MINI_BOSS4 == iCom_Texture)
	{
		MINIGAME->Set_Boss(5);
	}
	else if (Engine::CUI_Parents::MINI_BOSS5 == iCom_Texture)
	{
		MINIGAME->Set_Create_Ending(true);
	}
}

CMini_Boss * CMini_Boss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMini_Boss*	pInstance = new CMini_Boss(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMini_Boss::Clone(void * pArg)
{
	CMini_Boss*	pInstance = new CMini_Boss(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini_Boss");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
