#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
END

// 이펙트 다 만들어지면 모델 지우기 ============================================================

BEGIN(Client)

class CPillarOfFire final : public CMonster
{
public:
	typedef struct tagPillarDesc
	{
		_vector vPosition;
		_bool	bGravitation = false;
		_bool	bShortPillar = false;
	}PILLARDESC;

private:
	CPillarOfFire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPillarOfFire(const CPillarOfFire& rhs);
	virtual ~CPillarOfFire() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

public:
	void SetSoundChannel(_uint iChannelIndex) { m_iCurPlaySound = iChannelIndex; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	_bool Action(_double TimeDelta);

private:
	CShader*	m_pShaderCom   = nullptr;
	CRenderer*	m_pRendererCom = nullptr;

	_double		m_TimeAcc            = 0.0;
	_double		m_EffectInterval     = 0.0;
	_double		m_WaveEffectInterval = 0.0;

	PILLARDESC	m_PillarDesc;

	_vector		m_vRevolutionCenterPos;

	_bool		m_bEffectEnd = false;

	_uint		m_iCurPlaySound = 0;

public:
	static	CPillarOfFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END