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
	// AnimList 를 순차적으로 실행하는 함수

	// 마지막 애니메이션이 아닐 때
	if (m_iAnimPlayIndex != AnimList.size() - 1)
	{
		// AnimList 의 현재 애니메이션이 끝나면
		if (pModel->Check_CurAnim_And_Finished(AnimList[m_iAnimPlayIndex]))
		{
			// 다음 애니메이션 재생을 위해 인덱스 증가
			if (++m_iAnimPlayIndex > AnimList.size() - 1)
				--m_iAnimPlayIndex;

			// Loop 애니메이션의 경우에는 Set_TempIndex 로 초기화하고
			for (auto& Index : LoopAnimList)
			{
				if (AnimList[m_iAnimPlayIndex] == Index)
				{
					pModel->Set_TempIndex(AnimList[m_iAnimPlayIndex]);
					return false;
				}
			}

			// Once 애니메이션의 경우에는 Set_AnimAndReset 으로 초기화
			Set_AnimAndReset(pModel, AnimList[m_iAnimPlayIndex]);
		}
	}

	// 마지막 애니메이션 재생이 끝나면 true 리턴
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