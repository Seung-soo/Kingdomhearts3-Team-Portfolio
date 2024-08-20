#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CState_PlayerMove final : public CState
{
public:
	explicit CState_PlayerMove() = default;
	virtual ~CState_PlayerMove() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool	KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta);

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;

private:
	void Rotate_Player(_double TimeDelta);
	void Translate_Player(_double TimeDelta);

public:
	static _uint	g_Angle_for_ExceptionHandling;

private:
	class CPlayer* m_pPlayer = nullptr;
	class CCamera_Perspective* m_pCamera = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;

	CNavigation* m_pNavigation = nullptr;

	_uint m_iPlayedFrame = UINT_MAX;

private:
	void ChangeAnimation_OnNormalForm();
	void ChangeAnimation_OnHammerForm();
	void ChangeAnimation_OnRageForm();

	void PlayStepSound_OnRun();

public:
	static CState_PlayerMove* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END