#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;
class CHierarchyNode;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CDonald final : public CGameObject
{
public:
	enum STATE {STATE_APPEAR, STATE_DANCE, STATE_HEAL, STATE_ATTACK, STATE_EYE, STATE_END};
private:
	CDonald(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDonald(const CDonald& rhs);
	virtual ~CDonald() = default;
public:
	void				Set_Skill(STATE eSkill) { m_eSkill = eSkill; }

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

	_bool				m_bFire = false;

	_bool				m_bDance[4] = {};

	CHierarchyNode*		m_pNode = nullptr;
	_float4				m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	_float				m_fMakeIceTime = 0.f;

	_bool				m_bLastEffect = false;

	vector<CGameObject*> m_Ice;

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
	void	Heal(_double TimeDelta);
	void	Attack(_double TimeDelta);
	void	Dance(_double TimeDelta);
	void	Eye(_double TimeDelta);
public:
	static CDonald*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END