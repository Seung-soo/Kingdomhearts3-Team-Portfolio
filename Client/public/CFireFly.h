#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;
class CMonster;
class CNavigation;
class CTexture;
END

BEGIN(Client)

class CFireFly final : public CGameObject
{
public:
	typedef struct tagFireFlyDesc{
	_int	iLightIndex = 0;
	// 1.f일때 위로
	_float	fDir = 1.f; 

	_float  fRevolutionDir = 1.f;

	}FIREFLYDESC;

private:
	CFireFly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFireFly(const CFireFly& rhs);
	virtual ~CFireFly() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	HRESULT Update_Action(_double TimeDelta);

public:
	static CFireFly*	 Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void		 Free() override;

private:
	CShader* m_pShaderCom         = nullptr;
	CModel* m_pModelCom           = nullptr;
	CRenderer* m_pRendererCom     = nullptr;
	CTexture* m_pTextureCom       = nullptr;

	CGameObject* m_pPlayer         = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

	FIREFLYDESC						m_tDesc;

	_double							m_TimeAccum = 0.0;

	_vector							m_vOffsetPos;

	// Light Range를 3 ~ 5 사이로
	_float							m_fRange = 1.f;
	_float							m_fMaxRange = 5.f;
	_float							m_fMinRange = 1.f;
	_float4							m_vColor = _float4(0.f, 0.1f, -0.5f, -0.1f);

	_float							m_fMovingU = 0.f;
	_float							m_fMovingV = 0.f;

	_bool							m_bChase = false;

	_vector							m_vTargetPos;

	_bool							m_isReverse = false;

	_float							m_fPreDir = 1.f;

	_bool							m_isLoosingSpeed = false;

};

END