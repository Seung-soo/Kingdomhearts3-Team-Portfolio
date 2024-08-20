#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMonsterCooler final : public CComponent
{
public:
	typedef struct tagMonsterCoolerDesc
	{
		_double MaxAttackCool;
		_double AttackCool = 0.0;

		_double MaxChaseCool;
		_double ChaseCool = 0.0;

		_double MaxDynamicMoveCool;
		_double DynamicMoveCool = 0.0;

		_double MaxRunAwayCool;
		_double RunAwayCool = 0.0;

		_double MaxWaitCool;
		_double WaitCool = 0.0;
	}MONSTERCOOLERDESC;

	enum COOLTYPE
	{
		COOLTYPE_ATK, COOLTYPE_CHASE, COOLTYPE_DYNAMICMOVE, COOLTYPE_RUNAWAY, COOLTYPE_WAIT, COOLTYPE_END
	};

private:
	explicit CMonsterCooler(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonsterCooler(const CMonsterCooler& rhs);
	virtual ~CMonsterCooler() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void	Update(_double TimeDelta);
	_bool	Get_IsStatePossible(COOLTYPE eCoolType);
	void	Cool_State(COOLTYPE eCoolType);
	void	Reset_Cool(COOLTYPE eCoolType);
	void	Set_Cool(COOLTYPE eCoolType, _double CoolTime);

private:
	MONSTERCOOLERDESC m_MonsterCoolerDesc;

public:
	static	CMonsterCooler* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END