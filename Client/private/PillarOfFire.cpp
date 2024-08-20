#include "stdafx.h"
#include "PillarOfFire.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "Effect_Manager.h"
#include "Player.h"

CPillarOfFire::CPillarOfFire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CPillarOfFire::CPillarOfFire(const CPillarOfFire& rhs)
	: CMonster(rhs)
{
}

HRESULT CPillarOfFire::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPillarOfFire::NativeConstruct(void* pArg)
{
	if (pArg == nullptr)
	{
		BREAKPOINT;
	}
	else
	{
		memcpy(&m_PillarDesc, pArg, sizeof(PILLARDESC));
	}

	// �θ� �ʱ�ȭ�� �� Ʈ������ �ʱ�ȭ
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 25.f;
	TransformDesc.fRotationPerSec = 3.5f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// ��ġ ����
	m_pTransformCom->Set_Position(m_PillarDesc.vPosition);

	// ������Ʈ �ʱ�ȭ
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// ���̴��н� ����
	m_iPass = 31;

	// �÷��̾� ���� ��������
	m_pPlayer = m_pGameInstance->Get_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Player"), 0);
	m_pPlayerTransform = m_pPlayer->Get_Transform();

	if (m_pPlayer == nullptr || m_pPlayerTransform == nullptr)
	{
		BREAKPOINT;
	}

	// m_vRevolutionCenterPos ����
	m_vRevolutionCenterPos = m_pPlayerTransform->Get_Position();

	// ����Ʈ����
	if (m_PillarDesc.bShortPillar == true)
	{
		CEffect_Manager::GetInstance()->Create_GoatPillarOfFire_Short_Ready(m_pTransformCom);
		m_pGameInstance->LetsPlaySound_Monster(TEXT("Goat_60_FireBall.wav"), 0.7f, true, m_pTransformCom->Get_Position());
	}

	// ���� ���
	if (m_PillarDesc.bGravitation == true && m_PillarDesc.bShortPillar == false)
	{
		// �߷�
		m_iCurPlaySound = m_pGameInstance->PlayMonsterRoofSound(TEXT("Goat_57_PillarOfFire.wav"), 0.7f, true, m_pTransformCom->Get_Position());
	}

	m_EffectInterval     = 0.5;
	m_WaveEffectInterval = 1.0;

	return S_OK;
}

_int CPillarOfFire::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// ������ RESULT_DEAD ��ȯ
	if (true == m_bDead)
	{
		m_pGameInstance->StopMonsterSound(m_iCurPlaySound);
		return RESULT_DEAD;
	}

	// ��ų ����
	Action(TimeDelta);

	// ����Ʈ�� ��ġ����
	m_pTransformCom->Get_PositionFloat4_Pointer();

	return RESULT_NOPROBLEM;
}

_int CPillarOfFire::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;
	 
	// ����ü �ȿ� �ִٸ� + m_bDead ���°� �ƴϸ� �����׷쿡 �߰�
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && m_bDead == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CPillarOfFire::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

void CPillarOfFire::PostRender(_double TimeDelta)
{
	ImGui::Begin("PillarOfFire");

	CImGui_Manager::GetInstance()->SetGameObjectScale(string("PF Scale"), m_pTransformCom, 0.1f, 100.f);

	ImGui::Text("Dis : %.1f", m_pTransformCom->Get_Distance(m_pPlayerTransform));

	ImGui::End();
}

HRESULT CPillarOfFire::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPillarOfFire::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}

_bool CPillarOfFire::Action(_double TimeDelta)
{
	// �ð�����
	m_TimeAcc += TimeDelta;

	// ������� �ұ��
	if (m_PillarDesc.bGravitation == true && m_PillarDesc.bShortPillar == false)
	{
		// ���� ����Ʈ
		m_EffectInterval += TimeDelta;
		if (m_EffectInterval >= 0.5)
		{
			CEffect_Manager::GetInstance()->Create_GoatSmoke(m_pTransformCom->Get_Position(), 50, -10.f, 10.f, 0.f, 10.f, -10.f, 10.f, 1.f);
			m_EffectInterval = 0.0;
		}

		// ���̺� ����Ʈ
		m_WaveEffectInterval += TimeDelta;
		if (m_WaveEffectInterval >= 1.0)
		{
			CEffect_Manager::GetInstance()->Create_Goat_PillarWave(m_pTransformCom);
			m_WaveEffectInterval = 0.0;
		}

		// �÷��̾� �������
		if (2.2 < m_TimeAcc)
			m_pTransformCom->PullObject(m_pPlayerTransform, TimeDelta, 0.5);

		// �÷��̾ ���� ���� �ȿ� ������ �÷��̾�� ������
		if (m_pTransformCom->IsInRange(m_pPlayerTransform, 9.75f))
		{
			// �÷��̾� �浹 �Լ�
			static_cast<CPlayer*>(m_pPlayer)->Attacked_By_PillarOfFire(m_pTransformCom->Get_Position());
		}

		// ���� �ð� �̻� ������ ����
		if (m_TimeAcc >= 10.0)
		{
			m_bDead = true;
		}
	}

	// �������� �ʴ� �ұ��(����)
	if (m_PillarDesc.bGravitation == false && m_PillarDesc.bShortPillar == false)
	{
		// ���� �ð��� �� �������� ����
		if (m_TimeAcc < 9.0)
		{
			// ����
			m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), m_vRevolutionCenterPos, TimeDelta);

			// ������ �����ӵ� ������
			_float fSpeed = m_pTransformCom->Get_RotSpeedR();
			m_pTransformCom->Set_RotSpeedR(fSpeed + (_float)TimeDelta * 0.5f);
		}

		// �÷��̾ ���� ���� �ȿ� ������ �÷��̾�� ������
		if (m_pTransformCom->IsInRange(m_pPlayerTransform, 3.5f))
		{
			// �÷��̾� �浹 �Լ�
			static_cast<CPlayer*>(m_pPlayer)->Attacked_By_PillarOfFire(m_pTransformCom->Get_Position());
		}

		// �߾����� ����
		if (m_TimeAcc >= 9.0)
		{
			m_pTransformCom->Go_To_TargetXZ(m_vRevolutionCenterPos, 0.f, TimeDelta);
		}

		// �߾ӿ� ���� �Ÿ� �̻� �����ϸ� ����
		if (m_pTransformCom->Get_Distance(m_vRevolutionCenterPos) <= 0.3f || m_TimeAcc >= 10.0)
		{
			m_bDead = true;
		}
	}

	// ���� �ұ��
	if (m_PillarDesc.bShortPillar == true)
	{
		if (m_TimeAcc >= 1.0 && m_bEffectEnd == false)
		{
			CEffect_Manager::GetInstance()->Create_GoatPillarOfFire_Short(m_pTransformCom);
			m_iCurPlaySound = m_pGameInstance->PlayMonsterRoofSound(TEXT("Goat_57_PillarOfFire.wav"), 0.7f, true, m_pTransformCom->Get_Position());

			m_bEffectEnd = true;
		}

		if (m_TimeAcc >= 1.0)
		{
			// �÷��̾ ���� ���� �ȿ� ������ �÷��̾�� ������
			if (m_pTransformCom->IsInRange(m_pPlayerTransform, 3.f))
			{
				// �÷��̾� �浹 �Լ�
				static_cast<CPlayer*>(m_pPlayer)->Attacked_By_PillarOfFire(m_pTransformCom->Get_Position());
			}
		}

		// ���� �ð� �̻� ������ ����
		if (m_TimeAcc >= 6.0)
		{
			m_bDead = true;
		}
	}

	return false;
}

CPillarOfFire* CPillarOfFire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPillarOfFire* pInstance = new CPillarOfFire(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPillarOfFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPillarOfFire::Clone(void* pArg)
{
	CPillarOfFire* pInstance = new CPillarOfFire(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPillarOfFire");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPillarOfFire::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}