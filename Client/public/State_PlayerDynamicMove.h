#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CCamera_Perspective;
class CState_PlayerDynamicMove final : public CState
{
public:
	enum MOVE_TYPE
	{
		MOVE_TYPE_RUN_WALL, MOVE_TYPE_SOMERSAULT, MOVE_TYPE_STEPPING, MOVE_TYPE_END
	};

public:
	explicit CState_PlayerDynamicMove() = default;
	virtual ~CState_PlayerDynamicMove() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize(MOVE_TYPE _moveType);
	virtual _bool	KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta) override;

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;

private:
	class CPlayer* m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	CCamera_Perspective* m_pCamera = nullptr;
	CNavigation* m_pNavigation = nullptr;

	MOVE_TYPE m_eMoveType = MOVE_TYPE::MOVE_TYPE_END;
	_uint m_iPlayedFrame = UINT_MAX;

public:
	static CState_PlayerDynamicMove* Create(void* pArg);
	static CState_PlayerDynamicMove* Create(MOVE_TYPE _moveType);
	virtual void Free() override;
};

END