#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CStatus;
class CNavigation;
END

BEGIN(Client)

class CMini_CrowYellow final : public CGameObject
{
private:
	CMini_CrowYellow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMini_CrowYellow(const CMini_CrowYellow& rhs);
	virtual ~CMini_CrowYellow() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	class CPlayer* m_pPlayer = nullptr;
	_float4	m_vTargetPosition;
	_bool	bSet = false;

	_float Create_X = 0.f;
	_float Create_Y = 0.f;
	_float Create_Z = 0.f;

	_double	dCrowSpeedA = 0;
	_double	dCrowSpeedB = 0;
	_double dCrowSpeed = 0;

	_int	ChaseRandom = 0;
	_vector	vChase_Pos;

	_vector vPlayer_Pos;
	_vector vCrow_Pos;

	_bool	bTarget_On = false;

private:
	CShader*		m_pShaderCom = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	//	CCollider*		m_pMiniGameColliderCom = nullptr;

public:
	static	CMini_CrowYellow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END