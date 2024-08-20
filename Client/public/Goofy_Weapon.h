#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;
class CHierarchyNode;
class CCollider;
END

BEGIN(Client)

class CGoofy_Weapon final : public CGameObject
{
public:
	enum MODE { MODE_IDLE, MODE_RUN, MODE_THROW, MODE_END };
public:
	typedef struct tagWeaponDesc
	{
		CModel*			pModel = nullptr;
		CTransform*		pTransform = nullptr;
		const char*		pSocketName = nullptr;
		CGameObject*	pParent = nullptr;
		MODE			eMode = MODE_END;
	} WEAPON_DESC;

public:
	void			Set_ScaleUp(_bool bScaleUp) { m_bScaleUp = bScaleUp; }
	void			Set_ScaleDown(_bool bScaleDown) { m_bScaleDown = bScaleDown; }
	void			Set_ComeBackLerp(_float fComeBackLerp) { m_fComeBackLerp = fComeBackLerp; }
	void			Set_ComeBack(_bool bComeBack) { m_bComeBack = bComeBack; }
	void			Set_Throw(_bool bThrow) { m_bThrow = bThrow; }
	void			Set_Turn(_bool bTurn) { m_bTurn = bTurn; }
	void			Set_Run(_bool bRun) { m_bRun = bRun; }

	void			Set_Effect(_bool bEffect) { m_bEffect = bEffect; }
public:
	CGoofy_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGoofy_Weapon(const CGoofy_Weapon& rhs);
	virtual ~CGoofy_Weapon() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	HRESULT Run_Effect();
	void	Run_Attack();	// 방패로 미는 공격 충돌
	HRESULT Throw_Effect();
	HRESULT Throw_Particle();
private:
	CModel*					m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	class CGoofy*			m_pGoofy = nullptr;
	WEAPON_DESC				m_tWeaponDesc = {};

	CHierarchyNode*			m_pNode = nullptr;

	_float					m_fScale = 1.f;
	_float					m_fAlpha = 0.3f;
	_bool					m_bScaleDown = false;
	_bool					m_bScaleUp = false;

	_bool					m_bThrow = false;

	CCollider*				m_pTargetCollider = nullptr;
	_uint					m_iColliderIndex = 0;

	_bool					m_bComeBack = false;
	_float					m_fComeBackLerp = 0.f;

	_bool					m_bComeBackStart = false;
	_vector					m_vLerpStart;

	_bool					m_bTurn = false;

	_bool					m_bRun = false;

	list<CCollider*>		m_ColliderList;

	_bool					m_bFirstLateTick = false;

	//	이펙트 변수들
	_bool					m_bEffect = false;
	_bool					m_bEffectStart = false;
	_float4					m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	class CEffect_Mesh*		m_pEffect[2] = { nullptr };

	_bool					m_bPaticle = false;
	_float4					m_vPaticlePosition[4];

	vector<CGameObject*>	m_Particle;
public:
	static CGoofy_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END