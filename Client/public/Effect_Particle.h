#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Easing_Function.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CEffect_Particle final : public CGameObject
{
public:
	enum DIR { DIR_ONE, DIR_RANDOM, DIR_END };

	enum PATICLENUM { NUM_10 = 10, NUM_30 = 30, NUM_50 = 50, NUM_100 = 100, NUM_END };
	
	enum EASINGFUNCTION {
		INSINE, OUTSINE, INOUTSINE, INQUAD, OUTQUAD, INOUTQUAD,
		INCUBIC, OUTCUBIC, INOUTCUBIC, INQUART, OUTQUART, INOUTQUART,
		INQUINT, OUTQUINT, INOUTQUINT, INEXPO, OUTEXPO, INOUTEXPO,
		INCIRC, OUTCIRC, INOUTCIRC, INBACK, OUTBACK, INOUTBACK,
		INELASTIC, OUTELASTIC, INOUTELASTIC, INBOUNCE, OUTBOUNCE, INOUTBOUNCE,
		EASING_END
	};

	enum LIFE_TYPE { TIME_LIMIT, ALPHA_LIMIT, SCALE_LIMIT, DISTANCE_LIMIT, LIFE_TYPE_END };

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_END };

public:
	typedef struct tagParticleDesc
	{
		_int					iTexIndex = 0;
		_float					fMinSpeed = 0.f;
		_float					fMaxSpeed = 0.f;
		_float					fOriginScale = 1.f;
		_float					fScaleSpeed = 0.f;
		_float					fGravityPower = 0.f;
		_float					fLiveTime = 0.f;

		_bool					bRangeOneOrXYZ = false;
		_float					fMakeRange = 0.f;
		_float3					vMakeRangeXYZ = _float3(0.f, 0.f, 0.f);
		_float					fMaxDistance = 0.f;
		DIR						eDir = DIR_ONE;
		_vector					vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		PATICLENUM				eParticleNumber = NUM_10;
		EASINGFUNCTION			eEasing = INSINE;
		_bool					bLoop = false;
		_bool					bUseEasing = false;
		_bool					bChase = false;
		_bool					bTrail = false;
		_bool					bExplosion = false;
		_float					fTrailInterval = 0.f;
		_float4*				pParentPosition = nullptr;
		_bool*					pDead = nullptr;

		_float					fExplosionStartRange = 0.f;

		_bool					bSeperateAxisRange = false;
		_float3					vAxisRangeXYZ = _float3(0.f, 0.f, 0.f);
		STATE					eState = STATE_LOOK;
		_bool					bUsingAxis = false;
		_matrix					vParentWorldMatrix;

		_float					fAddColor[4] = {};
		_float					fMulColor[4] = {1.f, 1.f, 1.f, 1.f};
								
		_uint					iLevelIndex = 1;

		LIFE_TYPE				eLifeType = TIME_LIMIT;

		_bool*					pParentDead = nullptr;

		_bool					bBlur = true;
		//Pass
		_uint					iPass = 1;

		_bool					bNotTracing = false;
	}PARTICLEDESC;

private:
	CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Particle(const CEffect_Particle& rhs);

public:
	_bool Get_ParticleEnd() { return m_bParticleEnd; }
	void Set_ParticleEnd(_bool bEnd) { m_bParticleEnd = bEnd; }

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	void	UseEasingFuntion();

public:
	static CEffect_Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	class CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Point_Instance*	m_pVIBufferCom = nullptr;

	vector<_float4x4>			m_TransformMatrix;
	_vector						m_vStartPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_float						m_fCurrentTime = 0.f;

	PARTICLEDESC				m_tParticleDesc;
	
	//초기 설정 스피드(Original)
	vector<_float>				m_Speeds;
	//실제 사용되는 스피드(Variable)
	vector<_float>				m_CurrentSpeeds;

	vector<_vector>				m_Directions;
	vector<_float3>				m_FirstPositions;

	vector<_float>				m_CurrentScale;
	vector<_matrix>				m_LocalMatrix;
	
	_int						m_iParticleNum = 0;
	_int						m_iTrailNum = 0;
	_float						m_fTrailTime = 0.f;
	_bool						m_bParticleEnd = false;
	_int						m_iEndCycle = -1;
	_bool						m_bEndStart = false;

	_float4						m_vAddColor;
	_float4						m_vMulColor;
private:
	_float(*m_pEasingFunction)(_float, _float, _float);
};

END