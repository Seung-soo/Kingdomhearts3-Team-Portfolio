#pragma once

#include "Component.h"

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND, RENDER_UI, RENDER_STOPUI, RENDER_END };
	enum SCALE_TYPE { DOWNSCALE, UPSACLE, SCALE_END};
	enum SHADOW_TYPE {SHADOW_STATIC, SHADOW_DYNAMIC, SHADOW_END};

private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;

public:
	ID3D11ShaderResourceView*	Get_SRV(_tchar* pRenderTargetTag);
	CTexture*					Get_NoiseTexture() { return m_pNoiseTexture; }

	void						Set_StartLum(_int iStartLum) { m_iStartLum = iStartLum; }
	void						Set_FocusPosition(_float4 vFocusPosition) { m_vFocusPosition = vFocusPosition; }

	_float						Get_SSAORadius() { return m_fSSAO_Radius; }
	void						Set_SSAORadius(_float fSSAO_Radius) { m_fSSAO_Radius = fSSAO_Radius; }

	_float						Get_SSAOBias() { return m_fSSAO_Bias; }
	void						Set_SSAOBias(_float fSSAO_Bias) { m_fSSAO_Bias = fSSAO_Bias; }

	_bool						Get_SSAO() { return m_bSSAO; }
	void						Set_SSAO(_bool bSSAO) { m_bSSAO = bSSAO; }

	_float						Get_RadialBlurDist() { return m_fRadialBlurDist; }
	void						Set_RadialBlurDist(_float fRadialBlurDist) { m_fRadialBlurDist = fRadialBlurDist; }

	_float						Get_HDR_GrayScale() { return m_fHDR_GrayScale; }
	void						Set_HDR_GrayScale(_float fGrayScale) { m_fHDR_GrayScale= fGrayScale; }

	_float						Get_HDR_White() { return m_fHDE_White; }
	void						Set_HDR_White(_float fWhite) { m_fHDE_White= fWhite; }

	_bool						Get_HDR() { return m_bHDR; }
	void						Set_HDR(_bool bHDR) { m_bHDR = bHDR; }

	_bool						Get_Bloom() { return m_bBloom; }
	void						Set_Bloom(_bool bBloom) { m_bBloom = bBloom; }

	_float						Get_FogStart() { return m_fFogStart; }
	void						Set_FogStart(_float fFogStart) { m_fFogStart = fFogStart; }

	_float						Get_FogEnd() { return m_fFogEnd; }
	void						Set_FogEnd(_float fFogEnd) { m_fFogEnd = fFogEnd; }

	_float4						Get_FogColor() { return m_vFogColor; }
	void						Set_FogColor(_float4 vFogColor) { m_vFogColor = vFogColor; }

	_float						Get_MaxFogFactor() { return m_fMaxFogFactor; }
	void						Set_MaxFogFactor(_float fMaxFogFactor) { m_fMaxFogFactor = fMaxFogFactor; }

	_bool						Get_Fog() { return m_bFog; }
	void						Set_Fog(_bool bFog) { m_bFog = bFog; }

	_float						Get_DOF_MinDistance() { return m_fDOF_MinDistance; }
	void						Set_DOF_MinDistance(_float fMinDistance) { m_fDOF_MinDistance = fMinDistance; }

	_float						Get_DOF_MaxDistance() { return m_fDOF_MaxDistance; }
	void						Set_DOF_MaxDistance(_float fMaxDistance) { m_fDOF_MaxDistance = fMaxDistance; }

	_bool						Get_DOF() { return m_bDOF; }
	void						Set_DOF(_bool bDOF) { m_bDOF = bDOF; }

	_bool						Get_Outline() { return m_bOutline; }
	void						Set_Outline(_bool bOutline) { m_bOutline = bOutline; }

	_bool						Get_RenderDiffuse() { return m_bRenderDiffuse; }
	void						Set_RenderDiffuse(_bool bRenderDiffuse) { m_bRenderDiffuse = bRenderDiffuse; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT	Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject);
	HRESULT Clear_Renderer();

	// For Shadow
	HRESULT Reset_Values(_bool isRenderShadow = true);


	// For LUT
	_uint	Get_LUTTexIndex() { return m_iLUTTexIndex; }
	void	Set_LUTTexIndex(_uint iIndex) { m_iLUTTexIndex = iIndex; }
	_uint	Get_LUTTexIndexSecond() { return m_iLUTTexIndexSecond; }
	void	Set_LUTTexIndexSecond(_uint iIndex) { m_iLUTTexIndexSecond = iIndex; }
	void	Set_UsingLUT(_bool isUsing) { m_isUsingLUT = isUsing; }
	_bool	Get_UsingLUT() { return m_isUsingLUT; }

	void	Set_LUTDesc(_bool isUsing, _uint TexIndex);

	// For RadialBlur
	void	Set_RadialBlur(_bool bUse) { m_bRadialBlur = bUse; }

//#ifdef _DEBUG
	HRESULT	Add_Debug(class CComponent* pComponent);

	_bool	m_bDebugRenderController = true; // false 이면 Add_Debug 안함
	_bool*	Get_DebugRenderController() { return &m_bDebugRenderController; }
//#endif
	virtual HRESULT	Render();

private:
	// For Shadow
	HRESULT Render_Shadow();
	HRESULT Shadow_Blur();
	HRESULT Shadow_DownSample();
	HRESULT Shadow_UpSample();

	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	HRESULT Render_Effect();
	// 명암 연산 랜더
	HRESULT Render_Lights();
	// 타겟들을 합친 결과를 랜더
	HRESULT Render_Calculation();
	HRESULT Render_NonLight();
	HRESULT Render_Alpha();
	HRESULT Render_Combine();
	HRESULT Render_UI();

private:
	HRESULT HDR_FirstDownScale();
	HRESULT HDR_SecondDownScale();
	HRESULT HDR_ThirdDownScale();
	HRESULT HDR_FourthDownScale();
	HRESULT HDR_FifthDownScale();
	HRESULT HDR_SixthDownScale();

	HRESULT CalcAverLum();

	HRESULT Render_HDRDownScale();

	HRESULT Render_ToneMapping();
	HRESULT Render_PrevAverLum();

	HRESULT Bloom_Mask();
	HRESULT Bloom_Down();
	HRESULT Bloom_BlurX();
	HRESULT Bloom_BlurY();
	HRESULT Bloom_Up();
	HRESULT Bloom_Mapping();

	HRESULT Render_Bloom();

	// DOF
	HRESULT DOF_BlurX();
	HRESULT DOF_BlurY();
	HRESULT Render_DOF();

	// SSAO
	HRESULT Render_SSAO();
	HRESULT SSAO_BlurX();
	HRESULT SSAO_BlurY();

	// MotionBlur
	HRESULT Render_MotionBlur();

	// RadialBlur
	HRESULT Render_RadialBlur();

	// Fog
	HRESULT Render_Fog();
private:
	HRESULT	DownSampling();
	HRESULT UpSampling();

private:
	HRESULT Blur();
	HRESULT Blur_X();
	HRESULT Blur_Y();

private:
	HRESULT Distortion();

private:
	HRESULT LUT_Filtering();


private:
	HRESULT Render_Final();


private:
	//		랜더 타겟 추가하는 곳
	HRESULT Add_RenderTargets();

	//		멀티 랜더 타겟 추가하는 곳
	HRESULT Add_MultiRenderTargets();

	//		랜더러에서 당장 사용할 컴포넌트들 세팅
	HRESULT SetUp_Components();

	//		HDR 다운 샘플링을 할 때 쓰일 DepthStencil을 만들어 준다.
	HRESULT Make_HDRDepthStencil(_uint iWidth, _uint iHeight, _uint iIndex);
	HRESULT Make_CubeDepthStencil();
//#ifdef _DEBUG
	//		랜더 타겟을 랜더할 때의 정보 세팅
	HRESULT Ready_DebugDesc();

	//		콜라이더 같은 디버그 컴포넌트들 랜더
	HRESULT Render_Debug();
//#endif

public:
	static CRenderer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;

private:
	list<CGameObject*>			m_RenderObjects[RENDER_END];
	typedef list <CGameObject>	RENDEROBJECTS;

	// NONBLEND_STATIC
	list<CGameObject*>			m_ShadowObject[SHADOW_END];

private:
	list<class CComponent*>			m_DebugObject;
	typedef list<class CComponent*>	DEBUGOBJECT;

private:
	class CTarget_Manager*		m_pTarget_Manager = nullptr;
	class CLight_Manager*		m_pLight_Manager = nullptr;

private:
	class CVIBuffer_Rect*		m_pVIBuffer = nullptr;
	class CShader*				m_pShader = nullptr;
	class CShader*				m_pSSAOShader = nullptr;
	class CTexture*				m_pNoiseTexture = nullptr;
	class CTexture*				m_pLUTTexture = nullptr;

	//다운스케일 업스케일 버퍼
private:
	class CVIBuffer_OrthoWin*	m_pScaleWIn[SCALE_END];

	_float						m_fDistortionUV = 0.f;
	_uint						m_iStartLum = 0;
private:
	_float4x4					m_WorldMatrix;
	_float4x4					m_ViewMatrix;
	_float4x4					m_ProjMatrix;

	ID3D11DepthStencilView*		m_pDownDSV = nullptr;
	ID3D11DepthStencilView*		m_pHDRDownDSV[7] = { nullptr };
	ID3D11DepthStencilView*		m_pCubeDSV = nullptr;

	_float4						m_vFocusPosition = _float4(0.f, 0.f, 0.f, 1.f);

	_float						m_fWinCX = 1280.f;
	_float						m_fWinCY = 720.f;

	_bool						m_bRenderDiffuse = false;

	// For SSAO
	_float4						m_vRandomDir[64] = {};

	_float						m_fSSAO_Radius = 2.f;
	_float						m_fSSAO_Bias = 0.025f;
	_bool						m_bSSAO = true;

	// For HDR
	_float						m_fHDR_GrayScale = 0.5f;
	_float						m_fHDE_White = 1.f;
	_bool						m_bHDR = false;
	_bool						m_bBloom = false;

	// For Shadow
	_bool						m_isStaticShadowRender = false;
	_bool						m_isRenderShadow       = true;
	
	// For LUT
	_uint						m_iLUTTexIndex = 0;
	_uint						m_iLUTTexIndexSecond = 0;
	_bool						m_isUsingLUT = false;

	// For RadialBlur
	_float						m_fRadialBlurDist = 1.f;
	_bool						m_bRadialBlur = false;

	// For Fog
	_float						m_fFogStart = 30.f;
	_float						m_fFogEnd = 100.f;
	_float4						m_vFogColor = _float4(0.5f, 0.5f, 0.5f, 1.f);
	_float						m_fMaxFogFactor = 0.f;
	_bool						m_bFog = false;

	// For DOF
	_bool						m_bDOF = false;
	_float						m_fDOF_MinDistance = 0.f;
	_float						m_fDOF_MaxDistance = 0.f;

	// For Outline
	_bool						m_bOutline = false;
};

END