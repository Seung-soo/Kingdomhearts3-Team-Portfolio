#include "stdafx.h"
#include "..\public\Effect_Mesh.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CEffect_Mesh::CEffect_Mesh(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Mesh::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Mesh::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(30.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(EFFECT_MESHDESC));

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// =====================================================================================================

	if (true == m_tDesc.bUseEasing || true == m_tDesc.bUseEasing_Scaling)
		Set_EasingFunction();

	m_TransMat = XMMatrixTranslation(m_tDesc.vPos.x, m_tDesc.vPos.y, m_tDesc.vPos.z);

	if (m_tDesc.bMovingUV)
		m_tDesc.iPassIndex = 39;

	m_fMovingU = m_tDesc.fStartU;
	m_fMovingV = m_tDesc.fStartV;

	switch (m_tDesc.eDyingPattern)
	{
	case DYING_MULT:				//색깔 곱하기
		m_tDesc.iPassIndex = 34;
		break;
	case DYING_ADD:					//색깔 더하기
		m_tDesc.iPassIndex = 35;
		break;
	case DYING_COVER:				//색깔 입히기
		m_tDesc.iPassIndex = 33;
		break;
	case DYING_ONEVALUE:			//색깔 입히기
		m_tDesc.iPassIndex = 36;
		break;		
	}


	switch (m_tDesc.ePattern_MixTex)
	{
	case MIX_MULT:					// 텍스쳐 곱하기
		m_tDesc.iPassIndex = 38;
		break;
	case MIX_ADD:					// 텍스쳐 더하기
		m_tDesc.iPassIndex = 37;
		break;
	case MIX_NOISING:				// 노이즈 입히기
			m_tDesc.iPassIndex = 8;
		break;
	}

	if (m_tDesc.ePattern_Move == PATTERN_MOVE_RANDDIR)
	{
		//랜덤으로 방향 설정
		m_tDesc.vDir = XMVector4Normalize(XMVectorSet(_float(rand() % 360 - 180), _float(rand() % 360 - 180), _float(rand() % 360 - 180), 0.f));
	}

	if (m_tDesc.ePattern_Move != PATTERN_MOVE_NONE)
	{
		m_vFinalPos = XMLoadFloat4(&m_tDesc.vPos) + XMVector3Normalize(m_tDesc.vDir) * m_tDesc.fMovingDistance;
	}

	m_tDesc.vSpinAxis = XMVector3Normalize(m_tDesc.vSpinAxis);
	m_tDesc.vSpinAxis = XMVectorSetW(m_tDesc.vSpinAxis, 0.f);

	// =====================================================================================================

	m_fTransparency = m_tDesc.fTransparency;
	m_vCurrentScale = m_tDesc.vStartScale;

	_float4			vPosition = m_tDesc.vPos;

	if (m_tDesc.ePattern_Move == PATTERN_TRACING)
	{
		SynchToTarget();
	}

	else
	{
		m_pTransformCom->Scaled(m_vCurrentScale);
		m_pTransformCom->MakeSameDirection(m_tDesc.vLookDir);

		if (m_tDesc.ePattern_Move == PATTERN_TRACING_POSONLY)
		{
			SynchToTarget_OnlyPos();
		}

		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));

		m_pTransformCom->Set_WorldMatrix(m_tDesc.LocalMat_NoTrace * m_pTransformCom->Get_WorldMatrix());
	}

	LEVEL current_level = (LEVEL)CGameInstance::GetInstance()->Get_LevelIndex();

	if (LEVEL_EFFECTTOOL == current_level)
	{
		m_pTransformCom->AllRotation(__vector(
		XMConvertToRadians(m_tDesc.fRotationAngle.x),
		XMConvertToRadians(m_tDesc.fRotationAngle.y),
		XMConvertToRadians(m_tDesc.fRotationAngle.z), 0.f));
	}
	else
	{
		if (m_tDesc.fRotationAngle.x != 0.f)
		{
			m_pTransformCom->Rotation(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), XMConvertToRadians(m_tDesc.fRotationAngle.x));
		}

		if (m_tDesc.fRotationAngle.y != 0.f)
		{
			m_pTransformCom->Rotation(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)), XMConvertToRadians(m_tDesc.fRotationAngle.y));
		}

		if (m_tDesc.fRotationAngle.z != 0.f)
		{
			m_pTransformCom->Rotation(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), XMConvertToRadians(m_tDesc.fRotationAngle.z));
		}
	}
	
	// =====================================================================================================

	if (true == m_tDesc.bBillboarding)
	{
		if (false == m_tDesc.bNoRandomBillborad)
		{
			auto vRandAxis = XMVectorSet(RandomRange(-1.f, 1.f), RandomRange(-1.f, 1.f), RandomRange(-1.f, 1.f), 0.f);

			auto fRandAngle = RandomRange(-30.f, 30.f);

			m_RandMatrix = XMMatrixRotationAxis(vRandAxis, XMConvertToRadians(fRandAngle));

			auto vRight = XMVector3TransformNormal(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), m_RandMatrix);
			auto vUp = XMVector3TransformNormal(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)), m_RandMatrix);
			auto vLook = XMVector3TransformNormal(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), m_RandMatrix);

			m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
			m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
			m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));
		}

		else
			m_RandMatrix = m_tDesc.LocalMat;

	}

	return S_OK;
}

_int CEffect_Mesh::Tick(_double TimeDelta)
{
 	if (false == m_isActive)
		return RESULT_NOPROBLEM;

	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_bDead)
		return RESULT_DEAD;

	if (m_tDesc.pParentDead != nullptr && *m_tDesc.pParentDead == true)
		return RESULT_DEAD;

	m_fTimeAccum += (_float)TimeDelta;

	if (m_fTimeAccum >= m_tDesc.fLifeTime)
	{
		if(TIME_LIMIT == m_tDesc.eLifeType)
			m_bDead = true;
	}

	// 노이즈 텍스쳐 관련 코드
	if (MIX_NOISING == m_tDesc.ePattern_MixTex)
	{
		m_fMovingNoiseX += (_float)TimeDelta * m_tDesc.vNoiseMovingSpeed.x;
		m_fMovingNoiseY += (_float)TimeDelta * m_tDesc.vNoiseMovingSpeed.y;
	}


	// 투명도 관련 코드
	m_fTransparency += (_float)TimeDelta * m_tDesc.fTransparecySpeed;

	if (m_tDesc.fMaxTransparecy < 0)
	{
		if (m_fTransparency <= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;
		}
	}
	else
	{
		if (m_fTransparency >= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;

			if (true == m_tDesc.bAlphaReverse)
				m_tDesc.fTransparecySpeed = -m_tDesc.fTransparecySpeed;
		}
	}
	

	//UV좌표 이동 ===============================================

	m_fMovingU += (_float)TimeDelta * m_tDesc.vUVMovingSpeed.x;
	m_fMovingV += (_float)TimeDelta * m_tDesc.vUVMovingSpeed.y;

	if (m_tDesc.ePlayType == PLAY_LOOP)
	{
		if (false == m_bStartLoopUV)
		{
			if (1.f >= m_fMovingU && m_tDesc.vUVMovingSpeed.x < 0.f)
				m_bStartLoopUV = true;
			if (1.f >= m_fMovingV && m_tDesc.vUVMovingSpeed.y < 0.f)
				m_bStartLoopUV = true;
			if (0.f <= m_fMovingU && m_tDesc.vUVMovingSpeed.x > 0.f)
				m_bStartLoopUV = true;
			if (0.f <= m_fMovingV && m_tDesc.vUVMovingSpeed.y > 0.f)
				m_bStartLoopUV = true;

		}
		else
		{
			if (m_fMovingU > 1.f || -1.f > m_fMovingU)
				m_fMovingU = 0.f;

			if (m_fMovingV > 1.f || -1.f > m_fMovingV)
				m_fMovingV = 0.f;
		}
	}

	else
	{
		if (m_tDesc.eLifeType == LOOP_LIMIT)
		{
			if (m_fMovingU > 1.f || m_fMovingV > 1.f)
				m_bDead = true;
		}
	}
	
	// ===========================================================

	Use_EasingFunction();

	if (0 > Pattern_Moving(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (0 > Pattern_Scaling(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_pTransformCom->Get_PositionFloat4_Pointer();

	return RESULT_NOPROBLEM;
}

_int CEffect_Mesh::LateTick(_double TimeDelta)
{
	if (false == m_isActive)
		return RESULT_NOPROBLEM;

	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	if (m_tDesc.fMaxTransparecy < 0)
	{
		if (m_fTransparency <= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;
		}

	}

	else
	{
		if (m_fTransparency >= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;
		}
	}

	if (true == m_tDesc.bBillboarding)
	{
		//_matrix        ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

		//ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

		auto vRight = XMVector3TransformNormal(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)), m_RandMatrix);
		auto vUp = XMVector3TransformNormal(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)), m_RandMatrix);
		auto vLook = XMVector3TransformNormal(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), m_RandMatrix);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));
	}
	
	if (0 > Pattern_Spinning(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	
	m_pRendererCom->Add_RenderGroup(m_tDesc.eRenderGroup, this);

	return RESULT_NOPROBLEM;
}

HRESULT CEffect_Mesh::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMaterial = m_pModelCom->Get_NumberOfMaterials();

	for (_uint i = 0; i < iNumMaterial; ++i)
	{
		m_pModelCom->Render(m_pShaderCom, m_tDesc.iPassIndex, i);
	}

	return RESULT_NOPROBLEM;
}

void CEffect_Mesh::PostRender(_double TimeDelta)
{
	ImGui::Begin("Effect Mesh");

	ImGui::Text("Alive");

	ImGui::End();
}

HRESULT CEffect_Mesh::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Mesh's Renderer!");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Mesh's Shader!");
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tDesc.ModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Mesh's Model!");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Mesh's Texture!");
		return E_FAIL;
	}

	/* For.Com_Texture_Second */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture_Second"), (CComponent**)&m_pSecondTextureCom)))
	{
		BREAKPOINT;
		MSGBOX("Failed to Create CEffect_Mesh's Texture!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Mesh::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		MSGBOX("m_pTransformCom->Bind_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(View)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(Proj)");
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_tDesc.iTextureIndex)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	//	m_tDesc.vUVMovingSpeed.x

	_float			fMovingUVX = 0.f;
	_float			fMovingUVY = 0.f;

	if (true == m_tDesc.bMovingUV)
	{
		fMovingUVX = m_fMovingU;
		fMovingUVY = m_fMovingV;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_X", &fMovingUVX, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_X)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV_Y", &fMovingUVY, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

	_bool		IsPlayOnce = false;

	if (m_tDesc.ePlayType == PLAY_ONCE && m_tDesc.bMovingUV == true)
	{
		// 트레일
		IsPlayOnce = true;
	}

	if (m_tDesc.ePlayType == PLAY_LOOP && m_bStartLoopUV == false)
	{
		// 트레일
		IsPlayOnce = true;
	}
	else if (m_tDesc.ePlayType == PLAY_LOOP && m_bStartLoopUV == true)
		IsPlayOnce = false;

	if (FAILED(m_pShaderCom->Set_RawValue("g_isPlayOnce", &IsPlayOnce, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &m_fTransparency, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}
	

	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &m_tDesc.bUsingBlur, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_iBlur)");
		return E_FAIL;
	}

	_bool		isMul = false;

	if (m_tDesc.eDyingPattern == DYING_MULT)
		isMul = true;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bMulColor", &isMul, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_bMulColor)");
		return E_FAIL;
	}

	_bool		isAdd = false;

	if (m_tDesc.eDyingPattern == DYING_ADD)
		isAdd = true;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bAddColor", &isAdd, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_bAddColor)");
		return E_FAIL;
	}

	_bool		isOne = false;

	if (m_tDesc.eDyingPattern == DYING_ONEVALUE)
		isOne = true;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bOneValue", &isOne, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_bOneValue)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_iRGBNum", &m_tDesc.iRGBNum, sizeof(_int))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_iRGBNum)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_tDesc.vColor, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_vColor)");
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_vBlendColor", &m_tDesc.vBlendColor, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_vBlendColor)");
		return E_FAIL;
	}

	if (m_tDesc.ePattern_MixTex != MIX_NONE)
	{
		if (FAILED(m_pSecondTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_tDesc.iScondTextureIndex)))
		{
			MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
			return E_FAIL;
		}

		if (FAILED(m_pSecondTextureCom->SetUp_OnShader(m_pShaderCom, "g_ThirdTexture", m_tDesc.iScondTextureIndex)))
		{
			MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
			return E_FAIL;
		}

		if (MIX_NOISING == m_tDesc.ePattern_MixTex)
		{
			if (FAILED(m_pShaderCom->Set_RawValue("g_fNoisePower", &m_tDesc.fNoisePower, sizeof(_float))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fNoisePower)");
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_X", &m_fMovingNoiseX, sizeof(_float))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveNoise_X)");
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fMovingNoiseY, sizeof(_float))))
			{
				MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveNoise_Y)");
				return E_FAIL;
			}
		}
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &m_tDesc.bUsingDistortion, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_iRGBNum)");
		return E_FAIL;
	}

	return S_OK;
}

void CEffect_Mesh::Set_EasingFunction()
{
	if (m_tDesc.bUseEasing_Scaling)
	{
		switch (m_tDesc.eEasing_Scale)
		{
		case INSINE:
			m_pEasingFunction_Scale = EaseInSine;
			break;
		case OUTSINE:
			m_pEasingFunction_Scale = EaseOutSine;
			break;
		case INOUTSINE:
			m_pEasingFunction_Scale = EaseInOutSine;
			break;
		case INQUAD:
			m_pEasingFunction_Scale = EaseInQuad;
			break;
		case OUTQUAD:
			m_pEasingFunction_Scale = EaseOutQuad;
			break;
		case INOUTQUAD:
			m_pEasingFunction_Scale = EaseInOutQuad;
			break;
		case INCUBIC:
			m_pEasingFunction_Scale = EaseInCubic;
			break;
		case OUTCUBIC:
			m_pEasingFunction_Scale = EaseOutCubic;
			break;
		case INOUTCUBIC:
			m_pEasingFunction_Scale = EaseInOutCubic;
			break;
		case INQUART:
			m_pEasingFunction_Scale = EaseInQuart;
			break;
		case OUTQUART:
			m_pEasingFunction_Scale = EaseOutQuart;
			break;
		case INOUTQUART:
			m_pEasingFunction_Scale = EaseInOutQuart;
			break;
		case INQUINT:
			m_pEasingFunction_Scale = EaseInQuint;
			break;
		case OUTQUINT:
			m_pEasingFunction_Scale = EaseOutQuint;
			break;
		case INOUTQUINT:
			m_pEasingFunction_Scale = EaseInOutQuint;
			break;
		case INEXPO:
			m_pEasingFunction_Scale = EaseInExpo;
			break;
		case OUTEXPO:
			m_pEasingFunction_Scale = EaseOutExpo;
			break;
		case INOUTEXPO:
			m_pEasingFunction_Scale = EaseInOutExpo;
			break;
		case INCIRC:
			m_pEasingFunction_Scale = EaseInCirc;
			break;
		case OUTCIRC:
			m_pEasingFunction_Scale = EaseOutCirc;
			break;
		case INOUTCIRC:
			m_pEasingFunction_Scale = EaseInOutCirc;
			break;
		case INBACK:
			m_pEasingFunction_Scale = EaseInBack;
			break;
		case OUTBACK:
			m_pEasingFunction_Scale = EaseOutBack;
			break;
		case INOUTBACK:
			m_pEasingFunction_Scale = EaseInOutBack;
			break;
		case INELASTIC:
			m_pEasingFunction_Scale = EaseInElastic;
			break;
		case OUTELASTIC:
			m_pEasingFunction_Scale = EaseOutElastic;
			break;
		case INOUTELASTIC:
			m_pEasingFunction_Scale = EaseInOutElastic;
			break;
		case INBOUNCE:
			m_pEasingFunction_Scale = EaseInBounce;
			break;
		case OUTBOUNCE:
			m_pEasingFunction_Scale = EaseOutBounce;
			break;
		case INOUTBOUNCE:
			m_pEasingFunction_Scale = EaseInOutBounce;
			break;
		}
	}

	if (m_tDesc.bUseEasing)
	{
		switch (m_tDesc.eEasing)
		{
		case INSINE:
			m_pEasingFunction = EaseInSine;
			break;
		case OUTSINE:
			m_pEasingFunction = EaseOutSine;
			break;
		case INOUTSINE:
			m_pEasingFunction = EaseInOutSine;
			break;
		case INQUAD:
			m_pEasingFunction = EaseInQuad;
			break;
		case OUTQUAD:
			m_pEasingFunction = EaseOutQuad;
			break;
		case INOUTQUAD:
			m_pEasingFunction = EaseInOutQuad;
			break;
		case INCUBIC:
			m_pEasingFunction = EaseInCubic;
			break;
		case OUTCUBIC:
			m_pEasingFunction = EaseOutCubic;
			break;
		case INOUTCUBIC:
			m_pEasingFunction = EaseInOutCubic;
			break;
		case INQUART:
			m_pEasingFunction = EaseInQuart;
			break;
		case OUTQUART:
			m_pEasingFunction = EaseOutQuart;
			break;
		case INOUTQUART:
			m_pEasingFunction = EaseInOutQuart;
			break;
		case INQUINT:
			m_pEasingFunction = EaseInQuint;
			break;
		case OUTQUINT:
			m_pEasingFunction = EaseOutQuint;
			break;
		case INOUTQUINT:
			m_pEasingFunction = EaseInOutQuint;
			break;
		case INEXPO:
			m_pEasingFunction = EaseInExpo;
			break;
		case OUTEXPO:
			m_pEasingFunction = EaseOutExpo;
			break;
		case INOUTEXPO:
			m_pEasingFunction = EaseInOutExpo;
			break;
		case INCIRC:
			m_pEasingFunction = EaseInCirc;
			break;
		case OUTCIRC:
			m_pEasingFunction = EaseOutCirc;
			break;
		case INOUTCIRC:
			m_pEasingFunction = EaseInOutCirc;
			break;
		case INBACK:
			m_pEasingFunction = EaseInBack;
			break;
		case OUTBACK:
			m_pEasingFunction = EaseOutBack;
			break;
		case INOUTBACK:
			m_pEasingFunction = EaseInOutBack;
			break;
		case INELASTIC:
			m_pEasingFunction = EaseInElastic;
			break;
		case OUTELASTIC:
			m_pEasingFunction = EaseOutElastic;
			break;
		case INOUTELASTIC:
			m_pEasingFunction = EaseInOutElastic;
			break;
		case INBOUNCE:
			m_pEasingFunction = EaseInBounce;
			break;
		case OUTBOUNCE:
			m_pEasingFunction = EaseOutBounce;
			break;
		case INOUTBOUNCE:
			m_pEasingFunction = EaseInOutBounce;
			break;
		}
	}

}

void CEffect_Mesh::Use_EasingFunction()
{
	if (true == m_tDesc.bUseEasing_Scaling)
	{
		//점차 줄어드는 속도를 다이나믹하게 표현하기
		m_fScalingSpeed = m_tDesc.fScaleSpeed - m_pEasingFunction_Scale(m_tDesc.fScaleSpeed, m_fTimeAccum, m_tDesc.fLifeTime);
	}

	if (true == m_tDesc.bUseEasing)
	{
		m_fMovingSpeed = m_tDesc.fMovingSpeed - m_pEasingFunction(m_tDesc.fMovingSpeed, m_fTimeAccum, m_tDesc.fLifeTime);

		if (m_fMovingSpeed < 0.f)
			m_fMovingSpeed = 0.f;
	}

}

_int CEffect_Mesh::Pattern_Scaling(_double TimeDelta)
{
	if (m_tDesc.ePattern_Scale != PATTERN_SCALE_NONE)
	{
		if (XMVectorGetX(m_vCurrentScale) <= 0.f)
		{
			m_tDesc.ePattern_Scale = PATTERN_SCALE_NONE;

			if (m_tDesc.eLifeType == SCALE_LIMIT)
				m_bDead = true;
		}
	
		if (false == m_tDesc.bUseEasing_Scaling)
		{
			m_vCurrentScale = XMVectorLerp(m_vCurrentScale, m_tDesc.vMaxScale, (_float)TimeDelta * m_tDesc.fScaleSpeed);
		}

		else
		{
			m_vCurrentScale += m_fScalingSpeed * m_vCurrentScale * (_float)TimeDelta;
		}

		m_vCurrentScale = XMVectorSetW(m_vCurrentScale, 0.f);
		m_pTransformCom->Scaled(m_vCurrentScale);

		if (0.0001f >= DISTANCE(m_tDesc.vMaxScale - m_vCurrentScale))
		{
			if (m_tDesc.ePattern_Scale == PATTERN_DYNAMIC)
			{
				m_tDesc.vMaxScale = m_tDesc.vStartScale;

				return 0;
			}


			m_tDesc.ePattern_Scale = PATTERN_SCALE_NONE;

			if (m_tDesc.eLifeType == SCALE_LIMIT)
				m_bDead = true;
		}
	}

	return 0;
}

_int CEffect_Mesh::Pattern_Moving(_double TimeDelta)
{
	switch (m_tDesc.ePattern_Move)
	{
	case Client::CEffect_Mesh::PATTERN_MOVE_NONE:
		return RESULT_NOPROBLEM;

	case Client::CEffect_Mesh::PATTERN_TRACING:
		SynchToTarget();
		return RESULT_NOPROBLEM;

	case Client::CEffect_Mesh::PATTERN_TRACING_POSONLY:
		SynchToTarget_OnlyPos();
		return RESULT_NOPROBLEM;

	case Client::CEffect_Mesh::PATTERN_MOVE_ONEDIR:
	case Client::CEffect_Mesh::PATTERN_MOVE_RANDDIR:

		_vector vNextPos;

		if (false == m_tDesc.bUseEasing)
			vNextPos = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_vFinalPos, m_tDesc.fMovingSpeed * (_float)TimeDelta);
		else
			vNextPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_tDesc.vDir) * m_fMovingSpeed * (_float)TimeDelta;

		vNextPos = XMVectorSetW(vNextPos, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNextPos);

		if (0.1f > DISTANCE(vNextPos - m_vFinalPos))
		{
			m_tDesc.ePattern_Move = PATTERN_MOVE_NONE;

			if (m_tDesc.eLifeType == DISTANCE_LIMIT)
				m_bDead = true;
		}

		break;
	}

	/*if (m_tDesc.ePattern_Move == PATTERN_MOVE_NONE)
	return RESULT_NOPROBLEM;

	if (m_tDesc.ePattern_Move == PATTERN_TRACING)
	{
	SynchToTarget();
	return RESULT_NOPROBLEM;
	}

	if (m_tDesc.ePattern_Move == PATTERN_TRACING_POSONLY)
	{
	SynchToTarget_OnlyPos();
	return RESULT_NOPROBLEM;
	}*/

	/*_vector vNextPos;

	if (false == m_tDesc.bUseEasing)
		vNextPos = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_vFinalPos, m_tDesc.fMovingSpeed * (_float)TimeDelta);

	else
		vNextPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_tDesc.vDir) * m_fMovingSpeed * (_float)TimeDelta;

	vNextPos = XMVectorSetW(vNextPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNextPos);

	if (0.1f > DISTANCE(vNextPos - m_vFinalPos))
	{
		m_tDesc.ePattern_Move = PATTERN_MOVE_NONE;
		
		if (m_tDesc.eLifeType == DISTANCE_LIMIT)
			m_bDead = true;
	}*/

	return RESULT_NOPROBLEM;
}

_int CEffect_Mesh::Pattern_Spinning(_double TimeDelta)
{
	if (m_tDesc.bSpinning == false)
		return RESULT_NOPROBLEM;

	if (true == m_tDesc.bBillboarding)
		return RESULT_NOPROBLEM;

	if (nullptr == m_tDesc.pTargetWorldMatrix)
		m_pTransformCom->Turn(m_tDesc.vSpinAxis, TimeDelta * m_tDesc.fSpinningSpeed);
	else
		m_LocalMatrix = m_LocalMatrix * XMMatrixRotationAxis(m_tDesc.vSpinAxis, XMConvertToRadians(30.0f) * (_float)TimeDelta * m_tDesc.fSpinningSpeed);

	//if (true == m_tDesc.bBillboarding)
	//{
	///*	auto vTurn = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fTimeAccum * 100.f);
	//	auto vLook = XMVector4Transform(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vTurn);
	//	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);*/
	//}
	//
	//else
	//{
	//	if (nullptr == m_tDesc.pTargetWorldMatrix)
	//		m_pTransformCom->Turn(m_tDesc.vSpinAxis, TimeDelta * m_tDesc.fSpinningSpeed);
	//	else
	//		m_LocalMatrix = m_LocalMatrix * XMMatrixRotationAxis(m_tDesc.vSpinAxis, XMConvertToRadians(30.0f) * (_float)TimeDelta * m_tDesc.fSpinningSpeed);
	//}

	return RESULT_NOPROBLEM;
}

_float CEffect_Mesh::RandomRange(_float r1, _float r2)
{
	_float random = ((_float)rand()) / (_float)RAND_MAX;
	_float diff = r2 - r1;
	_float val = random * diff;

	return r1 + val;
}

void CEffect_Mesh::SynchToTarget()
{
	if (nullptr == m_tDesc.pTargetWorldMatrix)
	{
		BREAKPOINT;
		return;
	}

	_matrix TargetWorld = XMLoadFloat4x4(m_tDesc.pTargetWorldMatrix);

	TargetWorld.r[0] = XMVector4Normalize(TargetWorld.r[0]) * XMVectorGetX(m_vCurrentScale);
	TargetWorld.r[1] = XMVector4Normalize(TargetWorld.r[1]) * XMVectorGetY(m_vCurrentScale);
	TargetWorld.r[2] = XMVector4Normalize(TargetWorld.r[2]) * XMVectorGetZ(m_vCurrentScale);

	auto vPos = TargetWorld.r[3];

	m_pTransformCom->Set_WorldMatrix(m_LocalMatrix * m_tDesc.LocalMat * m_TransMat * TargetWorld);

	switch (m_tDesc.eFromTarget)
	{
	case RIGHT:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * m_tDesc.fFromTargetDistance);
		break;
	case UP:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * m_tDesc.fFromTargetDistance);
		break;
	case LOOK:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * m_tDesc.fFromTargetDistance);
		break;
	}
}

void CEffect_Mesh::SynchToTarget_OnlyPos()
{
	if(nullptr == m_tDesc.pParentPos)
		return;

	m_pTransformCom->Set_Position(XMLoadFloat4(m_tDesc.pParentPos));

	//_matrix World = m_tDesc.LocalMat_NoTrace * m_pTransformCom->Get_WorldMatrix();

	//m_pTransformCom->Set_WorldMatrix(World);
}

CEffect_Mesh * CEffect_Mesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Mesh*	pInstance = new CEffect_Mesh(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CEffect_Mesh!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Mesh::Clone(void * pArg)
{
	CEffect_Mesh*	pInstance = new CEffect_Mesh(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CEffect_Mesh!");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Mesh::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSecondTextureCom);
}
