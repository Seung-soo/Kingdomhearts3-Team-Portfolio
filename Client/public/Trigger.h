#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CTrigger final : public CGameObject
{
public:
	enum TRIGGER
	{
		TRIGGER_TT_FIRST, TRIGGER_TT_SECOND, TRIGGER_TT_BOSS,										// Twilight Town
		TRIGER_RA_FIRST, TRIGGER_RA_SECOND, TRIGGER_RA_THIRD, TRIGGER_RA_FOURTH, TRIGGER_RA_FIFTH,	// Rapunzel
		TRIGGER_RA_BOSS_BIGTREE,																	// Rapunzel Boss
		TRIGGER_END
	};

private:
	CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

public:
	virtual HRESULT		Save_ObjectData(HANDLE hFile) override;
	virtual CCollider*	Get_HitCollider() { return m_pColliderCom; }
	void				Set_TriggerType(TRIGGER eTriggerType) { m_eTrigger = eTriggerType; }

private:
	HRESULT SetUp_Components();

private:
	void OnCollision(_double TimeDelta);	// 충돌 처리 함수
	void ActiveTrigger(_double TimeDelta);	// 충돌 시 실행시킬 함수
	void PickingTrigger(_double TimeDelta); // 마우스로 피킹해서 데이터 설정하는 함수

private:
	CRenderer*	m_pRendererCom = nullptr;
	CCollider*	m_pColliderCom = nullptr;

	TRIGGER	 m_eTrigger = TRIGGER_END;
	const char* Triggers[TRIGGER_END + 1] = { 
		"TRIGGER_TT_FIRST", "TRIGGER_TT_SECOND", "TRIGGER_TT_BOSS",												// Twilight Town
		"TRIGGER_RA_FIRST", "TRIGGER_RA_SECOND", "TRIGGER_RA_THIRD", "TRIGGER_RA_FOURTH", "TRIGGER_RA_FIFTH",	// Rapunzel
		"TRIGGER_RA_BOSS_BIGTREE",																				// Rapunzel Boss
		"TRIGGER_END"
	};

public:
	static	CTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END