#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CModel;

class ENGINE_DLL CMonster abstract : public CGameObject
{
protected:
	explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_AnimAndReset(CModel* pModel, _uint iAnimIndex);
	void Set_MotionTrail(_bool bOption) { m_bUseMotionTrail = bOption; }
	void Set_Stun(_double StunTime);
	void Set_Slow(_double SlowRatio);

public:
	_uint			Get_MonsterType() { return m_MonsterType; }
	_bool			Get_IsBoss() { return m_bIsBoss; }
	virtual _bool	Damaged(CCollider* pAtkCollider, _float fDamage) { return false; }

public:
	// �ִϸ��̼� ����Ʈ�� ������ �̾ ��������ִ� �Լ� (Loop Ÿ���� �Ű������� ����)
	_bool	PlayAnimations(CModel* pModel, vector<_uint> AnimList, vector<_uint> LoopAnimList = vector<_uint>());
	// ���� �ݶ��̴� �Ѱ� ���ִ� �Լ�
	void	SetAttackTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime);
	void	SetMotionBlurTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime);
	void	SetMotionTrailTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime);
	void	ResetAnimPlayIndex() { m_iAnimPlayIndex = 0; }

protected:
	_uint m_MonsterType;

	vector<CGameObject*> m_CollisionParents;

	CGameObject*	m_pPlayer          = nullptr;
	CTransform*		m_pPlayerTransform = nullptr;

	// �ִϸ��̼� ���� ����� ���� �ε���
	_uint	m_iAnimPlayIndex = 0;
	_bool	m_bIsBoss = false;

protected:
	// ��� Ʈ����
	_bool					m_bUseMotionTrail = false;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END