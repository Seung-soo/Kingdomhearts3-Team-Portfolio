#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CGoofy final : public CGameObject
{
public:
	enum STATE { STATE_APPEAR, STATE_PUSH, STATE_RUN, STATE_THROWSTART, STATE_THROWING, STATE_THROWEND, STATE_EYE, STATE_END };
private:
	CGoofy(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGoofy(const CGoofy& rhs);
	virtual ~CGoofy() = default;
public:
	void			Set_Skill(STATE eSkill) { m_eSkill = eSkill; }
	void			Set_ThrowEnd(_bool bThrowEnd) { m_bThrowEnd = bThrowEnd; }

	vector<CMonster*>*	Get_AttackMonsterContainer() { return &m_AttackMonster; }
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT NativeConstruct_Pool(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	HRESULT			Add_AttackMonster(CMonster* pMonster);
private:
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CNavigation*		m_pNavigationCom = nullptr;
private:
	STATE				m_eCurrentState = STATE_APPEAR;
	STATE				m_eSkill = STATE_END;

	_bool				m_bMakeWeapon = false;

	_double				m_dAnimationSpeed = 1.2;

	class CGoofy_Weapon*	m_pWeapon = nullptr;

	_bool				m_bWeaponScaleDown = false;

	_bool				m_bThrowEnd = false;

	_bool				m_bEffectStart = false;
	_bool				m_bEffectEnd = false;

	vector<CMonster*>	m_AttackMonster;

	_bool				m_bMotionTrail = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	HRESULT Separate_Channels();

private:
	HRESULT	State_Manage(_double TimeDelta);

	void	Appear(_double TimeDelta);
	void	Push(_double TimeDelta);
	void	Run(_double TimeDelta);
	void	ThrowStart(_double TimeDelta);
	void	Throwing(_double TimeDelta);
	void	ThrowEnd(_double TimeDelta);
	void	Eye(_double TimeDelta);
public:
	static CGoofy*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END