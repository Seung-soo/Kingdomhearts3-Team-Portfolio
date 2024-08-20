#include "Monster.h"
#include "Model.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

_int CMonster::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	m_pTransformCom->Get_PositionFloat4_Pointer();

	return RESULT_NOPROBLEM;
}

_int CMonster::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	return RESULT_NOPROBLEM;
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Set_AnimAndReset(CModel* pModel, _uint iAnimIndex)
{
	pModel->ResetAnimation(iAnimIndex);
	pModel->Set_TempIndex(iAnimIndex);
}

void CMonster::Set_Stun(_double StunTime)
{
	CStatus* pStatus = dynamic_cast<CStatus*>(Get_Component(TEXT("Com_Status")));

	if (pStatus == nullptr)
		return;

	pStatus->Set_Stun(true, StunTime);
}

void CMonster::Set_Slow(_double SlowRatio)
{
	CStatus* pStatus = dynamic_cast<CStatus*>(Get_Component(TEXT("Com_Status")));

	if (pStatus == nullptr)
		return;

	pStatus->Set_Slow(SlowRatio);
}

_bool CMonster::PlayAnimations(CModel* pModel, vector<_uint> AnimList, vector<_uint> LoopAnimList)
{
	// AnimList �� ���������� �����ϴ� �Լ�

	// ������ �ִϸ��̼��� �ƴ� ��
	if (m_iAnimPlayIndex != AnimList.size() - 1)
	{
		// AnimList �� ���� �ִϸ��̼��� ������
		if (pModel->Check_CurAnim_And_Finished(AnimList[m_iAnimPlayIndex]))
		{
			// ���� �ִϸ��̼� ����� ���� �ε��� ����
			if (++m_iAnimPlayIndex > AnimList.size() - 1)
				--m_iAnimPlayIndex;

			// Loop �ִϸ��̼��� ��쿡�� Set_TempIndex �� �ʱ�ȭ�ϰ�
			for (auto& Index : LoopAnimList)
			{
				if (AnimList[m_iAnimPlayIndex] == Index)
				{
					pModel->Set_TempIndex(AnimList[m_iAnimPlayIndex]);
					return false;
				}
			}

			// Once �ִϸ��̼��� ��쿡�� Set_AnimAndReset ���� �ʱ�ȭ
			Set_AnimAndReset(pModel, AnimList[m_iAnimPlayIndex]);
		}
	}

	// ������ �ִϸ��̼� ����� ������ true ����
	if (m_iAnimPlayIndex == AnimList.size() - 1)
	{
		if (pModel->Check_CurAnim_And_Finished(AnimList.back()) == true)
		{
			m_iAnimPlayIndex = 0;
			return true;
		}
	}

	return false;
}

void CMonster::SetAttackTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime)
{
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= startTime)
		Set_IsAttacking(true);
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= endTime)
		Set_IsAttacking(false);
}

void CMonster::SetMotionBlurTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime)
{
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= startTime)
		Set_Pass(15);
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= endTime)
		Set_Pass(0);
}

void CMonster::SetMotionTrailTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime)
{
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= startTime)
		m_bUseMotionTrail = true;
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= endTime)
		m_bUseMotionTrail = false;
}

void CMonster::Free()
{
	__super::Free();
}