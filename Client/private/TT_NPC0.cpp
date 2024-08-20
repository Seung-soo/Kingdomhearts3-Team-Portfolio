#include "stdafx.h"
#include "..\public\TT_NPC0.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "Camera_Perspective.h"
#include "Player.h"
#include "Camera_Manager.h"
#include "UI_Chat.h"

CTT_NPC0::CTT_NPC0(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTT_NPC0::CTT_NPC0(const CTT_NPC0 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTT_NPC0::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTT_NPC0::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 0.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Add_OnceListAll();
	m_pModelCom->Rmv_OnceList(0);
	m_pModelCom->Add_LerpListAll();

	m_pModelCom->ResetAnimation(0);
	m_pModelCom->Set_TempIndex(0);

	//m_pModelCom->Set_TempIndex(0);

	_vector	vNPCPosition = XMVectorSet(-5.f, 0.5f, -27.f, 1.f);
	m_pTransformCom->Set_Position(vNPCPosition);
	
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	m_pPlayerCollider = static_cast<CCollider*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Player"))->Get_Component(TEXT("Com_Collider_Sphere")));
	m_pCamera = static_cast<CCamera_Perspective*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Camera")));

	return S_OK;
}


_int CTT_NPC0::Tick(_double TimeDelta)
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

	if (false == SetyBool)
	{
		dTimeSet += TimeDelta;

		if (0.01 < dTimeSet)
		{
			Sety_Speed *= 1.01f;
			Sety = Sety_Speed;
			dTimeSet = 0.0;
		}

		if (0.3f <= Sety)
		{
			Sety_Speed = 0.01f;
			Sety = 0.3f;
			SetyBool = true;
		}
	}
	else
	{
		dTimeSet += TimeDelta;

		if (0.01 < dTimeSet)
		{
			Sety_Speed *= 1.01f;
			Sety = 0.3f -Sety_Speed;
			dTimeSet = 0.0;
		}

		if (0.f >= Sety)
		{
			Sety_Speed = 0.01f;
			Sety = 0.f;
			SetyBool = false;
		}
	}

	_vector	vNPCPosition = XMVectorSet(-5.f, 0.7f + Sety, -27.f, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNPCPosition);

	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta*2.0);
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return RESULT_NOPROBLEM;
}

_int CTT_NPC0::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// NPC를 바라볼 수 있는 상태면
	if (true == m_bNpcLook)
	{
		// NPC 주변에 갔을 때
		if (true == m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, m_pPlayerCollider))
		{
			// 카메라를 NPC를 바라보게 만들어주고
			m_pCamera->Set_LookNPC(true, m_pTransformCom);

			// 플레이어 이동을 막고
			static_cast<CPlayer*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Player")))->Set_ControlMode(false);

			// 카메라 움직임, 회전을 막고
			GM->Set_bCameraMove(true);

			// 다 처리 후 바라볼 수 없는 상태로 만들어준다.
			m_bNpcLook = false;

			// 카메라를 바라보는 방향 벡터를 구함
			m_vDir = static_cast<CTransform*>(m_pCamera->Get_Component(TEXT("Com_Transform")))->Get_Position() - m_pTransformCom->Get_Position();
			m_vDir = XMVector3Normalize(XMVectorSetY(m_vDir, 0.f));

			GM->Get_Player()->MakeState_Idle();
			GM->Get_Player()->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
		}
	}


	if (m_pCamera->Get_LookNPC() && false == m_bTurn)
	{
		m_bTurn = m_pTransformCom->Turn_To_Direction(-m_vDir, TimeDelta * 3.f);
	}


	// 카메라로 NPC를 다 바라보고 대화중일 때
	if (true == m_pCamera->Get_TalkNPC())
	{
		// 퀘스트
		Quest(TimeDelta);

	}

	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 1.5f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CTT_NPC0::Render()
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
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CTT_NPC0::SetUp_Components()
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_TT_NPC0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(10.f, 10.f, 10.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTT_NPC0::SetUp_ConstantTable()
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

void CTT_NPC0::Quest(_double TimeDelta)
{
	if (false == m_bTalkStart)
	{
		CUI_Chat::CHATDESC tChatDesc;
		switch (m_iProgress)
		{
		case 0:
			// UI(대화창) 띄우기

			tChatDesc.eEvent = CUI_Chat::EVENT_QUEST;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Chat"), TEXT("Prototype_GameObject_UI_Chat"), &tChatDesc)))
			{
				BREAKPOINT;
				return;
			}

			break;
		case 1:
			// UI(대화창) 띄우기
			tChatDesc.eEvent = CUI_Chat::EVENT_QUESTCLEAR;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Chat"), TEXT("Prototype_GameObject_UI_Chat"), &tChatDesc)))
			{
				BREAKPOINT;
				return;
			}
			break;
		}

		m_bTalkStart = true;
	}

	if (true == CCamera_Manager::GetInstance()->Get_Finish())
	{
		m_pCamera->Look_NPC(TimeDelta);
		// NPC가 카메라 쪽으로 돈다.
	}

	// 대화가 끝나면
	if (true == m_bTalkFinish)
	{
		// 카메라가 NPC를 바라보지 않게 해주고
		m_pCamera->Set_LookNPC(false, m_pTransformCom);

		// 대화중인 상태도 꺼주고
		m_pCamera->Set_TalkNPC(false);

		// 카메라 다시 움직일 수 있게 해주고
		GM->Set_bCameraMove(false);

		// 플레이이 이동을 풀어준다.
		static_cast<CPlayer*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Player")))->Set_ControlMode(true);

		// 카메라가 NPC를 바라보는 보간시간도 초기화
		m_pCamera->Set_LookNPC_Time(0.f);


		// 카메라씬도 강제 종료
		CCamera_Manager::GetInstance()->Reset();

		_int iComTex = 0;
		switch (m_iProgress)
		{
		case 0:
			// 퀘스트 창

			iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 1;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			break;
		case 1:
			// 퀘스트 완료

			iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 2;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			break;
		}
		// NPC가 다음에 상호작용 했을 때 다음 상태를 처리할 수 있게 값을 증가시켜준다.
		m_iProgress++;

		m_bTalkStart  = false;
		m_bTalkFinish = false;
		m_bTurn       = false;
	}
}

CTT_NPC0 * CTT_NPC0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTT_NPC0*	pInstance = new CTT_NPC0(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTT_NPC0");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTT_NPC0::Clone(void * pArg)
{
	CTT_NPC0*	pInstance = new CTT_NPC0(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTT_NPC0");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTT_NPC0::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
