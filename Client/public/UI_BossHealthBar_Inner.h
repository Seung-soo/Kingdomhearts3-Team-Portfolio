#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CUnit;
class CBeetleQueen;
class CUI_BossHealthBar_Inner : public CUI
{
public:
	enum BOSS_BAR_TYPE
	{
		BOSS_BAR_FRONT, BOSS_BAR_MID, BOSS_BAR_BACK, BOSS_BAR_END
	};

	typedef struct tagBossHealthBarInnerStruct
	{
		CUnit* unit = nullptr;
		BOSS_BAR_TYPE boss_bar_type;

	} BOSS_INNER_DESC;

public:
	CUI_BossHealthBar_Inner(ID3D11Device* pDevice, ID3D11DeviceContext* pDC);
	CUI_BossHealthBar_Inner(const CUI_BossHealthBar_Inner& rhs);
	virtual ~CUI_BossHealthBar_Inner() = default;

public:
	virtual HRESULT Init_Proto() override;
	virtual HRESULT Init(void* pArg);
	virtual _int Update(_double deltaTime) override;
	virtual _int Late_Update(_double deltaTime) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_DefaultName() override;
	virtual HRESULT SetUp_Components() override;

private:
	BOSS_INNER_DESC bossBar_Inner_Desc;
	CBeetleQueen* m_pQueen = nullptr;

	CUI_BossHealthBar_Inner* m_pFront = nullptr;
	_float2 m_HoldingPointLB;
	_float2 m_HoldingPointCT;

	_bool m_bChange = false;
	_double m_dLerpTime = 0.0;
	wstring m_CurrentHP_Text;
	wstring m_MaxHP_Text;

public:
	static CUI_BossHealthBar_Inner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDC);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END