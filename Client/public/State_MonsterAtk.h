#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"

BEGIN(Engine)
class CTransform;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_MonsterAtk final : public CState
{
private:
	explicit CState_MonsterAtk();
	virtual ~CState_MonsterAtk() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual _bool	Action(_double TimeDelta);

private:
	void	JustLook(_double TimeDelta); // Player ������ ȸ��
	_bool	AtkAcc(_uint MaxAtkNum, _double TimeDelta); // ���� �ִϸ��̼� Ƚ�� ����
	_bool	Chase(_float fDistDiff, _double TimeDelta, _float fLookSpeed = 0.1f); // Player �Ĵٺ��鼭 �������� �Ѿƿ�

	// ���ͺ� ����ó��
	void	CrowSoldierAtk(_double TimeDelta);
	_bool	BowManAtk(_double TimeDelta);
	_bool	CannonAtk(_double TimeDelta);
	_bool	MonkeyAtk(_double TimeDelta);
	_bool	MowerAtk(_double TimeDelta);
	_bool	NightmareAtk(_double TimeDelta);
	_bool	NobodyAtk(_double TimeDelta);
	_bool	ThinSoldierAtk(_double TimeDelta);

private:
	CTransform*		m_pPlayerTransform   = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_END;
	CNavigation*	m_pMonsterNavigation = nullptr;

	class CMonster* m_pMonster = nullptr;

	_double m_TimeAcc = 0.0;
	_uint	m_iAtkNum = 0;

	_uint m_iNextAnimation = 0;

	_bool m_bIsSkillEnd[2] = { false, };

public:
	static CState_MonsterAtk* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END