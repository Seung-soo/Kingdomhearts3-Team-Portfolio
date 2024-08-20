#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Renderer.h"
#include "Easing_Function.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CEffect_Texture final :public CGameObject
{
public:
	enum PLAY_TYPE {PLAY_LOOP, PLAY_ONCE, PLAY_END};
	enum PATTERN_SCALE { PATTERN_SCALE_NONE, PATTERN_BULGE, PATTERN_SHRINK, PATTERN_DYNAMIC, PATTERN_SCALE_END };
	enum PATTERN_MOVE { PATTERN_MOVE_NONE, PATTERN_MOVE_ONEDIR, PATTERN_MOVE_RANDDIR, PATTERN_MOVE_END };

	enum EASINGFUNCTION {
		INSINE, OUTSINE, INOUTSINE, INQUAD, OUTQUAD, INOUTQUAD,
		INCUBIC, OUTCUBIC, INOUTCUBIC, INQUART, OUTQUART, INOUTQUART,
		INQUINT, OUTQUINT, INOUTQUINT, INEXPO, OUTEXPO, INOUTEXPO,
		INCIRC, OUTCIRC, INOUTCIRC, INBACK, OUTBACK, INOUTBACK,
		INELASTIC, OUTELASTIC, INOUTELASTIC, INBOUNCE, OUTBOUNCE, INOUTBOUNCE,
		EASING_END
	};

	enum LIFE_TYPE { TIME_LIMIT, ALPHA_LIMIT, SCALE_LIMIT, DISTANCE_LIMIT, SPRITE_LIMIT, LIFE_TYPE_END };

public:
	typedef struct tagDeathPartDesc {
		_float3						vPosition = _float3(0.f, 0.f, 0.f);
		_float3						vMovingDir = _float3(0.f, 0.f, 0.f);
		_float						fTotalTime = 3.f;
		_float3						vMinSize = _float3(0.f, 0.f, 0.f);
		_float3						vMaxSize = _float3(0.f, 0.f, 0.f);
		_float						fPlaySpeed = 0.f;
		_uint						iPassIndex = 0;

		_float4						vBaseColor = _float4(0.f, 0.f, 0.f, 0.f);
		_float4						vBlendingColor = _float4(1.f, 1.f, 1.f, 1.f);

		_float3						vLookDir = _float3(0.f, 0.f, 1.f);

		_bool						bNoising = false;

		_uint						iLevelIndex = 0;
		_tchar						TextureTag[60];
		_uint						iTextureIndex = 0;
		_uint						iSecondTextureIndex = 0;
		_float						fSpinSpeed = 0.f;
		_float						fScalingSpeed = 0.f;
		CRenderer::RENDERGROUP		eRenderGroup = CRenderer::RENDER_BLEND;
		EASINGFUNCTION				eEasing = INSINE;
		EASINGFUNCTION				eEasing_Scale = INSINE;

		_float						fStartAlpha = 0.f;
		_float						fTransparecySpeed = 0.f;
		_float						fMaxTransparecy = 0.f;
		_bool						IsTurning = false;
		
		_bool						bBlur = false;
		_bool						bDistortion = false;
		_bool						bBillboarding = false;
		_bool						bSprite = false;
		_bool						bUseEasing = false;
		_bool						bUseEasing_Scaling = false;

		_bool						bMultiSprite = false;

		_bool						bSpinning = false;

		_vector						vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

		_int						fSpriteMaxXIndex = 1;
		_int						fSpriteMaxYIndex = 1;

		PLAY_TYPE					ePlayType = PLAY_LOOP;

		PATTERN_SCALE				eScalePattern = PATTERN_SCALE_NONE;

		PATTERN_MOVE				eMovingPattern = PATTERN_MOVE_NONE;

		_float						fSpriteSpeed = 1.f;

		LIFE_TYPE					eLifeType = TIME_LIMIT;

		_bool*						pParentDead = nullptr;

		_float						fNoiseRot = 0.f;

		_matrix						RoatationMatrix = XMMatrixIdentity();

		_bool						bDefaultPass = false;
	}EFFECT_TEX_DESC;

private:
	explicit CEffect_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Texture(const CEffect_Texture& rhs);
	virtual ~CEffect_Texture() = default;

public:
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT	SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:
	_int	Pattern_Scaling(_double TimeDelta);
	_int	Pattern_Moving(_double TimeDelta);
	_int	Pattern_Spinning(_double TimeDelta);

	void    Set_EasingFunction();
	void    Use_EasingFunction();


public:
	static CEffect_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	static CEffect_Texture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
	virtual CGameObject* Clone(void* pArg);

private:
	CShader*			m_pShaderCom        = nullptr;
	CRenderer*			m_pRendererCom      = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom      = nullptr;
	CTexture*			m_pTextureCom       = nullptr;
	CTexture*			m_pSecondTextureCom = nullptr;

	_float				m_fTimeAccum = 0.f;
	_float				m_fTranparency = 0.f;

	_float3				m_vArrivalPoint;

	EFFECT_TEX_DESC		m_tDesc;

	_bool				m_isReverese = false;

	_float3				m_vAxis;

	_float				m_fCurFrameIndexX = 0;
	_float				m_fCurFrameIndexY = 0;

	_vector				m_vCurrentScale;
	_float				m_fScalingSpeed = 1.f;

	_vector				m_vFinalPos;

	_float				m_fMovingSpeed = 1.f;

	_float				m_fNoisingTimeAccum = 0.f;

private:
	_float(*m_pEasingFunction)(_float, _float, _float);
	_float(*m_pEasingFunction_Scale)(_float, _float, _float);
};

END