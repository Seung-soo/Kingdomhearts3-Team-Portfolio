#include "stdafx.h"
#include "..\public\UI_BossName.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "SoundManager.h"

CUI_BossName::CUI_BossName(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_BossName::CUI_BossName(const CUI_BossName & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_BossName::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_BossName::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());
	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, -150.f, 150.f)));
	// Setting =========


	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);


	switch (iCom_Texture)
	{
	case 0:
		SizeX = 800.f;
		SizeY = 100.f;;
		SetX = -60.f;
		SetY = 475.f;

		m_fDepth = 0.f; // 깊이
		m_fFrame = 0.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMConvertToRadians(20.f));
		break;
	case 1:
		SizeX = 800.f;
		SizeY = 100.f;;
		SetX = 1495.f;
		SetY = 380.f;

		m_fDepth = 0.f; // 깊이
		m_fFrame = 1.f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 1.f), XMConvertToRadians(20.f));
		break;
	case 2:
		SizeX = 1280.f;
		SizeY = 300.f;
		SetX = 640.f;
		SetY = 720.f + 75.f ;

		m_fDepth = 0.01f; // 깊이
		m_fFrame = 6.f;
		break;
	case 3:
		SizeX = 1280.f;
		SizeY = 300.f;
		SetX = 640.f;
		SetY = 0.f - 75.f;

		m_fDepth = 0.01f; // 깊이
		m_fFrame = 6.f;
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_BossName::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	dTime += TimeDelta;

	switch (iCom_Texture)
	{
	case 0:
		
		if (0.05 > dTime)
		{
			dSpeed = 1200.0;
		}
		else if (0.05 <= dTime && 1.95 > dTime)
		{
			dSpeed = 10.0;
		}
		else if (1.95 <= dTime)
		{
			dSpeed = 1200.0;

			if (false == bSound)
			{
				SM->Sound_Command(L"UI_BossSound2.wav", 1.f);
				bSound = true;
			}
		
		}
		else
		{
		}

		if (2.0 < dTime)
		{
			/* UI_Render ON */
			GM->Set_UIRender(true);
			/* BossTarget 생성 */
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_BossTarget"), TEXT("Prototype_GameObject_UI_BossTarget"))))
				return 0;
			
			/* UI_BossHp 생성*/

			iCom_Texture_Set = 0; 
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpBar"), &iCom_Texture_Set)))
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}

			if (LEVEL_TWILIGHT == GM->Get_CurrentLevel())
			{
				iLevel_Name = 1;
			}
			else if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
			{
				iLevel_Name = 2;
			}
			else if (LEVEL_FROZEN == GM->Get_CurrentLevel())
			{
				iLevel_Name = 3;
			}
			else if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel())
			{
				iLevel_Name = 4;
			}
			else if (LEVEL_FINALBOSS == GM->Get_CurrentLevel())
			{
				iLevel_Name = 5;
			}

			iCom_Texture_Set = iLevel_Name; 
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpBar"), &iCom_Texture_Set)))
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}
			iCom_Texture_Set = 6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpBar"), &iCom_Texture_Set)))
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}

			iCom_Texture_Set = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHp"), &iCom_Texture_Set)))
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}

			iCom_Texture_Set = 1;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHp"), &iCom_Texture_Set)))
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}

			/* UI 생성 */
			iCom_Texture_Set = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iCom_Texture_Set)))
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}

			iCom_Texture_Set = 6;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iCom_Texture_Set)))
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}


			/* */
			m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Monster"));

			m_bDead = true;
		}

		m_pTransformCom->Go_Right(TimeDelta * dSpeed);
		break;
	case 1:
		if (LEVEL_TWILIGHT == GM->Get_CurrentLevel())
		{
			if (0.05 > dTime)
			{
				dSpeed = 2300.0;
			}
			else if (0.05 <= dTime && 1.95 > dTime)
			{
				dSpeed = 10.0;
			}
			else if (1.95 <= dTime)
			{
				dSpeed = 1200.0;
			}
			else
			{
			}

			if (true == bUI_Create)
			{
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_Nightmare"));
				iMAX_MonsterHP = (_int)(static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP());	 // 최대 체력
				iCnt = (iMAX_MonsterHP / 100) - 1;
				for (int i = 0; i < iCnt; ++i)
				{
					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_Cnt"), TEXT("Prototype_GameObject_UI_BossHpCnt"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}

					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_CntBack"), TEXT("Prototype_GameObject_UI_BossHpCntBack"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}
				}
				bUI_Create = false;
			}
		

			m_fFrame = 1.f;
		}
		else if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
		{
			if (0.05 > dTime)
			{
				dSpeed = 2300.0;
			}
			else if (0.05 <= dTime && 1.95 > dTime)
			{
				dSpeed = 10.0;
			}
			else if (1.95 <= dTime)
			{
				dSpeed = 1200.0;
			}
			else
			{
			}

			if (true == bUI_Create)
			{
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_BigTree"));
				iMAX_MonsterHP = (_int)(static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP());	 // 최대 체력
				iCnt = (iMAX_MonsterHP / 100) - 1;
				for (int i = 0; i < iCnt; ++i)
				{
					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCnt"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}


					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCntBack"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}
				}


				bUI_Create = false;
			}

			m_fFrame = 2.f;
		}
		else if (LEVEL_FROZEN == GM->Get_CurrentLevel())
		{
			if (0.05 > dTime)
			{
				dSpeed = 2300.0;
			}
			else if (0.05 <= dTime && 1.95 > dTime)
			{
				dSpeed = 10.0;
			}
			else if (1.95 <= dTime)
			{
				dSpeed = 1200.0;
			}
			else
			{
			}

			if (true == bUI_Create)
			{
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_IceMan"));
				iMAX_MonsterHP = (_int)(static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP());	 // 최대 체력
				iCnt = (iMAX_MonsterHP / 100) - 1;
				for (int i = 0; i < iCnt; ++i)
				{
					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCnt"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}


					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCntBack"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}
				}
				bUI_Create = false;
			}

			m_fFrame = 3.f;
		}
		else if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel())
		{
			if (0.05 > dTime)
			{
				dSpeed = 2300.0;
			}
			else if (0.05 <= dTime && 1.95 > dTime)
			{
				dSpeed = 10.0;
			}
			else if (1.95 <= dTime)
			{
				dSpeed = 1200.0;
			}
			else
			{
			}

			if (true == bUI_Create)
			{
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_FrozenWolf"));
				iMAX_MonsterHP = (_int)(static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP());	 // 최대 체력
				iCnt = (iMAX_MonsterHP / 100) - 1;
				for (int i = 0; i < iCnt; ++i)
				{
					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCnt"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}


					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCntBack"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}
				}
				bUI_Create = false;
			}

			m_fFrame = 4.f;
		}
		else if (LEVEL_FINALBOSS == GM->Get_CurrentLevel())
		{
			if (0.05 > dTime)
			{
				dSpeed = 2300.0;
			}
			else if (0.05 <= dTime && 1.95 > dTime)
			{
				dSpeed = 10.0;
			}
			else if (1.95 <= dTime)
			{
				dSpeed = 1200.0;
			}
			else
			{
			}

			if (true == bUI_Create)
			{
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_Goat"));
				iMAX_MonsterHP = (_int)(static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP());	 // 최대 체력
				iCnt = (iMAX_MonsterHP / 100) - 1;
				for (int i = 0; i < iCnt; ++i)
				{
					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCnt"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}


					iCom_Texture_Set = i;
					if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"), TEXT("Prototype_GameObject_UI_BossHpCntBack"), &iCom_Texture_Set)))
					{
						BREAKPOINT;
						return RESULT_NOPROBLEM;
					}
				}
				bUI_Create = false;
			}

			m_fFrame = 5.f;
		}

		m_pTransformCom->Go_Left(TimeDelta * dSpeed);

		if (2.0 < dTime)
			m_bDead = true;
		break;

	case 2:
		if (0.1 > dTime)
		{
			dSpeed2 = 75.0;
		}
		else if (0.1 <= dTime && 1.9 > dTime)
		{
			dSpeed2 = 0.f;
		}
		else if (1.9 <= dTime)
		{
			dSpeed2 = -75.0;
		}
		else
		{
		}

		m_pTransformCom->Go_Down(TimeDelta * dSpeed2);

		if (2.0 < dTime)
			m_bDead = true;
		break;

	case 3:
		if (0.1 > dTime)
		{
			dSpeed2 = 75.0;
		}
		else if (0.1 <= dTime && 1.9 > dTime)
		{
			dSpeed2 = 0.f;
		}
		else if (1.9 <= dTime)
		{
			dSpeed2 = -75.0;
		}
		else
		{
		}

		m_pTransformCom->Go_Up(TimeDelta * dSpeed2);

		if (2.0 < dTime)
			m_bDead = true;
		break;

	default:
		break;
	}



	return RESULT_NOPROBLEM;
}

_int CUI_BossName::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case 0:

		break;
	case 1:

		break;
	default:
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return RESULT_NOPROBLEM;
}

HRESULT CUI_BossName::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());
	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_BossName::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossName"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_BossName::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	return S_OK;
}

CUI_BossName * CUI_BossName::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_BossName*	pInstance = new CUI_BossName(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_BossName");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_BossName::Clone(void * pArg)
{
	CUI_BossName*	pInstance = new CUI_BossName(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_BossName Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_BossName::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
