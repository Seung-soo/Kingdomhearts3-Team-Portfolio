#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Effect_Particle.h"
#include "Effect_Mesh.h"
#include "Effect_Texture.h"
#include "Renderer.h"

BEGIN(Client)

class CLevel_EffectTool final : public CLevel
{
public:
	enum BUTTON_TYPE {DELETE_BUTTON, LOAD_BUTTON, BUTTON_END};

public:
	explicit CLevel_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_EffectTool() = default;

public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Axis(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);

private:
	HRESULT Ready_Lights();

private:
	void	Select_EffectMesh();
public:
	static CLevel_EffectTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;

private:
	_float										m_vPosition[3] = { 0.f, 0.f, 0.f };
	_bool										m_isTracing = false;

	// Scale
	_float										m_vScale[3] = { 1.f, 1.f, 1.f };
	_float										m_fScalingTime = 1.f;
	_float										m_fScalingSpeed = 1.f;
	_float										m_vChangedScale[3] = { 1.f, 1.f, 1.f };

	// Transparency
	_float										m_fStartAlpha = 0.f;
	_float										m_fAlphaSpeed = 0.f;
	_float										m_fTransparentSpeed = 1.f;

	//Color
	_float										m_vBaseColor[3] = { 255.f, 255.f, 255.f };
	_float										m_vBlendingColor[3] = { 255.f, 255.f, 255.f };

	//Direction
	_float										m_vMovingDir[3] = { 0.f, 0.f, 0.f };

	//Transform
	_float										m_fPlaySpeed = 4.f;
	_float										m_fRotationAngle = 90.f;

	//PlayeTime
	_float										m_fPlayTime = 1.f;

	//PassIndex
	_int										m_iPassIndex = 0;
	_int										m_iTextureIndex = 0;
	_tchar										m_tTextureTag[60] = TEXT("");

	_bool										m_isTurning = false;

	//Particle
	CEffect_Particle::PARTICLEDESC				m_tParticleDesc;
	_float4										m_vParticlePosition = _float4(0.f, 0.f, 0.f, 1.f);

	CEffect_Mesh::EFFECT_MESHDESC				m_tEffectMeshDesc;
	_float4										m_vEffectMeshPosition = _float4(0.f, 0.f, 0.f, 1.f);


	_float										m_fTransparency = 0.f;

	_bool										m_bBillboarding = false;

	_bool										bBlur = false;
	_bool										bDistortion = false;
	_bool										bSprite = false;

	_bool										bMultiSprite = false;

	_int										iSpriteMaxXIndex;
	_int										iSpriteMaxYIndex;

	CEffect_Texture::PLAY_TYPE					m_eType;

	_float										fSpriteSpeed = 1.f;

	CRenderer::RENDERGROUP						eRenderGroup = CRenderer::RENDER_NONLIGHT;

	_uint										m_iLevelIndex = LEVEL_EFFECTTOOL;

	CEffect_Texture::EFFECT_TEX_DESC			m_tTexDesc;

	vector<CEffect_Mesh::EFFECT_MESHDESC>		m_MeshDesc;
	vector<CEffect_Texture::EFFECT_TEX_DESC>	m_TexDesc;
	vector<CEffect_Particle::PARTICLEDESC>		m_PtclDesc;

	_bool										m_bControlBox = false;

	BUTTON_TYPE									m_eButtonType = DELETE_BUTTON;

	_uint										m_iTextureSize = 7;
	_uint										m_iNoiseTextureSize = 99;

	_uint										m_iEffectMeshIndex = 0;
};

END