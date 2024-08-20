#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStatus final : public CComponent
{
public:
	typedef struct tagStatusDesc
	{
		// �⺻ ����
		_float	fMaxHP         = 100.f;		// �ִ� ü��
		_float	fHP            = fMaxHP;	// ���� ü��

		_float	fMaxMP		   = 100.f;		// �ִ� MP
		_float  fMP			   = fMaxMP;	// ���� MP

		_float	fDamaged       = 5.f;		// ���� ������
		_int	iLevel         = 0;			// ����
		_float	fEXP           = 0.f;		// ����ġ

		// ���� �̻� ����
		_bool	bStun          = false;		// ���� ����
		_double MaxStunTime    = 5.0;		// ���� ���� �ð�
		_double StunTime       = 0.0;		// ���� ���� ���� �ð�

		_double SlowSpeed	   = 1.0;		// ���ο� �����
	}STATUSDESC;

private:
	explicit CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	void Update(_double TimeDelta);

public: // Getter
	STATUSDESC	Get_StatusDesc()	{ return m_StatusDesc; }
	_float		Get_MaxHP()			{ return m_StatusDesc.fMaxHP; }
	_float		Get_HP_Ratio()		{ return m_StatusDesc.fHP / m_StatusDesc.fMaxHP; }
	_float		Get_HP()			{ return m_StatusDesc.fHP; }
	_float		Get_MaxMP()			{ return m_StatusDesc.fMaxMP; }
	_float		Get_MP_Ratio()		{ return m_StatusDesc.fMP/ m_StatusDesc.fMaxMP; }
	_float		Get_MP()			{ return m_StatusDesc.fMP; }
	_float		Get_Damage()		{ return m_StatusDesc.fDamaged; }
	_int		Get_Level()			{ return m_StatusDesc.iLevel; }
	_bool		Get_Stun()			{ return m_StatusDesc.bStun; }
	_double		Get_SlowSpeed()		{ return m_StatusDesc.SlowSpeed; }

public: // Setter
	void Set_HP(_float fHP)				{ m_StatusDesc.fHP = fHP; }
	void Set_MP(_float fMP)				{ m_StatusDesc.fMP = fMP; }
	void Set_Damage(_float fDamage)		{ m_StatusDesc.fDamaged = fDamage; }
	void Set_Stun(_bool bOption, _double StunTime);
	void Set_Slow(_double SlowRatio)	{ m_StatusDesc.SlowSpeed = SlowRatio; }

public: // �׼ǰ���
	_bool Heal(_float fHealAmount);
	_bool HealMP(_float fMP);
	_bool Revive();
	_bool Damaged(_float fDamage);
	_bool DamagedMP(_float fDamage);
	_bool ExpUp(_float fEXP);
	_bool LevelUp();

private:
	STATUSDESC m_StatusDesc;

public:
	static	CStatus*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END