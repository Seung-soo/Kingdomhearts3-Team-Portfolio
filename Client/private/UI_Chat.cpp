#include "stdafx.h"
#include "..\public\UI_Chat.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Progress_Manager.h"
#include "Camera_Manager.h"
#include "TT_NPC0.h"

CUI_Chat::CUI_Chat(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Chat::CUI_Chat(const CUI_Chat & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Chat::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Chat::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	CHATDESC tChatDesc;
	memcpy(&tChatDesc, pArg, sizeof(CHATDESC));

	m_eEvent = tChatDesc.eEvent;


	switch (m_eEvent)
	{
	case EVENT_QUEST:
		m_iTexIndex = 0;
		break;
	case EVENT_QUESTCLEAR:
		m_iTexIndex = 3;
		break;
	}



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

	SizeX = 400.f;
	SizeY = 150.f;
	SetX = 640.f;
	SetY = 75.f;

	m_fDepth = 0.2f;

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
	m_pGameInstance->LetsPlaySound(TEXT("NPC2.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);

	return S_OK;
}

_int CUI_Chat::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	switch (m_eEvent)
	{
	case EVENT_QUEST:
		if (m_pGameInstance->Get_KeyEnter(DIK_F))
		{
			m_iTexIndex++;
			if (2 == m_iTexIndex)
			{
				// 대화 중간에 카메라씬
				CCamera_Manager::GetInstance()->Load_Camera("Twilight_Quest_Start.dat");
				CCamera_Manager::GetInstance()->Play_Start();
				m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
				m_pGameInstance->LetsPlaySound(TEXT("NPC1.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);
			}
			if (2 < m_iTexIndex)
			{
				m_iTexIndex = 2;
				m_bDead = true;
				GM->Set_bCameraMove(false);
				static_cast<CTT_NPC0*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_NPC")))->Set_TalkFinish(true);
				PM->QuestNavi_TargetSetting(__vector(-8.5f, 0.f, -71.f, 1.f));
			}

			// 사운드

			switch (m_iTexIndex)
			{
			case 1:
				m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
				m_pGameInstance->LetsPlaySound(TEXT("NPC0.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);
				break;
			}
		}
		break;
	case EVENT_QUESTCLEAR:
		if (m_pGameInstance->Get_KeyEnter(DIK_F))
		{
			m_iTexIndex++;
			if (4 == m_iTexIndex)
			{
				// 대화 중간에 카메라씬
				CCamera_Manager::GetInstance()->Load_Camera("Twilight_PickUp_KeyBlade.dat");
				CCamera_Manager::GetInstance()->Play_Start();
				m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT);
				m_pGameInstance->LetsPlaySound(TEXT("NPC3.wav"), CSoundMgr::SYSTEM_EFFECT, 0.7f);
			}

			if (4 < m_iTexIndex)
			{
				m_iTexIndex = 4;
				m_bDead = true;
				GM->Set_bCameraMove(false);
				static_cast<CTT_NPC0*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_NPC")))->Set_TalkFinish(true);
				PM->QuestNavi_TargetSetting(__vector(80.5f, 0.f, -43.f, 1.f));
			}

		}
		break;
	}

	return _int();
}

_int CUI_Chat::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_Chat::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Chat::SetUp_Components()
{	
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Chat"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_Chat::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_iTexIndex));

	return S_OK;
}

CUI_Chat * CUI_Chat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Chat*	pInstance = new CUI_Chat(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Chat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Chat::Clone(void * pArg)
{
	CUI_Chat*	pInstance = new CUI_Chat(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Chat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Chat::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
