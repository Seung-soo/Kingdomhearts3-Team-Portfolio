#include "stdafx.h"
#include "..\public\MINI_Bullet.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"
#include "SoundManager.h"

CMINI_Bullet::CMINI_Bullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMINI_Bullet::CMINI_Bullet(const CMINI_Bullet & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMINI_Bullet::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMINI_Bullet::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	//iCom_Texture = (*(_int*)pArg);
	CHECK_FAILED(SetUp_Components());

	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========
	_float4 fPlayer_Position = MINIGAME->Get_PlayerPosition();

	SizeX = 16.f;
	SizeY = 16.f;
	SetX = fPlayer_Position.x;
	SetY = fPlayer_Position.y;

	m_fFrame = 1.f;

	m_fDepth = 0.5f;

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SetX, SetY, 0.f, 1.f));

	SM->Sound_Command(L"MiniGame_Attack.wav", 0.5f);
	return S_OK;
}

_int CMINI_Bullet::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	dTime += TimeDelta;
	/* 공격 */
	m_pTransformCom->Go_Up(TimeDelta * 200.0);

	/* 충돌 */
	Bullet_Collider();

	if (5.0 < dTime)
		m_bDead = true;

	return _int();
}

_int CMINI_Bullet::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMINI_Bullet::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CMINI_Bullet::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Player"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CMINI_Bullet::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	return S_OK;
}

void CMINI_Bullet::Bullet_Collider()
{
	_vector	Bullet_Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 fBullet_Postion;
	XMStoreFloat4(&fBullet_Postion, Bullet_Position);

	_float4 Boss_Position = MINIGAME->Get_BossPosition();

	if (1 == MINIGAME->Get_Boss()
		|| 2 == MINIGAME->Get_Boss()
		|| 3 == MINIGAME->Get_Boss()
		|| 4 == MINIGAME->Get_Boss())
	{
		if (Boss_Position.x + 160.f >= fBullet_Postion.x
			&& Boss_Position.x - 160.f <= fBullet_Postion.x
			&& Boss_Position.y + 65.f >= fBullet_Postion.y
			&& Boss_Position.y - 65.f <= fBullet_Postion.y)
		{
			/* 기능 */
			Create_Damage();

			SM->Sound_Command(L"MiniGame_Damage.wav", 0.5f);
			/* ==== */
			m_bDead = true;
		}
	}

	if (5 == MINIGAME->Get_Boss())
	{
		if (Boss_Position.x + 160.f >= fBullet_Postion.x
			&& Boss_Position.x - 160.f <= fBullet_Postion.x
			&& Boss_Position.y + 90.f >= fBullet_Postion.y
			&& Boss_Position.y - 90.f <= fBullet_Postion.y)
		{
			/* 기능 */
			Create_Damage();

			SM->Sound_Command(L"MiniGame_Damage.wav", 0.5f);
			/* ==== */
			m_bDead = true;
		}
	}
}

void CMINI_Bullet::Create_Damage()
{
	_vector	Bullet_Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 fBullet_Postion;
	XMStoreFloat4(&fBullet_Postion, Bullet_Position);

	MINIGAME_SETTING	Setting;

	if (1 == MINIGAME->Get_Boss())
	{
		Random_Damage = rand() % 3;

		if (0 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE5_0;
		}
		else if (1 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE5_1;
		}
		else
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE5_2;
		}

		Setting.pPosition = &fBullet_Postion;
		Setting.pDamage_Texture = &iCom_Texture;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_DAMAGE"), TEXT("Prototype_GameObject_Minigame_Damage"), &Setting)))
		{
			BREAKPOINT;
			return;
		}

		MINIGAME->Math_BOSS1_HP(-1);
	}

	/* === */

	if (2 == MINIGAME->Get_Boss())
	{
		Random_Damage = rand() % 3;

		if (0 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE4_0;
		}
		else if (1 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE4_1;
		}
		else
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE4_2;
		}

		Setting.pPosition = &fBullet_Postion;
		Setting.pDamage_Texture = &iCom_Texture;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_DAMAGE"), TEXT("Prototype_GameObject_Minigame_Damage"), &Setting)))
		{
			BREAKPOINT;
			return;
		}

		MINIGAME->Math_BOSS2_HP(-1);
	}

	/* === */

	if (3 == MINIGAME->Get_Boss())
	{
		Random_Damage = rand() % 3;

		if (0 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE3_0;
		}
		else if (1 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE3_1;
		}
		else
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE3_2;
		}

		Setting.pPosition = &fBullet_Postion;
		Setting.pDamage_Texture = &iCom_Texture;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_DAMAGE"), TEXT("Prototype_GameObject_Minigame_Damage"), &Setting)))
		{
			BREAKPOINT;
			return;
		}

		MINIGAME->Math_BOSS3_HP(-1);
	}

	/* === */

	if (4 == MINIGAME->Get_Boss())
	{
		Random_Damage = rand() % 4;

		if (0 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE2_0;
		}
		else if (1 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE2_1;
		}
		else if (2 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE2_2;
		}
		else
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE2_3;
		}

		Setting.pPosition = &fBullet_Postion;
		Setting.pDamage_Texture = &iCom_Texture;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_DAMAGE"), TEXT("Prototype_GameObject_Minigame_Damage"), &Setting)))
		{
			BREAKPOINT;
			return;
		}

		MINIGAME->Math_BOSS4_HP(-1);
	}

	/* === */

	if (5 == MINIGAME->Get_Boss())
	{
		Random_Damage = rand() % 6;

		if (0 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE1_0;
		}
		else if (1 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE1_1;
		}
		else if (2 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE1_2;
		}
		else if (3 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE1_3;
		}
		else if (4 == Random_Damage)
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE1_4;
		}
		else
		{
			iCom_Texture = Engine::CUI_Parents::MINI_DAMAGE1_5;
		}

		Setting.pPosition = &fBullet_Postion;
		Setting.pDamage_Texture = &iCom_Texture;
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_DAMAGE"), TEXT("Prototype_GameObject_Minigame_Damage"), &Setting)))
		{
			BREAKPOINT;
			return;
		}

		MINIGAME->Math_BOSS5_HP(-1);
	}

}

CMINI_Bullet * CMINI_Bullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMINI_Bullet*	pInstance = new CMINI_Bullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMINI_Bullet::Clone(void * pArg)
{
	CMINI_Bullet*	pInstance = new CMINI_Bullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMINI_Bullet::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
