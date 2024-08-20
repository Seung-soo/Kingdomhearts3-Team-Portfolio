#include "stdafx.h"
#include "..\public\TreasureBox.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameManager.h"
#include "Player.h"
#include "TT_NPC0.h"
#include "UI_TBoxOpen.h"
#include "Effect_Manager.h"
#include "Camera_Perspective.h"
#include "Progress_Manager.h"

CTreasureBox::CTreasureBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CTreasureBox::CTreasureBox(const CTreasureBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTreasureBox::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasureBox::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 15.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	memcpy(&m_tBoxDesc, pArg, sizeof(BOXDESC));

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pTransformCom->MakeSameDirection(m_tBoxDesc.vLook);
	m_pTransformCom->Set_Position(m_tBoxDesc.vPosition);

	m_pPlayerTransform = static_cast<CTransform*>(GM->Get_Player()->Get_Component(TEXT("Com_Transform")));


	m_pModelCom->Add_OnceListAll();


	m_eCurrentState = STATE_CLOSE;
	m_pModelCom->Set_TempIndex(m_eCurrentState);
	m_iPass = 2;


	return S_OK;
}

HRESULT CTreasureBox::NativeConstruct_Pool(void * pArg)
{

	return S_OK;
}

_int CTreasureBox::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return RESULT_DEAD;

	}
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (FAILED(State_Manage(TimeDelta)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta);

	return _int();
}

_int CTreasureBox::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 1.5f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CTreasureBox::Render()
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
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		//m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CTreasureBox::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	_tchar	szModelTag[128] = TEXT("");

	/* For.Com_Model */
	switch (m_tBoxDesc.eBoxTheme)
	{
	case THEME_TT:
		lstrcpy(szModelTag, TEXT("Prototype_Component_Model_TreasureBox_TT"));
		break;
	case THEME_RA:
		lstrcpy(szModelTag, TEXT("Prototype_Component_Model_TreasureBox_RA"));
		break;
	case THEME_FZ:
		lstrcpy(szModelTag, TEXT("Prototype_Component_Model_TreasureBox_FZ"));
		break;
	}

	if (FAILED(__super::Add_Component(GM->Get_CurrentLevel(), szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CTreasureBox::SetUp_ConstantTable()
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

	return S_OK;
}


HRESULT CTreasureBox::State_Manage(_double TimeDelta)
{
	switch (m_eCurrentState)
	{
	case STATE_CLOSE:
		Close(TimeDelta);
		break;
	case STATE_OPENNING:
		Openning(TimeDelta);
		break;
	case STATE_OPEN:
		Open(TimeDelta);
		break;
	}

	return S_OK;
}

void CTreasureBox::Close(_double TimeDelta)
{
	// 플레이어가 일정 거리 안에 들어오면 상자가 자동으로 열리거나 상호작용해서 열 수 있게
	_float	fDistance = DISTANCE(m_pPlayerTransform->Get_Position() - m_pTransformCom->Get_Position());


	if (2.f > fDistance)
	{
		switch (m_tBoxDesc.eBoxTheme)
		{
		case THEME_TT:
			break;
		case THEME_RA:
		case THEME_FZ:
			// 플레이어 상자 여는 
			break;
		}

		if (false == m_bButton)
		{
			_int iCom_Texture_Set = 0;
			m_pButtonUI = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &iCom_Texture_Set);
			if (nullptr == m_pButtonUI)
			{
				BREAKPOINT;
				return;
			}
			m_bButton = true;

			// 사운드 플레이
			GM->Get_Player()->Play_SideUI_PopUp_Sound();
		}
		if (true == m_pGameInstance->Get_KeyEnter(DIK_F))
		{
			m_bOpen = true;
			if (LEVEL_TWILIGHT != GM->Get_CurrentLevel())
			{
				// 카메라 워크
				GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_OPENBOX, m_pTransformCom);

				// 상자 오픈 사운드를 위한 변수 설정
				GM->Get_Player()->SetUp_BoxOpenSoundValues();
			}
			else
			{
				m_eCurrentState = STATE_OPENNING;
				m_pModelCom->Set_TempIndex(m_eCurrentState);
				CEffect_Manager::GetInstance()->Create_InBoxEffect(m_pTransformCom);

				// 상자 오픈 사운드를 위한 변수 설정
				GM->Get_Player()->SetUp_BoxOpenSoundValues();
			}
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

	if (true == m_bOpen)
	{
		m_fOpenTime += (_float)TimeDelta;
		if (1.5f < m_fOpenTime)
		{
			m_eCurrentState = STATE_OPENNING;
			m_pModelCom->Set_TempIndex(m_eCurrentState);

			// Effect_Test
			CEffect_Manager::GetInstance()->Create_InBoxEffect(m_pTransformCom);
			CEffect_Manager::GetInstance()->Create_OpenBoxEffect(m_pTransformCom);
		}
	}

}

void CTreasureBox::Openning(_double TimeDelta)
{
	// 중간에 이펙트
	if (false == m_bPlayedParticle)
	{
		m_fEffectTimeAccum += (_float)TimeDelta;

		if (m_fEffectTimeAccum >= 1.0f)
		{
			CEffect_Manager::GetInstance()->Create_ShineParticle(m_pTransformCom, __vector(0.f, 2.5f, 0.f, 0.f));
			m_fEffectTimeAccum = 0.f;
			m_bPlayedParticle = true;
		}
	}

	
	if (true == m_pModelCom->Get_CurrFinished())
	{
		m_eCurrentState = STATE_OPEN;
		m_pModelCom->Set_TempIndex(m_eCurrentState);

		// 각각 이벤트 처리
		Event();
	}
}

void CTreasureBox::Open(_double TimeDelta)
{
}

void CTreasureBox::Event()
{
	switch (m_tBoxDesc.eEvent)
	{
	case EVENT_QUEST:
		Quest();
		break;
	case EVENT_KEYBLADE:
		KeyBlade();
		break;
	case EVENT_DONALD:
		Donald();
		break;
	case EVENT_GOOFY:
		Goofy();
		break;
	case EVENT_RAZYFORM:
		RazyForm();
		break;
	case EVENT_HAMMER:
		Hammer();
		break;
	}


}

void CTreasureBox::Quest()
{
	static_cast<CTT_NPC0*>(m_pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_NPC")))->Set_NpcLook(true);

	//	별 획득 UI
	_int iCom_Texture_Set = 0;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	iCom_Texture_Set = 4;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	PM->QuestNavi_TargetSetting(__vector(-5.f, 0.5f, -27.f, 1.f));
}

void CTreasureBox::KeyBlade()
{
	// 키블레이드 잠금 해제
	GM->Set_KeyBladeLock(false);
	// 첫 번째 트리거 활성화
	GM->Loading_Triggers(LEVEL_TWILIGHT, TEXT("../SaveFiles/Objects/TT/TT_Triggers_First.dat"));

	//	키블레이드 획득 UI
	_int iCom_Texture_Set = 0;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	iCom_Texture_Set = 1;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	PM->QuestNavi_TargetSetting(__vector(82.f, 2.8f, -81.7f, 1.f));
}

void CTreasureBox::Donald()
{
	//	도널드 획득 UI
	_int iCom_Texture_Set = 0;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	iCom_Texture_Set = 2;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	GM->Set_Slot_On_Goofy(true);
}

void CTreasureBox::Goofy()
{
	//	구피 획득 UI
	_int iCom_Texture_Set = 0;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	iCom_Texture_Set = 3;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	GM->Set_Slot_On_Donald(true);
	PM->ToUseInterval(true, 2.f, CProgress_Manager::ACT_SUMMONTUTORIAL);
}

void CTreasureBox::Hammer()
{
	//	해머 획득 UI
	_int iCom_Texture_Set = 0;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}

	iCom_Texture_Set = 5;
	if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_GetCenter"), &iCom_Texture_Set)))
	{
		BREAKPOINT;
		return;
	}
}

void CTreasureBox::RazyForm()
{
}

CTreasureBox * CTreasureBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTreasureBox*	pInstance = new CTreasureBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTreasureBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTreasureBox::Clone(void * pArg)
{
	CTreasureBox*	pInstance = new CTreasureBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTreasureBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTreasureBox::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
