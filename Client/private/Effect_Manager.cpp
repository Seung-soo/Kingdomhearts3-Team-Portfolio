#include "stdafx.h"
#include "..\public\Effect_Manager.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Nightmare_Blade.h"
#include "Player.h"
#include "SummonMovingParticle.h"

READY_SINGLETON(CEffect_Manager)

const _tchar* CEffect_Manager::m_pLayerTag = TEXT("Layer_Effect");
const _tchar* CEffect_Manager::m_pMeshProtoTag = TEXT("Prototype_GameObject_EffectMesh");
const _tchar* CEffect_Manager::m_pParticleProtoTag = TEXT("Prototype_GameObject_Effect_Particle");
const _tchar* CEffect_Manager::m_pTextureProtoTag = TEXT("Prototype_GameObject_EffectTexture");

void CEffect_Manager::Initialize()
{
	Ready_MeshDesc();

	Ready_ParticleDesc();

	Ready_TextureDesc();
}

void CEffect_Manager::Free()
{
	m_MeshDesc.clear();
	m_PatricleDesc.clear();
	m_TextureDesc.clear();
}

HRESULT CEffect_Manager::Create_Effect(EFFECT_NAME eName, void* pArg)
{
	//switch (eName)
	//{
	//case HIT_STAR:
	//	_float4		vPosition = *(_float4*)pArg;
	//	Create_HitStar(vPosition);
	//	break;
	//}
	return S_OK;
}

void CEffect_Manager::Ready_MeshDesc()
{
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;
	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 한 방향으로 자전하면서 움직이는 이펙트 ============================
	tDesc.iLevelIndex = 5;
	tDesc.iTextureIndex = 10;
	tDesc.iPassIndex = 32;
	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_DYNAMIC;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_RANDDIR;
	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.fLifeTime = 0.5f;

	tDesc.vStartScale = XMVectorSet(0.001f, 0.001f, 0.001f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.005f, 0.005f, 0.005f, 0.f);
	tDesc.fScaleSpeed = 2.f;
	
	tDesc.vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tDesc.fMovingSpeed = 15.f;
	tDesc.fMovingDistance = 2.0f;
	
	tDesc.vUVMovingSpeed = _float2(0.8f, 0.2f);
	
	tDesc.bUsingEmissive = false;
	tDesc.bUsingBlur = true;
	tDesc.bUsingDistortion = false;
	tDesc.bMovingUV = false;
	tDesc.bUseEasing = false;
	tDesc.pParentPos = nullptr;
	tDesc.bSpinning = true;
	tDesc.fSpinningSpeed = 3.f;

	/*tDesc.fTransparency = 1.f;
	tDesc.fTransparecySpeed = 0.25f;*/

	m_MeshDesc.push_back(tDesc);
	// ===================================================================

	// 링 트레일 =========================================================
	tDesc.iLevelIndex = 5;
	tDesc.iTextureIndex = 0;
	tDesc.iPassIndex = 32;
	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_RANDDIR;
	tDesc.eRenderGroup = CRenderer::RENDER_NONLIGHT;

	tDesc.fLifeTime = 0.5f;

	tDesc.vStartScale = XMVectorSet(0.6f, 0.6f, 0.6f, 0.f);
	tDesc.vMaxScale = XMVectorSet(1.5f, 1.5f, 1.5f, 0.f);
	tDesc.fScaleSpeed = 2.f;

	tDesc.vDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tDesc.fMovingSpeed = 15.f;
	tDesc.fMovingDistance = 2.0f;

	tDesc.vUVMovingSpeed = _float2(4.5f, 2.25f);

	tDesc.bUsingEmissive = false;
	tDesc.bUsingBlur = true;
	tDesc.bUsingDistortion = false;
	tDesc.bMovingUV = true;
	tDesc.bUseEasing = false;
	tDesc.pParentPos = nullptr;
	tDesc.bSpinning = true;
	tDesc.fSpinningSpeed = 3.f;

	tDesc.fTransparency = 0.25f;
	tDesc.fTransparecySpeed = -2.5f;

	m_MeshDesc.push_back(tDesc);
	// ===================================================================

	// 몬스터 스폰 이펙트 =================================================
	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado05"));
	tDesc.iTextureIndex = 19;
	tDesc.fLifeTime = 1.f;
	tDesc.vLookDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	tDesc.fTransparecySpeed = -0.01f;

	tDesc.fScaleSpeed = 3.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = true;
	tDesc.bUsingBlur = true;
	tDesc.vUVMovingSpeed = _float2(0.f, 1.25f);
	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	m_MeshDesc.push_back(tDesc);

	//====================================================================

	// 플레이어 트레일 매쉬 이펙트 =================================================
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
	tDesc.iTextureIndex = 268;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.fLifeTime = 2.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vDir = __vector(0.f, 0.f, -1.f, 0.f);

	//tDesc.LocalMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 70.f, 0.f);

	tDesc.vUVMovingSpeed = _float2(4.f, 2.f);

	tDesc.fStartU = -0.7f;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.9f, 0.3f, -0.3f, -0.5f);

	m_MeshDesc.push_back(tDesc);

	//====================================================================

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tEffectMeshDesc.iTextureIndex = 333;
	tEffectMeshDesc.iScondTextureIndex = 5;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 3.f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 2.f;
	tEffectMeshDesc.fTransparecySpeed = -4.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(1.f, -0.5f);
	tEffectMeshDesc.fStartU = -0.5f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.1f, -1.f, -1.f, 2.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	m_MeshDesc.push_back(tEffectMeshDesc);

	// RageForm_BaseAttack ====================================================

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring05"));
	tEffectMeshDesc.iTextureIndex = 365;
	tEffectMeshDesc.iScondTextureIndex = 88;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -1.75f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bAlphaReverse = false;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = -5.f;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.vSpinAxis = tEffectMeshDesc.vLookDir;
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING_POSONLY;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.08f, 0.08f, 0.08f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 6.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(2.5f, 0.f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = -0.3f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.1f, -1.f, -1.f, 1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	m_MeshDesc.push_back(tEffectMeshDesc);

	// RageForm_Slash ====================================================
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
	tEffectMeshDesc.iTextureIndex = 181;
	tEffectMeshDesc.iScondTextureIndex = 5;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -3.25f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bAlphaReverse = false;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = -5.f;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.vSpinAxis = tEffectMeshDesc.vLookDir;
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.04f, 0.04f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 7.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = false;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 0.f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = -0.3f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.2f, -1.f, -1.f, 0.9f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Trail
	Ready_HammerFormMeshDesc_Trail();

	// Hammer Effect
	Ready_HammerFormMeshDesc_Hammer();

	// Hammer Attack Effect
	Ready_HammerFormMeshDesc_Attack();

	// NightMare Nail Ring
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring00"));
	tEffectMeshDesc.iTextureIndex = 153;
	tEffectMeshDesc.iScondTextureIndex = 5;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bAlphaReverse = false;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.vSpinAxis = tEffectMeshDesc.vLookDir;
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 5.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 2.5f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.f, 0.f, -0.2f, 0.2f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	m_MeshDesc.push_back(tEffectMeshDesc);
}

void CEffect_Manager::Ready_ParticleDesc()
{
	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;

	_float	fAddColor[4] = { 0.f, 0.f, 0.f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 2.5f, 2.f, 1.f, 0.24f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fMinSpeed = 0.11f;
	tParticleDesc.fMaxSpeed = 3.3f;
	tParticleDesc.fScaleSpeed = 3.f; 
	tParticleDesc.fOriginScale = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 349;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	m_PatricleDesc.push_back(tParticleDesc);

	// RageForm 파티클 ===================================================================
	
	tParticleDesc.iTexIndex = 353;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 5.f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fScaleSpeed = 0.12f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fMakeRange = 10.f;
	tParticleDesc.fMaxDistance = 10.f;

	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);

	tParticleDesc.fTrailInterval = 0.01f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;

	_float	fAddColor2[4] = { 0.2f, -0.5f, -0.5f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor2, sizeof(_float) * 4);

	_float	fMulColor2[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor2, sizeof(_float) * 4);

	m_PatricleDesc.push_back(tParticleDesc);

	// Hammer Attack
	Ready_HammerFormParticleDesc();

	// Attack =============================================================
	tParticleDesc.iTexIndex = 381;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 10.f;
	tParticleDesc.fOriginScale = 0.12f;
	tParticleDesc.fScaleSpeed = 0.25f;
	tParticleDesc.fGravityPower = 4.f;
	tParticleDesc.fMakeRange = 1.2f;
	tParticleDesc.fMaxDistance = 0.f;
	tParticleDesc.fLiveTime = 0.5f;

	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);

	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::INOUTSINE;
	tParticleDesc.bExplosion = true;
	tParticleDesc.fExplosionStartRange = 0.f;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.iPass = 3;
	tParticleDesc.bSeperateAxisRange = false;

	_float	fMulColor3[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor3, sizeof(_float) * 4);

	m_PatricleDesc.push_back(tParticleDesc);

	// Heal 파티클 ===================================================================

	tParticleDesc.iTexIndex = 369;
	tParticleDesc.fMinSpeed = 2.f;
	tParticleDesc.fMaxSpeed = 10.f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fScaleSpeed = 0.15f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 1.f;
	tParticleDesc.fMakeRange = 2.f;
	tParticleDesc.fMaxDistance = 4.f;

	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);

	tParticleDesc.fTrailInterval = 0.01f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::OUTQUAD;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor3[4] = { 1.0f, 0.48f, 1.0f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor3, sizeof(_float) * 4);

	_float	fMulColor4[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor4, sizeof(_float) * 4);

	m_PatricleDesc.push_back(tParticleDesc);

}

void CEffect_Manager::Ready_TextureDesc()
{
	// 고트 스모크 정보
	Ready_GoatSmokeDesc();
	// 빅트리 스모크 정보
	Ready_BigTreeSmokeDesc();

	// 레이지 폼 어택 텍스쳐 이펙트
	Ready_RageFormTextureDesc_Attack();
}

void CEffect_Manager::Ready_HammerFormMeshDesc_Trail()
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Hammer Attack Mesh Trail 1 ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_HalfRing02"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 160;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.3f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.045f, 0.045f, 0.045f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.5f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(-10.f, -30.f, 90.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -0.45f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 3.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.45f, 0.45f, 0.45f, 0.8f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.5f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Trail 2 ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_HalfRing02"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 160;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.5f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.045f, 0.045f, 0.045f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.5f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(30.f, -60.f, 90.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -0.45f;
	tEffectMeshDesc.fStartV = 1.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, -5.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.45f, 0.45f, 0.45f, 0.8f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.5f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Trail 3 ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 160;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 1.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.06f, 0.06f, 0.06f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.12f, 0.12f, 0.12f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 180.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = 0.4f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(5.f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.55f, 0.55f, 0.55f, 0.8f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.75f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Trail 4 ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 160;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 1.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.05f, 0.05f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, -90.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = -0.7f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(5.f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.55f, 0.55f, 0.55f, 0.8f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.75f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Trail 5 ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_HalfRing02"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 160;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.4f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.025f, 0.025f, 0.025f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.5f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 60.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -0.45f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 2.5f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.55f, 0.55f, 0.55f, 1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.5f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Trail LastAttack Ground ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_HalfRing02"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 160;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.3f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.045f, 0.09f, 0.045f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.06f, 0.12f, 0.06f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.5f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 90.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -0.45f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 4.5f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.45f, 0.45f, 0.45f, 0.6f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Trail LastAttack Air ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_HalfRing02"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 160;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.3f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.045f, 0.09f, 0.045f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.06f, 0.12f, 0.06f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.5f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, -45.f, 50.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -0.45f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 4.5f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.45f, 0.45f, 0.45f, 0.6f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);
}

void CEffect_Manager::Ready_HammerFormMeshDesc_Hammer()
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Hammer Attack Mesh Effect LastAttack UpStream ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado00"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 262;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 3.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.008f, 0.006f, 0.008f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tEffectMeshDesc.vMaxScale = XMVectorZero();
	tEffectMeshDesc.fScaleSpeed = 0.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END;

	// 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END;

	// 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐
	tEffectMeshDesc.iPassIndex = 0;

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = 0.75f;
	tEffectMeshDesc.fStartV = 0.5f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-2.f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.35f, 0.35f, 0.35f, 0.2f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	// DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다
	tEffectMeshDesc.iRGBNum = 0;

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.35f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::PATTERN_MIXTEX::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Effect LastAttack Spike ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_spark_flash"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 324;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.75f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.005f, 0.005f, 0.005f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.02f, 0.02f, 0.02f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 16.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END;

	// 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_MOVE_NONE;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END;

	// 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐
	tEffectMeshDesc.iPassIndex = 0;

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = false;

	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	// DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다
	tEffectMeshDesc.iRGBNum = 0;

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -1.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::PATTERN_MIXTEX::MIX_NOISING;
	tEffectMeshDesc.iScondTextureIndex = 61; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 1.5;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.25f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Mesh Effect LastAttack Circle ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring03"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 324;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.75f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.005f, 0.005f, 0.005f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 16.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END;

	// 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(45.f, 0.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_MOVE_NONE;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END;

	// 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐
	tEffectMeshDesc.iPassIndex = 0;

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = false;

	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.2f, -0.2f, -0.2f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	// DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다
	tEffectMeshDesc.iRGBNum = 0;

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -1.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::PATTERN_MIXTEX::MIX_NOISING;
	tEffectMeshDesc.iScondTextureIndex = 61; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 1.5;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.25f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Effect Head Stream  ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack07"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 255;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 3.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 0.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _float4(0.f, 0.f, 0.f, 1.f);

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -0.5f;
	tEffectMeshDesc.fStartV = -0.2f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(2.25f, -0.5f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(1.2f, 1.2f, 1.2f, 0.6f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = -0.05f;
	tEffectMeshDesc.fTransparecySpeed = -0.2f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = false;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Effect Head Aircut  ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring05"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 255;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 3.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.125f, 0.125f, 0.125f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.14f, 0.14f, 0.14f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 10.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END;

	// 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(90.f, 90.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _float4(1.6f, 0.f, 0.f, 1.f);

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_TRACING;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	// 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐
	tEffectMeshDesc.iPassIndex = 0; 

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.5f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, -3.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(1.2f, 1.2f, 1.2f, 0.8f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	// DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다
	tEffectMeshDesc.iRGBNum = 0; 

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = -0.15f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.iScondTextureIndex = 73; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 1.5f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.75f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = false;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Effect Aircut  ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring04"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 255;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 1.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.02f, 0.02f, 0.02f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.025f, 0.025f, 0.025f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 20.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(90.f, 0.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _float4(0.f, 0.f, 0.f, 1.f);

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_MOVE_NONE;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

									// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = -0.2f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, -0.5f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.8f, 0.8f, 0.8f, 1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

								 // 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);
}

void CEffect_Manager::Ready_HammerFormMeshDesc_Attack()
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Hammer Attack Effect Rings  ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Cup1"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 308;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 1.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.005f, 0.005f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.03f, 0.015f, 0.015f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

															  // 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 20.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_MOVE_NONE;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = false;

	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.4f, 0.4f, 1.f, 0.8f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -3.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 0; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Effect Wave  ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 324;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 1.f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.005f, 0.005f, 0.005f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.04f, 0.04f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 30.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	// 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_MOVE_NONE;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.iPassIndex = 0; // 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = -0.5f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 2.5f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	tEffectMeshDesc.iRGBNum = 0; // DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -1.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.iScondTextureIndex = 37; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 1.5f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.75f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Attack Effect Crown  ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_splash0"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 324;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 0.75f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.001f, 0.001f, 0.001f, 0.f) * 0.5f;

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.08f, 0.04f, 0.f) * 0.5f;
	tEffectMeshDesc.fScaleSpeed = 16.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END;

	// 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 300.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = false;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = 0.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _zeroPoint4;

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_MOVE_NONE;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END;

	// 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐
	tEffectMeshDesc.iPassIndex = 0;

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = false;

	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(1.f, 1.f, 1.f, 0.5f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	// DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다
	tEffectMeshDesc.iRGBNum = 0;

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -1.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::PATTERN_MIXTEX::MIX_NOISING;
	tEffectMeshDesc.iScondTextureIndex = 61; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 1.5f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.25f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);

	// Hammer Change Gust  ====================================================================================

	ZeroMemory(&tEffectMeshDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	// 기본 설정 변수
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado11"));

	tEffectMeshDesc.iLevelIndex = 0; // 안 채워도 됨
	tEffectMeshDesc.iTextureIndex = 232;

	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::LIFE_TYPE::TIME_LIMIT;

	tEffectMeshDesc.fLifeTime = 1.5f;

	// 크기 설정 변수
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.05f, 0.05f, 0.f);

	// 크기 연출 설정 변수
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tEffectMeshDesc.vMaxScale = __vector(0.025f, 0.025f, 0.025f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 6.f;

	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::EASING_END;

	// 회전 설정 변수
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	// 회전 연출 설정 변수
	tEffectMeshDesc.bSpinning = true;

	tEffectMeshDesc.vSpinAxis = _axisY;
	tEffectMeshDesc.fSpinningSpeed = -6.f;

	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.bNoRandomBillborad = false;

	// 위치 설정 변수
	tEffectMeshDesc.vPos = _float4(0.f, 0.5f, 0.f, 1.f);

	// 이동 연출 설정 변수
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE::PATTERN_MOVE_NONE;

	tEffectMeshDesc.vDir = _axisX;
	tEffectMeshDesc.fMovingSpeed = 0.f;
	tEffectMeshDesc.fMovingDistance = 0.f;

	tEffectMeshDesc.bUseEasing = false;
	tEffectMeshDesc.eEasing = CEffect_Mesh::EASING_END;

	// 아무거나 채워도 객체 초기화할 때 열거체 값에 따라서 알아서 값 채워짐
	tEffectMeshDesc.iPassIndex = 0;

	// UV 설정 변수
	tEffectMeshDesc.bMovingUV = true;

	tEffectMeshDesc.fStartU = -0.3f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vUVMovingSpeed = _float2(1.75f, 0.f);

	// 색상 설정 변수
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::PATTERN_DYING::DYING_MULT;
	tEffectMeshDesc.vColor = _float4(0.35f, 0.35f, 0.35f, 0.8f);
	tEffectMeshDesc.vBlendColor = _float4(0.f);

	// DYING_ONEVALUE 일 때에 사용하는 변수, { R, G, B, A } 중에 어느 원소에 접근할지를 나타낸다
	tEffectMeshDesc.iRGBNum = 0;

	// 투명도 설정 변수
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.75f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;

	tEffectMeshDesc.bAlphaReverse = false;

	// 노이즈 설정 변수
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::PATTERN_MIXTEX::MIX_NOISING;
	tEffectMeshDesc.iScondTextureIndex = 56; // 사용하지 않을 경우 아무 값이나 채워도 됨

	tEffectMeshDesc.fNoisePower = 1.5f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.25f, 0.f);

	// 셰이딩 기법 설정 변수
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;

	// 개별적으로 설정해서 사용해야하는 변수들
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END; // 위치 추가보정 기능을 위한 변수, 대상 기준으로 어느 방향으로 이동시킬지를 나타낸다
	tEffectMeshDesc.fFromTargetDistance = 0.f; // 위치 추가보정 기능을 위한 변수, 정해진 방향으로 얼마만큼 이동시킬지를 나타낸다

	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	m_MeshDesc.push_back(tEffectMeshDesc);
}

void CEffect_Manager::Ready_HammerFormParticleDesc()
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	// HammerForm 공격 파티클 ===========================================================

	ZeroMemory(&tParticleDesc, sizeof(CEffect_Particle::PARTICLEDESC));

	// 기본 설정 변수
	tParticleDesc.iTexIndex = 353;

	tParticleDesc.eLifeType = CEffect_Particle::LIFE_TYPE::TIME_LIMIT;
	tParticleDesc.fLiveTime = 5.f;

	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;

	// 크기 설정 변수
	tParticleDesc.fOriginScale = 0.3f;
	tParticleDesc.fScaleSpeed = 0.75f;

	// 위치 설정, 이동 연출 변수
	tParticleDesc.fMakeRange = 0.f;

	tParticleDesc.bRangeOneOrXYZ = false;
	tParticleDesc.vMakeRangeXYZ = _float3(0.f);

	tParticleDesc.fMaxDistance = 30.f;

	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.vDir = _axisY;

	tParticleDesc.fGravityPower = 17.5f;

	// 속도 설정 변수
	tParticleDesc.fMinSpeed = 3.f;
	tParticleDesc.fMaxSpeed = 9.f;

	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CEffect_Particle::EASING_END;

	// 연출 옵션 설정 변수
	tParticleDesc.bLoop = false;
	tParticleDesc.bChase = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.fTrailInterval = 0.f;

	// 색상 설정 변수
	_float	add_color[4] = { 0.55f, 0.55f, 0.55f, 0.f };
	memcpy(tParticleDesc.fAddColor, add_color, sizeof(_float) * 4);

	_float	mul_color[4] = { 0.8f, 0.4f, 0.45f, 1.f };
	memcpy(tParticleDesc.fMulColor, mul_color, sizeof(_float) * 4);

	// 아무 값이나 채워도 객체의 생성 또는 초기화 과정에서 알아서 값이 채워지는 변수
	tParticleDesc.iLevelIndex = 0;

	tParticleDesc.bBlur = true;
	tParticleDesc.iPass = 0;

	// 개별적으로 설정해서 사용해야하는 변수들
	tParticleDesc.pParentPosition = nullptr;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.pParentDead = nullptr;

	m_PatricleDesc.push_back(tParticleDesc);

	// HammerForm 지면 강타 글로우 파티클 ===========================================================

	ZeroMemory(&tParticleDesc, sizeof(CEffect_Particle::PARTICLEDESC));

	// 기본 설정 변수
	tParticleDesc.iTexIndex = 353;

	tParticleDesc.eLifeType = CEffect_Particle::LIFE_TYPE::TIME_LIMIT;
	tParticleDesc.fLiveTime = 2.5f;

	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

	// 크기 설정 변수
	tParticleDesc.fOriginScale = 0.2f;
	tParticleDesc.fScaleSpeed = 0.175f;

	// 위치 설정, 이동 연출 변수
	tParticleDesc.fMakeRange = 0.f;

	tParticleDesc.bRangeOneOrXYZ = true;
	tParticleDesc.vMakeRangeXYZ = _float3(7.5f, 4.f, 7.5f);

	tParticleDesc.fMaxDistance = 25.f;

	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir = _axisY;

	tParticleDesc.fGravityPower = 0.f;

	// 속도 설정 변수
	tParticleDesc.fMinSpeed = 0.75f;
	tParticleDesc.fMaxSpeed = 2.5f;

	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CEffect_Particle::EASING_END;

	// 연출 옵션 설정 변수
	tParticleDesc.bLoop = false;
	tParticleDesc.bChase = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.fTrailInterval = 0.f;

	// 색상 설정 변수
	_float	add_color_glow[4] = { 0.f, 0.f, 0.f, -0.5f };
	memcpy(tParticleDesc.fAddColor, add_color_glow, sizeof(_float) * 4);

	_float	mul_color_glow[4] = { 0.8f, 0.8f, 0.55f, 1.f };
	memcpy(tParticleDesc.fMulColor, mul_color_glow, sizeof(_float) * 4);

	// 아무 값이나 채워도 객체의 생성 또는 초기화 과정에서 알아서 값이 채워지는 변수
	tParticleDesc.iLevelIndex = 0;

	tParticleDesc.bBlur = true;
	tParticleDesc.iPass = 1;

	// 개별적으로 설정해서 사용해야하는 변수들
	tParticleDesc.pParentPosition = nullptr;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.pParentDead = nullptr;

	m_PatricleDesc.push_back(tParticleDesc);

	// HammerForm 지면 강타 돌덩이 파티클 ===========================================================

	ZeroMemory(&tParticleDesc, sizeof(CEffect_Particle::PARTICLEDESC));

	// 기본 설정 변수
	tParticleDesc.iTexIndex = 371;

	tParticleDesc.eLifeType = CEffect_Particle::LIFE_TYPE::TIME_LIMIT;
	tParticleDesc.fLiveTime = 2.5f;

	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;

	// 크기 설정 변수
	tParticleDesc.fOriginScale = 0.75f;
	tParticleDesc.fScaleSpeed = 0.5f;

	// 위치 설정, 이동 연출 변수
	tParticleDesc.fMakeRange = 0.f;

	tParticleDesc.bRangeOneOrXYZ = true;
	tParticleDesc.vMakeRangeXYZ = _float3(7.5f, 0.f, 7.5f);

	tParticleDesc.fMaxDistance = 25.f;

	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir = _axisY;

	tParticleDesc.fGravityPower = 50.f;

	// 속도 설정 변수
	tParticleDesc.fMinSpeed = 10.f;
	tParticleDesc.fMaxSpeed = 15.f;

	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CEffect_Particle::EASING_END;

	// 연출 옵션 설정 변수
	tParticleDesc.bLoop = false;
	tParticleDesc.bChase = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.fTrailInterval = 0.f;

	// 색상 설정 변수
	_float	add_color_rock[4] = { 0.f, 0.f, 0.f, -0.75f };
	memcpy(tParticleDesc.fAddColor, add_color_rock, sizeof(_float) * 4);

	_float	mul_color_rock[4] = { 0.4f, 0.3f, 0.25f, 1.f };
	memcpy(tParticleDesc.fMulColor, mul_color_rock, sizeof(_float) * 4);

	// 아무 값이나 채워도 객체의 생성 또는 초기화 과정에서 알아서 값이 채워지는 변수
	tParticleDesc.iLevelIndex = 0;

	tParticleDesc.bBlur = true;
	tParticleDesc.iPass = 1;

	// 개별적으로 설정해서 사용해야하는 변수들
	tParticleDesc.pParentPosition = nullptr;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.pParentDead = nullptr;

	m_PatricleDesc.push_back(tParticleDesc);

	// HammerForm Hammer Head Embers 파티클 ===========================================================

	ZeroMemory(&tParticleDesc, sizeof(CEffect_Particle::PARTICLEDESC));

	// 기본 설정 변수
	tParticleDesc.iTexIndex = 353;

	tParticleDesc.eLifeType = CEffect_Particle::LIFE_TYPE::TIME_LIMIT;
	tParticleDesc.fLiveTime = 0.5f;

	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;

	// 크기 설정 변수
	tParticleDesc.fOriginScale = 0.075f;
	tParticleDesc.fScaleSpeed = 0.1f;

	// 위치 설정, 이동 연출 변수
	tParticleDesc.fMakeRange = 0.075f;

	tParticleDesc.bRangeOneOrXYZ = false;
	tParticleDesc.vMakeRangeXYZ = _float3(0.f);

	tParticleDesc.fMaxDistance = 10.f;

	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.vDir = _axisY;

	tParticleDesc.fGravityPower = 0.f;

	// 속도 설정 변수
	tParticleDesc.fMinSpeed = 0.25f;
	tParticleDesc.fMaxSpeed = 3.f;

	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CEffect_Particle::EASING_END;

	// 연출 옵션 설정 변수
	tParticleDesc.bLoop = false;
	tParticleDesc.bChase = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.fTrailInterval = 0.f;

	// 색상 설정 변수
	_float	add_color_embers[4] = { 0.75f, 0.75f, 0.75f, 0.f };
	memcpy(tParticleDesc.fAddColor, add_color_embers, sizeof(_float) * 4);

	_float	mul_color_embers[4] = { 0.8f, 0.45f, 0.35f, 1.f };
	memcpy(tParticleDesc.fMulColor, mul_color_embers, sizeof(_float) * 4);

	// 아무 값이나 채워도 객체의 생성 또는 초기화 과정에서 알아서 값이 채워지는 변수
	tParticleDesc.iLevelIndex = 0;

	tParticleDesc.bBlur = true;
	tParticleDesc.iPass = 0;

	// 개별적으로 설정해서 사용해야하는 변수들
	tParticleDesc.pParentPosition = nullptr;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.pParentDead = nullptr;

	m_PatricleDesc.push_back(tParticleDesc);
}

void CEffect_Manager::Ready_GoatSmokeDesc()
{
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;
	tEffectTexDesc.iPassIndex = 5;
	tEffectTexDesc.iTextureIndex = 231;
	tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
	tEffectTexDesc.vLookDir = _float3(0.f, 0.f, 1.f);
	tEffectTexDesc.fTotalTime = 2.f;
	tEffectTexDesc.bBillboarding = true;
	tEffectTexDesc.bBlur = false;
	tEffectTexDesc.bSprite = false;
	tEffectTexDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectTexDesc.bSpinning = false;
	tEffectTexDesc.fPlaySpeed = 0.5f;
	tEffectTexDesc.vMovingDir = _float3(0.f, 1.f, 0.f);
	tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE_ONEDIR;
	tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_SHRINK;
	tEffectTexDesc.vBaseColor = _float4(1.f, 1.f, 0.6f, 1.f);
	tEffectTexDesc.vMinSize = 3.f;
	tEffectTexDesc.vMaxSize = 4.f;
	tEffectTexDesc.fScalingSpeed = 0.5f;
	tEffectTexDesc.fStartAlpha = 0.f;
	tEffectTexDesc.fTransparecySpeed = 0.5f;

	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));
	m_TextureDesc.push_back(tEffectTexDesc);
}

void CEffect_Manager::Ready_BigTreeSmokeDesc()
{
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;
	tEffectTexDesc.iPassIndex = 5;
	tEffectTexDesc.iTextureIndex = 231;
	tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
	tEffectTexDesc.vLookDir = _float3(0.f, 0.f, 1.f);
	tEffectTexDesc.fTotalTime = 4.f;
	tEffectTexDesc.bBillboarding = true;
	tEffectTexDesc.bBlur = false;
	tEffectTexDesc.bSprite = false;
	tEffectTexDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectTexDesc.bSpinning = false;
	tEffectTexDesc.fPlaySpeed = 0.5f;
	tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE_ONEDIR;
	tEffectTexDesc.vMovingDir = _float3(0.f, 1.f, 0.f);
	tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_SHRINK;
	//tEffectTexDesc.eEasing         = CEffect_Texture::OUTCIRC;
	//tEffectTexDesc.bUseEasing      = true;
	tEffectTexDesc.vBaseColor = _float4(178.f / 255.f, 159.f / 255.f, 134.f / 255.f, 1.f);
	tEffectTexDesc.vMinSize = 3.f;
	tEffectTexDesc.vMaxSize = 4.f;
	tEffectTexDesc.fScalingSpeed = 0.5f;
	tEffectTexDesc.fStartAlpha = 0.f;
	tEffectTexDesc.fTransparecySpeed = 0.25f;

	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

	m_TextureDesc.push_back(tEffectTexDesc);
}

void CEffect_Manager::Ready_RageFormTextureDesc_Attack()
{
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;

	// TEXTURE_RAGEFORM_ATTACK_SPARKLE
	ZeroMemory(&tEffectTexDesc, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	// 기본 설정 변수
	tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
	tEffectTexDesc.eRenderGroup = CRenderer::RENDERGROUP::RENDER_BLEND;
	tEffectTexDesc.iPassIndex = 0;

	tEffectTexDesc.iTextureIndex = 382;
	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));
	tEffectTexDesc.iSecondTextureIndex = 0;

	tEffectTexDesc.pParentDead = nullptr;
	tEffectTexDesc.bDefaultPass = true;

	tEffectTexDesc.eLifeType = CEffect_Texture::LIFE_TYPE::TIME_LIMIT;
	tEffectTexDesc.fTotalTime = 0.5f;

	// Once or Loop
	tEffectTexDesc.ePlayType = CEffect_Texture::PLAY_TYPE::PLAY_ONCE;

	// 초기 회전 값(로컬)
	tEffectTexDesc.RoatationMatrix = XMMatrixIdentity();

	// 크기 설정 변수
	tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_SCALE::PATTERN_BULGE;
	tEffectTexDesc.vMinSize = _float3(0.5f, 0.25f, 0.25f);
	tEffectTexDesc.vMaxSize = _float3(6.f, 3.f, 3.f);
	tEffectTexDesc.fScalingSpeed = 6.f;

	tEffectTexDesc.bUseEasing_Scaling = false;
	tEffectTexDesc.eEasing_Scale = CEffect_Texture::EASINGFUNCTION::EASING_END;

	// 회전 설정 변수
	tEffectTexDesc.vLookDir = _float3(0.f, 0.f, 1.f);
	tEffectTexDesc.bBillboarding = true;

	tEffectTexDesc.bSpinning = false;
	tEffectTexDesc.vSpinAxis = _axisY;
	tEffectTexDesc.fSpinSpeed = 0.f;

	// 위치 설정 변수
	tEffectTexDesc.vPosition = _float3(0.f);

	tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE::PATTERN_MOVE_NONE;
	tEffectTexDesc.vMovingDir = _float3(0.f);
	tEffectTexDesc.fPlaySpeed = 0.f;

	tEffectTexDesc.bUseEasing = false;
	tEffectTexDesc.eEasing = CEffect_Texture::EASINGFUNCTION::EASING_END;

	// 색상 설정 변수
	tEffectTexDesc.vBaseColor = _float4(-0.2f, 0.f, 0.f, 0.f);
	tEffectTexDesc.vBlendingColor = _float4(1.f);

	tEffectTexDesc.fStartAlpha = 0.f;
	tEffectTexDesc.fTransparecySpeed = -2.f;
	tEffectTexDesc.fMaxTransparecy = 0.f;

	// 노이즈 여부, 속도
	tEffectTexDesc.bNoising = false;
	tEffectTexDesc.fNoiseRot = 0.f;
	
	// 렌더링 기법
	tEffectTexDesc.bBlur = false;
	tEffectTexDesc.bDistortion = false;

	// 스프라이트 시트 이미지를 사용
	tEffectTexDesc.bSprite = false;

	// 여러개의 파일로 구성된 스프라이트 이미지를 사용
	tEffectTexDesc.bMultiSprite = false;
	tEffectTexDesc.fSpriteMaxXIndex	= 0;
	tEffectTexDesc.fSpriteMaxYIndex	= 0;

	// 스프라이트 이미지 넘기는 속도
	tEffectTexDesc.fSpriteSpeed = 0.f;

	m_TextureDesc.push_back(tEffectTexDesc);

	// ================================================================

	// TEXTURE_RAGEFORM_ATTACK_CIRCLE
	ZeroMemory(&tEffectTexDesc, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	// 기본 설정 변수
	tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
	tEffectTexDesc.eRenderGroup = CRenderer::RENDERGROUP::RENDER_BLEND;
	tEffectTexDesc.iPassIndex = 0;

	tEffectTexDesc.iTextureIndex = 19;
	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));
	tEffectTexDesc.iSecondTextureIndex = 0;

	tEffectTexDesc.pParentDead = nullptr;
	tEffectTexDesc.bDefaultPass = true;

	tEffectTexDesc.eLifeType = CEffect_Texture::LIFE_TYPE::TIME_LIMIT;
	tEffectTexDesc.fTotalTime = 0.5f;

	// Once or Loop
	tEffectTexDesc.ePlayType = CEffect_Texture::PLAY_TYPE::PLAY_ONCE;

	// 초기 회전 값(로컬)
	tEffectTexDesc.RoatationMatrix = XMMatrixIdentity();

	// 크기 설정 변수
	tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_SCALE::PATTERN_BULGE;
	tEffectTexDesc.vMinSize = _float3(0.25f);
	tEffectTexDesc.vMaxSize = _float3(2.f);
	tEffectTexDesc.fScalingSpeed = 6.f;

	tEffectTexDesc.bUseEasing_Scaling = false;
	tEffectTexDesc.eEasing_Scale = CEffect_Texture::EASINGFUNCTION::EASING_END;

	// 회전 설정 변수
	tEffectTexDesc.vLookDir = _float3(0.f, 0.f, 1.f);
	tEffectTexDesc.bBillboarding = true;

	tEffectTexDesc.bSpinning = false;
	tEffectTexDesc.vSpinAxis = _axisY;
	tEffectTexDesc.fSpinSpeed = 0.f;

	// 위치 설정 변수
	tEffectTexDesc.vPosition = _float3(0.f);

	tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE::PATTERN_MOVE_NONE;
	tEffectTexDesc.vMovingDir = _float3(0.f);
	tEffectTexDesc.fPlaySpeed = 0.f;

	tEffectTexDesc.bUseEasing = false;
	tEffectTexDesc.eEasing = CEffect_Texture::EASINGFUNCTION::EASING_END;

	// 색상 설정 변수
	tEffectTexDesc.vBaseColor = _float4(0.f);
	tEffectTexDesc.vBlendingColor = _float4(0.8f, 0.2f, 0.2f, 1.f);

	tEffectTexDesc.fStartAlpha = 0.f;
	tEffectTexDesc.fTransparecySpeed = -1.f;
	tEffectTexDesc.fMaxTransparecy = 0.f;

	// 노이즈 여부, 속도
	tEffectTexDesc.bNoising = false;
	tEffectTexDesc.fNoiseRot = 0.f;

	// 렌더링 기법
	tEffectTexDesc.bBlur = false;
	tEffectTexDesc.bDistortion = false;

	// 스프라이트 시트 이미지를 사용
	tEffectTexDesc.bSprite = false;

	// 여러개의 파일로 구성된 스프라이트 이미지를 사용
	tEffectTexDesc.bMultiSprite = false;
	tEffectTexDesc.fSpriteMaxXIndex = 0;
	tEffectTexDesc.fSpriteMaxYIndex = 0;

	// 스프라이트 이미지 넘기는 속도
	tEffectTexDesc.fSpriteSpeed = 0.f;

	m_TextureDesc.push_back(tEffectTexDesc);
}

HRESULT CEffect_Manager::Create_HitStar(CTransform*	pTransform, _float fOffsetY)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Mesh::EFFECT_MESHDESC tDesc = m_MeshDesc[TYPE_MOVEONE_SPIN];
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Star07"));
	tDesc.iLevelIndex = GM->Get_CurrentLevel();
	_vector vTargetPos     = pTransform->Get_Position() + __vector(0.f, fOffsetY, 0.f, 0.f);
	XMStoreFloat4(&tDesc.vPos, vTargetPos);
	tDesc.fRotationAngle   = _float3(0.f, 90.f, 0.f);
	tDesc.fScaleSpeed      = 15.f;
	tDesc.fLifeTime        = 0.3f;
	tDesc.vLookDir         = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	tDesc.vSpinAxis        = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tDesc.eDyingPattern    = CEffect_Mesh::DYING_ADD;
	tDesc.vColor           = _float4(-0.3f, -0.1f, 0.f, 0.f);
	tDesc.iTextureIndex    = 345;
	tDesc.bBillboarding    = true;
	tDesc.LocalMat         = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	_uint iNum = rand() % 3 + 7;

	for (_uint i = 0; i < iNum; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = true;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 0.f, 0.f, 0.f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 2.5f, 2.f, 1.f, 0.24f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 1.5f;
	tParticleDesc.fMakeRange     = 0.f;
	tParticleDesc.fMaxDistance   = 20.f;
	tParticleDesc.fMinSpeed      = 10.f;
	tParticleDesc.fMaxSpeed      = 20.f;
	tParticleDesc.fScaleSpeed    = 1.46f;
	tParticleDesc.fOriginScale   = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 349;
	tParticleDesc.iPass          = 2;
	tParticleDesc.pDead          = nullptr;
	tParticleDesc.eEasing        = CEffect_Particle::OUTCUBIC;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(pGameInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//// 히트 이펙트
	//CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;

	//_uint  iTexIndex = 364;

	//tEffectTexDesc.iTextureIndex = iTexIndex;
	//tEffectTexDesc.iPassIndex = 0;
	//tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
	//tEffectTexDesc.fTotalTime = 2.f;
	//tEffectTexDesc.bBillboarding = true;
	//tEffectTexDesc.bBlur = true;
	//tEffectTexDesc.bSprite = true;
	//tEffectTexDesc.fSpriteMaxXIndex = 4;
	//tEffectTexDesc.fSpriteMaxYIndex = 2;
	//tEffectTexDesc.fSpriteSpeed = 6.25f;
	//tEffectTexDesc.ePlayType = CEffect_Texture::PLAY_ONCE;
	//tEffectTexDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	//tEffectTexDesc.eLifeType = CEffect_Texture::SPRITE_LIMIT;
	//tEffectTexDesc.bSpinning = false;
	//tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_SCALE_NONE;
	//tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE_NONE;
	//tEffectTexDesc.vMovingDir = _float3(0.f, 1.f, 0.f);
	//tEffectTexDesc.eEasing = CEffect_Texture::OUTCIRC;
	//tEffectTexDesc.bUseEasing = false;
	//tEffectTexDesc.vMinSize = _float3(1.f, 1.f, 1.f);
	//tEffectTexDesc.vMaxSize = _float3(0.1f, 0.1f, 0.1f);
	//tEffectTexDesc.fScalingSpeed = 3.f;
	//tEffectTexDesc.fStartAlpha = 0.f;
	//tEffectTexDesc.vBaseColor = _float4(0.f, 0.f, 0.f, 0.f);
	//tEffectTexDesc.fPlaySpeed = 2.f;

	//_vector vPosition = pTransform->Get_Position();
	//vPosition = XMVectorSetY(vPosition, vecy(vPosition) - fOffsetY);
	//tEffectTexDesc.vPosition = XMStoreFloat3RT(vPosition);
	//lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

	//if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}


	return S_OK;
}

HRESULT CEffect_Manager::Create_HitStar(_float4* pPosition, _float fOffsetY)
{
	CGameInstance* pGameInstance        = GET_INSTANCE(CGameInstance);
	CEffect_Mesh::EFFECT_MESHDESC tDesc = m_MeshDesc[TYPE_MOVEONE_SPIN];

	_float4 vTargetPos = *pPosition;
	vTargetPos.y      += fOffsetY;

	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Star07"));
	tDesc.vPos             = vTargetPos;
	tDesc.iLevelIndex      = pGameInstance->Get_LevelIndex();
	tDesc.fRotationAngle   = _float3(0.f, 90.f, 0.f);
	tDesc.fScaleSpeed      = 15.f;
	tDesc.fLifeTime        = 0.3f;
	tDesc.vLookDir         = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	tDesc.vSpinAxis        = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tDesc.eDyingPattern    = CEffect_Mesh::DYING_ADD;
	tDesc.vColor           = _float4(-0.3f, -0.1f, 0.f, 0.f);
	tDesc.iTextureIndex    = 345;
	tDesc.bBillboarding    = true;
	tDesc.LocalMat         = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	_uint iNum = rand() % 3 + 7;

	for (_uint i = 0; i < iNum; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = true;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 0.f, 0.f, 0.f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 2.5f, 2.f, 1.f, 0.24f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 1.5f;
	tParticleDesc.fMakeRange     = 0.f;
	tParticleDesc.fMaxDistance   = 20.f;
	tParticleDesc.fMinSpeed      = 10.f;
	tParticleDesc.fMaxSpeed      = 20.f;
	tParticleDesc.fScaleSpeed    = 1.46f;
	tParticleDesc.fOriginScale   = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 349;
	tParticleDesc.iPass          = 2;
	tParticleDesc.pDead          = nullptr;
	tParticleDesc.eEasing        = CEffect_Particle::OUTCUBIC;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pPosition;
	pPos->y = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(pGameInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;
	// 358 357
	_uint  iTexIndex = 342;
	_vector vPos = XMLoadFloat4(pPosition);

	CPlayer::SORA_FORM curr_form = GM->Get_Player()->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:
	case Client::CPlayer::SF_HAMMER:

		tEffectTexDesc.iTextureIndex = iTexIndex;
		tEffectTexDesc.iPassIndex = 0;
		tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
		tEffectTexDesc.fTotalTime = 0.35f;
		tEffectTexDesc.bBillboarding = true;
		tEffectTexDesc.bBlur = true;
		tEffectTexDesc.bSprite = false;
		tEffectTexDesc.fSpriteMaxXIndex = 8;
		tEffectTexDesc.fSpriteMaxYIndex = 8;
		tEffectTexDesc.fSpriteSpeed = 11.f;
		tEffectTexDesc.ePlayType = CEffect_Texture::PLAY_ONCE;
		tEffectTexDesc.eRenderGroup = CRenderer::RENDER_BLEND;
		tEffectTexDesc.eLifeType = CEffect_Texture::TIME_LIMIT;
		tEffectTexDesc.bSpinning = true;
		tEffectTexDesc.fSpinSpeed = 4.5f;
		tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_DYNAMIC;
		tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE_NONE;
		tEffectTexDesc.eEasing = CEffect_Texture::INSINE;
		tEffectTexDesc.bUseEasing_Scaling = false;
		tEffectTexDesc.vMinSize = _float3(0.2f, 0.2f, 0.2f);
		tEffectTexDesc.fStartAlpha = 0.f;
		tEffectTexDesc.fScalingSpeed = 22.f;
		tEffectTexDesc.vMaxSize = _float3(1.f, 1.f, 1.f);
		//tEffectTexDesc.vBaseColor = _float4(1.0f, 1.0f, -0.1f, -0.3f);
		tEffectTexDesc.vBaseColor = _float4(-0.1f, -0.2f, -0.4f, -0.3f);
		tEffectTexDesc.vBlendingColor = _float4(1.f, 1.f, 1.f, 1.f);
		tEffectTexDesc.bDefaultPass = true;
		XMStoreFloat3(&tEffectTexDesc.vPosition, vPos);

		if (tEffectTexDesc.vPosition.y < 0.f)
			tEffectTexDesc.vPosition.y = 0.f;

		lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		break;
	case Client::CPlayer::SF_RAGE:
		Create_RageForm_Effect_Attack_Texture(vPos);
		break;
	}
	
	// 히트 이펙트
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.bExplosion = true;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;

	_float	fAddColor1[4] = { 1.0f, 1.0f, 0.42f, -0.06f};
	memcpy(tParticleDesc.fAddColor, fAddColor1, sizeof(_float) * 4);

	_float	fMulColor1[4] = { 1.f, 1.f, 1.f, 1.0f};
	memcpy(tParticleDesc.fMulColor, fMulColor1, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 1.3f;
	tParticleDesc.fLiveTime = 2.f;
	tParticleDesc.fMakeRange = 1.3f;
	tParticleDesc.fMaxDistance = 0.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 10.f;
	tParticleDesc.fScaleSpeed = 0.15f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 349;
	tParticleDesc.iPass = 2;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eEasing = CEffect_Particle::OUTCUBIC;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	pPos = pPosition;
	pPos->y = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(pGameInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_AttackedEffect(_float4* pPosition, MONSTERTYPE eMonsterType, _float fOffsetY)
{
	// 작은 이펙트들
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_ATTACKED_PTCL];

	_float4* pPos = pPosition;

	auto vPosition = XMLoadFloat4(pPos);

	vPosition = XMVectorSetY(vPosition, vecy(vPosition) + fOffsetY);

	_float4	vPos;

	XMStoreFloat4(&vPos, vPosition);

	*pPos = vPos;

	tParticleDesc.pParentPosition = pPos;

	switch (eMonsterType)
	{
	case MONSTERTYPE_SKULLSOLDIER:
	{
		_float	fAddColor[4] = { 1.0f, -0.62f, -0.83f, -0.11f};
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_CROWSOLDIER:
	{
		_float	fAddColor[4] = { 0.03f, -0.11f, 1.f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}

	break;
	case MONSTERTYPE_THINSOLDIER:
	{
		_float	fAddColor[4] = { 1.0f, -0.62f, -0.83f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_NIGHTMARE:
	{
		_float	fAddColor[4] = { 1.0f, 0.55f, -0.6f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_SHININGEGG:
	{
		_float	fAddColor[4] = { 0.29f, 1.0f, -0.36f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_MONKEY:
	{
		_float	fAddColor[4] = { 1.0f, -0.62f, -0.83f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_MOWER:
	{
		_float	fAddColor[4] = { 0.93f, 1.0f, -0.62f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_NOBODY:
	{
		_float	fAddColor[4] = { 1.0f, -0.06f, -0.17f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_BIGTREE:
	{
		_float	fAddColor[4] = { -0.4f, 1.f, -0.31f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_ICEMAN:
	{
		_float	fAddColor[4] = { 0.f, 0.15f, 0.f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_FROZENWOLF:
	{
		_float	fAddColor[4] = { 0.19f, -0.13f, 0.0f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_SUMMONWOLF:
	{
		_float	fAddColor[4] = { 0.19f, -0.13f, 0.0f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	case MONSTERTYPE_GOAT:
	{
		// 미정
		_float	fAddColor[4] = { 1.0f, -0.2f, -0.62f, -0.11f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	}
	break;
	}


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Manager::Create_SummonParticle(CTransform * pTransform)
{
	CSummonMovingParticle::SUMPARTDESC tSummonParticleDesc;
	tSummonParticleDesc.pTargetTransform = pTransform;
	tSummonParticleDesc.vLocalPosition = _float3(1.f, 0.f, 0.f);
	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SummonMovingParticle"), &tSummonParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tSummonParticleDesc.vLocalPosition = _float3(-1.f, 0.f, 0.f);
	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SummonMovingParticle"), &tSummonParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tSummonParticleDesc.vLocalPosition = _float3(0.f, 0.f, 1.f);
	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SummonMovingParticle"), &tSummonParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tSummonParticleDesc.vLocalPosition = _float3(0.f, 0.f, -1.f);
	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_SummonMovingParticle"), &tSummonParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_SwordEffect(_float4x4* pTargetWorld)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Plane1"));
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex      = 186;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 2.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.pParentDead        = nullptr;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_DYNAMIC;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTargetWorld;
	tEffectMeshDesc.vStartScale        = __vector(0.0001f, 0.0001f, 0.0001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.01f, 0.01f, 0.013f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 9.f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 3.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = -1.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.4f, 0.7f, 1.0f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.2f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixTranslation(-25.f, 0.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_SwordEffectParticle(CTransform* pTransform)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;
	tParticleDesc.bRangeOneOrXYZ = false;
	tParticleDesc.fMakeRange = 1.f;
	tParticleDesc.fMaxDistance = 2.f;


	_float	fAddColor[4] = { 0.51f, 1.f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 2.f;
	tParticleDesc.fMaxDistance = 1.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 3.f;
	tParticleDesc.fScaleSpeed = 0.1f;
	tParticleDesc.fOriginScale = 0.05f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 344;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::INSINE;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.pParentPosition = pPos;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	auto vAxis = XMVector3Normalize(pTransform->Get_Right()) * 0.5f;

	pPos->x -= vecx(vAxis);
	pPos->y -= vecy(vAxis);
	pPos->z -= vecz(vAxis);


	//_vector vOffsetPos = vOffset;
	//pPos->y += vecy(vOffsetPos);
	tParticleDesc.iPass = 2;


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_ReadyToAttackEffect(CTransform* pTransform)
{
	// 바닥에 파장
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring00"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 0.1f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex      = 173;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.pParentDead        = nullptr;
	tEffectMeshDesc.fTransparecySpeed  = -1.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.5f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, -1.25f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.3f, -0.4f, -0.7f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.2f);
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(90.f)) /** XMMatrixTranslation(-25.f, 0.f, 0.f)*/;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 기둥
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere02"));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.iTextureIndex      = 172;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.pParentDead        = nullptr;
	tEffectMeshDesc.fTransparecySpeed  = -1.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.5f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, -1.25f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.3f, -0.4f, -0.7f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.2f);
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationZ(XMConvertToRadians(-90.f)) /** XMMatrixTranslation(-25.f, 0.f, 0.f)*/;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.bExplosion = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;
	tParticleDesc.bRangeOneOrXYZ = true;
	tParticleDesc.vMakeRangeXYZ = _float3(2.f, 0.f, 2.f);

	_float    fAddColor[4] = { 1.0f, 0.46f, 0.16f, 0.f};
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float    fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 1.f;
	tParticleDesc.fMaxDistance = 5.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 5.f;
	tParticleDesc.fScaleSpeed = 0.22f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fTrailInterval = 0.02f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 353;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fExplosionStartRange = 0.5f;
	tParticleDesc.bSeperateAxisRange = false;
	tParticleDesc.vAxisRangeXYZ = _float3(0.f, 0.f, 360.f);

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bUsingAxis = false;
	tParticleDesc.vParentWorldMatrix = pTransform->Get_WorldMatrix();


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_PlayerHealEffect(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Heart1"));
	tDesc.iTextureIndex = 73;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.75f;

	tDesc.bSpinning = true;
	tDesc.fSpinningSpeed = 15.f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = false;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(0.f, 1.0f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.4f, -0.2f, 0.2f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 50.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_GETITEM_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bExplosion = false;
	tParticleDesc.bLoop = true;
	tParticleDesc.bChase = true;
	tParticleDesc.iPass = 2;
	tParticleDesc.bExplosion = false;


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_PlayerHealEffect_Full(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Heart1"));
	tDesc.iTextureIndex = 73;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.75f;

	tDesc.bSpinning = true;
	tDesc.fSpinningSpeed = 15.f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = false;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(0.f, 1.0f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.4f, -0.2f, 0.2f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 50.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tDesc.LocalMat = XMMatrixTranslation(-20.f, 100.f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.02f, 0.02f, 0.02f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	tDesc.LocalMat = XMMatrixTranslation(20.f, 100.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_GETITEM_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bExplosion = false;
	tParticleDesc.bLoop = true;
	tParticleDesc.bChase = true;
	tParticleDesc.iPass = 2;
	tParticleDesc.bExplosion = false;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_PlayerHealEffect_MP(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Heart1"));
	tDesc.iTextureIndex = 73;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.75f;

	tDesc.bSpinning = true;
	tDesc.fSpinningSpeed = 15.f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = false;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(0.f, 1.0f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(-0.5f, -0.2f, 0.5f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 50.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_GETITEM_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bExplosion = false;
	tParticleDesc.bLoop = true;
	tParticleDesc.bChase = true;
	tParticleDesc.iPass = 2;
	tParticleDesc.bExplosion = false;


	_float	fAddColor[4] = { 0.2f, 0.5f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Manager::Create_PlayerBuffEffect(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall00"));
	tDesc.iTextureIndex = 258;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.f;

	tDesc.bSpinning = false;
	tDesc.fSpinningSpeed = 3.75f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.15f, 0.15f, 0.15f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(1.f, 3.f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.4f, 0.2f, -0.1f,  0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 5.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall01"));
	tDesc.iTextureIndex = 258;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.f;

	tDesc.bSpinning = false;
	tDesc.fSpinningSpeed = 3.75f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.15f, 0.15f, 0.15f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(1.f, 1.75f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(1.f, 0.2f, -0.1f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 5.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_GETITEM_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.bExplosion = false;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bLoop = true;
	tParticleDesc.bChase = true;
	tParticleDesc.iPass = 2; 

	_float	fAddColor[4] = { 1.0f, 1.0f, 0.62f, 0.f};
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_PlayerBuffEffect_AttackSpeed(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall00"));
	tDesc.iTextureIndex = 258;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.f;

	tDesc.bSpinning = false;
	tDesc.fSpinningSpeed = 3.75f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.15f, 0.15f, 0.15f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(1.f, 3.f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(1.f, 0.1f, -0.2f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 5.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall01"));
	tDesc.iTextureIndex = 258;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.f;

	tDesc.bSpinning = false;
	tDesc.fSpinningSpeed = 3.75f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.15f, 0.15f, 0.15f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(1.f, 1.75f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(1.f, -0.05f, -0.05f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 5.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_GETITEM_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.bExplosion = false;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bLoop = true;
	tParticleDesc.bChase = true;
	tParticleDesc.iPass = 2;
	tParticleDesc.fLiveTime = 0.5f;
	_float	fAddColor[4] = { 1.0f, 0.8f, 0.3f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_PlayerBuffEffect_MovingSpeed(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall00"));
	tDesc.iTextureIndex = 258;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.f;

	tDesc.bSpinning = false;
	tDesc.fSpinningSpeed = 3.75f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.15f, 0.15f, 0.15f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(1.f, 3.f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.3f, 1.f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 5.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall01"));
	tDesc.iTextureIndex = 258;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.f;

	tDesc.bSpinning = false;
	tDesc.fSpinningSpeed = 3.75f;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.15f, 0.15f, 0.15f, 0.f);
	tDesc.fScaleSpeed = 5.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(1.f, 1.75f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.3f, 1.f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tDesc.LocalMat = XMMatrixTranslation(0.f, 5.f, 0.f);

	if (FAILED(pInstance->Add_GameObject(GM->Get_CurrentLevel(), m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_GETITEM_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.bExplosion = false;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bLoop = true;
	tParticleDesc.bChase = true;
	tParticleDesc.iPass = 2;
	tParticleDesc.fLiveTime = 0.5f;
	_float	fAddColor[4] = { 0.2f, 0.8f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_TrailMesh(class CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tDesc.iTextureIndex = 46;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 0.56f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -0.3f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(7.5f, 1.0f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	tDesc.vColor = _float4(5.4f, 3.1f, 0.3f, 1.0f);

	// 트레일 메시 이펙트 가로로 출력되는 것 수정사항
	tDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 15.f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 반짝이 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_SHINING_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	tParticleDesc.pParentPosition = pPos;

	tParticleDesc.fMakeRange = 6.f;
	tParticleDesc.fMaxDistance = 6.f;
	tParticleDesc.fScaleSpeed = 1.5f;
	tParticleDesc.fLiveTime = 0.3f;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.iPass = 2;
	tParticleDesc.vDir = __vector(0.f, 0.f, 0.f, 0.f);


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_TrailMesh_Spin(_float4x4* pTargetWorld)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tDesc.iTextureIndex = 46;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTargetWorld;

	tDesc.fLifeTime = 0.56f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -0.3f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(7.5f, 1.0f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	tDesc.vColor = _float4(5.4f, 3.1f, 0.3f, 1.0f);

	// 트레일 메시 이펙트 가로로 출력되는 것 수정사항
	tDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixTranslation(0.f, 30.f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Dash(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Attack05"));
	tDesc.iTextureIndex = 24;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 0.2f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.LocalMat = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 30.f, 0.f);

	tDesc.vStartScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = false;

	tDesc.vUVMovingSpeed = _float2(7.5f, 1.0f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.f, 0.f, -0.1f);

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	//CEffect_Particle::PARTICLEDESC tParticleDesc;
	//tParticleDesc.bChase = false;
	//tParticleDesc.bLoop = false;
	//tParticleDesc.bTrail = false;
	//tParticleDesc.bUseEasing = false;
	//tParticleDesc.bExplosion = true;
	//tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	//tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	//tParticleDesc.bRangeOneOrXYZ = false;
	//tParticleDesc.vMakeRangeXYZ = _float3(20.f, 0.f, 20.f);
	//tParticleDesc.fMakeRange = 2.f;
	//tParticleDesc.bSeperateAxisRange = true;
	//tParticleDesc.vAxisRangeXYZ = _float3(360.f, 360.f, 360.f);
	//tParticleDesc.eState = CEffect_Particle::STATE_LOOK;

	//_float    fAddColor[4] = { 0.02f, 0.14f, -0.34f, -0.03f };
	//memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	//_float    fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	//memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	//tParticleDesc.fGravityPower = 0.f;
	//tParticleDesc.fLiveTime = 0.82f;
	//tParticleDesc.fMaxDistance = 0.f;
	//tParticleDesc.fMinSpeed = 1.f;
	//tParticleDesc.fMaxSpeed = 10.f;
	//tParticleDesc.fScaleSpeed = 0.36f;
	//tParticleDesc.fOriginScale = 0.2f;
	//tParticleDesc.fTrailInterval = 0.02f;
	//tParticleDesc.iLevelIndex = 0;
	//tParticleDesc.iTexIndex = 39;
	//tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	//tParticleDesc.fExplosionStartRange = 1.f;

	//_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	//_vector vOffsetPos = __vector(0.f, 1.f, 0.f, 0.f);
	//pPos->y += vecy(vOffsetPos);
	//tParticleDesc.iPass = 2;
	//tParticleDesc.pParentPosition = pPos;
	//tParticleDesc.bUsingAxis = false;
	//tParticleDesc.vParentWorldMatrix = pTransform->Get_WorldMatrix();


	//if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	//{
	//	BREAKPOINT;
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CEffect_Manager::Create_AirWind(_float4x4* pTargetWorld)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tDesc.iTextureIndex = 245;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	//XMStoreFloat4(&tDesc.vPos, vPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	tDesc.pTargetWorldMatrix = pTargetWorld;

	tDesc.LocalMat = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 20.f, 0.f);

	tDesc.fLifeTime = 0.1f;

	tDesc.fRotationAngle = _float3(0.f, 90.f, 90.f);

	tDesc.fTransparecySpeed = -0.1f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	
	tDesc.vStartScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.1f, 0.1f, 0.1f, 0.f);
	tDesc.fSpinningSpeed = 2.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = false;

	tDesc.vUVMovingSpeed = _float2(0.f, 7.5f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.f, 0.f, -0.1f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Guard(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado12"));
	tDesc.iTextureIndex = 16;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();

	tDesc.fLifeTime = 1.f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.1f, 0.1f, 0.1f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.LocalMat = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 10.f, 10.f);

	tDesc.vUVMovingSpeed = _float2(0.f, 4.f);

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.f, 0.f, 0.f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.bExplosion = true;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ = false;
	tParticleDesc.vMakeRangeXYZ = _float3(20.f, 0.f, 20.f);
	tParticleDesc.fMakeRange = 2.f;

	_float    fAddColor[4] = { 0.f, 0.f, 0.f, -0.11f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float    fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 4.f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.fMaxDistance = 0.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 6.f;
	tParticleDesc.fScaleSpeed = 0.73f;
	tParticleDesc.fOriginScale = 0.2f;
	tParticleDesc.fTrailInterval = 0.02f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 381;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fExplosionStartRange = 0.5f;
	tParticleDesc.bSeperateAxisRange = true;
	tParticleDesc.vAxisRangeXYZ = _float3(0.f, 0.f, 360.f);
	tParticleDesc.eState = CEffect_Particle::STATE_UP;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	_vector vOffsetPos = XMVector3Normalize(pTransform->Get_Look()) + __vector(0.f, 1.f, 0.f, 0.f);
	pPos->y += vecy(vOffsetPos);
	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.bUsingAxis = true;
	tParticleDesc.vParentWorldMatrix = pTransform->Get_WorldMatrix();


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_TrailMesh_Short(_float4x4* pTargetWorld, TRAIL_TYPE eTrailType, _float4* pPosition)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc = m_MeshDesc[TYPE_TRAIL_SHORT];

	tDesc.iLevelIndex        = pInstance->Get_LevelIndex();
	tDesc.pTargetWorldMatrix = pTargetWorld;
	tDesc.eFromTarget        = CEffect_Mesh::DIR_END;
	_float4 vPosition        = _float4((*pTargetWorld).m[3][0], (*pTargetWorld).m[3][1], (*pTargetWorld).m[3][2], 1.f);
	tDesc.vPos               = vPosition;
	tDesc.vLookDir           = XMVector3Normalize(__vector((*pTargetWorld).m[2][0], (*pTargetWorld).m[2][1], (*pTargetWorld).m[2][2], 0.f));
	tDesc.LocalMat_NoTrace = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 80.f, 50.f);

	switch (eTrailType) 
	{
	case TRAIL_LEFT:
		tDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
		tDesc.fTransparecySpeed  = -1.25f;
		tDesc.LocalMat_NoTrace   = XMMatrixIdentity();
		tDesc.iTextureIndex      = 240;
		tDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;  
		tDesc.vColor             = _float4(1.f, 1.f, 1.f, -0.2f);
		tDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
		tDesc.bUsingBlur         = true;
		tDesc.fStartU            = 0.f;
		tDesc.vUVMovingSpeed     = _float2(2.5f, 0.f);
		tDesc.vStartScale        = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
		tDesc.LocalMat           = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(-160.f)) * XMMatrixRotationAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(-30.f, 50.f, 20.f);
		break;
	case TRAIL_RIGHT:
		tDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
		tDesc.fTransparecySpeed  = -1.25f;
		tDesc.LocalMat_NoTrace   = XMMatrixIdentity();
		tDesc.iTextureIndex      = 240; 
		tDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
		tDesc.vColor             = _float4(1.f, 1.f, 1.f, -0.2f);
		tDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
		tDesc.bUsingBlur         = true;
		tDesc.vStartScale        = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
		tDesc.fStartU            = 0.f;
		tDesc.vUVMovingSpeed     = _float2(2.5f, 0.f);
		tDesc.LocalMat           = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(-20.f)) * XMMatrixRotationAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixTranslation(30.f, 50.f, 20.f);
		break;
	case TRAIL_DEFAULT:
		CEffect_Particle::PARTICLEDESC tParticleDesc;

		tParticleDesc = m_PatricleDesc[TYPE_SHINING_PTCL];

		_float4* pPos = pPosition;

		tParticleDesc.pParentPosition = pPos;
		tParticleDesc.fMakeRange = 6.f;
		tParticleDesc.fMaxDistance = 6.f;
		tParticleDesc.fScaleSpeed = 1.5f;
		tParticleDesc.fLiveTime = 0.3f;
		tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
		tParticleDesc.iPass = 2;
		tParticleDesc.vDir = __vector(0.f, 0.f, 0.f, 0.f);


		if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		break;
	}

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc))) 
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Gust(CTransform * pTransform, _fvector vPosition)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_GUST];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition);

	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Attack1(CTransform * pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_ATK1];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();
	
	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	tEffectMeshDesc.LocalMat = 
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
		XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::RIGHT;
	tEffectMeshDesc.fFromTargetDistance = -1.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Attack2(CTransform * pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_ATK2];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
		XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = -1.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Attack3(CTransform * pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_ATK3];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
		XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::RIGHT;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Attack4(CTransform * pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_ATK4];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
		XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::RIGHT;
	tEffectMeshDesc.fFromTargetDistance = -1.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Attack5(CTransform * pTransform, _fvector vOffset, _uint iSequence)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_ATK5];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	if (0 == iSequence)
	{
		tEffectMeshDesc.LocalMat =
			XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
			XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
			XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
			XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

		tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
		tEffectMeshDesc.fFromTargetDistance = -1.f;
	}
	else
	{
		tEffectMeshDesc.LocalMat =
			XMMatrixRotationAxis(_axisX, XMConvertToRadians(180.f)) *
			XMMatrixRotationAxis(_axisY, XMConvertToRadians(0.f)) *
			XMMatrixRotationAxis(_axisZ, XMConvertToRadians(135.f)) *
			XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

		tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::RIGHT;
		tEffectMeshDesc.fFromTargetDistance = -1.f;
	}

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_LastAttack_Ground(CTransform * pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_LAST_ATTACK_GROUND];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
		XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = -1.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_LastAttack_Air(CTransform * pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_LAST_ATTACK_AIR];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
		XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = -1.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_LastAttack_UpStream(CTransform * pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_LAST_UPSTREAM];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z)) *
		XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_LastAttack_Spike(CTransform * pTransform, _fvector vPoint, _fvector vLook)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_LAST_SPIKE];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPoint);

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_LastAttack_Circle(CTransform * pTransform, _fvector vPoint, _fvector vLook)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_LAST_RING];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPoint);
	tEffectMeshDesc.vLookDir = vLook;

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameObject* effect = CGameInstance::GetInstance()->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"));

	if (nullptr == effect)
	{
		_breakpoint;
		return E_FAIL;
	}

	effect->Get_Transform()->MakeSameDirection(_vec3norm(vLook));

	_vector current_right = effect->Get_Transform()->Get_Right();
	effect->Get_Transform()->NowRotation(_vec3norm(current_right), XMConvertToRadians(45.f));

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_AttackEffect_Beads(_fvector vPoint)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc = m_PatricleDesc[TYPE_HAMMERFORM_ATTACK_PTCL];

	_float4 position;
	XMStoreFloat4(&position, vPoint);

	tParticleDesc.pParentPosition = &position;
	tParticleDesc.pParentDead = nullptr;
	tParticleDesc.pDead = nullptr;

	tParticleDesc.iPass = 1;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_AttackEffect_Rings(CTransform* pTransform, _fvector vPoint)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_RINGS];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPoint);

	// 방향 설정
	_vector player_position = pTransform->Get_Position();
	_vector effect_creation_point = vPoint;

	vecy(player_position) = 0.f;
	vecy(effect_creation_point) = 0.f;

	tEffectMeshDesc.vLookDir = player_position - effect_creation_point;

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tEffectMeshDesc.fRotationAngle.z = 160.f;
	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Effect_GroundHit_Glow(_fvector vPoint, _uint iAnimIndex)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc = m_PatricleDesc[TYPE_HAMMERFORM_GROUND_GLOW_PTCL];

	_float4 position;
	XMStoreFloat4(&position, vPoint);

	tParticleDesc.pParentPosition = &position;
	tParticleDesc.pParentDead = nullptr;
	tParticleDesc.pDead = nullptr;

	tParticleDesc.iPass = 1;

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)iAnimIndex;
	_uint loop_number = 0;
	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:
		tParticleDesc.vMakeRangeXYZ = _float3(7.5f, 4.f, 7.5f);
		loop_number = 1;
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
		tParticleDesc.vMakeRangeXYZ = _float3(4.5f, 4.f, 4.5f);
		loop_number = 1;
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
		tParticleDesc.vMakeRangeXYZ = _float3(12.5f, 6.f, 12.5f);
		loop_number = 3;
		break;
	}

	for (_uint i = 0; i < loop_number; ++i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Effect_GroundHit_Rock(_fvector vPoint, _uint iAnimIndex)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc = m_PatricleDesc[TYPE_HAMMERFORM_GROUND_ROCK_PTCL];

	_float4 position;
	XMStoreFloat4(&position, vPoint);

	tParticleDesc.pParentPosition = &position;
	tParticleDesc.pParentDead = nullptr;
	tParticleDesc.pDead = nullptr;

	tParticleDesc.iPass = 1;
	tParticleDesc.bBlur = false;

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)iAnimIndex;
	_uint loop_number = 0;
	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:
		tParticleDesc.vMakeRangeXYZ = _float3(7.5f, 1.f, 7.5f);
		loop_number = 2;
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
		tParticleDesc.vMakeRangeXYZ = _float3(4.5f, 1.f, 4.5f);
		loop_number = 1;
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
		tParticleDesc.vMakeRangeXYZ = _float3(12.5f, 1.f, 12.5f);
		loop_number = 3;
		break;
	}

	_uint rock_color = rand() % 2;
	_float4 mul_color = _float4(0.f);
	switch (rock_color)
	{
	case 0:
		mul_color = _float4(0.4f, 0.3f, 0.25f, 1.f);
		break;
	case 1:
		mul_color = _float4(0.4f * 0.25f, 0.3f * 0.25f, 0.25f * 0.25f, 1.f);
		break;
	}
	
	memcpy(&tParticleDesc.fMulColor, &mul_color, sizeof(_float) * 4);

	for (_uint i = 0; i < loop_number; ++i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Effect_GroundHit_Wave(CTransform * pTransform, _fvector vPoint, _uint iAnimIndex)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_WAVE];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)iAnimIndex;
	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:
		tEffectMeshDesc.vStartScale = __vector(0.005f, 0.005f, 0.005f, 0.f);
		tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.04f, 0.04f, 0.f);
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
		tEffectMeshDesc.vStartScale *= 0.7f;
		tEffectMeshDesc.vMaxScale *= 0.7f;
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
		tEffectMeshDesc.vStartScale *= 2.f;
		tEffectMeshDesc.vMaxScale *= 2.f;
		break;
	}

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPoint);

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Effect_GroundHit_Crown(CTransform* pTransform, _fvector vPoint, _float fRotRange, _uint iAnimIndex)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_CROWN];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	// 크기 설정
	CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)iAnimIndex;
	switch (curr_anim)
	{
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:
		tEffectMeshDesc.vStartScale = __vector(0.005f, 0.005f, 0.005f, 0.f);
		tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.04f, 0.04f, 0.f);
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
		tEffectMeshDesc.vStartScale *= 0.7f;
		tEffectMeshDesc.vMaxScale *= 0.7f;
		break;
	case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
		tEffectMeshDesc.vStartScale *= 2.f;
		tEffectMeshDesc.vMaxScale *= 2.f;
		break;
	}

	// 회전 값 설정
	tEffectMeshDesc.vLookDir = pTransform->Get_Look();
	tEffectMeshDesc.fRotationAngle.y = vecz(tEffectMeshDesc.vLookDir) * -300.f;

	_float f_rand = RandomRange(-fRotRange, fRotRange);
	tEffectMeshDesc.fRotationAngle.y += f_rand;

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	// 위치 설정
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPoint);

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = -1.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentPos = nullptr;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Effect_Swing_HeadStream(CTransform* pTransform, _float4x4* pMatrix, _uint iAnimIndex)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_HEAD_STREAM];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(180.f)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::RIGHT;
	tEffectMeshDesc.fFromTargetDistance = 1.4f;

	//CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)iAnimIndex;
	//switch (curr_anim)
	//{
	//case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_02:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_04:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//case Client::CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK:
	//	tEffectMeshDesc.fLifeTime = ;
	//	break;
	//}

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pMatrix;
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HammerForm_Effect_Swing_HeadAirCut(CTransform * pTransform, _float4x4* pMatrix)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_HEAD_AIRCUT];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pMatrix;
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CGameObject* CEffect_Manager::Create_HammerForm_Effect_Swing_HeadEmbers(CTransform* pTransform, _float4* pPos)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc = m_PatricleDesc[TYPE_HAMMERFORM_EMBERS_PTCL];

	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tParticleDesc.pDead = pTransform->Get_Parent()->Get_DeadPointer();

	tParticleDesc.iPass = 1;

	for (_uint i = 0; i < 2; ++i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return nullptr;
		}
	}

	return CGameInstance::GetInstance()->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"));
}

HRESULT CEffect_Manager::Create_HammerForm_Effect_Swing_AirCut(CTransform * pTransform, _fvector vPoint, _fvector vLookDir)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_HAMMER_AIRCUT];

	// 레벨 설정
	tEffectMeshDesc.iLevelIndex = CGameInstance::GetInstance()->Get_LevelIndex();

	XMStoreFloat4(&tEffectMeshDesc.vPos, vPoint);

	tEffectMeshDesc.LocalMat =
		XMMatrixRotationAxis(_axisX, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.x)) *
		XMMatrixRotationAxis(_axisY, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.y)) *
		XMMatrixRotationAxis(_axisZ, XMConvertToRadians(tEffectMeshDesc.fRotationAngle.z));

	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	tEffectMeshDesc.eFromTarget = CEffect_Mesh::MOVE_DIR::DIR_END;
	tEffectMeshDesc.fFromTargetDistance = 0.f;

	// 포인터 변수 설정
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.pTarget_Transform = pTransform;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentPos = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameObject* effect = CGameInstance::GetInstance()->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"));

	if (nullptr == effect)
	{
		_breakpoint;
		return E_FAIL;
	}

	effect->Get_Transform()->MakeSameDirection(_vec3norm(vLookDir));

	_vector current_right = effect->Get_Transform()->Get_Right();
	effect->Get_Transform()->NowRotation(_vec3norm(current_right), XMConvertToRadians(90.f));

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_FormChange(CTransform* pTransform)
{
	_matrix LocalMatrix;
	_float3	vOffset;				// 기존위치에서 위치 조정을 하고 싶을 시 이 변수 값 변경 (값 조절 필요)

	vOffset = _float3(200.f, 50.f, -20.f);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_SCRATCH];


	tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.04f, 0.04f, 0.f);
	tEffectMeshDesc.fTransparecySpeed -= 1.5f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());

	LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-180.f))  * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);

	tEffectMeshDesc.LocalMat_NoTrace = LocalMatrix;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_RedPearl(_fvector vPosition)
{
	auto Position = vPosition + __vector(0.f, 1.f, 0.f, 0.f);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RED_PEARL];

	XMStoreFloat4(&tEffectMeshDesc.vPos, Position);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_Ripple(_fvector vPosition)
{
	auto Position = vPosition + __vector(0.f, 1.f, 0.f, 0.f);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, Position);
	tEffectMeshDesc.iTextureIndex      = 258;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -1.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.08f, 0.08f, 0.08f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 6.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 0.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = -0.2f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.1f, -1.f, -0.9f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_Marble(_fvector vPosition)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Circle1"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition);
	tEffectMeshDesc.iTextureIndex      = 75;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 2.f;
	tEffectMeshDesc.fTransparecySpeed  = -1.f;
	tEffectMeshDesc.fMaxTransparecy    = 1.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.07f, 0.07f, 0.07f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 16.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.5f, 0.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.1f, -1.f, -1.f, 2.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_Particle(CTransform* pTransform)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc = m_PatricleDesc[TYPE_RAGEFORM_PTCL];

	auto pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y -= -1.21f;

	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.iPass = 2;
	tParticleDesc.fLiveTime = 2.f;


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_Roaring(CTransform* pTransform)
{
	_vector	 vOffset = __vector(0.f, 2.f, 0.f, 0.f);
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tEffectMeshDesc.iTextureIndex      = 38;
	tEffectMeshDesc.iScondTextureIndex = 45;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 1.25f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(0.2f, 0.f);
	tEffectMeshDesc.fLifeTime          = 2.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + vOffset);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -0.3f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 6.75f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.5f, 1.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.4f, -1.f, -1.f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_Guard(CTransform* pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	auto vPosition = pTransform->Get_Position() + vOffset;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	//XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition);
	tEffectMeshDesc.iTextureIndex      = 333;
	tEffectMeshDesc.iScondTextureIndex = 3;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 5.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 1.f;
	tEffectMeshDesc.fTransparecySpeed  = -8.25f;
	tEffectMeshDesc.fMaxTransparecy    = 2.f;
	tEffectMeshDesc.bAlphaReverse      = true;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.08f, 0.08f, 0.08f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 6.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.8f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.1f, -1.f, -1.f, 2.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixScaling(0.6f, 0.6f, 0.6f) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_Guard_Success(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere02"));
	tEffectMeshDesc.iTextureIndex      = 62;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 1.0f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bAlphaReverse      = false;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = -5.f;
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.vSpinAxis          = tEffectMeshDesc.vLookDir;
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 2.25f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, -4.25f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.4f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.1f, -1.f, -2.4f, 0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 60.f, 80.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::INOUTSINE;
	tParticleDesc.bExplosion = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;
	tParticleDesc.bRangeOneOrXYZ = false;
	tParticleDesc.vMakeRangeXYZ = _float3(20.f, 0.f, 20.f);
	tParticleDesc.fMakeRange = 2.f;
	tParticleDesc.fMaxDistance = 5.f;

	_float    fAddColor[4] = { 0.2f, -0.52f, -0.53f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float    fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 1.f;
	tParticleDesc.fMakeRange = 2.f;
	tParticleDesc.fMaxDistance = 5.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 3.f;
	tParticleDesc.fScaleSpeed = 0.4f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fTrailInterval = 0.02f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 338;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fExplosionStartRange = 0.f;
	tParticleDesc.iPass = 2;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	_vector vOffsetPos = XMVector3Normalize(pTransform->Get_Look()) + __vector(0.f, 1.f, 0.f, 0.f);
	pPos->y += vecy(vOffsetPos);
	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPos;


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_DoubleTap(_float4* pPos, _fvector vLookDir)
{
	//  pPos를 매개변수로 받는 effect는 트레이싱할 뼈 위치 포인터를 받음
	//  클래스 내부에서 매틱 갱신해줘야함

	auto	vOffset = _float3(0.f, 5.f, 0.f);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_BASEATTACK];

	tEffectMeshDesc.vLookDir           = XMVector3Normalize(vLookDir);
	tEffectMeshDesc.pParentPos		   = pPos;
	tEffectMeshDesc.vStartScale        = __vector(0.4f, 0.4f, 0.4f, 0.f);
	tEffectMeshDesc.fTransparecySpeed -= 1.2f;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-30.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(60.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);

	tEffectMeshDesc.fLifeTime = 0.35f;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_TripleTap(_float4* pPos, _fvector vLookDir)
{
	auto	vOffset = _float3(0.f, 2.f, 0.f);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_BASEATTACK];

	tEffectMeshDesc.vLookDir           = XMVector3Normalize(vLookDir);
	tEffectMeshDesc.pParentPos         = pPos;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.5f, 0.f);
	tEffectMeshDesc.fTransparecySpeed -= 2.2f;
	tEffectMeshDesc.vStartScale        = __vector(0.7f, 0.7f, 0.7f, 0.f);
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_RageAttack1(_float4* pPos, _fvector vLookDir, _uint iSequence)
{
	_matrix LocalMatrix;
	_float3	vOffset;				// 기존위치에서 위치 조정을 하고 싶을 시 이 변수 값 변경


	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_BASEATTACK];

	tEffectMeshDesc.vLookDir           = XMVector3Normalize(vLookDir);
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.fTransparecySpeed -= 1.2f;
	tEffectMeshDesc.pParentPos         = pPos;
	tEffectMeshDesc.vStartScale        = __vector(0.4f, 0.4f, 0.4f, 0.f);

	switch (iSequence)
	{
	case 0:
		vOffset = _float3(0.f, 2.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;
	case 1:
		vOffset = _float3(0.f, 1.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(10.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;
	case 2:
		vOffset = _float3(0.f, 1.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(70.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;
	}

	tEffectMeshDesc.LocalMat_NoTrace = LocalMatrix;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_RageAttack2(_float4* pPos, _fvector vLookDir, _uint iSequence)
{
	_matrix LocalMatrix;
	_float3	vOffset;				// 기존위치에서 위치 조정을 하고 싶을 시 이 변수 값 변경 (값 조절 필요)


	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_BASEATTACK];

	tEffectMeshDesc.vLookDir = XMVector3Normalize(vLookDir);
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.fTransparecySpeed -= 1.2f;
	tEffectMeshDesc.pParentPos = pPos;
	tEffectMeshDesc.vStartScale = __vector(0.4f, 0.4f, 0.4f, 0.f);

	switch (iSequence)
	{
	case 0:
		vOffset = _float3(0.f, 2.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;
	case 1:
		vOffset = _float3(0.f, 5.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;
	case 2:
		vOffset = _float3(0.f, 1.f, 0.f);
		tEffectMeshDesc. vStartScale = __vector(0.6f, 0.6f, 0.6f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(100.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;
	}

	tEffectMeshDesc.LocalMat_NoTrace = LocalMatrix;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_RageAttack3(_float4* pPos, _fvector vLookDir)
{
	auto	vOffset = _float3(0.f, 5.f, 0.f);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 파장 이펙트
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring05"));
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(vLookDir);
	tEffectMeshDesc.pParentPos         = pPos;
	tEffectMeshDesc.iTextureIndex      = 271;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -0.25f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.7f, 0.7f, 0.7f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 15.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.2f, -0.7f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.5f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = false;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.9f, -1.f, -1.f, -0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 슬래시 이펙트
	tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_BASEATTACK];

	tEffectMeshDesc.vLookDir           = XMVector3Normalize(vLookDir);
	tEffectMeshDesc.pParentPos         = pPos;
	tEffectMeshDesc.vStartScale        = __vector(0.6f, 0.6f, 0.6f, 0.f);
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.f, 0.f);
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_RageAttack4(_matrix WorldMatrix)
{
	// WorldMatrix는 뼈 월드 메트릭스를 넣어주면 됨

	_matrix LocalMatrix;
	_float3	vOffset;				// 기존위치에서 위치 조정을 하고 싶을 시 이 변수 값 변경 (값 조절 필요)

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_SCRATCH];

	XMStoreFloat4(&tEffectMeshDesc.vPos,WorldMatrix.r[3]);
	tEffectMeshDesc.vLookDir = XMVector3Normalize(WorldMatrix.r[2]);
	tEffectMeshDesc.fTransparecySpeed -= 0.8f;

	tEffectMeshDesc.LocalMat_NoTrace *= XMMatrixTranslation(0.f, 0.f, 70.f) * WorldMatrix;
	tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.04f, 0.04f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_RageAttack5(_float4* pPos, _fvector vLookDir, _uint iSequence)
{
	// LookDir은 플레이어 LookDir 넣어주면 됨
	// 매번 생성시 방향이 다르기때문에 iSequence는 0부터 순차적으로 증가 시켜줘서 넣어주면 됨

	_matrix LocalMatrix;
	_float3	vOffset;


	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_BASEATTACK];

	tEffectMeshDesc.vLookDir = XMVector3Normalize(vLookDir);
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.fTransparecySpeed -= 1.2f;
	tEffectMeshDesc.pParentPos = pPos;
	tEffectMeshDesc.vStartScale = __vector(0.4f, 0.4f, 0.4f, 0.f);

	switch (iSequence)
	{
	case 0:
		vOffset = _float3(0.f, 2.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(45.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;

	case 1:
		vOffset = _float3(0.f, 2.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(20.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;

	case 2:
		vOffset = _float3(0.f, 2.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(20.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;

	case 3:
		vOffset = _float3(0.f, 2.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(60.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;

	case 4:
		vOffset = _float3(0.f, 2.f, 0.f);
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(vOffset.x, vOffset.y, vOffset.z);
		break;
	}

	tEffectMeshDesc.LocalMat_NoTrace = LocalMatrix;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_RageAttack5_Finish(CTransform* pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack12"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + vOffset);
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 75;
	tEffectMeshDesc.iScondTextureIndex = 58;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 2.f;
	tEffectMeshDesc.fTransparecySpeed  = -3.75f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.001f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.02f, 0.02f, 0.02f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 15.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.5f, -0.75f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.4f, -1.f, -1.f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_FinishAttack_Start(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Big Tornado
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Big00"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 176;
	tEffectMeshDesc.iScondTextureIndex = 58;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 2.25f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -0.5f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.02f, 0.02f, 0.02f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 15.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 2.25f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = -1.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-1.f, -1.f, -1.f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Ribbon
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small07"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 207;
	tEffectMeshDesc.iScondTextureIndex = 58;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -0.35f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.03f, 0.02f, 0.03f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.02f, 0.02f, 0.02f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 15.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.75f, 0.f);
	tEffectMeshDesc.fStartU            = -1.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.1f, -2.9f, -1.4f, -0.4f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Ripple
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 258;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -0.5f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.25f, 1.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.1f, -1.f, -1.f, -0.2f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_FinishAttack(CTransform* pTransform, _fvector vOffset)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Tornado =========================================================================
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado13"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 260;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -1.25f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.04f, 0.04f, 0.04f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 2.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(-3.75f, 0.f);
	tEffectMeshDesc.fStartU            = 1.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.6f, -0.7f, -0.2f, 0.9f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Dia =============================================================================
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Item00"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.iTextureIndex = 62;
	tEffectMeshDesc.iScondTextureIndex = 5;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower = 0.25f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(0.f, 0.75f);
	tEffectMeshDesc.fLifeTime = 3.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.3f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = true;
	tEffectMeshDesc.fSpinningSpeed = 3.75f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_ONEDIR;
	tEffectMeshDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.fMovingDistance = 9.f;
	tEffectMeshDesc.fMovingSpeed = 5.f;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTELASTIC;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.5f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = false;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-3.75f, 0.f);
	tEffectMeshDesc.fStartU = 1.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.5f, -1.f, -0.5f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Ribbon ==========================================================================
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small06"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.iTextureIndex = 259;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 3.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -1.25f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 3.75f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.03f, 0.02f, 0.03f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 5.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(1.75f, 0.f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-1.f, -1.f, -1.f, 1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Marble =========================================================================
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Circle1"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + vOffset);
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iTextureIndex = 75;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 3.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 2.f;
	tEffectMeshDesc.fTransparecySpeed = -2.f;
	tEffectMeshDesc.fMaxTransparecy = 1.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.15f, 0.15f, 0.15f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.25f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.5f, 0.5f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.1f, -1.f, -1.f, 2.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_FinishAttack_Wave(CTransform* pTransform)
{
	// FinishAttack()이랑 같이 호출함 
	// 파동같은 느낌으로 만듬 -> 0.1f 간격으로 FinishAttack()이펙트가 끝날때까지 반복 생성시켜주면 좋을거 같음

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 265;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -1.25f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.2f, 0.2f, 0.2f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 2.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.6f, 0.25f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.1f, -1.f, -1.f, 0.4f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_CounterAttack_Weapon(_float4x4* pTargetWorld)
{
	// 수치 조정 필요
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_RAGEFORM_BASEATTACK];

	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vStartScale = __vector(0.4f, 0.4f, 0.4f, 0.f);
	tEffectMeshDesc.fTransparecySpeed -= 0.2f;
	tEffectMeshDesc.pTargetWorldMatrix = pTargetWorld;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_RageForm_Effect_Attack_Texture(_fvector vPoint)
{
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;
	ZeroMemory(&tEffectTexDesc, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	tEffectTexDesc = m_TextureDesc[TEXTURE_RAGEFORM_ATTACK_SPARKLE];
	tEffectTexDesc.vPosition = _float3(vecx(vPoint), vecy(vPoint), vecz(vPoint));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tEffectTexDesc = m_TextureDesc[TEXTURE_RAGEFORM_ATTACK_CIRCLE];
	tEffectTexDesc.vPosition = _float3(vecx(vPoint), vecy(vPoint), vecz(vPoint));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Summon(CTransform * pTransform)
{
	if (nullptr == pTransform)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small00"));
	tEffectMeshDesc.iTextureIndex = 46;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.pParentPos = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.035f, 0.035f, 0.035f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(1.f, 1.25f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(2.5f, 1.f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.1f, 0.6f, 0.3f, -0.3f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, -27.f, 10.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, -30.f, 0.f);
	CGameObject*	pEffect = CGameInstance::GetInstance()->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);
	//pEffect->Get_Transform()->Rotation(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	return S_OK;
}

HRESULT CEffect_Manager::Create_DonaldBye(_fvector vPosition)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Cyclinder00"));
	tEffectMeshDesc.iTextureIndex = 10;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.5f;
	tEffectMeshDesc.vPos = XMStoreFloat4RT(vPosition + __vector(0.f, 0.001f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTCIRC;
	tEffectMeshDesc.vStartScale = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(4.f, 3.f, 4.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 4.25f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.5f, 0.2f, 1.f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_GoofyBye(_fvector vPosition)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Cyclinder00"));
	tEffectMeshDesc.iTextureIndex = 10;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.5f;
	tEffectMeshDesc.vPos = XMStoreFloat4RT(vPosition + __vector(0.f, 0.001f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTCIRC;
	tEffectMeshDesc.vStartScale = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(4.f, 3.f, 4.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 4.25f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(1.f, 0.3f, 0.f, 0.3f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_GoofyHitEffect(_fvector vPosition, _fvector vLook)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sticker01"));
	tEffectMeshDesc.iTextureIndex = 10;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.vPos = XMStoreFloat4RT(vPosition);
	tEffectMeshDesc.vLookDir = vLook;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -3.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTCIRC;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 5.25f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = false;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 4.25f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = false;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(1.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);

	(CGameInstance::GetInstance()->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))->Get_Transform()->MakeSameDirection(vLook);


	return S_OK;
}

HRESULT CEffect_Manager::Create_HitStarDonald(CTransform * pTransform, _float fOffsetY)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Mesh::EFFECT_MESHDESC tDesc = m_MeshDesc[TYPE_MOVEONE_SPIN];
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Star07"));
	tDesc.iLevelIndex = pGameInstance->Get_LevelIndex();
	_vector vTargetPos = pTransform->Get_Position() + __vector(0.f, fOffsetY, 0.f, 0.f);
	XMStoreFloat4(&tDesc.vPos, vTargetPos);
	tDesc.fRotationAngle = _float3(0.f, 90.f, 0.f);
	tDesc.fScaleSpeed = 15.f;
	tDesc.fLifeTime = 0.3f;
	tDesc.vLookDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	tDesc.vSpinAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(-0.6f, -0.1f, 0.5f, 0.f);
	tDesc.iTextureIndex = 345;
	tDesc.bBillboarding = true;
	tDesc.LocalMat = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	_uint iNum = rand() % 3 + 7;

	for (_uint i = 0; i < iNum; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 1.f, 1.f, 1.f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 0.24f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 1.5f;
	tParticleDesc.fMakeRange = 0.f;
	tParticleDesc.fMaxDistance = 20.f;
	tParticleDesc.fMinSpeed = 10.f;
	tParticleDesc.fMaxSpeed = 20.f;
	tParticleDesc.fScaleSpeed = 1.46f;
	tParticleDesc.fOriginScale = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 349;
	tParticleDesc.iPass = 2;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eEasing = CEffect_Particle::OUTCUBIC;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(pGameInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_HitStarGoofy(CTransform * pTransform, _float fOffsetY)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CEffect_Mesh::EFFECT_MESHDESC tDesc = m_MeshDesc[TYPE_MOVEONE_SPIN];
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Star07"));
	tDesc.iLevelIndex = pGameInstance->Get_LevelIndex();
	_vector vTargetPos = pTransform->Get_Position() + __vector(0.f, fOffsetY, 0.f, 0.f);
	XMStoreFloat4(&tDesc.vPos, vTargetPos);
	tDesc.fRotationAngle = _float3(0.f, 90.f, 0.f);
	tDesc.fScaleSpeed = 15.f;
	tDesc.fLifeTime = 0.3f;
	tDesc.vLookDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
	tDesc.vSpinAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(-0.6f, 0.9f, 0.2f, 0.f);
	tDesc.iTextureIndex = 345;
	tDesc.bBillboarding = true;
	tDesc.LocalMat = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	_uint iNum = rand() % 3 + 7;

	for (_uint i = 0; i < iNum; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 1.f, 1.f, 1.f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 0.24f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 1.5f;
	tParticleDesc.fMakeRange = 0.f;
	tParticleDesc.fMaxDistance = 20.f;
	tParticleDesc.fMinSpeed = 10.f;
	tParticleDesc.fMaxSpeed = 20.f;
	tParticleDesc.fScaleSpeed = 1.46f;
	tParticleDesc.fOriginScale = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 349;
	tParticleDesc.iPass = 2;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eEasing = CEffect_Particle::OUTCUBIC;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(pGameInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Scratch(CTransform* pTransform, _uint iSequence)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	if (iSequence != 3)
	{
		lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash02"));

		tEffectMeshDesc.iTextureIndex = 68;
		tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
		tEffectMeshDesc.fLifeTime = 1.0f;
		tEffectMeshDesc.pParentPos = nullptr;
		tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
		tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
		tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
		tEffectMeshDesc.iPassIndex = 0;
		tEffectMeshDesc.fTransparency = 0.f;
		tEffectMeshDesc.fTransparecySpeed = 0.f;
		tEffectMeshDesc.fMaxTransparecy = 0.f;
		tEffectMeshDesc.bSpinning = false;
		tEffectMeshDesc.fSpinningSpeed = 0.f;
		tEffectMeshDesc.vSpinAxis = __vector(0.f, -1.f, 0.f, 0.f);
		tEffectMeshDesc.bBillboarding = false;
		tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
		tEffectMeshDesc.bUseEasing_Scaling = false;
		tEffectMeshDesc.vStartScale = __vector(1.f, 1.f, 1.f, 0.f);
		tEffectMeshDesc.vMaxScale = __vector(1.f, 1.f, 1.f, 0.f);
		tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
		tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
		tEffectMeshDesc.bMovingUV = true;

		_float fUVMovingX = 9.25f;

		if (iSequence == 1)
			fUVMovingX = -9.25f;

		tEffectMeshDesc.vUVMovingSpeed = _float2(fUVMovingX, 0.f);
		tEffectMeshDesc.bUsingEmissive = false;
		tEffectMeshDesc.bUsingBlur = true;
		tEffectMeshDesc.bUsingDistortion = false;
		tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
		tEffectMeshDesc.vColor = _float4(0.4f, -0.2f, 1.0f, -0.5f);
		tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
		tEffectMeshDesc.pTarget_Transform = nullptr;

		_float fRotation = -20.f;
		if (iSequence == 1)
			fRotation = 20.f;

		_float3 vScale = _float3(0.2f, 0.05f, 0.2f);

		if (iSequence == 2)
			vScale = _float3(0.3f, 0.05f, 0.3f);

		tEffectMeshDesc.LocalMat = XMMatrixScaling(vScale.x, vScale.y, vScale.z) * XMMatrixRotationZ(XMConvertToRadians(fRotation)) * XMMatrixTranslation(0.f, 2.5f, 2.5f);

		if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		tEffectMeshDesc.LocalMat = XMMatrixScaling(vScale.x, vScale.y, vScale.z) * XMMatrixRotationZ(XMConvertToRadians(fRotation)) * XMMatrixTranslation(0.f, 3.5f, 2.5f);

		if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		tEffectMeshDesc.LocalMat = XMMatrixScaling(vScale.x, vScale.y, vScale.z) * XMMatrixRotationZ(XMConvertToRadians(fRotation)) * XMMatrixTranslation(0.f, 1.5f, 2.5f);

		if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
		{
			BREAKPOINT;
			return E_FAIL;
		}


	}
	
	else
	{
		_vector		vOffSetY = __vector(0.f, -6.f, 0.f, 0.f);

		auto		vPosition = pTransform->Get_Position();

		CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

		lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
		tEffectMeshDesc.iTextureIndex = 143;
		tEffectMeshDesc.iScondTextureIndex = 33;
		tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
		tEffectMeshDesc.fNoisePower = 0.f;
		tEffectMeshDesc.vNoiseMovingSpeed = _float2(-1.f, 0.f);
		tEffectMeshDesc.fLifeTime = 2.f;
		tEffectMeshDesc.pParentPos = nullptr;
		XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition);
		tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
		tEffectMeshDesc.iPassIndex = 0;
		tEffectMeshDesc.fTransparency = 0.08f;
		tEffectMeshDesc.fTransparecySpeed = 0.f;
		tEffectMeshDesc.fMaxTransparecy = 0.f;
		tEffectMeshDesc.bSpinning = false;
		tEffectMeshDesc.fSpinningSpeed = 0.f;
		tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
		tEffectMeshDesc.bBillboarding = false;
		tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
		tEffectMeshDesc.vStartScale = __vector(0.2f, 0.2f, 0.2f, 0.f);
		tEffectMeshDesc.vMaxScale = __vector(0.06f, 0.06f, 0.06f, 0.f);
		tEffectMeshDesc.fScaleSpeed = 2.f;
		tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
		tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
		tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
		tEffectMeshDesc.pTargetWorldMatrix = nullptr;
		tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
		tEffectMeshDesc.bMovingUV = true;
		tEffectMeshDesc.vUVMovingSpeed = _float2(-4.25f, 0.f);
		tEffectMeshDesc.fStartU = 0.f;
		tEffectMeshDesc.fStartV = 0.f;
		tEffectMeshDesc.bUsingEmissive = false;
		tEffectMeshDesc.bUsingBlur = true;
		tEffectMeshDesc.bUsingDistortion = false;
		tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
		tEffectMeshDesc.vColor = _float4(-0.2f, -0.2f, -0.1f, -0.5f);
		tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
		tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(-0.5f, 6.75f, 75.f);

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition + vOffSetY);
		tEffectMeshDesc.vUVMovingSpeed = _float2(4.25f, 0.f);

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_SmokeWave(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.iTextureIndex = 121;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -1.25f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.4f, 0.4f, 0.4f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 5.5f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-3.25f, 0.5f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.5f, -0.3f, 1.f, 0.4f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Fire(CTransform* pTransform, _bool* pDead, _float fOffsetY)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	auto vPosition = pTransform->Get_Position();

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	//XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition);
	tEffectMeshDesc.iTextureIndex = 86;
	tEffectMeshDesc.iScondTextureIndex = 3;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 10.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.pParentDead = pDead;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bAlphaReverse = false;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.08f, 0.08f, 0.08f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 6.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-1.f, 2.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.3f, -1.f, 0.f, 1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Wind
	auto vOffset = __vector(0.f, 0.f, 0.2f, 0.f);

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere02"));
	tEffectMeshDesc.iTextureIndex = 113;
	tEffectMeshDesc.iScondTextureIndex = 3;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 100.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.pParentDead = pDead;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 2.f;
	tEffectMeshDesc.bAlphaReverse = false;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.07f, 0.07f, 0.07f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.08f, 0.08f, 0.08f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 6.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(5.25f, -2.25f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = 1.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.f, -0.4f, -0.3f, -0.5f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixTranslation(vecx(vOffset), vecy(vOffset), vecz(vOffset));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Fire_Dead(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Texture::EFFECT_TEX_DESC      tDesc_Texture;

	// Smoke
	auto vPosition = pTransform->Get_Position();

	//ZeroMemory(&tDesc_Texture, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	tDesc_Texture.iPassIndex = 5;
	tDesc_Texture.iTextureIndex = 338;
	tDesc_Texture.iLevelIndex = LEVEL_STATIC;

	tDesc_Texture.fPlaySpeed = 4.f;
	tDesc_Texture.bBillboarding = true;

	XMStoreFloat3(&tDesc_Texture.vPosition, vPosition);

	tDesc_Texture.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Texture.eLifeType = CEffect_Texture::TIME_LIMIT;

	tDesc_Texture.bSpinning = false;

	tDesc_Texture.fSpinSpeed = 2.5f;

	tDesc_Texture.vMinSize = _float3(4.f, 4.f, 4.f);
	tDesc_Texture.vMaxSize = _float3(6.f, 6.f, 6.f);
	tDesc_Texture.fScalingSpeed = 5.5f;
	tDesc_Texture.eScalePattern = CEffect_Texture::PATTERN_SCALE_NONE;
 
	tDesc_Texture.vBaseColor = _float4(0.14f, 0.f, 0.24f, 1.f);
	tDesc_Texture.vBlendingColor = _float4(0.14f, 0.f, 0.24f, 1.f);

	tDesc_Texture.fStartAlpha = 0.f;
	tDesc_Texture.fTransparecySpeed = 1.5f;
	tDesc_Texture.fTotalTime = 1.f;

	tDesc_Texture.bBlur = false;
	tDesc_Texture.bDistortion = false;

	if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex(), m_pLayerTag, m_pTextureProtoTag, &tDesc_Texture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 0.8f, 0.f, 1.0f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 0.82f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 1.f;
	tParticleDesc.fMakeRange = 1.f;
	tParticleDesc.fMaxDistance = 1.f;
	tParticleDesc.fMaxDistance = 1.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 2.f;
	tParticleDesc.fScaleSpeed = 0.19f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 353;
	tParticleDesc.iPass = 2;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	tParticleDesc.pParentPosition = pPos;
	pPos->y = pPos->y + 1.f;

	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_SoaringRipple(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere03"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.iTextureIndex = 41;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -2.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale = __vector(0.2f, 0.2f, 0.2f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.85f, 1.1f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.8f, 0.8f, 1.f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_SpinAttackRing(_float4x4* pWorld, _bool* pDead)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex = 86;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.pParentDead = pDead;
	tEffectMeshDesc.fLifeTime = 100.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pWorld;
	tEffectMeshDesc.vStartScale = __vector(0.4f, 0.3f, 0.4f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-5.25f, 0.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.4f, -0.5f, 0.1f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tEffectMeshDesc.vStartScale = __vector(0.2f, 0.1f, 0.2f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, -2.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, -10.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tEffectMeshDesc.vStartScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, -4.f, 0.f);
	//tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, -20.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_SoaringRibbon(_float4x4* pTargetWorld)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small08"));
	tEffectMeshDesc.iTextureIndex = 50;
	tEffectMeshDesc.iScondTextureIndex = 33;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -2.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.vPos = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed = 2.f;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTargetWorld;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-3.75f, 1.1f);
	tEffectMeshDesc.fStartU = 1.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.1f, -0.2f, 1.0f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, -100.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return S_OK;
	}

	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, -100.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return S_OK;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Marble()
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Circle1")); 
	XMStoreFloat4(&tEffectMeshDesc.vPos, __vector(0.f, 50.f, 0.f, 1.f));
	tEffectMeshDesc.iTextureIndex = 28;
	tEffectMeshDesc.iScondTextureIndex = 48;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime = 20.f;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 9.f;
	tEffectMeshDesc.fTransparecySpeed = -0.55f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(3.f, 3.f, 3.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.f;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.05f, 0.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.1f, -0.8f, 0.1f, 1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace =XMMatrixRotationAxis(__vector(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Wave
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, __vector(0.f, 50.f, 0.f, 1.f));
	tEffectMeshDesc.iTextureIndex = 255;
	tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime = 3.75f;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -0.9f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.25f;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(2.25f, 1.25f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = -1.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.f, -0.4f, 1.2f, -0.3f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Rain(CTransform* pTransform, _bool* pDead)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Skill04"));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex = 41;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime = 10.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.pParentDead = pDead;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vStartScale = __vector(0.002f, 0.01f, 0.002f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 3.f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = false;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-0.5f, 0.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(1.f, 0.4f, 1.1f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.2f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat = XMMatrixRotationZ(XMConvertToRadians(180.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Rain_Splash(CTransform* pTransform)
{
	// Splash
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;

	_uint  iTexIndex = 347;

	tEffectTexDesc.iTextureIndex = iTexIndex;
	tEffectTexDesc.iPassIndex = 0;
	tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
	tEffectTexDesc.fTotalTime = 3.f;
	tEffectTexDesc.bBillboarding = true;
	tEffectTexDesc.bBlur = true;
	tEffectTexDesc.bSprite = true;
	tEffectTexDesc.fSpriteMaxXIndex = 8;
	tEffectTexDesc.fSpriteMaxYIndex = 8;
	tEffectTexDesc.fSpriteSpeed = 11.f;
	tEffectTexDesc.ePlayType = CEffect_Texture::PLAY_ONCE;
	tEffectTexDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectTexDesc.eLifeType = CEffect_Texture::SPRITE_LIMIT;
	tEffectTexDesc.bSpinning = false;
	tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_SCALE_NONE;
	tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE_NONE;
	tEffectTexDesc.eEasing = CEffect_Texture::OUTCIRC;
	tEffectTexDesc.bUseEasing = false;
	tEffectTexDesc.vMinSize = _float3(15.f, 15.f, 15.f);
	tEffectTexDesc.fStartAlpha = -0.5f;
	tEffectTexDesc.vMaxSize = 10.f;
	tEffectTexDesc.vBaseColor = _float4(0.f, -0.3f, 0.7f, 0.f);
	//tEffectTexDesc.vBlendingColor = _float4(0.f, 0.f, 0.f, 0.f);

	_vector vPos = pTransform->Get_Position();
	tEffectTexDesc.vPosition = XMStoreFloat3RT(vPos);

	if(tEffectTexDesc.vPosition.y < 0.f)
		tEffectTexDesc.vPosition.y = 0.f;
	
	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Rock(_fvector vPos)
{
	// Splash
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;

	_uint  iTexIndex = 375;

	tEffectTexDesc.iTextureIndex = iTexIndex;
	tEffectTexDesc.iPassIndex = 0;
	tEffectTexDesc.iLevelIndex = LEVEL_STATIC;
	tEffectTexDesc.fTotalTime = 2.f;
	tEffectTexDesc.bBillboarding = true; 
	tEffectTexDesc.bBlur = false;
	tEffectTexDesc.bSprite = true;
	tEffectTexDesc.fSpriteMaxXIndex = 8;
	tEffectTexDesc.fSpriteMaxYIndex = 8;
	tEffectTexDesc.fSpriteSpeed = 30.f;
	tEffectTexDesc.ePlayType = CEffect_Texture::PLAY_LOOP;
	tEffectTexDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectTexDesc.eLifeType = CEffect_Texture::TIME_LIMIT;
	tEffectTexDesc.bSpinning = false;
	tEffectTexDesc.eScalePattern = CEffect_Texture::PATTERN_BULGE;
	tEffectTexDesc.eMovingPattern = CEffect_Texture::PATTERN_MOVE_ONEDIR;
	tEffectTexDesc.vMovingDir = _float3(0.f, 1.f, 0.f);
	tEffectTexDesc.eEasing = CEffect_Texture::OUTCIRC;
	tEffectTexDesc.bUseEasing = false;
	tEffectTexDesc.vMinSize = _float3(2.f, 2.f, 2.f);
	tEffectTexDesc.vMaxSize = _float3(0.1f, 0.1f, 0.1f);
	tEffectTexDesc.fScalingSpeed = 3.f;
	tEffectTexDesc.fStartAlpha = -0.1f;
	tEffectTexDesc.vBaseColor = _float4(-0.6f, -0.4f, 0.f, 0.f);
	tEffectTexDesc.fPlaySpeed = 2.f;

	_vector vPosition = vPos;
	tEffectTexDesc.vPosition = XMStoreFloat3RT(vPos);
	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_Ptcl(CTransform* pTransform, _fvector vOffset)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;
	tParticleDesc.bRangeOneOrXYZ = true;
	tParticleDesc.vMakeRangeXYZ = _float3(20.f, 0.f, 20.f);


	_float    fAddColor[4] = { 1.f, 0.2f, 0.8f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float    fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.fMaxDistance = 0.f;
	tParticleDesc.fMinSpeed = 20.f;
	tParticleDesc.fMaxSpeed = 30.f;
	tParticleDesc.fScaleSpeed = 0.53f;
	tParticleDesc.fOriginScale = 0.3f;
	tParticleDesc.fTrailInterval = 0.02f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 352;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::INSINE;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	_vector vOffsetPos = vOffset;
	pPos->y += vecy(vOffsetPos);
	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPos;


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_SpawnEffect(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Marble
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Star06"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 0.5f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex = 150;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.5f, 0.5f, 0.5f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 8.f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-1.25f, 2.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.f, -0.2f, 0.4f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationZ(XMConvertToRadians(90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Ring
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex = 87;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.fTransparecySpeed = -2.25f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale = __vector(0.4f, 0.2f, 0.4f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.5f, 0.5f, 0.5f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 8.f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-4.25f, 0.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.4f, -0.1f, 0.8f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Ribbon
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado00"));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex = 127;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(1.0f, 1.0f, 1.0f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 5.25f;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(2.f, -2.75f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(1.f, 0.7f, 1.6f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Particle
	Create_Wolf_Ptcl(pTransform, __vector(0.f, 20.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_LandingEffect(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Marble
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere03"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 0.5f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex = 151;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.fTransparecySpeed = -1.25f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.005f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.3f, 0.08f, 0.3f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 8.f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::INOUTELASTIC;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-0.5f, -2.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.1f, -0.3f, 0.2f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_TailWind(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado03"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iTextureIndex = 27;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.pParentDead = nullptr;
	tEffectMeshDesc.fTransparecySpeed = -0.2f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 4.5f;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::INOUTELASTIC;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-3.75f, -0.75f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.3f, 0.f, 1.f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Create_Wolf_Ptcl(pTransform, __vector(0.f, 2.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CEffect_Manager::Create_Wolf_BlowingParticle(_float4* pPos, _fvector vOffset)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ = false;
	tParticleDesc.fMakeRange = 3.f;
	tParticleDesc.vMakeRangeXYZ = _float3(2.f, 2.f, 2.f);


	_float	fAddColor[4] = { 0.46f, 0.37f, 1.0f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.fMaxDistance = 0.f;
	tParticleDesc.fMinSpeed = 5.f;
	tParticleDesc.fMaxSpeed = 10.f;
	tParticleDesc.fScaleSpeed = 0.09f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 342;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing = CEffect_Particle::OUTEXPO;

	//_vector v
	//_vector vPosition = vPos;
	//_float4* pPos;
	//XMStoreFloat4(pPos, vPos);
	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPos;


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_AppearAction(_fvector vPosition, MONSTER_TYPE eType)
{
	CEffect_Mesh::EFFECT_MESHDESC		tDesc = m_MeshDesc[TYPE_APPEAR];
	//ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	//tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado05"));

	_vector		vReviseValue, vStartScale;
	_float4		vColor;

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	switch (eType)
	{
	case SKULL:
		vReviseValue = XMVectorSet(0.f, -0.5f, 0.f, 0.f);
		vStartScale  = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
		vColor       = _float4(0.2f, -0.3f, -0.5f, 1.5f);
		break;
	case CROW:
		vReviseValue = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		vStartScale  = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
		vColor       = _float4(0.1f, -0.4f, 0.4f, 0.9f);
		break;
	case THIN:
		vReviseValue = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		vStartScale  = XMVectorSet(0.015f, 0.015f, 0.015f, 0.f);
		vColor       = _float4(0.2f, -0.3f, -0.5f, 1.5f);
		break;

	case SHININGEGG:
		vReviseValue = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		vStartScale  = XMVectorSet(0.006f, 0.006f, 0.005f, 0.f);
		vColor       = _float4(0.8f, 0.6f, -0.5f, 1.5f);
		break;

	case MONKEY:
		vReviseValue = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		vStartScale  = XMVectorSet(0.006f, 0.006f, 0.005f, 0.f);
		vColor       = _float4(1.f, 0.3f, -0.5f, 1.5f);
		break;
	}
	
	XMStoreFloat4(&tDesc.vPos, vPosition + vReviseValue);

	tDesc.vStartScale = vStartScale;

	tDesc.vColor = vColor;

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_AttackSlash(_float4x4*	pTargetWorld, _bool* pDead)
{
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Slash02"));
	tDesc.iTextureIndex = 42;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex= 4;

	tDesc.fLifeTime = 1.5f;

	tDesc.pTargetWorldMatrix = pTargetWorld;

	tDesc.fFromTargetDistance = 1.f;
	tDesc.pParentDead = pDead;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT; 
	
	tDesc.vStartScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	
	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(-4.5f, 1.75f);
	
	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.1f, -1.0f, -0.5f, 0.4f); 

	tDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tDesc.LocalMat = XMMatrixTranslation(0.f, 20.f, 20.f);

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_AttackSlash_Short(_float4x4 * pTargetWorld, _bool* pDead)
{
	CEffect_Mesh::EFFECT_MESHDESC tDesc;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Slash02"));
	tDesc.iTextureIndex       = 42;
	tDesc.ePattern_MixTex     = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex  = 4;
	tDesc.fLifeTime           = 1.5f;
	tDesc.pTargetWorldMatrix  = pTargetWorld;
	tDesc.fFromTargetDistance = 1.f;
	tDesc.eLifeType           = CEffect_Mesh::TIME_LIMIT;
	tDesc.vStartScale         = XMVectorSet(0.02f, 0.02f, 0.02f, 0.f);
	tDesc.ePattern_Scale      = CEffect_Mesh::PATTERN_SCALE_NONE;
	tDesc.ePattern_Move       = CEffect_Mesh::PATTERN_TRACING;
	tDesc.eRenderGroup        = CRenderer::RENDER_BLEND;
	tDesc.ePlayType           = CEffect_Mesh::PLAY_ONCE;
	tDesc.pParentDead         = pDead;
	tDesc.bMovingUV           = true;
	tDesc.vUVMovingSpeed      = _float2(-4.5f, 1.75f);
	tDesc.bUsingBlur          = true;
	tDesc.eDyingPattern       = CEffect_Mesh::DYING_ADD;
	tDesc.vColor              = _float4(-1.f, -1.0f, -1.5f, 0.4f);
	tDesc.LocalMat            = XMMatrixTranslation(0.f, 50.f, 50.f);
	tDesc.LocalMat_NoTrace    = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Cloud(_fvector vPosition)
{
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Item02"));
	tDesc.iTextureIndex = 337;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	XMStoreFloat4(&tDesc.vPos, vPosition);

	tDesc.vSpinAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	tDesc.vLookDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	tDesc.fLifeTime = 1.f;
	
	tDesc.bSpinning = true;
	tDesc.fSpinningSpeed = 5.f;
	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.004f, 0.004f, 0.004f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.001f, 0.001f, 0.001f, 0.f);
	tDesc.fScaleSpeed = 0.75f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = false;
	tDesc.vUVMovingSpeed = _float2(0.f, 0.f);

	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	tDesc.vColor = _float4(0.8f, 0.8f, 0.8f, 1.0f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Tornado(_float4x4 * pTargetWorld, _bool* pDead)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;


	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado06"));
	tDesc.iTextureIndex = 78;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex = 2;

	tDesc.fLifeTime = 2.25f;

	tDesc.pTargetWorldMatrix = pTargetWorld;
	tDesc.eFromTarget = CEffect_Mesh::LOOK;
	tDesc.fFromTargetDistance = 0.f;
	tDesc.pParentDead = pDead;
	tDesc.fTransparecySpeed = -0.35f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.008f, 0.008f, 0.008f, 0.f);
	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(2.75f, 0.f);

	tDesc.bUsingBlur = true;


	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(-0.4f, 0.f, -1.f, -0.2f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_MagicGrass(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC      tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Grass00"));
	tDesc.iTextureIndex = 274;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 2;

	tDesc.fLifeTime = 2.5f;

	XMStoreFloat4(&tDesc.vPos, pTransform->Get_Position());

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.bSpinning = true;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tDesc.fSpinningSpeed = 2.25f;
	tDesc.fTransparecySpeed = -0.5f;
	tDesc.vMaxScale = XMVectorSet(0.02f, 0.02f, 0.02f, 0.f);

	tDesc.fScaleSpeed = 0.75f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(0.f, 2.25f);

	tDesc.bUsingBlur = true;

	tDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tDesc.vColor = _float4(-0.5f, -0.4f, -0.7f, 0.f);

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_ShiningEggPatricle(CTransform* pTransform)
{
	// 높이 위치 조절용
	_float fOffsetY = 0.f;
	
	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::OUTSINE;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;
	tParticleDesc.bExplosion = true;
	tParticleDesc.fExplosionStartRange = 0.3f;

	_float	fAddColor[4] = { 0.69f, 1.f, 0.35f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.53f;
	tParticleDesc.fLiveTime = 1.f;
	tParticleDesc.fMakeRange = 0.8f;
	tParticleDesc.fMaxDistance = 0.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 5.f;
	tParticleDesc.fOriginScale = 0.1f;
	tParticleDesc.fScaleSpeed = 0.15f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 343;
	tParticleDesc.iPass = 3;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect_Manager::Create_ShockWave(_float4x4 * pTargetWorld)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ring00"));
	tDesc.iTextureIndex = 28;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 2;

	tDesc.fLifeTime = 1.0f;

	tDesc.pTargetWorldMatrix = pTargetWorld;
	// tDesc.eFromTarget = CEffect_Mesh::LOOK;
	tDesc.fFromTargetDistance = 0.f;

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.001f, 0.001f, 0.001f, 0.f);
	tDesc.vMaxScale  = XMVectorSet(0.006f, 0.006f, 0.006f, 0.f);

	tDesc.fScaleSpeed = 8.5f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(0.f, 1.25f);

	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	tDesc.vColor = _float4(1.0f, 0.5f, -0.3f, 1.5f);

	tDesc.LocalMat = XMMatrixRotationY(XMConvertToRadians(70.f)) * XMMatrixTranslation(-50.f, 100.f, 100.f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Monkey_AttackSlash(_float4x4* pTargetWorld)
{
	CEffect_Mesh::EFFECT_MESHDESC tDesc;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Slash02"));
	tDesc.iTextureIndex       = 42;
	tDesc.ePattern_MixTex     = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex  = 4;
	tDesc.fLifeTime           = 1.5f;
	tDesc.pTargetWorldMatrix  = pTargetWorld;
	tDesc.fFromTargetDistance = 1.f;
	tDesc.eLifeType           = CEffect_Mesh::TIME_LIMIT;
	tDesc.vStartScale         = XMVectorSet(0.02f, 0.02f, 0.02f, 0.f);
	tDesc.ePattern_Scale      = CEffect_Mesh::PATTERN_SCALE_NONE;
	tDesc.ePattern_Move       = CEffect_Mesh::PATTERN_TRACING;
	tDesc.eRenderGroup        = CRenderer::RENDER_BLEND;
	tDesc.ePlayType           = CEffect_Mesh::PLAY_ONCE;
	tDesc.bMovingUV           = true;
	tDesc.vUVMovingSpeed      = _float2(-4.5f, 1.75f);
	tDesc.bUsingBlur          = true;
	tDesc.eDyingPattern       = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.1f, -1.0f, -0.5f, 0.4f);
	tDesc.LocalMat_NoTrace    = XMMatrixIdentity();
	tDesc.LocalMat            = XMMatrixTranslation(0.f, 50.f, 50.f);

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_BigTornado(_fvector vPosition)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC      tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon00"));
	tDesc.iTextureIndex = 49;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 2;

	tDesc.fLifeTime = 0.8f;

	XMStoreFloat4(&tDesc.vPos, vPosition + __vector(0.f, 1.f, 0.f, 0.f));

	tDesc.fTransparecySpeed = -1.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.005f, 0.005f, 0.005f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.02f, 0.02f, 0.02f, 0.f);

	tDesc.fScaleSpeed = 0.75f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(-1.75f, 1.0f);

	tDesc.bUsingBlur = true;

	tDesc.vLookDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	tDesc.eDyingPattern    = CEffect_Mesh::DYING_MULT;
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();
	tDesc.vColor           = _float4(1.3f, 0.6f, 0.f, 0.8f);

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Poison(_float4x4 * pTargetWorld)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tDesc.iTextureIndex = 42;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 2;

	tDesc.fLifeTime = 1.0f;

	tDesc.pTargetWorldMatrix = pTargetWorld;
	tDesc.eFromTarget = CEffect_Mesh::LOOK;
	tDesc.fFromTargetDistance = 0.f;

	tDesc.fTransparecySpeed = -0.1f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.1f, 0.1f, 0.1f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.006f, 0.006f, 0.006f, 0.f);

	tDesc.fScaleSpeed = 0.75f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(2.f, -0.5f);

	tDesc.bUsingBlur = true;


	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	tDesc.vColor = _float4(0.3f, 0.f, 0.5f, 0.8f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_CrowParticle(CTransform* pTransform, _bool* pDead)
{
	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;

	_float	fAddColor[4] = { 0.6f, 0.f, 0.6f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 15.0f;
	tParticleDesc.fMakeRange     = 1.f;
	tParticleDesc.fMaxDistance   = 1.f;
	tParticleDesc.fMinSpeed      = 0.5f;
	tParticleDesc.fMaxSpeed      = 1.f;
	tParticleDesc.fScaleSpeed    = 0.2f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 353;
	tParticleDesc.iPass          = 2;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.pDead          = pDead;

	_float4* pPos                 = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.pParentPosition = pPos;

	for (_uint i = 0; i < 7; ++i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_DashMotion(_float4x4*	pTargetWorld)
{
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon04"));
	tDesc.iTextureIndex = 0;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.fLifeTime = 0.1f;
	//XMStoreFloat4(&tDesc.vPos, vTargetPos + XMVectorSet(0.5f, 0.75f, 0.f, 0.f));

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.04f, 0.04f, 0.04f, 0.f);
	tDesc.fScaleSpeed = 1.5f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	tDesc.pTargetWorldMatrix = pTargetWorld;
	tDesc.eFromTarget = CEffect_Mesh::RIGHT;
	tDesc.fFromTargetDistance = 0.5f;

	tDesc.eRenderGroup = CRenderer::RENDER_NONLIGHT;


	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(-4.5f, 0.f);

	tDesc.bUsingBlur = true;


	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	tDesc.vColor = _float4(0.f, 0.f, 0.f, 0.8f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();


	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tDesc.pTargetWorldMatrix = pTargetWorld;
	tDesc.eFromTarget = CEffect_Mesh::RIGHT;
	tDesc.fFromTargetDistance = -0.5f;
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tDesc.pTargetWorldMatrix = pTargetWorld;
	tDesc.eFromTarget = CEffect_Mesh::UP;
	tDesc.fFromTargetDistance = 1.f;
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect_Manager::Create_Ribbons(_float4x4*	pTargetWorld)
{
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;
	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon03"));
	tDesc.iTextureIndex = 244;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex = 4;

	tDesc.fLifeTime = 2.25f;
	//XMStoreFloat4(&tDesc.vPos, vTargetPos + XMVectorSet(-2.75f, -2.f, -5.f, 0.f));
	//tDesc.vLookDir = vLook;
	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -0.1f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	//tDesc.vMaxScale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.f);
	//tDesc.fScaleSpeed = 1.5f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	
	tDesc.ePattern_Move= CEffect_Mesh::PATTERN_TRACING;
	tDesc.eFromTarget = CEffect_Mesh::LOOK;
	tDesc.pTargetWorldMatrix = pTargetWorld;

	tDesc.fFromTargetDistance = 1.f;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(0.f, 1.5f);

	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_MULT;
	tDesc.vColor = _float4(0.f, 0.f, 0.f, 0.8f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon01"));
	tDesc.iTextureIndex      = 55;
	tDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex = 6;

	tDesc.fLifeTime = 1.0f;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.07f, 0.07f, 0.07f, 0.f);

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tDesc.pTargetWorldMatrix = pTargetWorld;

	tDesc.eFromTarget = CEffect_Mesh::LOOK;

	tDesc.fFromTargetDistance = 1.f;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(0.f, -2.0f);

	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(-0.4f, -1.3f, -1.1f, 1.0f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_DeadEffect(CTransform* pTransform, _float fSize, _float fOffsetY)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Texture::EFFECT_TEX_DESC      tDesc_Texture;
	
	// Smoke
	auto vPosition = pTransform->Get_Position();

	//ZeroMemory(&tDesc_Texture, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	tDesc_Texture.iPassIndex    = 5;
	tDesc_Texture.iTextureIndex = 338;
	tDesc_Texture.iLevelIndex   = LEVEL_STATIC;

	tDesc_Texture.fPlaySpeed = 4.f;
	tDesc_Texture.bBillboarding = true;

	XMStoreFloat3(&tDesc_Texture.vPosition, vPosition + XMVectorSet(0.f, fOffsetY, 0.f, 0.f));

	tDesc_Texture.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Texture.eLifeType = CEffect_Texture::TIME_LIMIT;

	tDesc_Texture.bSpinning = false;

	tDesc_Texture.fSpinSpeed = 2.5f;

	tDesc_Texture.vMinSize = _float3(fSize, fSize, fSize);
	tDesc_Texture.vMaxSize = _float3(6.f, 6.f, 6.f);
	tDesc_Texture.fScalingSpeed = 5.5f;
	tDesc_Texture.eScalePattern = CEffect_Texture::PATTERN_SCALE_NONE;

	tDesc_Texture.vBaseColor     = _float4(0.14f, 0.f, 0.24f, 1.f);
	tDesc_Texture.vBlendingColor = _float4(0.14f, 0.f, 0.24f, 1.f);

	tDesc_Texture.fStartAlpha       = 0.f;
	tDesc_Texture.fTransparecySpeed = 1.5f;
	tDesc_Texture.fTotalTime        = 1.f;

	tDesc_Texture.bBlur = false;
	tDesc_Texture.bDistortion = false;

	if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex() , m_pLayerTag, m_pTextureProtoTag, &tDesc_Texture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 0.8f, 0.f, 1.0f, 0.f};
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 0.82f};
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 1.f;
	tParticleDesc.fMakeRange     = 1.f;
	tParticleDesc.fMaxDistance   = 1.f;
	tParticleDesc.fMaxDistance   = 1.f;
	tParticleDesc.fMinSpeed      = 1.f;
	tParticleDesc.fMaxSpeed      = 2.f;
	tParticleDesc.fScaleSpeed    = 0.19f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 353;
	tParticleDesc.iPass = 2;
	tParticleDesc.pDead = nullptr;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	tParticleDesc.pParentPosition = pPos;
	pPos->y                       = pPos->y + fOffsetY;

	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_SkillDeadEffect(CTransform* pTransform, _float fOffsetY)
{
	// 스파크
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_spark_flash"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, fOffsetY, 0.f, 0.f));
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 380;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.4f, 0.4f, 0.4f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 5.5f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, -1.75f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.8f, 1.4f, 0.6f, -0.7f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 파장(링)
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring05"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, fOffsetY, 0.f, 0.f));
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 25;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.3f, 0.3f, 0.3f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 5.75f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, -2.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.3f, -0.3f, -0.5f, -0.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex       = 381;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ  = false;
	tParticleDesc.bExplosion      = true;
	tParticleDesc.fMakeRange      = 1.5f;
	tParticleDesc.vMakeRangeXYZ   = _float3(2.f, 2.f, 2.f);
	tParticleDesc.fMinSpeed       = 1.f;
	tParticleDesc.fMaxSpeed       = 10.f;
	tParticleDesc.fOriginScale    = 0.1f;
	tParticleDesc.fScaleSpeed     = 0.06f;
	tParticleDesc.fGravityPower   = 0.f;
	tParticleDesc.fLiveTime       = 1.f;
	tParticleDesc.fMaxDistance    = 0.f;
	tParticleDesc.fTrailInterval  = 0.f;
	tParticleDesc.iLevelIndex     = 0;
	tParticleDesc.eLifeType       = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing         = CEffect_Particle::OUTEXPO;


	_float	fAddColor[4] = { 0.5f, 0.27f, -0.38f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);


	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pTransform->Get_PositionFloat4_Pointer();


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_ShineParticle(CTransform* pTransform, _fvector vOffset)
{
	// 작은 이펙트들
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_SHINING_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	auto vPosition = XMLoadFloat4(pPos) + vOffset;

	_float4	vPos;

	XMStoreFloat4(&vPos, vPosition);

	*pPos = vPos;

	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.fMakeRange = 0.f;
	tParticleDesc.fMaxDistance = 10.f;
	tParticleDesc.fMinSpeed = 2.f;
	tParticleDesc.fMaxSpeed = 3.f;
	tParticleDesc.fScaleSpeed = 1.2f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.iPass = 2;
	tParticleDesc.bTrail = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	_float	fAddColor[4] = { 0.3f, 0.3f, 0.3f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	tParticleDesc.vDir = __vector(0.f, 0.f, 0.f, 0.f);


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_OpenBoxEffect(CTransform* pTransform)
{
	_vector				vOffset = __vector(0.f, 1.f, 0.f, 0.f);
	// 리플
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;
	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Sticker09"));
	tDesc.iTextureIndex = 24;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.fLifeTime = 1.0f;
	tDesc.bBillboarding = false;

	tDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	
	XMStoreFloat4(&tDesc.vPos, pTransform->Get_Position() + vOffset);

	tDesc.fTransparecySpeed = -0.1f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);
	tDesc.fScaleSpeed = 7.25f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tDesc.eFromTarget = CEffect_Mesh::DIR_END;
	tDesc.pTargetWorldMatrix = nullptr;

	tDesc.fFromTargetDistance = 1.f;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(0.f, 1.0f);
	tDesc.fStartV = 0.2f;

	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.f, -0.3f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	//// 중간에 번쩍이는 이펙트
	CEffect_Texture::EFFECT_TEX_DESC		tDesc_Texture;
	ZeroMemory(&tDesc_Texture, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	lstrcpy(tDesc_Texture.TextureTag, TEXT("Prototype_Component_Texture_Effect"));
	tDesc_Texture.iPassIndex    = 7;
	tDesc_Texture.iTextureIndex = 361;
	tDesc_Texture.iLevelIndex   = LEVEL_STATIC;
	tDesc_Texture.vLookDir      = _float3(0.f, 0.f, 1.f);
	tDesc_Texture.fPlaySpeed    = 0.f;
	tDesc_Texture.bBillboarding = true;
	XMStoreFloat3(&tDesc_Texture.vPosition, pTransform->Get_Position() + vOffset);

	tDesc_Texture.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Texture.eLifeType = CEffect_Texture::TIME_LIMIT;

	tDesc_Texture.bSpinning  = true;
	tDesc_Texture.fSpinSpeed = 0.05f;

	tDesc_Texture.bUseEasing_Scaling = false;
	tDesc_Texture.eEasing_Scale      = CEffect_Texture::INSINE;

	tDesc_Texture.vMinSize      = _float3(0.1f, 0.1f, 0.1f);
	tDesc_Texture.vMaxSize      = _float3(2.f, 2.f, 2.f);
	tDesc_Texture.fScalingSpeed = 20.f;

	tDesc_Texture.eScalePattern  = CEffect_Texture::PATTERN_DYNAMIC;
	tDesc_Texture.fStartAlpha    = 0.f;
	tDesc_Texture.vBaseColor     = _float4(1.f, 1.f, 1.f, 0.f);
	tDesc_Texture.vBlendingColor = _float4(1.f, 0.8f, 0.3f, 0.f);

	tDesc_Texture.fTransparecySpeed = 0.3f;
	tDesc_Texture.fTotalTime        = 0.7f;

	if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex(), m_pLayerTag, m_pTextureProtoTag, &tDesc_Texture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 작은 이펙트들
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_SHINING_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y += XMVectorGetY(vOffset);

	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.fMakeRange = 0.f;
	tParticleDesc.fMaxDistance = 10.f;
	tParticleDesc.fMinSpeed = 2.f;
	tParticleDesc.fMaxSpeed = 3.f;
	tParticleDesc.fScaleSpeed = 1.2f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.iPass = 2;
	tParticleDesc.bTrail = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	_float	fAddColor[4] = { 0.3f, 0.3f, 0.3f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	tParticleDesc.vDir = __vector(0.f, 0.f, 0.f, 0.f);


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_InBoxEffect(CTransform* pTransform)
{
	_vector				vOffset = __vector(0.f, 1.f, 0.f, 0.f);
	
	// 리본
	CEffect_Mesh::EFFECT_MESHDESC		tDesc;
	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small07"));
	tDesc.iTextureIndex = 220;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.fLifeTime = 2.5f;
	tDesc.bBillboarding = false;

	tDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());

	XMStoreFloat4(&tDesc.vPos, pTransform->Get_Position() + __vector(0.f, 0.f, 0.f, 0.f));

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);
	tDesc.fScaleSpeed = 7.25f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tDesc.eFromTarget = CEffect_Mesh::DIR_END;
	tDesc.pTargetWorldMatrix = nullptr;

	tDesc.fFromTargetDistance = 1.f;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = true;
	tDesc.vUVMovingSpeed = _float2(2.25f, 0.f);
	tDesc.fStartU = -1.f;

	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.f, 0.f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	tDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 별

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Star08"));
	tDesc.iTextureIndex = 345;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	tDesc.fLifeTime = 1.2f;
	tDesc.bBillboarding = false;

	tDesc.bSpinning = true;
	tDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tDesc.fSpinningSpeed = 3.5f;

	tDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());

	XMStoreFloat4(&tDesc.vPos, pTransform->Get_Position() + __vector(0.f, -1.f, 0.f, 0.f));

	tDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_ONEDIR;
	tDesc.fMovingDistance = 3.8f;
	tDesc.fMovingSpeed = 3.8f;

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	tDesc.fScaleSpeed = 1.f;

	tDesc.fTransparecySpeed = 0.f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eEasing_Scale = CEffect_Mesh::INSINE;
	tDesc.bUseEasing_Scaling = true;

	tDesc.eFromTarget = CEffect_Mesh::DIR_END;
	tDesc.pTargetWorldMatrix = nullptr;

	tDesc.fFromTargetDistance = 1.f;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;

	tDesc.bMovingUV = false;
	tDesc.vUVMovingSpeed = _float2(2.25f, 0.f);

	tDesc.bUsingBlur = true;

	tDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc.vColor = _float4(0.f, 0.f, 0.f, 0.f);

	tDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 버블
	CEffect_Texture::EFFECT_TEX_DESC		tDesc_Texture;
	ZeroMemory(&tDesc_Texture, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	lstrcpy(tDesc_Texture.TextureTag, TEXT("Prototype_Component_Texture_Effect"));
	tDesc_Texture.iPassIndex = 7;
	tDesc_Texture.iTextureIndex = 363;
	tDesc_Texture.iLevelIndex = LEVEL_STATIC;
	tDesc_Texture.vLookDir = _float3(0.f, 0.f, 1.f);
	tDesc_Texture.fPlaySpeed = 3.8f;
	tDesc_Texture.bBillboarding = true;
	XMStoreFloat3(&tDesc_Texture.vPosition, pTransform->Get_Position() + __vector(0.f, -1.f, 0.f, 0.f));

	tDesc_Texture.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Texture.eLifeType = CEffect_Texture::TIME_LIMIT;

	tDesc_Texture.bSpinning = false;
	tDesc_Texture.fSpinSpeed = 0.05f;

	tDesc_Texture.bUseEasing_Scaling = false;
	tDesc_Texture.bBlur = true;

	tDesc_Texture.vMinSize = _float3(0.2f, 0.2f, 0.2f);
	tDesc_Texture.vMaxSize = _float3(1.f, 1.f, 1.f);
	tDesc_Texture.fScalingSpeed = 3.f;

	tDesc_Texture.eScalePattern = CEffect_Texture::PATTERN_BULGE;
	tDesc_Texture.eMovingPattern = CEffect_Texture::PATTERN_MOVE_ONEDIR;
	tDesc_Texture.vMovingDir = _float3(0.f, 1.f, 0.f);
	
	tDesc_Texture.fStartAlpha = 0.f;
	tDesc_Texture.vBaseColor = _float4(0.5f, 0.5f, 0.7f, 0.f);
	tDesc_Texture.vBlendingColor = _float4(1.f, 0.8f, 0.3f, 0.f);

	tDesc_Texture.fTransparecySpeed = 0.6f;
	tDesc_Texture.fTotalTime = 1.2f;

	if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex(), m_pLayerTag, m_pTextureProtoTag, &tDesc_Texture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 박스 샤인 이미지
	tDesc_Texture.iPassIndex = 20;
	tDesc_Texture.iTextureIndex = 189;
	tDesc_Texture.bNoising = true;
	tDesc_Texture.iLevelIndex = LEVEL_STATIC;
	tDesc_Texture.iSecondTextureIndex = 2;
	tDesc_Texture.fNoiseRot = 0.2f;

	XMStoreFloat3(&tDesc_Texture.vLookDir, XMVector3Normalize(pTransform->Get_Look()));

	tDesc_Texture.fPlaySpeed = 3.f;
	tDesc_Texture.bBillboarding = false;
	XMStoreFloat3(&tDesc_Texture.vPosition, pTransform->Get_Position() + __vector(0.f, 0.3f, 0.f, 0.f));

	tDesc_Texture.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Texture.eLifeType = CEffect_Texture::TIME_LIMIT;

	tDesc_Texture.bSpinning = false;
	tDesc_Texture.fSpinSpeed = 0.05f;

	tDesc_Texture.bUseEasing_Scaling = false;
	tDesc_Texture.bBlur = true;

	tDesc_Texture.vMinSize = _float3(0.8f, 0.7f, 1.0);
	tDesc_Texture.vMaxSize = _float3(1.f, 1.f, 1.f);
	tDesc_Texture.fScalingSpeed = 3.f;

	tDesc_Texture.eScalePattern = CEffect_Texture::PATTERN_SCALE_NONE;
	tDesc_Texture.eMovingPattern = CEffect_Texture::PATTERN_MOVE_NONE;
	tDesc_Texture.vMovingDir = _float3(0.f, 1.f, 0.f);

	tDesc_Texture.fStartAlpha = 0.f;
	tDesc_Texture.vBaseColor = _float4(0.5f, 0.5f, 0.7f, 0.f);
	tDesc_Texture.vBlendingColor = _float4(1.f, 0.8f, 0.3f, 0.f);

	tDesc_Texture.fTransparecySpeed = 0.f;
	tDesc_Texture.fTotalTime = 1.5f;

	auto vRight = XMVector3Cross(__vector(0.f, 1.f, 0.f, 0.f), XMLoadFloat3(&tDesc_Texture.vLookDir));

	//tDesc_Texture.RoatationMatrix = XMMatrixRotationAxis(XMVector3Normalize(pTransform->Get_Right()), XMConvertToRadians(90.f));
	 tDesc_Texture.RoatationMatrix = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	//tDesc_Texture.RoatationMatrix = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex(), m_pLayerTag, m_pTextureProtoTag, &tDesc_Texture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 작은 이펙트들
	CEffect_Particle::PARTICLEDESC tParticleDesc;

	tParticleDesc = m_PatricleDesc[TYPE_SHINING_PTCL];

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();

	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;
	tParticleDesc.pParentPosition = pPos;
	tParticleDesc.fMakeRange = 0.f;
	tParticleDesc.fMaxDistance = 10.f;
	tParticleDesc.fMinSpeed = 2.f;
	tParticleDesc.fMaxSpeed = 3.f;
	tParticleDesc.fScaleSpeed = 1.2f;
	tParticleDesc.fLiveTime = 0.5f;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.iPass = 2;
	tParticleDesc.bTrail = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	_float	fAddColor[4] = { 0.3f, 0.3f, 0.3f, -0.54f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
	tParticleDesc.vDir = __vector(0.f, 0.f, 0.f, 0.f);


	if (FAILED(pInstance->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Landing(_fvector vPosition)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Texture::EFFECT_TEX_DESC		tDesc;
	ZeroMemory(&tDesc, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	lstrcpy(tDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));
	tDesc.iPassIndex    = 7;
	tDesc.iTextureIndex = 231;
	tDesc.iLevelIndex   = LEVEL_STATIC;
	tDesc.vLookDir      = _float3(0.f, 0.f, 1.f);
	tDesc.fPlaySpeed    = 3.f;
	tDesc.bBillboarding = true;
	XMStoreFloat3(&tDesc.vPosition, vPosition);

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.eLifeType = CEffect_Texture::TIME_LIMIT;

	tDesc.vMinSize = _float3(1.f, 1.f, 1.f);
	tDesc.vMaxSize = _float3(9.f, 9.f, 9.f);
	tDesc.fScalingSpeed = 7.f;

	tDesc.eScalePattern = CEffect_Texture::PATTERN_SHRINK;
	tDesc.fStartAlpha = 0.f;
	tDesc.vBaseColor = _float4(0.2f, 0.2f, 0.2f, 0.f);

	tDesc.fTransparecySpeed = 1.0f;
	tDesc.fTotalTime        = 5.f;

	if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex(), m_pLayerTag, m_pTextureProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CEffect_Mesh::EFFECT_MESHDESC		tDesc_Mesh;
	ZeroMemory(&tDesc_Mesh, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	tDesc_Mesh.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc_Mesh.ModelTag, TEXT("Prototype_Component_Model_Ring00"));
	tDesc_Mesh.iTextureIndex = 46;
	tDesc_Mesh.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tDesc_Mesh.iScondTextureIndex = 2;

	tDesc_Mesh.vLookDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	tDesc_Mesh.fLifeTime = 1.f;
	XMStoreFloat4(&tDesc_Mesh.vPos, vPosition + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
	tDesc_Mesh.fRotationAngle = _float3(0.f, 0.f, 90.f);

	tDesc_Mesh.fTransparency = 0.f;
	tDesc_Mesh.fTransparecySpeed = 0;

	tDesc_Mesh.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc_Mesh.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc_Mesh.vMaxScale = XMVectorSet(0.1f, 0.1f, 0.1f, 0.f);
	tDesc_Mesh.fScaleSpeed = 2.0f;

	tDesc_Mesh.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc_Mesh.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc_Mesh.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Mesh.ePlayType = CEffect_Mesh::PLAY_LOOP;

	tDesc_Mesh.bMovingUV = true;
	tDesc_Mesh.vUVMovingSpeed = _float2(0.8f, 0.75f);

	tDesc_Mesh.bUsingBlur = true;

	tDesc_Mesh.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tDesc_Mesh.vColor        = _float4(-0.6f, -1.0f, -1.3f, -0.1f);
	tDesc_Mesh.LocalMat_NoTrace   = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc_Mesh.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc_Mesh)))
	{
		BREAKPOINT;
		return E_FAIL;	
	}

	return S_OK;
}

CEffect_Mesh * CEffect_Manager::Create_CrowAttack_LongDi_Moon(CTransform* pTransform, _bool* pDead, _float fOffsetY)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CEffect_Mesh::EFFECT_MESHDESC tDesc;

	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Moon00"));
	tDesc.iLevelIndex         = pInstance->Get_LevelIndex();
	tDesc.iTextureIndex       = 352;
	tDesc.ePattern_MixTex     = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex  = 0;
	tDesc.fLifeTime           = 1.0f;
	tDesc.fRotationAngle      = _float3(0.f, 0.f, 0.f);
	tDesc.fTransparecySpeed   = 0.f;
	tDesc.pParentDead         = pDead;
	tDesc.eLifeType           = CEffect_Mesh::TIME_LIMIT;
	tDesc.vStartScale         = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.ePattern_Scale      = CEffect_Mesh::PATTERN_SCALE_NONE;
	tDesc.eRenderGroup        = CRenderer::RENDER_BLEND;
	tDesc.ePlayType           = CEffect_Mesh::PLAY_ONCE;
	tDesc.ePattern_Move       = CEffect_Mesh::PATTERN_TRACING;
	tDesc.pTargetWorldMatrix  = pTransform->Get_WorldFloat4x4_Pointer();
	tDesc.eFromTarget         = CEffect_Mesh::LOOK;
	tDesc.fFromTargetDistance = 0.f;
	tDesc.bMovingUV           = false;
	tDesc.bUsingBlur          = true;
	tDesc.eDyingPattern       = CEffect_Mesh::DYING_MULT;
	tDesc.vColor              = _float4(0.7f, 0.6f, 4.0f, 1.6f);
	tDesc.LocalMat_NoTrace    = XMMatrixIdentity();
	tDesc.LocalMat            = XMMatrixRotationY(XMConvertToRadians(180.f));

	auto pObject = static_cast<CEffect_Mesh*>(pInstance->Add_GameObject_And_Return(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc));

	if (nullptr == pObject)
	{
		BREAKPOINT;
		return nullptr;
	}

	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;

	_float	fAddColor[4] = { 0.f, 0.f, 0.6f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 1.5f;
	tParticleDesc.fMakeRange     = 1.f;
	tParticleDesc.fMaxDistance   = 1.f;
	tParticleDesc.fMinSpeed      = 0.5f;
	tParticleDesc.fMaxSpeed      = 1.f;
	tParticleDesc.fScaleSpeed    = 0.2f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 353;
	tParticleDesc.iPass          = 2;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.pDead          = pDead;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y       = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	for (_uint i = 0; i < 7; ++i)
	{
		if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
		}
	}


	return pObject;
}

HRESULT CEffect_Manager::Create_CrowAttack_LongDi_Wind(_float4x4* pTargetWorld)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Attack07"));
	tDesc.iTextureIndex = 255;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tDesc.iScondTextureIndex = 0;

	tDesc.pTargetWorldMatrix = pTargetWorld;

	//XMStoreFloat4(&tDesc.vPos, vPosition);

	tDesc.fLifeTime = 0.75f;

	//tDesc.vLookDir = vLook;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.03f, 0.03f, 0.03f, 0.f);

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;

	//tDesc.eFromTarget = CEffect_Mesh::LOOK;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(2.75f, 0.f);

	tDesc.eDyingPattern    = CEffect_Mesh::DYING_ADD;
	tDesc.vColor           = _float4(0.2f, 0.2f, 0.2f, -0.3f);
	tDesc.LocalMat         = XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(0, 50.f, -150.f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_CrowAttack_Roaring(_fvector vPosition)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Attack12"));
	tDesc.iTextureIndex = 10;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	XMStoreFloat4(&tDesc.vPos, vPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	tDesc.fLifeTime = 1.f;

	tDesc.bBillboarding = true;

	tDesc.vLookDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = 0.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.001f, 0.001f, 0.001f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.08f, 0.08f, 0.08f, 0.f);
	tDesc.fScaleSpeed = 2.25f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.eFromTarget = CEffect_Mesh::LOOK;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(-0.5f, 0.5f);

	tDesc.eDyingPattern    = CEffect_Mesh::DYING_ADD;
	tDesc.vColor           = _float4(0.f, -1.7f, -0.7f, 0.f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Teleport(_fvector vPosition)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Texture::EFFECT_TEX_DESC      tDesc_Texture;
	// Smoke

	ZeroMemory(&tDesc_Texture, sizeof(CEffect_Texture::EFFECT_TEX_DESC));

	tDesc_Texture.iPassIndex = 5;
	tDesc_Texture.iTextureIndex = 338;
	tDesc_Texture.iLevelIndex = LEVEL_STATIC;

	//tDesc_Texture.vLookDir = _float3(0.f, 0.f, 1.f);
	tDesc_Texture.fPlaySpeed = 4.f;
	tDesc_Texture.bBillboarding = true;

	XMStoreFloat3(&tDesc_Texture.vPosition, vPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	tDesc_Texture.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Texture.eLifeType = CEffect_Texture::TIME_LIMIT;

	tDesc_Texture.bSpinning = false;

	tDesc_Texture.fSpinSpeed = 2.5f;

	tDesc_Texture.vMinSize = _float3(1.f, 1.f, 1.f);
	tDesc_Texture.vMaxSize = _float3(6.f, 6.f, 6.f);
	tDesc_Texture.fScalingSpeed = 5.5f;
	tDesc_Texture.eScalePattern = CEffect_Texture::PATTERN_SHRINK;

	tDesc_Texture.vBaseColor = _float4(0.14f, 0.f, 0.24f, 1.f);

	tDesc_Texture.fStartAlpha = 0.f;
	tDesc_Texture.fTransparecySpeed = 1.0f;
	tDesc_Texture.fTotalTime = 4.f;

	tDesc_Texture.bBlur = false;
	tDesc_Texture.bDistortion = false;

	if (FAILED(pInstance->Add_GameObject(tDesc_Texture.iLevelIndex, m_pLayerTag, m_pTextureProtoTag, &tDesc_Texture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Thorn
	CEffect_Mesh::EFFECT_MESHDESC      tDesc;

	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Thorn"));
	tDesc.iTextureIndex = 236;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	XMStoreFloat4(&tDesc.vPos, vPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	tDesc.fLifeTime = 1.f;

	tDesc.bBillboarding = false;

	tDesc.vLookDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	tDesc.iPassIndex = 0;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = 0.f;

	tDesc.bSpinning = true;
	tDesc.fSpinningSpeed = 5.75f;

	tDesc.vSpinAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.00001f, 0.00001f, 0.00001f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.04f, 0.04f, 0.04f, 0.f);
	tDesc.fScaleSpeed = 1.f;

	tDesc.bUseEasing_Scaling = false;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.eFromTarget = CEffect_Mesh::LOOK;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = true;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(-1.f, 0.f);

	tDesc.eDyingPattern    = CEffect_Mesh::DYING_ADD;
	tDesc.vColor           = _float4(0.f, 0.f, 0.f, 0.9f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ZeroMemory(&tDesc, sizeof(CEffect_Mesh::EFFECT_MESHDESC));

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tDesc.iTextureIndex = 12;
	tDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex = 0;

	XMStoreFloat4(&tDesc.vPos, vPosition + XMVectorSet(0.f, 1.f, 0.f, 0.f));

	tDesc.fLifeTime = 0.8f;

	tDesc.bBillboarding = false;

	tDesc.vLookDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);

	tDesc.iPassIndex = 0;

	tDesc.fRotationAngle = _float3(0.f, 0.f, 0.f);

	tDesc.fTransparecySpeed = -1.f;

	tDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;

	tDesc.vStartScale = XMVectorSet(0.05f, 0.05f, 0.05f, 0.f);
	tDesc.vMaxScale = XMVectorSet(0.15f, 0.15f, 0.15f, 0.f);
	tDesc.fScaleSpeed = 7.5f;

	tDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;

	tDesc.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;

	tDesc.eFromTarget = CEffect_Mesh::LOOK;

	tDesc.fFromTargetDistance = 0.f;

	tDesc.bMovingUV = false;

	tDesc.bUsingBlur = true;

	tDesc.vUVMovingSpeed = _float2(0.f, 1.f);

	tDesc.eDyingPattern    = CEffect_Mesh::DYING_COVER;
	tDesc.vColor           = _float4(1.f, 1.f, 1.f, 0.9f);
	tDesc.LocalMat_NoTrace = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Manager::Create_NightmareSpawnTornado(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado13"));
	tEffectMeshDesc.iTextureIndex      = 309;
	tEffectMeshDesc.iScondTextureIndex = 37;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 5.f;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 5.f;
	tEffectMeshDesc.fTransparecySpeed  = -2.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 39.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	//tEffectMeshDesc.vStartScale      = __vector(1.f, 1.5f, 1.f, 0.f);
	tEffectMeshDesc.vStartScale        = __vector(1.f, 1.5f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(2.f, 1.5f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.6f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 1.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = true;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ONEVALUE;
	tEffectMeshDesc.iRGBNum            = 1;
	tEffectMeshDesc.vColor             = _float4(0.3f, 0.3f, 0.3f, 3.9f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);

	//_matrix EffectWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	//EffectWorldMatrix = XMMatrixScaling(0.08f, 0.06f, 0.06f) * XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 1.5f, 1.f) * m_pTransformCom->Get_WorldMatrix();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareSpawnXCross(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sticker04"));
	tEffectMeshDesc.iTextureIndex = 132;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.pParentPos = nullptr;
	XMStoreFloat4(&tEffectMeshDesc.vPos,  pTransform->Get_Position() + __vector(0.f, 5.f, 0.f, 0.f));
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, -1.f, 0.f), XMConvertToRadians(45.f));
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 39.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 16.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(-0.75f, 0.f);
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.8f, 2.6f, -1.2f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_NightmareAppear"), pTransform)))
	{
		BREAKPOINT;
	}
}

void CEffect_Manager::Create_NightmareRunEffect(class CTransform* pTransform, _uint* iEffectCount)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Wind03"));
	tEffectMeshDesc.iTextureIndex      = 15;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 0.5f;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	//tEffectMeshDesc.vLookDir         = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(2.f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = false;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-1.1f, -0.9f, -1.f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	_matrix EffectWorldMatrix = pTransform->Get_WorldMatrix();

	switch (*iEffectCount)
	{
	case 0:
		EffectWorldMatrix = XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 1.5f, 0.f) * pTransform->Get_WorldMatrix();

		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return;
		}
		static_cast<CTransform*>(pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"))->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(EffectWorldMatrix);
		break;
	case 1:
		EffectWorldMatrix = XMMatrixTranslation(0.f, 1.5f, 0.f) * pTransform->Get_WorldMatrix();

		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return;
		}
		static_cast<CTransform*>(pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"))->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(EffectWorldMatrix);
		break;
	case 2:
		EffectWorldMatrix = XMMatrixTranslation(0.f, 0.6f, 0.f) * pTransform->Get_WorldMatrix();

		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return;
		}
		static_cast<CTransform*>(pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"))->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(EffectWorldMatrix);
		break;
	case 3:
		EffectWorldMatrix = XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 0.6f, 0.f) * pTransform->Get_WorldMatrix();

		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
		{
			BREAKPOINT;
			return;
		}
		static_cast<CTransform*>(pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"))->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(EffectWorldMatrix);
		break;
	}

	(*iEffectCount)++;
	if (3 < *iEffectCount)
		*iEffectCount = 0;
}

void CEffect_Manager::Create_NightmareNails(CTransform* pTransform, _bool* pParendDead, _uint iNum = 5)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 방향 정해주는 용
	if (iNum == 5)
		lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash05"));
	else
		lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash06"));

	tEffectMeshDesc.iTextureIndex      = 0;
	tEffectMeshDesc.pParentDead        = pParendDead;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.0f;
	tEffectMeshDesc.pParentPos         = pTransform->Get_PositionFloat4_Pointer();
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f); 
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_COVER;
	tEffectMeshDesc.vColor             = _float4(0.6f, 0.6f, 0.4f, 0.8f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.pTarget_Transform  = pTransform;

	_matrix EffectWorldMatrix = pTransform->Get_WorldMatrix();
	EffectWorldMatrix = XMMatrixScaling(2.f, 2.f, 2.f) * XMMatrixTranslation(0.f, 1.5f, 1.f) * pTransform->Get_WorldMatrix();

	CGameObject* pEffect = nullptr;
	pEffect = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);

	if (pEffect == nullptr)
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareNails_New(CTransform* pTransform, _bool* pParendDead, _uint iNum)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Moon00"));
	tEffectMeshDesc.iTextureIndex      = 152;
	tEffectMeshDesc.pParentDead        = pParendDead;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 10.0f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(3.f, 0.f);
	tEffectMeshDesc.fStartU            = -1.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.f, 0.f, -0.2f, 1.0f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.pTarget_Transform  = pTransform;

	if(iNum == 5)
		tEffectMeshDesc.LocalMat = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationZ(XMConvertToRadians(-54.f));

	else
		tEffectMeshDesc.LocalMat = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixRotationZ(XMConvertToRadians(54.f));


	CGameObject* pEffect = nullptr;
	pEffect = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);

	if (pEffect == nullptr)
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareNailTrail(CTransform* pTransform, _uint iNum)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
	tEffectMeshDesc.iTextureIndex = 183;
	tEffectMeshDesc.iScondTextureIndex = 2;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.0f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 29.25f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.04f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(1.f, 1.25f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 0.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(8.5f, 0.f);
	tEffectMeshDesc.fStartU = -0.4f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.5f, 0.3f, -0.2f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);

	if (iNum == 0)
	{
		/*tEffectMeshDesc.vUVMovingSpeed = _float2(-8.5f, 0.f);
		tEffectMeshDesc.fStartU = 0.4f;*/
		tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(132.f)) * XMMatrixTranslation(0.f, 50.f, 10.f);
	}

	else
		tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(48.f)) * XMMatrixTranslation(0.f, 50.f, 10.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareBeforeNails_Rings(CTransform* pTransform, _uint iSequence)
{
	_matrix LocalMatrix;
	_vector	vOffset = __vector(0.f, 2.f, 0.f, 0.f);				// 기존위치에서 위치 조정을 하고 싶을 시 이 변수 값 변경 (값 조절 필요)


	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc = m_MeshDesc[TYPE_WAVERING];

	tEffectMeshDesc.vLookDir = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + vOffset);

	switch (iSequence)
	{
	case 0:
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(54.f));
		break;
	case 1:
		LocalMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-54.f));
		break;
	case 2:
		LocalMatrix = tEffectMeshDesc.LocalMat_NoTrace;
		break;
	}

	tEffectMeshDesc.LocalMat_NoTrace = LocalMatrix;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 파티클
	if (iSequence == 1)
	{
		CEffect_Particle::PARTICLEDESC tParticleDesc;
		tParticleDesc.bChase = false;
		tParticleDesc.bLoop = false;
		tParticleDesc.bTrail = false;
		tParticleDesc.bUseEasing = true;
		tParticleDesc.bExplosion = true;
		tParticleDesc.fExplosionStartRange = 0.5f;
		tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
		tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

		_float	fAddColor[4] = { 0.12f, -0.08f, -0.52f, 0.f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

		_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
		memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

		tParticleDesc.fGravityPower = 2.87f;
		tParticleDesc.fLiveTime = 2.f;
		tParticleDesc.fMakeRange = 5.f;
		tParticleDesc.fMaxDistance = 10.f;
		tParticleDesc.fMinSpeed = 10.f;
		tParticleDesc.fMaxSpeed = 20.f;
		tParticleDesc.fScaleSpeed = 0.46f;
		tParticleDesc.fOriginScale = 0.3f;
		tParticleDesc.fTrailInterval = 0.f;
		tParticleDesc.iLevelIndex = 0;
		tParticleDesc.iTexIndex = 381;
		tParticleDesc.iPass = 3;
		tParticleDesc.pDead = pTransform->Get_Parent()->Get_DeadPointer();
		tParticleDesc.eEasing = CEffect_Particle::OUTSINE;
		tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;

		_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
		pPos->y = pPos->y + 0.5f;

		tParticleDesc.pParentPosition = pPos;

		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return;
		}
	}

	return;
}

void CEffect_Manager::Create_NightmareRotationAtk(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado13"));
	tEffectMeshDesc.iTextureIndex      = 243;
	tEffectMeshDesc.iScondTextureIndex = 2;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 0.75f;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 29.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.6f, 1.25f, 0.6f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.25f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 1.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(2.f, 4.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = true;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_MULT;
	tEffectMeshDesc.vColor             = _float4(2.f, 2.f, 0.5f, 0.6f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	_matrix EffectWorldMatrix = pTransform->Get_WorldMatrix();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareRush(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small09"));
	tEffectMeshDesc.iTextureIndex      = 15;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 0.5;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 76.5;
	tEffectMeshDesc.vPos               = _float4(0.f, 3.f, 0.f, 1.f);
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(6.f, 6.f, 6.f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(6.f, 6.f, 6.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND; 
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.f, 0.f, -0.3f, 0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat           = XMMatrixTranslation(0.f, -2.5f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareFlyKick(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small10"));
	tEffectMeshDesc.iTextureIndex      = 15;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 0.5;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, -0.8f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 76.5;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -0.8f, 1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(5.f, 5.f, 5.f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(5.f, 5.f, 5.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.f, 0.f, -0.3f, 0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixTranslation(0.f, 0.5f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareFlyKick_Before(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack10"));
	tEffectMeshDesc.iTextureIndex      = 62;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 4.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -0.8f, 1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(6.f, 6.f, 6.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 2.f);
	tEffectMeshDesc.fStartV            = -1.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.5f, 0.3f, 0.2f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

	_float	fAddColor[4] = { -0.5f, 0.2f, -0.1f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 1.f;
	tParticleDesc.fMakeRange     = 5.f;
	tParticleDesc.fMaxDistance   = 5.0f;
	tParticleDesc.fMinSpeed      = 0.1f;
	tParticleDesc.fMaxSpeed      = 0.56f;
	tParticleDesc.fScaleSpeed    = 1.f;
	tParticleDesc.fOriginScale   = 0.5f;
	tParticleDesc.fTrailInterval = 0.06f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 350;
	tParticleDesc.iPass          = 2;
	tParticleDesc.pDead          = nullptr;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.pParentPosition = pPos;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareShuriken(CTransform * pTransform, _bool* pParendDead, _bool bReserve)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sticker04"));
	tEffectMeshDesc.iTextureIndex      = 70;
	tEffectMeshDesc.iScondTextureIndex = 41;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 5.f;
	tEffectMeshDesc.vPos               = _float4(0.f, 60.f, 0.f, 1.f);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 35.f;
	tEffectMeshDesc.vSpinAxis          = __vector(1.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTCIRC;
	tEffectMeshDesc.vStartScale        = __vector(0.015f, 0.015f, 0.015f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 3.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 4.25f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.5f, 10.f, 10.f, -0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pParentDead        = pParendDead;

	if (bReserve == false)
		tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-45.f));
	else
		tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(45.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tEffectMeshDesc.iTextureIndex      = 90;
	tEffectMeshDesc.iScondTextureIndex = 2;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 6.f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(0.f, 25.25f);
	tEffectMeshDesc.vPos               = _float4(0.f, 1.8f, 0.25f, 1.f);
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.vStartScale        = __vector(0.5f, 0.5f, 0.5f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vColor             = _float4(-0.2f, -0.3f, 0.f, -0.1f);
	tEffectMeshDesc.pParentDead        = pParendDead;

	if (bReserve == false)
		tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-45.f));
	else
		tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(45.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	CGameObject* pEffect = CGameInstance::GetInstance()->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"));

	// 파티클 생성
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

	_float	fAddColor[4] = { 1.f, -0.36f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower   = 0.f;
	tParticleDesc.fLiveTime       = 300.f;
	tParticleDesc.fMakeRange      = 10.f;
	tParticleDesc.fMaxDistance    = 300.f;
	tParticleDesc.fMinSpeed       = 0.f;
	tParticleDesc.fMaxSpeed       = 0.f;
	tParticleDesc.fScaleSpeed     = 0.5f;
	tParticleDesc.fOriginScale    = 0.25f;
	tParticleDesc.fTrailInterval  = 0.f;
	tParticleDesc.iLevelIndex     = 0;
	tParticleDesc.iTexIndex       = 2;
	tParticleDesc.pDead           = pParendDead;
	tParticleDesc.pParentPosition = pEffect->Get_Transform()->Get_PositionFloat4_Pointer();
	tParticleDesc.vDir            = __vector(0.f, -1.f, 0.f, 0.f);

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return;
	}

	_float	fSecondAddColor[4] = { 1.f, -0.36f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fSecondAddColor, sizeof(_float) * 4);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return;
	}

	_float	fThirdAddColor[4] = { 1.f, -0.36f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fThirdAddColor, sizeof(_float) * 4);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NightmareBlade(CTransform * pTransform)
{
	CNightmare_Blade::BLADEDESC tBladeDesc;
	tBladeDesc.LocalMatrix = XMMatrixScaling(2.f,2.f,2.f) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(0.f, 2.f, 1.5f);
	tBladeDesc.pParentTransform = pTransform; 

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_NightmareBlade"), &tBladeDesc)))
	{
		BREAKPOINT;
		return;
	}
}

HRESULT CEffect_Manager::Create_SkillDeadEffect_Tornado(CTransform* pTransform)
{	
	// 스파크
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado08"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 221;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.5f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.002f, 0.002f, 0.002f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 15.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(3.f, 2.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.3f, 0.1f, 1.2f, -0.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex            = 381;
	tParticleDesc.bChase               = false;
	tParticleDesc.bLoop                = false;
	tParticleDesc.bTrail               = false;
	tParticleDesc.bUseEasing           = true;
	tParticleDesc.eDir                 = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber      = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ       = false;
	tParticleDesc.bExplosion           = true;
	tParticleDesc.fMakeRange           = 1.f;
	tParticleDesc.fExplosionStartRange = 0.2f;
	tParticleDesc.vMakeRangeXYZ        = _float3(2.f, 2.f, 2.f);
	tParticleDesc.fMinSpeed            = 1.f;
	tParticleDesc.fMaxSpeed            = 10.f;
	tParticleDesc.fOriginScale         = 0.12f;
	tParticleDesc.fScaleSpeed          = 0.19f;
	tParticleDesc.fGravityPower        = 0.64f;
	tParticleDesc.fLiveTime            = 1.f;
	tParticleDesc.fMaxDistance         = 0.f;
	tParticleDesc.fTrailInterval       = 0.f;
	tParticleDesc.iLevelIndex          = 0;
	tParticleDesc.eLifeType            = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing              = CEffect_Particle::INOUTSINE;


	_float	fAddColor[4] = { 0.26f, 0.f, 0.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);


	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pTransform->Get_PositionFloat4_Pointer();


	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Manager::Create_NobodySpawnMagicCircle(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sticker03"));
	tEffectMeshDesc.iTextureIndex      = 106;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 2.5f;

	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 5.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 0.8f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = -0.5f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.f, -0.1f, -0.2f, 1.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	_matrix EffectWorldMatrix = pTransform->Get_WorldMatrix();


	EffectWorldMatrix = XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 1.5f, 0.f) * pTransform->Get_WorldMatrix();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
	static_cast<CTransform*>(CGameInstance::GetInstance()->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"))->Get_Component(TEXT("Com_Transform")))->Set_WorldMatrix(EffectWorldMatrix);
}

void CEffect_Manager::Create_NobodySpawnTurn(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Big00"));
	tEffectMeshDesc.iTextureIndex      = 106;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.5f;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 5.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.8f, 0.3f, 0.8f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed        = 0.2f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(2.25f, 2.25f);
	tEffectMeshDesc.fStartU            = -1.f;
	tEffectMeshDesc.fStartV            = -1.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.f, -0.1f, -0.2f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	//tEffectMeshDesc.LocalMat         = XMMatrixRotationY(XMConvertToRadians(-90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NobodyChargeRush(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack06"));
	tEffectMeshDesc.iTextureIndex      = 106;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 2.5f;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir           = __vector(1.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 5.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.03f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.1f, 0.1f, 0.03f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 20.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(2.25f, 2.25f);
	tEffectMeshDesc.fStartU            = -1.f;
	tEffectMeshDesc.fStartV            = -1.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.f, -0.1f, -0.2f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat           = XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 10.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NobodyRush(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere01"));
	tEffectMeshDesc.iTextureIndex      = 112;
	tEffectMeshDesc.iScondTextureIndex = 2;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 1.25f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(0.f, 0.f);
	tEffectMeshDesc.fLifeTime          = 0.3f;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f); 
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 5.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::INQUART;
	tEffectMeshDesc.vStartScale        = __vector(0.03f, 0.03f, 0.04f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 50.f, 0.f, 1.f); 
	tEffectMeshDesc.fScaleSpeed        = 4.5f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(7.25f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.f, 0.3f, 0.6f, -0.4f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	//tEffectMeshDesc.LocalMat         = XMMatrixRotationY(XMConvertToRadians(-90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_NobodyRotationAtk(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tEffectMeshDesc.iTextureIndex      = 179;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fLifeTime          = 2.5f;
	tEffectMeshDesc.pParentPos         = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 5.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.125f, 0.25f, 0.125f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 40.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed        = 0.f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.f, 0.f);
	tEffectMeshDesc.fStartU            = -1.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.5f, 0.2f, 0.2f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixTranslation(0.f, -22.f, 0.f);
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_Flowers(CTransform* pTransform)
{
	// 꽃잎
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;
	tEffectTexDesc.iPassIndex        = 0;
	tEffectTexDesc.iTextureIndex     = 336;
	tEffectTexDesc.iLevelIndex       = LEVEL_STATIC;
	tEffectTexDesc.fTotalTime        = 3.f;
	tEffectTexDesc.bBillboarding     = true;
	tEffectTexDesc.bBlur             = true;
	tEffectTexDesc.bSprite           = true;
	tEffectTexDesc.fSpriteMaxXIndex  = 4;
	tEffectTexDesc.fSpriteMaxYIndex  = 4;
	tEffectTexDesc.ePlayType         = CEffect_Texture::PLAY_ONCE;
	tEffectTexDesc.eRenderGroup      = CRenderer::RENDER_NONLIGHT;
	tEffectTexDesc.eLifeType         = CEffect_Texture::SPRITE_LIMIT;
	tEffectTexDesc.bSpinning         = false;
	tEffectTexDesc.bSprite           = true;
	tEffectTexDesc.eScalePattern     = CEffect_Texture::PATTERN_SCALE_NONE;
	tEffectTexDesc.eMovingPattern    = CEffect_Texture::PATTERN_MOVE_RANDDIR;
	tEffectTexDesc.eEasing           = CEffect_Texture::OUTCIRC;
	tEffectTexDesc.bUseEasing        = true;
	tEffectTexDesc.vMinSize          = 0.4f;
	tEffectTexDesc.vMaxSize          = 0.4f;
	tEffectTexDesc.fStartAlpha       = 0.f;

	_vector vPos = pTransform->Get_Position() + pTransform->Get_Up();
	tEffectTexDesc.vPosition = XMStoreFloat3RT(vPos);
	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

	for (_uint i = 0; i < 100; i++)
	{
		tEffectTexDesc.fSpriteSpeed = _float(rand() % 101) / 20.f + 3.f;
		tEffectTexDesc.fPlaySpeed = tEffectTexDesc.fSpriteSpeed * 5.f;
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
		{
			BREAKPOINT;
			return;
		}
	}
}

void CEffect_Manager::Create_BigTreeHowling(CTransform* pTransform)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Roar Ring
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring00"));
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.iTextureIndex      = 273;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 5.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.5f, 1.5f, 1.5f, 0.f);
	XMStoreFloat4(& tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 10.f, 0.f, 0.f));
	tEffectMeshDesc.fScaleSpeed        = 0.7f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.0f, 1.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = -0.5f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.2f, 0.4f, 0.3f, 0.0f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) /** XMMatrixTranslation(0.f, 50.f, 0.f)*/;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BigTreeHowling"), pTransform)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTreeSmoke(CTransform* pTransform, _uint iNum, _float fMinX, _float fMaxX, _float fMinY, _float fMaxY, _float fMinZ, _float fMaxZ, _float fInterval)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < iNum; i++)
	{
		_float	fX = RandomRange(fMinX, fMaxX);
		_float	fY = RandomRange(fMinY, fMaxY);
		_float	fZ = RandomRange(fMinZ, fMaxZ);

		_vector vDir = XMVector3Normalize(__vector(fX, 0.f, fZ, 0.f));

		_vector vPos = pTransform->Get_Position() + __vector(fX, fY, fZ, 0.f) + vDir * fInterval;
		m_TextureDesc[TEXTURE_BIGTREESMOKE].vPosition = XMStoreFloat3RT(vPos);
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &m_TextureDesc[TEXTURE_BIGTREESMOKE])))
		{
			BREAKPOINT;
			return;
		}
	}
}

void CEffect_Manager::Create_BigTreeSmoke(_float4 vBonePos, _uint iNum, _float fMinX, _float fMaxX, _float fMinY, _float fMaxY, _float fMinZ, _float fMaxZ, _float fInterval)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < iNum; i++)
	{
		_float	fX = RandomRange(fMinX, fMaxX);
		_float	fY = RandomRange(fMinY, fMaxY);
		_float	fZ = RandomRange(fMinZ, fMaxZ);

		_vector vDir = XMVector3Normalize(__vector(fX, 0.f, fZ, 0.f));

		_vector vPos = XMLoadFloat4(&vBonePos);
		
		vPos += __vector(fX, fY, fZ, 0.f) + vDir * fInterval;
		m_TextureDesc[TEXTURE_BIGTREESMOKE].vPosition = XMStoreFloat3RT(vPos);
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &m_TextureDesc[TEXTURE_BIGTREESMOKE])))
		{
			BREAKPOINT;
			return;
		}
	}
}

void CEffect_Manager::Create_BigTreeFall(CTransform* pTransform, _float4x4* pBoneMatrix, _float fLifeTime, _fmatrix localMat)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 흰색 이펙트
	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall00"));
	//tEffectMeshDesc.iTextureIndex          = 346;
	tEffectMeshDesc.iTextureIndex            = 332;
	tEffectMeshDesc.iScondTextureIndex       = 0;
	tEffectMeshDesc.ePattern_MixTex          = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime                = fLifeTime;
	tEffectMeshDesc.pParentPos               = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir                 = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex               = 0;
	tEffectMeshDesc.fTransparency            = 0.f;
	tEffectMeshDesc.fTransparecySpeed        = 0.f;
	tEffectMeshDesc.fMaxTransparecy          = 0.f;
	tEffectMeshDesc.bSpinning                = true;
	tEffectMeshDesc.fSpinningSpeed           = 3.f;
	tEffectMeshDesc.vSpinAxis                = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding            = false;
	tEffectMeshDesc.eLifeType                = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling       = false;
	//tEffectMeshDesc.vStartScale            = __vector(1.1f, 1.1f, 1.1f, 0.f);
	tEffectMeshDesc.vStartScale              = __vector(1.5f, 1.5f, 1.5f, 0.f);
	tEffectMeshDesc.vMaxScale                = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.fScaleSpeed              = 0.f;
	tEffectMeshDesc.eRenderGroup             = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType                = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.ePattern_Move            = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix       = pBoneMatrix;
	tEffectMeshDesc.bMovingUV                = true;
	//tEffectMeshDesc.vUVMovingSpeed         = _float2(0.f, 8.75f);
	tEffectMeshDesc.vUVMovingSpeed           = _float2(0.f, 3.f);
	tEffectMeshDesc.fStartV                  = -1.f;
	tEffectMeshDesc.bUsingEmissive           = false;
	//tEffectMeshDesc.bUsingBlur               = false;
	tEffectMeshDesc.bUsingBlur               = true;
	tEffectMeshDesc.bUsingDistortion         = false;
	tEffectMeshDesc.eDyingPattern            = CEffect_Mesh::DYING_ADD;
	//tEffectMeshDesc.vColor                 = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.vColor                   = _float4(-0.1f, 0.f, -0.1f, -0.8f);
	tEffectMeshDesc.vBlendColor              = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace         = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat                 = localMat;
	tEffectMeshDesc.pParentDead = pTransform->Get_Parent()->Get_DeadPointer();

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 초록색 이펙트
	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall01"));
	//tEffectMeshDesc.iTextureIndex        = 346;
	tEffectMeshDesc.iTextureIndex          = 331;
	tEffectMeshDesc.iScondTextureIndex     = 0;
	tEffectMeshDesc.ePattern_MixTex        = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime              = fLifeTime;
	tEffectMeshDesc.pParentPos             = &(pTransform->Get_Position_F4());
	tEffectMeshDesc.vLookDir               = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex             = 0;
	tEffectMeshDesc.fTransparency          = 0.f;
	tEffectMeshDesc.fTransparecySpeed      = 0.f;
	tEffectMeshDesc.fMaxTransparecy        = 0.f;
	tEffectMeshDesc.bSpinning              = true;
	//tEffectMeshDesc.fSpinningSpeed       = 9.f;
	tEffectMeshDesc.fSpinningSpeed         = 5.f;
	tEffectMeshDesc.vSpinAxis              = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding          = false;
	tEffectMeshDesc.eLifeType              = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling     = false;
	//tEffectMeshDesc.vStartScale          = __vector(0.9f, 0.9f, 0.9f, 0.f);
	tEffectMeshDesc.vStartScale            = __vector(1.5f, 1.5f, 1.5f, 0.f);
	tEffectMeshDesc.vMaxScale              = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.fScaleSpeed            = 0.f;
	tEffectMeshDesc.eRenderGroup           = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType              = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.ePattern_Move          = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix     = pBoneMatrix;
	tEffectMeshDesc.bMovingUV              = true;
	//tEffectMeshDesc.vUVMovingSpeed       = _float2(0.f, 8.75f);
	tEffectMeshDesc.vUVMovingSpeed         = _float2(0.f, 3.f);
	tEffectMeshDesc.fStartV                = -1.f;
	tEffectMeshDesc.bUsingEmissive         = false;
	tEffectMeshDesc.bUsingBlur             = true;
	tEffectMeshDesc.bUsingDistortion       = false;
	tEffectMeshDesc.eDyingPattern          = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor                 = _float4(-0.4f, -0.1f, -0.6f, -0.7f);
	tEffectMeshDesc.vBlendColor            = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace       = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat               = localMat;

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
	tEffectMeshDesc.fStartV = -1.5f;
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

HRESULT CEffect_Manager::Create_Apple(_float4x4* pTargetWorld)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC		tDesc;

	tDesc.iLevelIndex = pInstance->Get_LevelIndex();
	lstrcpy(tDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tDesc.iTextureIndex              = 42;
	tDesc.ePattern_MixTex            = CEffect_Mesh::MIX_NONE;
	tDesc.iScondTextureIndex         = 2;
	tDesc.fLifeTime                  = 2.f;
	tDesc.pTargetWorldMatrix         = pTargetWorld;
	tDesc.eFromTarget                = CEffect_Mesh::LOOK;
	tDesc.fFromTargetDistance        = 0.f;
	tDesc.fTransparecySpeed          = 0.05f;
	tDesc.eLifeType                  = CEffect_Mesh::TIME_LIMIT;
	tDesc.vStartScale                = XMVectorSet(0.5f, 0.5f, 0.5f, 0.f);
	tDesc.vMaxScale                  = XMVectorSet(0.006f, 0.006f, 0.006f, 0.f);
	tDesc.fScaleSpeed                = 0.75f;
	tDesc.ePattern_Scale             = CEffect_Mesh::PATTERN_SCALE_NONE;
	tDesc.ePattern_Move              = CEffect_Mesh::PATTERN_TRACING;
	tDesc.eRenderGroup               = CRenderer::RENDER_BLEND;
	tDesc.ePlayType                  = CEffect_Mesh::PLAY_LOOP;
	tDesc.bMovingUV                  = true;
	tDesc.vUVMovingSpeed             = _float2(-2.f, 1.f);
	tDesc.bUsingBlur                 = true;
	tDesc.eDyingPattern              = CEffect_Mesh::DYING_MULT;
	tDesc.vColor                     = _float4(1.f, 0.f, 0.f, 1.f);
	tDesc.LocalMat_NoTrace           = XMMatrixIdentity();

	if (FAILED(pInstance->Add_GameObject(tDesc.iLevelIndex, m_pLayerTag, m_pMeshProtoTag, &tDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Manager::Create_FruitZone(_vector vPosition)
{
	// 공중에서 과일 뿌릴 때 나오는 녹색 이펙트
	_float4	vPos = XMStoreFloat4RT(vPosition);
	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_DropFruitZone"), &vPos)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTreeSpawn(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	tEffectMeshDesc.bBillboarding = false;
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Big00"));
	tEffectMeshDesc.iTextureIndex      = 57;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f; 
	tEffectMeshDesc.fTransparecySpeed  = -0.4f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.2f, 0.1f, 0.15f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.3f, 0.15f, 0.3f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed        = 1.75f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 1.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = -0.8f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.2f, -0.4f, 0.4f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTree_LandingWind(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Small07"));
	tEffectMeshDesc.iTextureIndex      = 14;
	tEffectMeshDesc.iScondTextureIndex = 0;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 5.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.4f, 0.4f, 0.4f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed        = 1.75f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(-0.5f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.1f, -0.3f, 0.5f, -0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTree_SummonWings(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sticker10"));
	tEffectMeshDesc.iTextureIndex      = 152;
	tEffectMeshDesc.iScondTextureIndex = 33;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 1.75f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime          = 3.5f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.08f;
	tEffectMeshDesc.fTransparecySpeed  = -0.26f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed        = 2.f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(-2.25f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_MULT;
	tEffectMeshDesc.vColor             = _float4(1.f, 2.f, 1.f, 1.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(-0.5f, 150.f, 0.f);
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.5f, 150.f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTree_Slash(CTransform* pTransform, _fvector vOffset)
{
	// 왼쪽공격 오른쪽공격 사이 간격 보정 변수
	_vector		vOffSetY = __vector(0.f, -6.f, 0.f, 0.f);

	auto		vPosition = pTransform->Get_Position() + vOffset;

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
	tEffectMeshDesc.iTextureIndex = 143;
	tEffectMeshDesc.iScondTextureIndex = 33;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fNoisePower = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime = 2.f;
	tEffectMeshDesc.pParentPos = nullptr;
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition);
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.08f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.2f, 0.2f, 0.2f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 2.f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(-4.25f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.9f, 0.4f, 0.0f, -0.3f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(-0.5f, 6.75f, 75.f);
	tEffectMeshDesc.pParentDead        = pTransform->Get_Parent()->Get_DeadPointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition + vOffSetY);
	tEffectMeshDesc.vUVMovingSpeed = _float2(4.25f, 0.f);

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_SoaringWind(_float4x4* pBoneMatrix)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado09"));
	tEffectMeshDesc.iTextureIndex      = 79;
	tEffectMeshDesc.iScondTextureIndex = 33;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fNoisePower        = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.08f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.15f, 0.15f, 0.15f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.06f, 0.06f, 0.06f, 0.f);
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.fScaleSpeed        = 2.f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pBoneMatrix;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(2.f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.f, 1.6f, 1.2f, -0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTreeSummonApple(CTransform* pTransform)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Moon
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Moon00"));
	tEffectMeshDesc.iTextureIndex      = 127;
	tEffectMeshDesc.iScondTextureIndex = 33; 
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fNoisePower        = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime          = 16.5f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.08f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 3.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.07f, 0.07f, 0.07f, 0.f);
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 22.5f, 0.f, 0.f));
	tEffectMeshDesc.fScaleSpeed        = 10.7f; 
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.1f, 0.f, 0.7f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTreeSummonAppleParticle(CTransform* pTransform)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 달에서 생성되는 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = true;
	tParticleDesc.eDir            = CEffect_Particle::DIR_ONE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 0.09f, -0.03f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fMinSpeed      = 10.f;
	tParticleDesc.fMaxSpeed      = 20.f;
	tParticleDesc.fOriginScale   = 0.8f; 
	tParticleDesc.fScaleSpeed    = 1.25f;
	tParticleDesc.fGravityPower  = 0.86f;
	tParticleDesc.fLiveTime      = 1.37f;
	tParticleDesc.fMakeRange     = 40.f;
	tParticleDesc.fMaxDistance   = 40.f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 361;
	tParticleDesc.iPass          = 2;
	tParticleDesc.pDead          = nullptr;
	tParticleDesc.eEasing        = CEffect_Particle::INOUTQUAD;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y = pPos->y + 22.5f;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(pGameInstance->Add_GameObject(pGameInstance->Get_LevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return;
	}

	_float	fAddColor1[4] = { 0.4f, 0.35f, 0.09f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor1, sizeof(_float) * 4);

	if (FAILED(pGameInstance->Add_GameObject(pGameInstance->Get_LevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_BigTreeSummonApple_Ring(CTransform* pTransform)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// Ring
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring00"));
	tEffectMeshDesc.iTextureIndex      = 143;
	tEffectMeshDesc.iScondTextureIndex = 33;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fNoisePower        = 0.f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(-1.f, 0.f);
	tEffectMeshDesc.fLifeTime          = 2.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.08f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 3.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::INOUTBACK;
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(0.f, 22.5f, 0.f, 0.f));
	tEffectMeshDesc.fScaleSpeed        = 4.f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.9f, 0.4f, 0.f, -0.3f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_Explosion(_vector vPosition, _float vScale, _uint iNum)
{
	// iNum == 0 -> 한번 터지는 텍스쳐
	// iNum != 0 -> 두번 터지는 텍스쳐
	
	// Explosion
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;

	_uint  iTexIndex = 347;

	if (iNum != 0)
	{
		iTexIndex = 348;
	}

	tEffectTexDesc.iTextureIndex    = iTexIndex;
	tEffectTexDesc.iPassIndex       = 0;
	tEffectTexDesc.iLevelIndex      = LEVEL_STATIC;
	tEffectTexDesc.fTotalTime       = 3.f;
	tEffectTexDesc.bBillboarding    = true;
	tEffectTexDesc.bBlur            = false;
	tEffectTexDesc.bSprite          = true;
	tEffectTexDesc.fSpriteMaxXIndex = 8;
	tEffectTexDesc.fSpriteMaxYIndex = 8; 
	tEffectTexDesc.fSpriteSpeed     = 11.f;
	tEffectTexDesc.ePlayType        = CEffect_Texture::PLAY_ONCE;
	tEffectTexDesc.eRenderGroup     = CRenderer::RENDER_NONLIGHT;
	tEffectTexDesc.eLifeType        = CEffect_Texture::SPRITE_LIMIT;
	tEffectTexDesc.bSpinning        = false;
	tEffectTexDesc.eScalePattern    = CEffect_Texture::PATTERN_SCALE_NONE;
	tEffectTexDesc.eMovingPattern   = CEffect_Texture::PATTERN_MOVE_NONE;
	tEffectTexDesc.eEasing          = CEffect_Texture::OUTCIRC;
	tEffectTexDesc.bUseEasing       = false;
	tEffectTexDesc.vMinSize         = vScale;
	tEffectTexDesc.vMaxSize         = 0.4f;
	tEffectTexDesc.fStartAlpha      = -0.1f;

	_vector vPos             = vPosition;
	tEffectTexDesc.vPosition = XMStoreFloat3RT(vPos);
	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_Leaves(_vector vPosition, _fvector vOffset)
{
	CEffect_Texture::EFFECT_TEX_DESC tEffectTexDesc;
	tEffectTexDesc.iPassIndex       = 5;
	tEffectTexDesc.iTextureIndex    = 340;
	tEffectTexDesc.iLevelIndex      = LEVEL_STATIC;
	tEffectTexDesc.fTotalTime       = 3.f;
	tEffectTexDesc.bBillboarding    = true;
	tEffectTexDesc.bBlur            = false;
	tEffectTexDesc.bSprite          = true;
	tEffectTexDesc.fSpriteMaxXIndex = 4;
	tEffectTexDesc.fSpriteMaxYIndex = 4;
	tEffectTexDesc.ePlayType        = CEffect_Texture::PLAY_ONCE;
	tEffectTexDesc.eRenderGroup     = CRenderer::RENDER_NONLIGHT;
	tEffectTexDesc.eLifeType        = CEffect_Texture::SPRITE_LIMIT;
	tEffectTexDesc.bSpinning        = false;
	tEffectTexDesc.eScalePattern    = CEffect_Texture::PATTERN_SCALE_NONE;
	tEffectTexDesc.eMovingPattern   = CEffect_Texture::PATTERN_MOVE_RANDDIR;
	tEffectTexDesc.eEasing          = CEffect_Texture::OUTCIRC;
	tEffectTexDesc.bUseEasing       = true;
	tEffectTexDesc.vMinSize         = 0.4f;
	tEffectTexDesc.vMaxSize         = 0.4f;
	tEffectTexDesc.fStartAlpha      = 0.f;
	tEffectTexDesc.vBaseColor       = _float4(0.1f, 1.0f, 0.5f, 0.f);
	
	_vector vPos             = vPosition + vOffset;
	tEffectTexDesc.vPosition = XMStoreFloat3RT(vPos);
	lstrcpy(tEffectTexDesc.TextureTag, TEXT("Prototype_Component_Texture_Effect"));

	for (_uint i = 0; i < 100; i++)
	{
		tEffectTexDesc.fSpriteSpeed = _float(rand() % 101) / 20.f + 3.f;
		tEffectTexDesc.fPlaySpeed = tEffectTexDesc.fSpriteSpeed * 1.2f;
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &tEffectTexDesc)))
		{
			BREAKPOINT;
			return;
		}
	}
}

void CEffect_Manager::Create_BigTreeParticle(_float4* pPos, _bool* pDead)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = true;
	tParticleDesc.bLoop = false; 
	tParticleDesc.bTrail = true; 
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 0.1f, 0.8f, 0.1f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 10.f;
	tParticleDesc.fMakeRange = 20.f;
	tParticleDesc.fMaxDistance = 20.f;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 6.f;
	tParticleDesc.fScaleSpeed = 0.41f;
	tParticleDesc.fOriginScale = 0.5f;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 353;
	tParticleDesc.iPass = 3;
	tParticleDesc.pDead = pDead;
	tParticleDesc.eEasing = CEffect_Particle::OUTCUBIC;
	tParticleDesc.eLifeType = CEffect_Particle::LIFE_TYPE_END;


	//pPos->y = pPos->y + fOffsetY;

	tParticleDesc.pParentPosition = pPos;

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_IceManSlash(CTransform* pTransform, _bool bIsLeftHandSlash)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash02"));

	tEffectMeshDesc.iTextureIndex      = 100;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.0f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;

	_float fUVMovingX = -4.f;
	if (bIsLeftHandSlash == true)
		fUVMovingX = 4.f;

	tEffectMeshDesc.vUVMovingSpeed     = _float2(fUVMovingX, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.8f, -0.7f, 1.0f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	_float fRotation = 20.f;
	if (bIsLeftHandSlash == true)
		fRotation = -20.f;

	tEffectMeshDesc.LocalMat = XMMatrixScaling(0.1f, 0.01f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(fRotation)) * XMMatrixTranslation(0.f, 2.5f, 2.5f);

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.LocalMat = XMMatrixScaling(0.1f, 0.01f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(fRotation)) * XMMatrixTranslation(0.f, 3.5f, 2.5f);

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.LocalMat = XMMatrixScaling(0.1f, 0.01f, 0.1f) * XMMatrixRotationZ(XMConvertToRadians(fRotation)) * XMMatrixTranslation(0.f, 1.5f, 2.5f);

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_IceManJump(CTransform* pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 바닥 충격파
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere03"));
	tEffectMeshDesc.iTextureIndex      = 41;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.0f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -2.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.fScaleSpeed        = 3.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.85f, 1.1f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.3f, -0.3f, 0.0f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}

	// 올라가는 충격파
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado13"));
	tEffectMeshDesc.iTextureIndex     = 167;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.eEasing_Scale     = CEffect_Mesh::INELASTIC;
	tEffectMeshDesc.vStartScale       = __vector(1.f, 0.7f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale         = __vector(3.f, 2.1f, 3.f, 0.f);
	tEffectMeshDesc.fScaleSpeed       = 3.f;
	tEffectMeshDesc.vUVMovingSpeed    = _float2(2.f, 3.f);
	tEffectMeshDesc.vColor            = _float4(0.f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.LocalMat          = XMMatrixRotationY(XMConvertToRadians(20.f));

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}

	// 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = true;
	tParticleDesc.eEasing         = CEffect_Particle::INOUTSINE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

	_float	fAddColor[4] = { -0.6f, -0.5f, 1.0f, 0.0f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 23;
	tParticleDesc.fMinSpeed      = 5.f;
	tParticleDesc.fMaxSpeed      = 10.f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fScaleSpeed    = 0.f;
	tParticleDesc.fGravityPower  = 2.f;
	tParticleDesc.fLiveTime      = 5.f;
	tParticleDesc.fMakeRange     = 3.5f;
	tParticleDesc.fMaxDistance   = 15.f;
	tParticleDesc.eDir = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead          = nullptr;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y = pPos->y + 1.f;
	tParticleDesc.pParentPosition = pPos;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc))
	{
		BREAKPOINT;
		return;
	}

	return;
}

void CEffect_Manager::Create_IceManJumpLanding(CTransform* pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 바닥 충격파
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere03"));
	tEffectMeshDesc.iTextureIndex   = 41;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime       = 1.0f;
	tEffectMeshDesc.pParentPos      = nullptr;
	tEffectMeshDesc.ePattern_Move   = CEffect_Mesh::PATTERN_MOVE_NONE;
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -2.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(2.f, 2.f, 2.f, 0.f);
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.fScaleSpeed        = 3.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.85f, 1.1f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.3f, -0.3f, 0.0f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}

	// 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = true;
	tParticleDesc.eEasing = CEffect_Particle::INOUTSINE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

	_float	fAddColor[4] = { -0.6f, -0.5f, 1.0f, 0.0f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 23;
	tParticleDesc.fMinSpeed      = 5.f;
	tParticleDesc.fMaxSpeed      = 10.f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fScaleSpeed    = 0.f;
	tParticleDesc.fGravityPower  = 2.f;
	tParticleDesc.fLiveTime      = 5.f;
	tParticleDesc.fMakeRange     = 3.5f;
	tParticleDesc.fMaxDistance   = 15.f;
	tParticleDesc.eDir           = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead          = nullptr;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y = pPos->y + 1.f;
	tParticleDesc.pParentPosition = pPos;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc))
	{
		BREAKPOINT;
		return;
	}

	return;
}

void CEffect_Manager::Create_IceManClap(CTransform* pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 박수파장 메쉬
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Circle00"));

	tEffectMeshDesc.iTextureIndex      = 12;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 0.5f;
	tEffectMeshDesc.pParentPos         = pTransform->Get_PositionFloat4_Pointer();
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.0f;
	tEffectMeshDesc.fTransparecySpeed  = -0.25f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(2.5f, 2.5f, 2.5f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 5.f;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = true;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.3f, -0.3f, -0.1f, -0.9f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixScaling(0.01f, 2.5f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 2.f, 5.f);

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}

	// 공중으로 올라가는 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eEasing         = CEffect_Particle::INOUTSINE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

	_float	fAddColor[4] = { 0.f, 0.f, 0.f, 0.0f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 0;
	tParticleDesc.fMinSpeed      = 5.f;
	tParticleDesc.fMaxSpeed      = 20.f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fScaleSpeed    = 0.1f;
	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 2.f;
	tParticleDesc.fMakeRange     = 20.f;
	tParticleDesc.fMaxDistance   = 20.f;
	tParticleDesc.eDir           = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir           = __vector(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead          = nullptr;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.pParentPosition = pPos;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_IceManJumpAtkLanding(CTransform* pTransform, _bool bIsLeftHandBlow)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 바닥 충격파
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere03"));
	tEffectMeshDesc.iTextureIndex      = 41;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.0f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move	   = CEffect_Mesh::PATTERN_MOVE_NONE;
	if (bIsLeftHandBlow == true)
		XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(-2.f, 0.f, 3.5f, 0.f));
	else
		XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + __vector(2.f, 0.f, 3.5f, 0.f));
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -2.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTSINE;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.15f, 0.15f, 0.15f, 0.f);
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.fScaleSpeed        = 3.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.85f, 1.1f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.3f, -0.3f, 0.0f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_IceManJumpAtkGuide(CTransform* pTransform, _bool* pParentDead)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Cup1"));
	tEffectMeshDesc.iTextureIndex      = 127;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 10.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentDead        = pParentDead;
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.02f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.5f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.f, -0.5f, -0.5f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_IceBall(CTransform* pTransform, _bool* pParentDead)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Item06"));
	tEffectMeshDesc.iTextureIndex      = 82;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 3.5f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.pParentDead        = pParentDead;
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(-1.f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.2f, 0.f, 0.9f, 0.2f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_IceBallParticle(CTransform* pTransform, _bool* pParentDead)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eEasing         = CEffect_Particle::INOUTSINE;
	tParticleDesc.bBlur           = true;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;

	_float	fAddColor[4] = { -0.6f, -0.5f, 0.5f, 0.0f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 22;
	tParticleDesc.fMinSpeed      = 1.f;
	tParticleDesc.fMaxSpeed      = 5.f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fScaleSpeed    = 0.1f;
	tParticleDesc.fGravityPower  = 3.f;
	tParticleDesc.fLiveTime      = 1.5f;
	tParticleDesc.fMakeRange     = 0.f;
	tParticleDesc.fMaxDistance   = 30.f;
	tParticleDesc.eDir           = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.vDir           = __vector(0.f, 0.f, 0.f, 0.f);
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead          = pParentDead;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	tParticleDesc.pParentPosition = pPos;

	for (_uint i = 0; i < 5; ++i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
		}
	}
}

void CEffect_Manager::Create_IcicleEffect(CTransform* pTransform)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eEasing         = CEffect_Particle::OUTBOUNCE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;

	_float	fAddColor[4] = { -0.5f, 0.f, 0.f, 0.0f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 358;
	tParticleDesc.fMinSpeed      = 1.f;
	tParticleDesc.fMaxSpeed      = 20.f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fScaleSpeed    = 0.1f;
	tParticleDesc.fGravityPower  = 30.f;
	tParticleDesc.fLiveTime      = 5.f;
	tParticleDesc.fMakeRange     = 0.f;
	tParticleDesc.fMaxDistance   = 50.f;
	tParticleDesc.eDir           = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.vDir           = __vector(0.f, 0.f, 0.f, 0.f);
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead          = nullptr;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	pPos->y += 2.f;
	tParticleDesc.pParentPosition = pPos;

	if (nullptr == CGameInstance::GetInstance()->Add_GameObject_And_Return(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_IceManRoar(CTransform* pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance(); 
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado12"));
	tEffectMeshDesc.iTextureIndex      = 34;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 3.0f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position() + XMVector4Normalize(pTransform->Get_Look()) * 7.f + __vector(0.f, 2.5f, 0.f, 0.f));
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -0.3f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, -1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = true;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.3f, 0.3f, 0.3f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 5.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 0.f);
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = false;
	tEffectMeshDesc.bUsingDistortion   = true;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.1f, 0.f, 0.2f, 0.25f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	if (nullptr == pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc))
	{
		BREAKPOINT;
		return;
	}
}

HRESULT CEffect_Manager::Create_IceBallBreak(CTransform* pTransform)
{
	// 스파크
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_spark_flash"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 380;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.4f, 0.4f, 0.4f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 5.5f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, -1.75f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.8f, 1.4f, 0.6f, -0.7f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 파장(링)
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring05"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, pTransform->Get_Position());
	tEffectMeshDesc.vLookDir           = XMVector3Normalize(pTransform->Get_Look());
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.iTextureIndex      = 25;
	tEffectMeshDesc.iScondTextureIndex = 25;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.3f, 0.3f, 0.3f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 5.75f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, -2.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.3f, -0.3f, -0.5f, -0.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 파티클
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.iTexIndex       = 381;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = false;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ  = false;
	tParticleDesc.bExplosion      = true;
	tParticleDesc.fMakeRange      = 1.5f;
	tParticleDesc.vMakeRangeXYZ   = _float3(2.f, 2.f, 2.f);
	tParticleDesc.fMinSpeed       = 1.f;
	tParticleDesc.fMaxSpeed       = 10.f;
	tParticleDesc.fOriginScale    = 0.1f;
	tParticleDesc.fScaleSpeed     = 0.06f;
	tParticleDesc.fGravityPower   = 0.f;
	tParticleDesc.fLiveTime       = 1.f;
	tParticleDesc.fMaxDistance    = 0.f;
	tParticleDesc.fTrailInterval  = 0.f;
	tParticleDesc.iLevelIndex     = 0;
	tParticleDesc.eLifeType       = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing         = CEffect_Particle::OUTEXPO;

	_float	fAddColor[4] = { 0.5f, 0.27f, -0.38f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pTransform->Get_PositionFloat4_Pointer();

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Manager::Create_GoatTeleport(CTransform * pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 링 두개
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring04"));
	tEffectMeshDesc.iTextureIndex = 143;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 0.5f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.vPos = XMStoreFloat4RT(pTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f));
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -3.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.07f, 0.07f, 0.07f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 12.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(1.75f, 0.f);
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.8f, 0.f, 0.8f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform = nullptr;
	_matrix LocalMat = pTransform->Get_WorldMatrix();
	LocalMat.r[3] = __vector(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(pTransform->Get_NLook(), XMConvertToRadians(30.f));

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(pTransform->Get_NLook(), XMConvertToRadians(-30.f));

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}



	// 안쪽 구체
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tEffectMeshDesc.iTextureIndex = 28;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.fLifeTime = 0.5f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.vPos = XMStoreFloat4RT(pTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f));
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -4.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.7f, 0.7f, 0.7f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 13.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.25f, -0.75f);
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = false;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.f, -0.3f, 0.f, 1.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform = nullptr;
	LocalMat = pTransform->Get_WorldMatrix();
	LocalMat.r[3] = __vector(0.f, 0.f, 0.f, 1.f);

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}


	// 줄어드는 검정색
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tEffectMeshDesc.iTextureIndex = 120;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.fLifeTime = 0.5f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.vPos = XMStoreFloat4RT(pTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f));
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -3.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.vStartScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 1.5f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(1.75f, -2.f);
	tEffectMeshDesc.fStartU = -0.7f;
	tEffectMeshDesc.fStartV = 0.6f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = false;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-1.f, -1.f, -1.f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform = nullptr;
	LocalMat = pTransform->Get_WorldMatrix();
	LocalMat.r[3] = __vector(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.LocalMat_NoTrace = XMMatrixRotationAxis(pTransform->Get_NRight(), XMConvertToRadians(90.f));

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 퍼지는 원
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring01"));
	tEffectMeshDesc.iTextureIndex = 305;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.fLifeTime = 0.5f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.vPos = XMStoreFloat4RT(pTransform->Get_Position() + __vector(0.f, 1.f, 0.f, 0.f));
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = -2.5f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 14.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.25f, 0.f);
	tEffectMeshDesc.fStartU = 0.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(0.8f, 0.f, 0.8f, 0.f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform = nullptr;
	LocalMat = pTransform->Get_WorldMatrix();
	LocalMat.r[3] = __vector(0.f, 0.f, 0.f, 1.f);
	LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * LocalMat;

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// Patricle
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = false;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eEasing = CEffect_Particle::OUTBOUNCE;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_30;

	_float	fAddColor[4] = { 0.8f, 0.f, 1.0f, 0.0f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 0.8f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 353;
	tParticleDesc.fMinSpeed = 1.f;
	tParticleDesc.fMaxSpeed = 5.f;
	tParticleDesc.fOriginScale = 0.09f;
	tParticleDesc.fScaleSpeed = 0.17f;
	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 1.f;
	tParticleDesc.fMakeRange = 3.08f;
	tParticleDesc.fMaxDistance = 4.f;
	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.fTrailInterval = 0.f;
	tParticleDesc.pDead = nullptr;

	_float4* pPos = pTransform->Get_PositionFloat4_Pointer();
	//pPos->y ;
	tParticleDesc.pParentPosition = pPos;
	
	if (nullptr == CGameInstance::GetInstance()->Add_GameObject_And_Return(tParticleDesc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc))
	{
		BREAKPOINT;
		return;
	}
}

vector<CGameObject*> CEffect_Manager::Create_GoatSpear(_float4x4* pBoneMatrix)
{
	vector<CGameObject*> EffectObjects;
	EffectObjects.reserve(2);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 스피어
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Skill04"));
	//tEffectMeshDesc.iTextureIndex      = 10;
	tEffectMeshDesc.iTextureIndex      = 48;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	//tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.iScondTextureIndex = 1;
	tEffectMeshDesc.fLifeTime          = 10.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pBoneMatrix;
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	//tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparency      = 2.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.vStartScale        = __vector(0.008f, 0.008f, 0.008f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 14.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_NONLIGHT;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	//tEffectMeshDesc.vUVMovingSpeed     = _float2(0.25f, 3.f);
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.75f, 1.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	//tEffectMeshDesc.vColor             = _float4(1.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.vColor             = _float4(0.f, -1.f, -0.5f, 1.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f, 150.f, 0.f);

	CGameObject* pEffect = nullptr;
	pEffect = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);
	if (pEffect == nullptr)
	{
		BREAKPOINT;
		return EffectObjects;
	}
	else
	{
		EffectObjects.push_back(pEffect);
	}

	// 바람
	/*lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack05"));
	tEffectMeshDesc.iTextureIndex      = 224;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.fLifeTime          = 10.f;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pBoneMatrix;
	tEffectMeshDesc.vStartScale        = __vector(0.025f, 0.025f, 0.08f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 12.f);
	tEffectMeshDesc.vColor             = _float4(-1.f, -1.f, -1.f, -0.2f);
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.0001f, 50.f, -20.f);*/

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Big00"));
	tEffectMeshDesc.fTransparency        = 0.f;
	tEffectMeshDesc.iTextureIndex        = 76;
	tEffectMeshDesc.ePattern_MixTex      = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime            = 10.f;
	tEffectMeshDesc.ePattern_Move        = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix   = pBoneMatrix;
	tEffectMeshDesc.vStartScale          = __vector(0.03f, 0.03f, 0.03f, 0.f);
	tEffectMeshDesc.vMaxScale            = __vector(1.f, 1.f, 1.f, 0.f);
	tEffectMeshDesc.bMovingUV            = true;
	tEffectMeshDesc.vUVMovingSpeed       = _float2(1.25f, 2.25f);
	tEffectMeshDesc.fStartU              = -1.f;
	tEffectMeshDesc.vColor               = _float4(-1.f, -1.f, -1.f, -0.3f);
	//tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.0001f, 50.f, -20.f);
	tEffectMeshDesc.LocalMat             = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0001f, 50.f, 20.f);


	pEffect = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);
	if (pEffect == nullptr)
	{
		BREAKPOINT;
		return EffectObjects;
	}
	else
	{
		EffectObjects.push_back(pEffect);
	}

	return EffectObjects;
}

void CEffect_Manager::Create_GoatFlySwing(CTransform * pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 트레일
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
	tEffectMeshDesc.iTextureIndex      = 160;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.vStartScale        = __vector(0.035f, 0.03f, 0.035f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 14.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_NONLIGHT;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(4.f, 0.f);
	tEffectMeshDesc.fStartU            = -1.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.8f, -1.f, -0.8f, -0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-50.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(65.f)) * XMMatrixTranslation(30.f, 80.f, 40.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.vColor = _float4(-0.8f, -1.f, -1.f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(80.f)) * XMMatrixTranslation(30.f, 100.f, 40.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
	tEffectMeshDesc.vColor = _float4(-0.8f, -0.9f, -0.8f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(30.f, 90.f, 45.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.vColor = _float4(-1.f, -1.f, -1.f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(30.f, 60.f, 45.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_GoatDashSwing(CTransform * pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 트레일
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
	tEffectMeshDesc.iTextureIndex = 160;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.fLifeTime = 3.f;
	tEffectMeshDesc.pParentPos = nullptr;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex = 0;
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.fMaxTransparecy = 0.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.fSpinningSpeed = 0.f;
	tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding = false;
	tEffectMeshDesc.eLifeType = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.vStartScale = __vector(0.035f, 0.03f, 0.035f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed = 14.f;
	tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_NONLIGHT;
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(4.f, 0.f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.bUsingEmissive = false;
	tEffectMeshDesc.bUsingBlur = true;
	tEffectMeshDesc.bUsingDistortion = false;
	tEffectMeshDesc.eDyingPattern = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor = _float4(-0.8f, -1.f, -0.8f, -0.1f);
	tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform = nullptr;
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-50.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(225.f)) * XMMatrixTranslation(-30.f, 20.f, 40.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.vColor = _float4(-0.8f, -1.f, -1.f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(240.f)) * XMMatrixTranslation(-30.f, 40.f, 40.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
	tEffectMeshDesc.vColor = _float4(-0.8f, -0.9f, -0.8f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(200.f)) * XMMatrixTranslation(-30.f, 30.f, 45.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.vColor = _float4(-1.f, -1.f, -1.f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(200.f)) * XMMatrixTranslation(-30.f, 0.f, 45.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

}

void CEffect_Manager::Create_GoatJumpDashSwing(CTransform * pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 스피어
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Skill04"));
	tEffectMeshDesc.iTextureIndex      = 48;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 1;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 2.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 30.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.008f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.015f, 0.01f, 0.015f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.75f, 1.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.f, -1.f, -0.5f, 1.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat           = XMMatrixTranslation(0.f, 350.f, 0.f);

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 바람
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Big00"));
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.iTextureIndex = 76;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(1.25f, -2.25f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.vColor = _float4(-1.f, -1.f, -1.f, -0.3f);
	tEffectMeshDesc.LocalMat =XMMatrixTranslation(0.f, -30.f, 0.f);


	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 작은 흰색 링
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall01"));
	tEffectMeshDesc.iTextureIndex = 275;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 2.f);
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 50.f, 0.f);

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 번개같은 느낌의 이펙트
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack15"));
	tEffectMeshDesc.iTextureIndex = 370;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.fLifeTime = 1.f;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vStartScale = __vector(0.001f, 0.001f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.013f, 0.013f, 0.013f, 0.f);
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(3.25f, 1.75f);
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vColor = _float4(1.7f, 0.2f, 0.4f, 0.f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 50.f, 65.f);

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}


	// 이전 버전 ======================================================================
	//// 나선
	//lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack17"));
	//tEffectMeshDesc.iTextureIndex = 129;
	//tEffectMeshDesc.iScondTextureIndex = 28;
	//tEffectMeshDesc.fLifeTime = 1.f;
	//tEffectMeshDesc.fTransparecySpeed = -1.7f;
	//tEffectMeshDesc.bSpinning = true;
	//tEffectMeshDesc.fSpinningSpeed = 30.f;
	//tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	//tEffectMeshDesc.eEasing_Scale = CEffect_Mesh::OUTQUAD;
	//tEffectMeshDesc.ePattern_Scale = CEffect_Mesh::PATTERN_SHRINK;
	//tEffectMeshDesc.vStartScale = __vector(0.001f, 0.04f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale = __vector(0.04f, 0.08f, 0.04f, 0.f);
	//tEffectMeshDesc.fScaleSpeed = 12.f;
	//tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	//tEffectMeshDesc.bMovingUV = false;
	//tEffectMeshDesc.bUsingBlur = true;
	//tEffectMeshDesc.vColor = _float4(-1.f, -1.f, -1.f, -0.2f);
	//tEffectMeshDesc.vBlendColor = _float4(0.f, 0.f, 0.f, 0.f);
	//tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, 50.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}

	//// 왕관
	//lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Thorn"));
	//tEffectMeshDesc.iTextureIndex = 10;
	//tEffectMeshDesc.fLifeTime = 1.f;
	//tEffectMeshDesc.fTransparecySpeed = -2.f;
	//tEffectMeshDesc.bSpinning = true;
	//tEffectMeshDesc.fSpinningSpeed = 60.f;
	//tEffectMeshDesc.vSpinAxis = __vector(0.f, 1.f, 0.f, 0.f);
	//tEffectMeshDesc.vStartScale = __vector(0.001f, 0.025f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale = __vector(0.025f, 0.025f, 0.025f, 0.f);
	//tEffectMeshDesc.fScaleSpeed = 10.f;
	//tEffectMeshDesc.eRenderGroup = CRenderer::RENDER_BLEND;
	//tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	//tEffectMeshDesc.bUsingBlur = true;
	//tEffectMeshDesc.vColor = _float4(1.f, 0.f, -0.1f, 0.f);
	//tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, 180.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}
	//tEffectMeshDesc.vStartScale = __vector(0.001f, 0.025f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale = __vector(0.0375f, 0.025f, 0.0375f, 0.f);
	//tEffectMeshDesc.fScaleSpeed = 10.f;
	//tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, 90.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}
	//tEffectMeshDesc.vStartScale = __vector(0.001f, 0.025f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale = __vector(0.0425f, 0.025f, 0.0425f, 0.f);
	//tEffectMeshDesc.fScaleSpeed = 15.f;
	//tEffectMeshDesc.LocalMat = XMMatrixTranslation(0.f, 0.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}
}

void CEffect_Manager::Create_GoatUnderSwing(CTransform* pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 스피어
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Skill04"));
	tEffectMeshDesc.iTextureIndex      = 48;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 1;
	tEffectMeshDesc.fLifeTime          = 0.25f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -0.3f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 30.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.008f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.015f, 0.01f, 0.015f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.75f, 1.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.f, -1.f, -0.5f, 1.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat           = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 250.f, 0.f);
	
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 바람
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ribbon_Big00"));
	tEffectMeshDesc.fTransparency = 0.f;
	tEffectMeshDesc.iTextureIndex = 76;
	tEffectMeshDesc.fTransparecySpeed = 0.f;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.fLifeTime = 0.4f;
	tEffectMeshDesc.bSpinning = false;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(1.25f, -2.25f);
	tEffectMeshDesc.fStartU = -1.f;
	tEffectMeshDesc.vColor = _float4(-1.f, -1.f, -1.f, -0.3f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 150.f, 0.f);


	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 작은 흰색 링
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_BigTreeFall01"));
	tEffectMeshDesc.iTextureIndex = 275;
	tEffectMeshDesc.ePattern_MixTex = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.fLifeTime = 0.4f;
	tEffectMeshDesc.ePattern_Move = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.ePlayType = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.vStartScale = __vector(0.01f, 0.01f, 0.01f, 0.f);
	tEffectMeshDesc.vMaxScale = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.bMovingUV = true;
	tEffectMeshDesc.vUVMovingSpeed = _float2(0.f, 2.f);
	tEffectMeshDesc.fStartV = 0.f;
	tEffectMeshDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 25.f, 0.f);

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	// 번개같은 느낌의 이펙트
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack15"));
	tEffectMeshDesc.iTextureIndex      = 370;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.fLifeTime          = 0.5f;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.001f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.013f, 0.013f, 0.013f, 0.f);
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(3.25f, 1.75f);
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.vColor             = _float4(1.7f, 0.2f, 0.4f, 0.f);
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 150.f, 100.f);

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT; 
		return;
	}
	//// 나선
	//lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Attack17"));
	//tEffectMeshDesc.iTextureIndex      = 129;
	//tEffectMeshDesc.iScondTextureIndex = 28;
	//tEffectMeshDesc.fLifeTime          = 0.25f;
	//tEffectMeshDesc.fTransparecySpeed  = -0.3f;
	//tEffectMeshDesc.bSpinning          = true;
	//tEffectMeshDesc.fSpinningSpeed     = 30.f;
	//tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	//tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	//tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SHRINK;
	//tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.04f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale          = __vector(0.04f, 0.08f, 0.04f, 0.f);
	//tEffectMeshDesc.fScaleSpeed        = 12.f;
	//tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	//tEffectMeshDesc.bMovingUV          = false;
	//tEffectMeshDesc.bUsingBlur         = true;
	//tEffectMeshDesc.vColor             = _float4(-1.f, -1.f, -1.f, -0.2f);
	//tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	//tEffectMeshDesc.LocalMat           = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 50.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}

	//// 왕관
	//lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Thorn"));
	//tEffectMeshDesc.iTextureIndex     = 10;
	//tEffectMeshDesc.fLifeTime         = 0.25f;
	//tEffectMeshDesc.fTransparecySpeed = -0.3f;
	//tEffectMeshDesc.bSpinning         = true;
	//tEffectMeshDesc.fSpinningSpeed    = 60.f;
	//tEffectMeshDesc.vSpinAxis         = __vector(0.f, 1.f, 0.f, 0.f);
	//tEffectMeshDesc.vStartScale       = __vector(0.001f, 0.025f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale         = __vector(0.025f, 0.025f, 0.025f, 0.f);
	//tEffectMeshDesc.fScaleSpeed       = 10.f;
	//tEffectMeshDesc.eRenderGroup      = CRenderer::RENDER_BLEND;
	//tEffectMeshDesc.ePlayType         = CEffect_Mesh::PLAY_LOOP;
	//tEffectMeshDesc.bUsingBlur        = true;
	//tEffectMeshDesc.vColor            = _float4(1.f, 0.f, -0.1f, 0.f);
	//tEffectMeshDesc.LocalMat          = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 180.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}
	//tEffectMeshDesc.vStartScale = __vector(0.001f, 0.025f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale   = __vector(0.0375f, 0.025f, 0.0375f, 0.f);
	//tEffectMeshDesc.fScaleSpeed = 10.f;
	//tEffectMeshDesc.LocalMat    = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 90.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}
	//tEffectMeshDesc.vStartScale = __vector(0.001f, 0.025f, 0.001f, 0.f);
	//tEffectMeshDesc.vMaxScale   = __vector(0.0425f, 0.025f, 0.0425f, 0.f);
	//tEffectMeshDesc.fScaleSpeed = 15.f;
	//tEffectMeshDesc.LocalMat    = XMMatrixRotationX(XMConvertToRadians(180.f)) * XMMatrixTranslation(0.f, 0.f, 0.f);
	//if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	//{
	//	BREAKPOINT;
	//	return;
	//}
}

vector<CGameObject*> CEffect_Manager::Create_GoatSpinDashSwing(CTransform * pTransform)
{
	vector<CGameObject*> EffectObjects;
	EffectObjects.reserve(2);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 트레일
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Ring05"));
	tEffectMeshDesc.iTextureIndex      = 164;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.fLifeTime          = 100.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 30.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.vStartScale        = __vector(0.75f, 0.4f, 0.75f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.015f, 0.01f, 0.015f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.5f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.4f, -0.9f, -0.5f, -0.2f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat           = XMMatrixTranslation(0.f, 3.f, 0.f);

	CGameObject* pEffect = nullptr;

	pEffect = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);

	if (pEffect == nullptr)
	{
		BREAKPOINT;
	}
	else
	{
		EffectObjects.push_back(pEffect);
	}

	// 회전
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado00"));
	tEffectMeshDesc.iTextureIndex      = 37;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.iScondTextureIndex = 97;
	tEffectMeshDesc.fLifeTime          = 100.f;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 34.5f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.vStartScale        = __vector(0.02f, 0.001f, 0.02f, 0.f);
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(0.f, 1.5f);
	tEffectMeshDesc.vColor             = _float4(0.9f, -0.4f, -0.6f, -0.2f);
	tEffectMeshDesc.LocalMat           = XMMatrixTranslation(0.f, 1000.f, 0.f);

	pEffect = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);

	if (pEffect == nullptr)
	{
		BREAKPOINT;
	}
	else
	{
		EffectObjects.push_back(pEffect);
	}

	return EffectObjects;
}

void CEffect_Manager::Create_SpinDashSwingFinish(CTransform* pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 트레일
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Slash01"));
	tEffectMeshDesc.iTextureIndex      = 160;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NONE;
	tEffectMeshDesc.iScondTextureIndex = 28;
	tEffectMeshDesc.fLifeTime          = 3.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.vStartScale        = __vector(0.035f, 0.03f, 0.035f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 14.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_NONLIGHT;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_ONCE;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(4.f, 0.f);
	tEffectMeshDesc.fStartU            = -1.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.8f, -1.f, -0.8f, -0.1f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-50.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(65.f)) * XMMatrixTranslation(30.f, 80.f, 40.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.vColor   = _float4(-0.8f, -1.f, -1.f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(80.f)) * XMMatrixTranslation(30.f, 100.f, 40.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
	tEffectMeshDesc.vColor   = _float4(-0.8f, -0.9f, -0.8f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(30.f, 90.f, 45.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	tEffectMeshDesc.vColor   = _float4(-1.f, -1.f, -1.f, -0.1f);
	tEffectMeshDesc.LocalMat = XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f)) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(40.f)) * XMMatrixTranslation(30.f, 60.f, 45.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

CTransform* CEffect_Manager::Create_GoatPillarOfFire(CTransform * pTransform)
{
	// 얘는 처음 등장할 때 트랜스폼 받아서 가지고 저 밑에서 올라오게 해야돼
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 불기둥
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Pillar01"));
	tEffectMeshDesc.iTextureIndex      = 335;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 0.25f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(0.f, 2.f);
	tEffectMeshDesc.iScondTextureIndex = 36;
	tEffectMeshDesc.fLifeTime          = 10.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.vPos               = XMStoreFloat4RT(pTransform->Get_Position()/* + __vector(0.f, -70.f, 0.f, 0.f)*/);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 30.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.vStartScale        = __vector(0.2f, 0.2f, 0.2f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.015f, 0.01f, 0.015f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.9f, -0.3f, -0.6f, 0.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	//tEffectMeshDesc.LocalMat_NoTrace = XMMatrixTranslation(0.f, -21.f, 0.f);

	// 	tEffectMeshDesc.vUVMovingSpeed     = _float2(2.f, 0.f);
	// 	tEffectMeshDesc.vNoiseMovingSpeed     = _float2(0.f, 0.5f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return nullptr;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase = false;
	tParticleDesc.bLoop = false;
	tParticleDesc.bTrail = true;
	tParticleDesc.bUseEasing = false;
	tParticleDesc.eDir = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir = __vector(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ = false;
	tParticleDesc.fMakeRange = 40.f;
	tParticleDesc.fMaxDistance = 50.f;
	tParticleDesc.vMakeRangeXYZ = _float3(2.f, 2.f, 2.f);


	_float	fAddColor[4] = { 1.f, 0.27f, 0.06f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower = 0.f;
	tParticleDesc.fLiveTime = 10.f;
	tParticleDesc.fMinSpeed = 10.f;
	tParticleDesc.fMaxSpeed = 20.f;
	tParticleDesc.fScaleSpeed = 0.25f;
	tParticleDesc.fOriginScale = 0.3f;
	tParticleDesc.fTrailInterval = 0.01f;
	tParticleDesc.iLevelIndex = 0;
	tParticleDesc.iTexIndex = 353;
	tParticleDesc.eLifeType = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing = CEffect_Particle::INSINE;
	tParticleDesc.bNotTracing = false;

	//_vector vPos = XMLoadFloat4(pPos) + vOffsetPos;
	//XMStoreFloat4(pPos, vPos);


	_float4* pPosition = pTransform->Get_PositionFloat4_Pointer();

	auto vPosition = XMLoadFloat4(pPosition);

	_float4	vPos;

	XMStoreFloat4(&vPos, vPosition);

	*pPosition = vPos;

	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPosition;

	for(_uint i = 0; i < 2; ++ i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return nullptr;
		}
	}
	
	return pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect"))->Get_Transform();
}

void CEffect_Manager::Create_GoatPillarOfFire_Short(CTransform * pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 불기둥
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Pillar01"));
	tEffectMeshDesc.iTextureIndex      = 335;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 0.25f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(0.f, 2.f);
	tEffectMeshDesc.iScondTextureIndex = 36;
	tEffectMeshDesc.fLifeTime          = 5.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.vPos               = XMStoreFloat4RT(pTransform->Get_Position()/* + __vector(0.f, -70.f, 0.f, 0.f)*/);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 30.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.2f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.2f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.9f, -0.3f, -0.6f, 0.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	//tEffectMeshDesc.LocalMat_NoTrace = XMMatrixTranslation(0.f, -21.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir            = __vector(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ  = false;
	tParticleDesc.fMakeRange      = 10.f;
	tParticleDesc.fMaxDistance    = 20.f;
	tParticleDesc.vMakeRangeXYZ   = _float3(2.f, 2.f, 2.f);


	_float	fAddColor[4] = { 1.f, 0.27f, 0.06f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 6.f;
	tParticleDesc.fMinSpeed      = 10.f;
	tParticleDesc.fMaxSpeed      = 20.f;
	tParticleDesc.fScaleSpeed    = 0.25f;
	tParticleDesc.fOriginScale   = 0.3f;
	tParticleDesc.fTrailInterval = 0.01f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 353;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing        = CEffect_Particle::INSINE;
	tParticleDesc.bNotTracing    = false;

	//_vector vPos = XMLoadFloat4(pPos) + vOffsetPos;
	//XMStoreFloat4(pPos, vPos);


	_float4* pPosition = pTransform->Get_PositionFloat4_Pointer();
	auto vPosition = XMLoadFloat4(pPosition);
	_float4	vPos;
	XMStoreFloat4(&vPos, vPosition);
	*pPosition = vPos;
	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPosition;

	for (_uint i = 0; i < 2; ++i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return;
		}
	}
}

void CEffect_Manager::Create_GoatPillarOfFire_Short_Ready(CTransform * pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 불기둥
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Pillar01"));
	tEffectMeshDesc.iTextureIndex      = 335;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 0.25f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(0.f, 2.f);
	tEffectMeshDesc.iScondTextureIndex = 36;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_NONE;
	tEffectMeshDesc.vPos               = XMStoreFloat4RT(pTransform->Get_Position()/* + __vector(0.f, -70.f, 0.f, 0.f)*/);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 30.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SCALE_NONE;
	tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.2f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.2f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.9f, -0.3f, -0.6f, 0.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	//tEffectMeshDesc.LocalMat_NoTrace = XMMatrixTranslation(0.f, -21.f, 0.f);
	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}
}

void CEffect_Manager::Create_GoatPillarOfFire_Rotation(CTransform * pTransform)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// 불기둥
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Pillar01"));
	tEffectMeshDesc.iTextureIndex      = 335;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_NOISING;
	tEffectMeshDesc.fNoisePower        = 0.25f;
	tEffectMeshDesc.vNoiseMovingSpeed  = _float2(0.f, 2.f);
	tEffectMeshDesc.iScondTextureIndex = 36;
	tEffectMeshDesc.fLifeTime          = 10.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vPos               = XMStoreFloat4RT(pTransform->Get_Position());
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 30.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTQUAD;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_SHRINK;
	tEffectMeshDesc.vStartScale        = __vector(0.001f, 0.2f, 0.001f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.05f, 0.2f, 0.05f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 12.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = false;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.f, 0.f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(-0.2f, -0.5f, 0.f, 0.5f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;

	if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return;
	}

	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = false;
	tParticleDesc.eDir            = CEffect_Particle::DIR_ONE;
	tParticleDesc.vDir            = __vector(0.f, 1.f, 0.f, 0.f);
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ  = false;
	tParticleDesc.fMakeRange      = 20.f;
	tParticleDesc.fMaxDistance    = 30.f;
	tParticleDesc.vMakeRangeXYZ   = _float3(2.f, 2.f, 2.f);


	_float	fAddColor[4] = { 1.f, 0.2f, 0.8f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 10.f;
	tParticleDesc.fMinSpeed      = 10.f;
	tParticleDesc.fMaxSpeed      = 20.f;
	tParticleDesc.fScaleSpeed    = 0.25f;
	tParticleDesc.fOriginScale   = 0.3f;
	tParticleDesc.fTrailInterval = 0.01f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 353;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing        = CEffect_Particle::INSINE;
	tParticleDesc.bNotTracing    = false;

	//_vector vPos = XMLoadFloat4(pPos) + vOffsetPos;
	//XMStoreFloat4(pPos, vPos);


	_float4* pPosition = pTransform->Get_PositionFloat4_Pointer();
	auto vPosition = XMLoadFloat4(pPosition);
	_float4	vPos;
	XMStoreFloat4(&vPos, vPosition);
	*pPosition = vPos;
	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPosition;

	for (_uint i = 0; i < 2; ++i)
	{
		if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return;
		}
	}
}

void CEffect_Manager::Create_GoatSmoke(_fvector vPosition, _uint iNum, _float fMinX, _float fMaxX, _float fMinY, _float fMaxY, _float fMinZ, _float fMaxZ, _float fInterval)
{
	/*호출 예시*/
	//CEffect_Manager::GetInstance()->Create_GoatSmoke(m_pTransformCom->Get_Position(), 60, -1.f, 1.f, 1.f, 5.f, -1.f, 1.f, 12.f);
	//CEffect_Manager::GetInstance()->Create_GoatSmoke(m_pTransformCom->Get_Position(), 40, -1.f, 1.f, 0.f, 1.f, -1.f, 1.f, 12.f);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < iNum; i++)
	{
		_float	fX = RandomRange(fMinX, fMaxX);
		_float	fY = RandomRange(fMinY, fMaxY);
		_float	fZ = RandomRange(fMinZ, fMaxZ);

		_vector vDir = XMVector3Normalize(__vector(fX, 0.f, fZ, 0.f));

		_vector vPos = vPosition;

		vPos += __vector(fX, fY, fZ, 0.f) + vDir * fInterval;
		m_TextureDesc[TEXTURE_GOATSMOKE].vPosition = XMStoreFloat3RT(vPos);
		m_TextureDesc[TEXTURE_GOATSMOKE].vMovingDir = XMStoreFloat3RT(__vector(fX, fY - 0.1f, fZ, 0.f));
		m_TextureDesc[TEXTURE_GOATSMOKE].fPlaySpeed = (5.f - fY) * (5.f - fY) / (5.f) + 0.1f;

		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectTexture"), &m_TextureDesc[TEXTURE_GOATSMOKE])))
		{
			BREAKPOINT;
			return;
		}
	}
}

CGameObject* CEffect_Manager::Create_GoatParticle(CTransform* pTransform, _fvector vOffset)
{
	CEffect_Particle::PARTICLEDESC tParticleDesc;
	tParticleDesc.bChase          = false;
	tParticleDesc.bLoop           = false;
	tParticleDesc.bTrail          = true;
	tParticleDesc.bUseEasing      = true;
	tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
	tParticleDesc.eParticleNumber = CEffect_Particle::NUM_50;
	tParticleDesc.bRangeOneOrXYZ  = false;
	tParticleDesc.fMakeRange      = 2.f;
	tParticleDesc.fMaxDistance    = 2.f;
	tParticleDesc.vMakeRangeXYZ   = _float3(2.f, 2.f, 2.f);


	_float	fAddColor[4] = { 1.f, 0.4f, 1.f, 0.f };
	memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

	_float	fMulColor[4] = { 1.f, 1.f, 1.f, 1.f };
	memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

	tParticleDesc.fGravityPower  = 0.f;
	tParticleDesc.fLiveTime      = 0.5f;
	tParticleDesc.fMinSpeed      = 2.f;
	tParticleDesc.fMaxSpeed      = 2.f;
	tParticleDesc.fScaleSpeed    = 0.25f;
	tParticleDesc.fOriginScale   = 0.1f;
	tParticleDesc.fTrailInterval = 0.015f;
	tParticleDesc.iLevelIndex    = 0;
	tParticleDesc.iTexIndex      = 353;
	tParticleDesc.eLifeType      = CEffect_Particle::TIME_LIMIT;
	tParticleDesc.eEasing        = CEffect_Particle::INSINE;
	tParticleDesc.bNotTracing    = true;

	_float4* pPosition = pTransform->Get_PositionFloat4_Pointer();
	auto vPosition = XMLoadFloat4(pPosition) + vOffset;
	_float4	vPos;

	XMStoreFloat4(&vPos, vPosition);

	*pPosition = vPos;

	tParticleDesc.iPass = 2;
	tParticleDesc.pParentPosition = pPosition;

	CGameObject* pEffect = nullptr;
	pEffect = CGameInstance::GetInstance()->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc);
	if (pEffect == nullptr)
	{
		BREAKPOINT;
	}

	return pEffect;
}

HRESULT CEffect_Manager::Create_Goat_FireBallRing(CTransform* pTransform, _bool* pParentDead)
{
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	auto vPosition = pTransform->Get_Position();

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Circle00"));
	tEffectMeshDesc.iTextureIndex      = 79;
	tEffectMeshDesc.iScondTextureIndex = 21;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 10.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.pParentDead        = pParentDead;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bAlphaReverse      = false;
	tEffectMeshDesc.bSpinning          = true;
	tEffectMeshDesc.fSpinningSpeed     = 3.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(1.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.4f, 0.4f, 0.4f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 10.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(-0.5f, 3.f);
	tEffectMeshDesc.fStartU            = 1.f;
	tEffectMeshDesc.fStartV            = -1.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.7f, 0.1f, -1.8f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat           = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(23.5f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Goat_FireBallExplosion(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Texture::EFFECT_TEX_DESC      tDesc_Texture;

	// Smoke
	auto vPosition = pTransform->Get_Position();
	tDesc_Texture.iPassIndex = 5;
	tDesc_Texture.iTextureIndex = 378;
	tDesc_Texture.iLevelIndex = LEVEL_STATIC;

	tDesc_Texture.fPlaySpeed = 3.f;
	tDesc_Texture.bBillboarding = true;

	XMStoreFloat3(&tDesc_Texture.vPosition, vPosition);

	tDesc_Texture.eRenderGroup = CRenderer::RENDER_BLEND;

	tDesc_Texture.eLifeType = CEffect_Texture::SPRITE_LIMIT;

	tDesc_Texture.bSprite = true;
	tDesc_Texture.ePlayType = CEffect_Texture::PLAY_ONCE;
	tDesc_Texture.fSpriteMaxXIndex = 8;
	tDesc_Texture.fSpriteMaxYIndex = 6;
	tDesc_Texture.fSpriteSpeed = 12.75f;

	tDesc_Texture.bSpinning = false;

	tDesc_Texture.fSpinSpeed = 2.5f;

	tDesc_Texture.vMinSize = _float3(3.f, 3.f, 3.f);
	tDesc_Texture.vMaxSize = _float3(6.f, 6.f, 6.f);
	tDesc_Texture.fScalingSpeed = 0.f;
	tDesc_Texture.eScalePattern = CEffect_Texture::PATTERN_SCALE_NONE;

	tDesc_Texture.vBaseColor = _float4(0.f, 0.f, 0.f, 0.f);
	tDesc_Texture.vBlendingColor = _float4(0.14f, 0.f, 0.24f, 1.f);

	tDesc_Texture.fStartAlpha = 0.f;
	tDesc_Texture.fTransparecySpeed = 0.f;
	tDesc_Texture.fTotalTime = 1.f;

	tDesc_Texture.bBlur = false;
	tDesc_Texture.bDistortion = false;

	if (FAILED(pInstance->Add_GameObject(pInstance->Get_LevelIndex(), m_pLayerTag, m_pTextureProtoTag, &tDesc_Texture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Manager::Create_Goat_FireBall(CTransform* pTransform, _bool* pParentDead)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	// FireBall
	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Sphere04"));
	tEffectMeshDesc.iTextureIndex      = 87;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.iScondTextureIndex = 13;
	tEffectMeshDesc.fLifeTime          = 10.f;
	tEffectMeshDesc.pParentPos         = nullptr;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.pParentDead        = pParentDead;
	tEffectMeshDesc.vPos               = _float4(0.f, 0.f, 0.f, 1.f);
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, 1.f, 0.f);
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 2.f;
	tEffectMeshDesc.fTransparecySpeed  = 0.f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 0.f;
	tEffectMeshDesc.vSpinAxis          = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.eEasing_Scale      = CEffect_Mesh::OUTELASTIC;
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.vStartScale        = __vector(0.1f, 0.1f, 0.1f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(0.8f, 0.8f, 0.8f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 15.f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_NONLIGHT;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(1.5f, 1.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(0.5f, 0.3f, 0.0f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.pTarget_Transform  = nullptr;
	tEffectMeshDesc.LocalMat           = XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	CGameObject* pEffect = nullptr;
	pEffect = pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc);
}

HRESULT CEffect_Manager::Create_Goat_PillarWave(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Texture::EFFECT_TEX_DESC      tDesc_Texture;

	// Pillar
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	auto vPosition = pTransform->Get_Position() + __vector(0.f, 35.f, 0.f, 0.f);

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado13"));
	XMStoreFloat4(&tEffectMeshDesc.vPos, vPosition);
	tEffectMeshDesc.iTextureIndex      = 321;
	tEffectMeshDesc.iScondTextureIndex = 40;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -1.2f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bAlphaReverse      = false;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 3.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(1.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_MOVE_ONEDIR;
	tEffectMeshDesc.vDir               = __vector(0.f, 1.f, 0.f, 0.f);
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = nullptr;
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.05f, 0.05f, 0.05f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(6.f, 6.f, 6.f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 7.25f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(2.75f, 0.5f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.2f, 0.3f, -0.2f, 0.2f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationZ(XMConvertToRadians(180.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Manager::Create_Goat_PillarWave_Rotation(CTransform* pTransform)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CEffect_Texture::EFFECT_TEX_DESC      tDesc_Texture;

	// Pillar
	CEffect_Mesh::EFFECT_MESHDESC tEffectMeshDesc;

	lstrcpy(tEffectMeshDesc.ModelTag, TEXT("Prototype_Component_Model_Tornado13"));
	tEffectMeshDesc.iTextureIndex      = 321;
	tEffectMeshDesc.iScondTextureIndex = 40;
	tEffectMeshDesc.ePattern_MixTex    = CEffect_Mesh::MIX_MULT;
	tEffectMeshDesc.fLifeTime          = 1.f;
	tEffectMeshDesc.iPassIndex         = 0;
	tEffectMeshDesc.fTransparency      = 0.f;
	tEffectMeshDesc.fTransparecySpeed  = -1.2f;
	tEffectMeshDesc.fMaxTransparecy    = 0.f;
	tEffectMeshDesc.bAlphaReverse      = false;
	tEffectMeshDesc.bSpinning          = false;
	tEffectMeshDesc.fSpinningSpeed     = 3.25f;
	tEffectMeshDesc.vSpinAxis          = __vector(1.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.vLookDir           = __vector(0.f, 0.f, -1.f, 0.f);
	tEffectMeshDesc.bBillboarding      = false;
	tEffectMeshDesc.eLifeType          = CEffect_Mesh::TIME_LIMIT;
	tEffectMeshDesc.ePattern_Move      = CEffect_Mesh::PATTERN_TRACING;
	tEffectMeshDesc.vDir               = __vector(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.ePattern_Scale     = CEffect_Mesh::PATTERN_BULGE;
	tEffectMeshDesc.pTargetWorldMatrix = pTransform->Get_WorldFloat4x4_Pointer();
	tEffectMeshDesc.bUseEasing_Scaling = false;
	tEffectMeshDesc.vStartScale        = __vector(0.025f, 0.025f, 0.025f, 0.f);
	tEffectMeshDesc.vMaxScale          = __vector(1.25f, 1.25f, 1.25f, 0.f);
	tEffectMeshDesc.fScaleSpeed        = 7.25f;
	tEffectMeshDesc.eRenderGroup       = CRenderer::RENDER_BLEND;
	tEffectMeshDesc.ePlayType          = CEffect_Mesh::PLAY_LOOP;
	tEffectMeshDesc.bMovingUV          = true;
	tEffectMeshDesc.vUVMovingSpeed     = _float2(5.0f, 1.0f);
	tEffectMeshDesc.fStartU            = 0.f;
	tEffectMeshDesc.fStartV            = 0.f;
	tEffectMeshDesc.bUsingEmissive     = false;
	tEffectMeshDesc.bUsingBlur         = true;
	tEffectMeshDesc.bUsingDistortion   = false;
	tEffectMeshDesc.eDyingPattern      = CEffect_Mesh::DYING_ADD;
	tEffectMeshDesc.vColor             = _float4(1.2f, -0.1f, 1.f, 0.f);
	tEffectMeshDesc.vBlendColor        = _float4(0.f, 0.f, 0.f, 0.f);
	tEffectMeshDesc.LocalMat           = XMMatrixIdentity();
	tEffectMeshDesc.LocalMat_NoTrace   = XMMatrixRotationZ(XMConvertToRadians(180.f));

	if (FAILED(CGameInstance::GetInstance()->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_EffectMesh"), &tEffectMeshDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}
