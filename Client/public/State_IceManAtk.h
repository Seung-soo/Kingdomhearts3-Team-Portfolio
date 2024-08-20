#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"
#include "IceMan.h"

BEGIN(Engine)
class CTransform;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_IceManAtk final : public CState
{
private:
	explicit CState_IceManAtk();
	virtual ~CState_IceManAtk() = default;

private: // �ʱ�ȭ �Լ� (�� ���¸� ������ �� ����Ǵ� �Լ�)
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState(); // �� ���°� ���¸ӽſ� ���ε��� �� ���¸ӽ��� BindState �Լ����� �� �� ��������ִ� �Լ�
	virtual _bool	Action(_double TimeDelta); // ��ü�� �� Tick ���� ���� �Լ�
	virtual HRESULT ClearState(); // �ٸ� ������ �������� ������ ������ �� ȣ��Ǵ� �Լ�


private: // IceMan �� �� ���ݿ� ���� �ൿ ����
	_bool Atk_WalkSlash(_double TimeDelta);
	_bool Atk_Clap(_double TimeDelta);
	_bool Atk_LongSlash(_double TimeDelta);
	_bool Atk_JumpDownwardBlow(_double TimeDelta);

private:
	_bool ChasePlayer(_double TimeDelta);

private:
	void Create_IceBall(_double TimeDelta, _fvector vFirstDir);

private:
	// �θ�Ŭ������ State �� GameObejct, Model, Transform �� �̸� ���ǵǾ�����
	// ������ �ʿ��� �͵鸸 ������ �����ָ� ��
	CIceMan*		m_pIceMan            = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_ICEMAN;
	CTransform*		m_pPlayerTransform   = nullptr;
	CNavigation*	m_pMonsterNavigation = nullptr;

	// IceMan �� ����� ����(�Լ��� ���ε��� �� �Ű������� �޾Ƽ� �������� ����)
	CIceMan::ICEMAN_SKILL m_eIceManSkill = CIceMan::ICEMAN_SKILL_END;

	// �ð�������, ����Ʈ ���ຯ��, ���� ���ຯ�� (���߿� �ʿ��� ���� ����ؼ� �̸� �߰��ص�)
	_double m_TimeAcc       = 0.0;
	_bool	m_bEffectEnd[3] = { false, };
	_bool	m_bSoundEnd[3]  = { false, };

	// �������� ���� ������
	_vector			m_vJumpDir;
	_double			m_GoPower;
	CGameObject*	m_pSkill = nullptr;

	// �ڼ����� ���� ����
	_bool m_bPushEnd = false;

private:
	// ���� �ִϸ��̼� �ε���
	// �� ������ ��κ��� �ִϸ��̼ǵ��� �ϳ��� �ִϸ��̼��� �������� �߰� �ɰ�����
	// �׷��� �������ݵ� �׳� �ִϸ��̼� �ϳ��� �ƴ϶� 35��~39�� �ִϸ��̼ǵ��� ���������� ������Ѿ���
	// Mosnter Ŭ������ �ִϸ��̼��� ���������� �����Ű�� ���� PlayAnimations �Լ��� �����״µ�, ���������� �����ų �ִϸ��̼��� vector �Ű������� �ޱ� ������ �̸� �����ص�
	// m_LongSlashIndices �������� ��쿣 �� �ִϸ��̼�(32��)�� ���� �� �����ų ���̶� ������ ���ڸ� �ݺ��ؼ� �־����
	vector<_uint> m_WalkSlash               = { 29 };
	vector<_uint> m_Clap                    = { 30, 30 };
	vector<_uint> m_LongSlashIndices        = { 31, 32, 32, 32, 32, 32, 32, 34 };
	vector<_uint> m_JumpDownwardBlowIndices = { 35, 36, 37, 38, 39 };

public:
	static CState_IceManAtk* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END