#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CState_Jump final : public CState
{
public:
	typedef struct tagJumpStateStruct
	{
		CGameObject::OBJECT_TYPE eObjectType = CGameObject::OBJECT_TYPE::OBJECT_END;
		
		_uint iNumberOfJumpAnim	= 0;		// ���� ���� �ִϸ��̼��� ����(ex : jump_up, jump_loop, jump_end�� ��� 3��)
		_uint iJumpAnimIndex	= 0;		// ���õ� �ִϸ��̼� ü�ο� ������ �ε��� ����

		_bool bChangeAnimation = true;

	} JUMP_STATE_DESC;

public:
	explicit CState_Jump() = default;
	virtual ~CState_Jump() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta);

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;

public:
	void Reset_State();					// ���� ���� ����
	void Change_Animation();			// ���� �ִϸ��̼� ü���� ���� �ִϸ��̼� ����
	void Change_Animation_Falling();	// ���� �ִϸ��̼����� ����

	// ���� �ִϸ��̼� ü�� ����
	void SetUp_AnimationChain(_bool bSecond = false); 

	// ���ڷ� ���� �ִϸ��̼��� ���� �ִϸ��̼� ü�� �ȿ� �ִ��� Ȯ��
	_bool Check_JumpAnimation(CPlayer::SORA_ANIM eAnimation); 

public:
	static _float g_Falling_Speed_Limit;
	static _float g_Falling_Speed_Limit_Hovering;
	static _double g_SecondJump_CoolDown;

private:
	CTransform::JUMP_DESC* m_JumpDesc = nullptr;

	class CPlayer*	m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	CNavigation*	m_pNavigation = nullptr;
	
	_bool m_bSecondJump = false;
	_double m_dSecondJumpTimer = 0.0;

	JUMP_STATE_DESC m_Desc;
	vector<_uint> m_JumpAnimChain;

public:
	static CState_Jump* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END