#pragma once

#include "Base.h"
#include "Collider.h"
BEGIN(Engine)

class CComponent;
class CTexture;

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum OBJECT_TYPE
	{
		OBJECT_CAMERA, OBJECT_UI, OBJECT_PLAYER, OBJECT_WEAPON, OBJECT_MONSTER, OBJECT_END
	};

	enum ATTACK_TYPE
	{
		ATTACK_TYPE_NORMAL,			// 일반 공격
		ATTACK_TYPE_HEAVY,			// 강한 공격
		ATTACK_TYPE_AERIAL,			// 띄우는 공격
		ATTACK_TYPE_NORECATION,		// 피격 모션이 등장하지 않는 공격
		ATTACK_TYPE_END
	};

protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	CComponent*			Get_Component(const _tchar* pComponentTag);
	class CTransform*	Get_Transform();

	OBJECT_TYPE		Get_ObjectType() { return m_eObjectType; }
	void			Set_ObjectType(OBJECT_TYPE eObjectType) { m_eObjectType = eObjectType; }

	virtual void	Set_Active(_bool isActive) { m_isActive = isActive; }
	_bool			Is_Active() { return m_isActive; }

	ATTACK_TYPE		Get_AttackType() { return m_eAttackType; }
	void			Set_AttackType(ATTACK_TYPE eAttackType) { m_eAttackType = eAttackType; }

	_float			Get_Depth() { return m_fDepth; }

	virtual void	Set_Dead(_bool bDead) { m_bDead = bDead; }
	virtual _bool	Get_Dead() { return m_bDead; }
	_bool*			Get_DeadPointer() { return &m_bDead; }

	void			Set_IsAttacking(_bool bOption) { m_bIsAttacking = bOption; }
	_bool			Get_IsAttacking() { return m_bIsAttacking; }

	void			Set_ReadyToDie(_bool bReadyToDie) { m_bReadyToDie = bReadyToDie; }
	_bool			Get_ReadyToDie() { return m_bReadyToDie; }

	void			Set_Pass(_uint iPass) { m_iPass = iPass; }
	_float			Get_AlphaDepth();

	_float			GetFrustumRange() { return m_fFrustumRange; }
	void			SetFrustumRange(_float fRange) { m_fFrustumRange = fRange; }

	void			Set_ImGuiController(_bool bOption)	{ m_bImGuiController = bOption; }
	_bool			Get_ImGuiController()				{ return m_bImGuiController; }

	_bool			Get_FirstTick() { return m_bFirstTick; }
	void			Set_FirstTick(_bool bFirstTick) { m_bFirstTick = bFirstTick; }

	void			StartDissolve(_float fDissolveSpeed);
	_float			GetDissolveAmount() { return m_fDissolveAmount; }


	const _tchar*	Get_LayerTag() { return m_LayerTag; }
	void			Set_LayerTag(const _tchar* pLayerTag) { lstrcpy(m_LayerTag, pLayerTag); }

	void StartRimLight(_uint iPass) { m_iPassBeforeRimLight = m_iPass; m_iPass = iPass; }
	void EndRimLight() { m_iPass = m_iPassBeforeRimLight; }

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT NativeConstruct_Pool(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	LateTick(_double dTimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta); // 객체 내부에서 ImGui 컨트롤하기 위한 함수

	//Shadow를 그리기 위한 함수 각 객체에서 오버라이딩 해서 만들어준다
	virtual HRESULT Render_Shadow();
public:
	HRESULT			Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** pOut, void* pArg = nullptr);
public:
	virtual HRESULT			Save_ObjectData(HANDLE hFile) { return S_OK; }
	virtual HRESULT			Load_ObjectData(HANDLE hFile) { return S_OK; }
	virtual CCollider*		Get_HitCollider() { return nullptr; }

	// For Shadow
	_bool					Is_ShadowStatic() { return m_isStaticShadow; }

protected:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	
	class CTransform*		m_pTransformCom = nullptr;

public:
	static const _tchar*	m_pTransformTag;

protected:
	map<const _tchar*, CComponent*>			m_Components;
	typedef map<const _tchar*, CComponent*> COMPONENTS;

	OBJECT_TYPE			m_eObjectType	= OBJECT_TYPE::OBJECT_END;
	ATTACK_TYPE			m_eAttackType	= ATTACK_TYPE::ATTACK_TYPE_END;

	_float				m_fDepth        = 0.f;
	_uint				m_iPass         = 0;
	_tchar				m_LayerTag[128]	= TEXT("");

	_bool				m_bFirstTick	= true;
	_uint				m_iTickCount	= 0;

	_bool				m_isCloned      = false;
	_bool				m_bSpawn        = false;
	_bool				m_bReadyToDie	= false;
	_bool				m_bDead         = false;
	_float				m_fFrustumRange = 5.f;

	_bool				m_bIsAttacking = false;
	_bool				m_isActive = true;


	// For Dissolve
	_bool				m_bDissolve        = false;
	_float				m_fDissolveSpeed   = 0.1f;
	_float				m_fDissolveAmount  = 0.f;
	CTexture*			m_pDissolveTexture = nullptr;

	// For Shadow
	_bool				m_isStaticShadow   = false; 

	// For RimLight
	_bool	m_bBlur     = false;
	_bool	m_bIsRim    = true;
	_float	m_fRimPower = 3.f;
	_float	m_fRimRange = 1.f;
	_float4 m_vRimColor = _float4(0.5f, 0.f, 0.f, 0.f);
	_float4 m_vCamPosition;
	_uint	m_iPassBeforeRimLight;

protected: 
	class CGameInstance*	m_pGameInstance = nullptr;
protected:
	_bool m_bImGuiController = FALSE; // 객체 내부에서 ImGui 컨트롤하기 위한 변수
protected:
	CComponent*		Find_Component(const _tchar* pComponentTag);
public:
	virtual CGameObject*	Clone(void* pArg) = 0;
	virtual void			Free() override;
};

END