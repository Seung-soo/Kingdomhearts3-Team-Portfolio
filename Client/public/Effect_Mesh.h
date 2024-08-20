#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "Renderer.h"
#include "Easing_Function.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)

class CEffect_Mesh final: public CGameObject
{
public:
	enum PLAY_TYPE { PLAY_ONCE, PLAY_LOOP, PLAY_TYPE_END };
	enum PATTERN_SCALE { PATTERN_SCALE_NONE, PATTERN_BULGE, PATTERN_SHRINK, PATTERN_DYNAMIC, PATTERN_SCALE_END};

	enum PATTERN_MOVE { 
		PATTERN_MOVE_NONE,			// 이동 없음
		PATTERN_MOVE_ONEDIR,		// 정해진 방향으로 고정 이동
		PATTERN_MOVE_RANDDIR,		// 무작위 방향으로 고정 이동
		PATTERN_TRACING,			// 상태 행렬을 갱신하면서 타겟 추적 이동
		PATTERN_TRACING_POSONLY,	// 위치만 갱신하면서 타겟 추적 이동
		PATTERN_MOVE_END};

	enum MOVE_DIR {RIGHT, UP, LOOK, DIR_END};

	enum EASINGFUNCTION {
		INSINE, OUTSINE, INOUTSINE, INQUAD, OUTQUAD, INOUTQUAD,
		INCUBIC, OUTCUBIC, INOUTCUBIC, INQUART, OUTQUART, INOUTQUART,
		INQUINT, OUTQUINT, INOUTQUINT, INEXPO, OUTEXPO, INOUTEXPO,
		INCIRC, OUTCIRC, INOUTCIRC, INBACK, OUTBACK, INOUTBACK,
		INELASTIC, OUTELASTIC, INOUTELASTIC, INBOUNCE, OUTBOUNCE, INOUTBOUNCE,
		EASING_END
	};

	enum PATTERN_DYING {DYING_NONE, DYING_ADD, DYING_MULT, DYING_COVER, DYING_ONEVALUE, DYING_END};
	enum PATTERN_MIXTEX { MIX_NONE, MIX_ADD, MIX_MULT, MIX_NOISING, MIX_END };

	enum LIFE_TYPE { TIME_LIMIT, ALPHA_LIMIT, SCALE_LIMIT, LOOP_LIMIT, DISTANCE_LIMIT, TARGET_LIMIT, LIFE_TYPE_END };

	typedef struct tagEffectMeshDesc {
		_int								iLevelIndex        = 5;
		_int								iTextureIndex      = 0;
		_int								iScondTextureIndex = 1;
		_int								iPassIndex         = 29;
		_tchar								ModelTag[60]       = TEXT(""); //까먹지 말고 채워두기!!!!

		PLAY_TYPE							ePlayType       = PLAY_ONCE;
		PATTERN_SCALE						ePattern_Scale  = PATTERN_SCALE_NONE;
		PATTERN_MOVE						ePattern_Move   = PATTERN_MOVE_NONE;
		CRenderer::RENDERGROUP				eRenderGroup    = CRenderer::RENDER_BLEND;
		PATTERN_MIXTEX						ePattern_MixTex = MIX_NONE;
		EASINGFUNCTION						eEasing         = INSINE;
		EASINGFUNCTION						eEasing_Scale   = INSINE;

		_float								fLifeTime      = 2.f;
		_vector								vStartScale    = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		_vector								vMaxScale      = XMVectorSet(1.5f, 1.5f, 1.5f, 0.f);
		_float								fScaleSpeed    = 2.f;
		_float								fSpinningSpeed = 1.f;

		_float3								fRotationAngle = _float3(0.f, 0.f, 0.f);

		_bool								bNoRandomBillborad = false;

		_vector								vDir            = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_float								fMovingSpeed    = 2.f;
		_float								fMovingDistance = 1.f;

		_float2								vUVMovingSpeed = _float2(0.5f, 0.5f);

		_bool								bUsingEmissive     = false;
		_bool								bUsingBlur         = false;
		_bool								bUsingDistortion   = false;
		_bool								bMovingUV          = false;
		_bool								bUseEasing         = false;
		_bool								bUseEasing_Scaling = false;
		_bool								bAlphaReverse      = false;

		_vector								vSpinAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector								vLookDir  = XMVectorSet(0.f, -1.f, 0.f, 0.f);
		_bool								bSpinning = false;

		_float4*							pParentPos = nullptr;
		_float4								vPos       = _float4(0.f, 0.f, 0.f, 1.f);

		_float								fTransparency     = 0.f;
		_float								fTransparecySpeed = 0.f;
		_float								fMaxTransparecy   = 0.f;

		_float4								vColor        = _float4(0.f, 0.f, 0.f, 1.f);
		_float4								vBlendColor   = _float4(0.f, 0.f, 0.f, 1.f);
		PATTERN_DYING						eDyingPattern = DYING_ADD;

		_int								iRGBNum       = 0;
		_bool								bBillboarding = false;

		_float								fNoisePower = 0.25f;
		_float2								vNoiseMovingSpeed = _float2(0.1f, 0.1f);

		LIFE_TYPE							eLifeType = TIME_LIMIT;

		_bool*								pParentDead = nullptr;

		_float4x4*							pTargetWorldMatrix = nullptr;
		MOVE_DIR							eFromTarget = DIR_END;
		_float								fFromTargetDistance = 1.f;

		_float								fStartU = 0.f;
		_float								fStartV = 0.f;

		_matrix								LocalMat = XMMatrixIdentity();
		_matrix								LocalMat_NoTrace = XMMatrixIdentity();

		CTransform*							pTarget_Transform = nullptr;
	}EFFECT_MESHDESC;

private:
	explicit CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Mesh(const CEffect_Mesh& rhs);
	virtual ~CEffect_Mesh() = default;

public:
	void	Set_MeshLifeType(LIFE_TYPE eLifeType) { m_tDesc.eLifeType = eLifeType; }
	void	Set_PlayType(PLAY_TYPE ePlayType) { m_tDesc.ePlayType = ePlayType; }

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);
	void			SynchToTarget();
	void			SynchToTarget_OnlyPos();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
	void    Set_EasingFunction();

	void    Use_EasingFunction();

	_int	Pattern_Scaling(_double TimeDelta);
	_int	Pattern_Moving(_double TimeDelta);
	_int	Pattern_Spinning(_double TimeDelta);

	_float	RandomRange(_float r1, _float r2);

public:
	static CEffect_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pSecondTextureCom = nullptr;
	CModel*						m_pModelCom = nullptr;

	EFFECT_MESHDESC				m_tDesc;

	_float						m_fTimeAccum = 0.f;
	_float						m_fMovingU = 0.f;
	_float						m_fMovingV = 0.f;
	_bool						m_bStartLoopUV = false;

	_float						m_fScalingSpeed = 0.f;
	_float						m_fMovingSpeed = 0.f;
	
	_vector						m_vCurrentScale;
	_vector						m_vFinalPos;

	_float						m_fMovingNoiseX = 0.f;
	_float						m_fMovingNoiseY = 0.f;

	_matrix						m_LocalMatrix = XMMatrixIdentity();
	_matrix						m_TransMat = XMMatrixIdentity();

	// 빌보드시 사용되는 랜덤 축
	_matrix						m_RandMatrix = XMMatrixIdentity();

private:
	_float(*m_pEasingFunction)(_float, _float, _float);
	_float(*m_pEasingFunction_Scale)(_float, _float, _float);

	_float						m_fTransparency = 0.f;
};

END