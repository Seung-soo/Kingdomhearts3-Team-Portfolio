
#include "stdafx.h"
#include "..\public\Portal.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "UI_FadeOut.h"
#include "Camera_Perspective.h"
#include "UI_TBoxOpen.h"
#include "Player.h"

CPortal::CPortal(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPortal::CPortal(const CPortal & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPortal::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPortal::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec    = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec  = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CPortal::NativeConstruct");
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		MSGBOX("SetUp_Components returned E_FAIL in CPortal::NativeConstruct");
		return E_FAIL;
	}

	m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
	m_pGameInstance->LetsPlaySound(TEXT("CreatePortal.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);

	switch (GM->Get_CurrentLevel())
	{
	case LEVEL_TWILIGHT:
		m_pTransformCom->Rotation(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
		m_pTransformCom->Set_Position(__vector(82.f, 2.8f, -81.7f, 1.f));
		m_pTransformCom->Scaled(__vector(6.f, 6.f, 6.f, 0.f));
		m_iTexIndex = 0;
		break;
	case LEVEL_RAPUNZEL:
		m_pTransformCom->Set_Position(__vector(126.f, 3.3f, 211.5f, 1.f));
		m_pTransformCom->Scaled(__vector(6.f, 6.f, 6.f, 0.f));
		m_iTexIndex = 0;
		break;
	case LEVEL_RAPUNZEL_BOSS:
		m_pTransformCom->Set_Position(__vector(128.77f, 0.3f, 160.9f, 1.f));
		m_pTransformCom->Scaled(__vector(6.f, 6.f, 6.f, 0.f));
		m_iTexIndex = 1;
		break;
	case LEVEL_FROZEN:
		m_pTransformCom->Set_Position(__vector(0.f, 2.8f, 33.f, 1.f));
		m_pTransformCom->MakeSameDirection(XMVector3Normalize(__vector(-0.059f, 0.f, -0.998f, 0.f)));
		m_pTransformCom->Scaled(__vector(6.f, 6.f, 6.f, 0.f));
		m_iTexIndex = 2;
		break;
	case LEVEL_KEYGRAVE:
		m_pTransformCom->Set_Position(__vector(0.f, 2.8f, 0.f, 1.f));
		m_pTransformCom->Scaled(__vector(6.f, 6.f, 6.f, 0.f));
		m_iTexIndex = 3;
		break;
	}

	m_iPass = 8;

	return S_OK;
}

_int CPortal::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
	{
		MSGBOX("0 > __super::Tick in CPortal::Tick");
		return RESULT_ERROR;
	}

	m_fNoiseUV += (_float)TimeDelta * 0.25f;
	m_fAlpha -= (_float)TimeDelta;
	if (m_fAlpha < 0.f)
	{
		if (-0.5f > m_fAlpha)
		{
			if (-0.6f < m_fAlpha)
			{
				GM->Get_Camera()->Set_Focusing(true);
				GM->Set_UIRender(true);
				m_iPass = 20;
			}
		}
	}

	_vector vPlayerPos = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_Position();
	_vector vPos       = m_pTransformCom->Get_Position();
	vecy(vPlayerPos)   = 0.f;
	vecy(vPos)         = 0.f;

	_float fDist = DISTANCE(vPlayerPos - m_pTransformCom->Get_Position());

	if (5.f > fDist && false == m_bGoNextLevel)
	{
		// 플레이어가 일정 거리 안에 들어오면 상자가 자동으로 열리거나 상호작용해서 열 수 있게
		_float	fDistance = DISTANCE(static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_Position() - m_pTransformCom->Get_Position());

		if (false == m_bButton)
		{
			_int iCom_Texture_Set = 2;
			m_pButtonUI = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &iCom_Texture_Set);
			if (nullptr == m_pButtonUI)
			{
				BREAKPOINT;
				return RESULT_NOPROBLEM;
			}
			m_bButton = true;
			GM->Get_Player()->Play_SideUI_PopUp_Sound();
		}

		if (true == m_pGameInstance->Get_KeyUp(DIK_F))
		{
			_int iCom_Texture = 0;
			m_pFadeOut        = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_LoadingBlack"), &iCom_Texture);
			m_bGoNextLevel    = true;
			static_cast<CUI_TBoxOpen*>(m_pButtonUI)->Set_Case0(true);
			m_pButtonUI = nullptr;

			m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
			m_pGameInstance->LetsPlaySound(TEXT("WarpNextStage.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);
		}
	}
	else
	{
		if (nullptr != m_pButtonUI)
		{
			static_cast<CUI_TBoxOpen*>(m_pButtonUI)->Set_Case0(true);
			m_pButtonUI = nullptr;
		}
		m_bButton = false;
	}

	if (true == m_bGoNextLevel)
	{
		if (0.f >= static_cast<UI_FadeOut*>(m_pFadeOut)->Get_FadeOut())
		{
			if (m_pGameInstance->Get_LevelIndex() == LEVEL_TWILIGHT)
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_RAPUNZEL);
			}
			else if (m_pGameInstance->Get_LevelIndex() == LEVEL_RAPUNZEL)
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_RAPUNZEL_BOSS);
			}
			else if (m_pGameInstance->Get_LevelIndex() == LEVEL_RAPUNZEL_BOSS)
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_FROZEN);
			}
			else if (m_pGameInstance->Get_LevelIndex() == LEVEL_FROZEN)
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_KEYGRAVE);
			}
			else if (m_pGameInstance->Get_LevelIndex() == LEVEL_KEYGRAVE)
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_FINALBOSS);
			}
		}
	}

	return _int();
}

_int CPortal::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		MSGBOX("0 > __super::LateTick in CPortal::LateTick");
		return RESULT_ERROR;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return _int();
}

HRESULT CPortal::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
	{
		MSGBOX("SetUp_ConstantTable returned E_FAIL in CPortal::Render");
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render(m_pShaderCom, m_iPass)))
	{
		MSGBOX("m_pVIBufferCom->Render returned E_FAIL in CPortal::Render");
		return E_FAIL;
	}
	return S_OK;
}

void CPortal::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// 월드행렬 테스트
	pImGui_Manager->SetGameObjectPosition(string("POR Pos"), m_pTransformCom);
	pImGui_Manager->SetGameObjectRotation(string("POR Rot"), m_pTransformCom);
	pImGui_Manager->SetGameObjectScale(string("POR Scale"), m_pTransformCom);
	pImGui_Manager->SmallSeparator();

	ImGui::End();
}


HRESULT CPortal::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPortal::SetUp_Components(Renderer)");
		return E_FAIL;
	}
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPortal::SetUp_Components(Shader");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPortal::SetUp_Components(VIBuffer_Rect)");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PortalImage"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("__super::Add_Component returned E_FAIL in CPortal::SetUp_Components(Texture)");
		return E_FAIL;
	}

	/* For.Com_NoiseTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_NoiseTexture"), (CComponent**)&m_pNoiseTextureCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPortal::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		MSGBOX("m_pShaderCom is nullptr in CPortal::SetUp_ConstantTable");
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CPortal::SetUp_ConstantTable");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPortal::SetUp_ConstantTable(View)");
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CPortal::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}


	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pNoiseTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", 37)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float fAlpha = m_fAlpha;
	if (m_fAlpha < 0.f)
		fAlpha = 0.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fTranslucency", &m_fAlpha, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	_bool bBlur = 1.f;
	if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
		bBlur = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float fDistortion = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fDistortion", &fDistortion, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Set_RawValue("g_fNoiseRot", &m_fNoiseUV, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}



CPortal * CPortal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPortal*	pInstance = new CPortal(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPortal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPortal::Clone(void * pArg)
{
	CPortal*	pInstance = new CPortal(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPortal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPortal::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNoiseTextureCom);
}
