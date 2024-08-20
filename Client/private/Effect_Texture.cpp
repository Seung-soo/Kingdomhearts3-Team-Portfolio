#include "stdafx.h"
#include "..\Public\Effect_Texture.h"
#include "GameInstance.h"

CEffect_Texture::CEffect_Texture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
} 

CEffect_Texture::CEffect_Texture(const CEffect_Texture & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Texture::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		tTransformDesc;

	if (nullptr != pArg)
	{
		memcpy(&m_tDesc, pArg, sizeof(EFFECT_TEX_DESC));

		tTransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
		tTransformDesc.fScalingPerSec = m_tDesc.fScalingSpeed;
		tTransformDesc.fSpeedPerSec = m_tDesc.fPlaySpeed;

	/*	m_tDesc.vBaseColor = _float3(m_tDesc.vBaseColor.x / 255.f, m_tDesc.vBaseColor.y / 255.f, m_tDesc.vBaseColor.z / 255.f);
		m_tDesc.vBlendingColor = _float3(m_tDesc.vBlendingColor.x / 255.f, m_tDesc.vBlendingColor.y / 255.f, m_tDesc.vBlendingColor.z / 255.f);*/

		m_fTranparency = m_tDesc.fStartAlpha;
	}

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (true == m_tDesc.bDefaultPass)
		m_tDesc.iPassIndex = 21;

	if (m_tDesc.bSprite)
		m_tDesc.iPassIndex = 17;

	if (m_tDesc.eMovingPattern == PATTERN_MOVE_RANDDIR)
	{
		//랜덤으로 방향 설정
		XMStoreFloat3(&m_tDesc.vMovingDir, XMVector4Normalize(XMVectorSet(_float(rand() % 360 - 180), _float(rand() % 360 - 180), _float(rand() % 360 - 180), 0.f)));
	}

	_vector vPosition = XMLoadFloat3(&m_tDesc.vPosition);
	vPosition = XMVectorSetW(vPosition, 1.f);

	m_vCurrentScale = XMLoadFloat3(&m_tDesc.vMinSize);
	
	m_vFinalPos = vPosition + XMVector3Normalize(XMLoadFloat3(&m_tDesc.vMovingDir)) * m_tDesc.fPlaySpeed;
	m_vFinalPos = XMVectorSetW(m_vFinalPos, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Scaled(XMLoadFloat3(&m_tDesc.vMinSize));

	m_fMovingSpeed = m_tDesc.fPlaySpeed;

	if (true == m_tDesc.bUseEasing || true == m_tDesc.bUseEasing_Scaling)
		Set_EasingFunction();

	if (true == m_tDesc.bMultiSprite)
	{
		if (m_tDesc.fSpriteMaxXIndex > (_int)m_pTextureCom->Get_Size())
			m_tDesc.fSpriteMaxXIndex = (_int)(m_pTextureCom->Get_Size() - 1);
	}

	if (true == m_tDesc.bBillboarding)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		auto CamMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
		auto CamWorld = XMMatrixInverse(nullptr, CamMatrix);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, (CamWorld.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT)));
		m_pTransformCom->Set_State(CTransform::STATE_UP, (CamWorld.r[1])* m_pTransformCom->Get_Scale(CTransform::STATE_UP));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, (CamWorld.r[2])* m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));
	}

	else
	{
		m_pTransformCom->MakeSameDirection(XMVector3Normalize(XMLoadFloat3(&m_tDesc.vLookDir)));

		m_pTransformCom->Set_WorldMatrix(m_tDesc.RoatationMatrix * m_pTransformCom->Get_WorldMatrix());
	}



	m_tDesc.vSpinAxis = XMVector3Normalize(m_tDesc.vSpinAxis);

	return S_OK;
}

_int CEffect_Texture::Tick(_double TimeDelta)
{
	if (!m_isActive)
		return RESULT_NOPROBLEM;

	if (m_tDesc.pParentDead != nullptr && *m_tDesc.pParentDead == true)
		return RESULT_DEAD;

	//Time
	m_fTimeAccum += (_float)TimeDelta;

	if (m_tDesc.bNoising)
		m_fNoisingTimeAccum += (_float)TimeDelta * m_tDesc.fNoiseRot;

	if (m_fTimeAccum >= m_tDesc.fTotalTime)
	{
		if (TIME_LIMIT == m_tDesc.eLifeType)
			m_bDead = true;
	}

	//Alpha
	m_fTranparency += (_float)TimeDelta * m_tDesc.fTransparecySpeed;

	if (m_tDesc.fMaxTransparecy < 0)
	{
		if (m_fTranparency <= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;
		}

	}

	else
	{
		if (m_fTranparency >= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;
		}
	}

	if (true == m_bDead)
		return RESULT_DEAD;

	if (true == m_tDesc.bSprite || true == m_tDesc.bMultiSprite)
	{
		m_fCurFrameIndexX += (_float)m_tDesc.fSpriteMaxXIndex * (_float)TimeDelta * m_tDesc.fSpriteSpeed;

		if (m_fCurFrameIndexX >= (_float)m_tDesc.fSpriteMaxXIndex)
		{

			m_fCurFrameIndexX = 0.f;

			if (m_fCurFrameIndexY < m_tDesc.fSpriteMaxYIndex)
				m_fCurFrameIndexY += 1.f;

			if (m_fCurFrameIndexY >= m_tDesc.fSpriteMaxYIndex)
			{

				if (m_tDesc.ePlayType == PLAY_LOOP)
					m_fCurFrameIndexY = 0.f;

				else
				{
					m_tDesc.bSprite = false;
					m_tDesc.bMultiSprite = false;

					if (m_tDesc.eLifeType == SPRITE_LIMIT)
						m_bDead = true;
				}
			
			}

		}
	}



	if (m_tDesc.bUseEasing || m_tDesc.bUseEasing_Scaling)
		Use_EasingFunction();

	if (0 > Pattern_Scaling(TimeDelta))
		return RESULT_NOPROBLEM;

	if(0 > Pattern_Moving(TimeDelta))
		return RESULT_NOPROBLEM;

	if (0 > Pattern_Spinning(TimeDelta))
		return RESULT_NOPROBLEM;



	return RESULT_NOPROBLEM;
}

_int CEffect_Texture::LateTick(_double TimeDelta)
{
	if (false == m_isActive)
		return RESULT_NOPROBLEM;

	if (m_tDesc.fMaxTransparecy < 0)
	{
		if (m_fTranparency <= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;
		}

	}

	else
	{
		if (m_fTranparency >= m_tDesc.fMaxTransparecy)
		{
			if (ALPHA_LIMIT == m_tDesc.eLifeType)
				m_bDead = true;
		}
	}

	if (true == m_bDead)
		return RESULT_DEAD;


	if (true == m_tDesc.bBillboarding && m_tDesc.bSpinning == false)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		auto CamMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
		auto CamWorld = XMMatrixInverse(nullptr, CamMatrix);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, CamWorld.r[0] * vecx(m_vCurrentScale));
		m_pTransformCom->Set_State(CTransform::STATE_UP, CamWorld.r[1] * vecy(m_vCurrentScale));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, CamWorld.r[2] * vecz(m_vCurrentScale));
	}

	m_pRendererCom->Add_RenderGroup(m_tDesc.eRenderGroup, this);

	return RESULT_NOPROBLEM;
}

HRESULT CEffect_Texture::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBufferCom->Render(m_pShaderCom, m_tDesc.iPassIndex);

	return S_OK;
}

HRESULT CEffect_Texture::SetUp_Component()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSGBOX("Failed to Create CUI_NoAnim's Renderer!");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSGBOX("Failed to Create CUI_NoAnim's Shader!");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSGBOX("Failed to Create CUI_NoAnim's VI_Buffer!");
		return E_FAIL;
	}

	/* For.Com_Texture */

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSGBOX("Failed to Create CUI_NoAnim's Texture!");
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture_Second"), (CComponent**)&m_pSecondTextureCom)))
	{
		MSGBOX("Failed to Create CUI_NoAnim's Texture!");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Texture::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));

	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4)));

	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4)));

	if (true == m_tDesc.bMultiSprite)
	{
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fCurFrameIndexX));
	}

	else
	{
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", m_tDesc.iTextureIndex));
	}
	// 소프트이펙트를 위해 Depth 타겟을 넘겨줌
	if (FAILED(m_pShaderCom->Set_Texture("g_SourTexture", pGameInstance->Get_RenderTargetSRV(TEXT("Target_Depth")))))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_vBlendingColor", &m_tDesc.vBaseColor, sizeof(_float4)));

	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_vExtraColor", &m_tDesc.vBlendingColor, sizeof(_float4)));

	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_fTranslucency", &m_fTranparency, sizeof(_float)));
	
	_float		fDistortion = 1.f; 

	if(false == m_tDesc.bDistortion)
		fDistortion = 0.f;

	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_fDistortion", &fDistortion, sizeof(_float)));
	
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &m_tDesc.bBlur, sizeof(_bool)));

	if (true == m_tDesc.bSprite)
	{
		CHECK_FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxXIndex", &m_tDesc.fSpriteMaxXIndex, sizeof(_int)));

		CHECK_FAILED(m_pShaderCom->Set_RawValue("g_iSpriteMaxYIndex", &m_tDesc.fSpriteMaxYIndex, sizeof(_int)));

		_int	iCurIndex = _int(m_fCurFrameIndexX);

		CHECK_FAILED(m_pShaderCom->Set_RawValue("g_iSpriteIndex", &iCurIndex, sizeof(_int)));

		iCurIndex = _int(m_fCurFrameIndexY);

		CHECK_FAILED(m_pShaderCom->Set_RawValue("g_iSpriteYIndex", &iCurIndex, sizeof(_int)));
	}

	if (true == m_tDesc.bNoising)
	{
		CHECK_FAILED(m_pSecondTextureCom->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_tDesc.iSecondTextureIndex));
		CHECK_FAILED(m_pShaderCom->Set_RawValue("g_fNoiseRot", &m_fNoisingTimeAccum, sizeof(_float)));

	}

	return S_OK;
}

_int CEffect_Texture::Pattern_Scaling(_double TimeDelta)
{
	if (m_tDesc.eScalePattern != PATTERN_SCALE_NONE)
	{
		if (XMVectorGetX(m_vCurrentScale) <= 0.f)
		{
			m_tDesc.eScalePattern = PATTERN_SCALE_NONE;

			if (m_tDesc.eLifeType == SCALE_LIMIT)
				m_bDead = true;
		}

		if (false == m_tDesc.bUseEasing_Scaling)
		{
			m_vCurrentScale = XMVectorLerp(m_vCurrentScale, XMLoadFloat3(&m_tDesc.vMaxSize), (_float)TimeDelta * m_tDesc.fScalingSpeed);
		}

		else
		{
			m_vCurrentScale += m_fScalingSpeed * m_vCurrentScale * (_float)TimeDelta;
		}

		m_vCurrentScale = XMVectorSetW(m_vCurrentScale, 0.f);
		m_pTransformCom->Scaled(m_vCurrentScale);

		if (0.0001f >= DISTANCE(XMLoadFloat3(&m_tDesc.vMaxSize) - m_vCurrentScale))
		{
			if (m_tDesc.eScalePattern== PATTERN_DYNAMIC)
			{
				m_tDesc.vMaxSize = m_tDesc.vMinSize;

				return 0;
			}


			m_tDesc.eScalePattern = PATTERN_SCALE_NONE;

			if (m_tDesc.eLifeType == SCALE_LIMIT)
				m_bDead = true;
		}
	}

	return 0;
}

_int CEffect_Texture::Pattern_Moving(_double TimeDelta)
{
	if (m_tDesc.eMovingPattern == PATTERN_MOVE_NONE)
		return RESULT_NOPROBLEM;

	_vector vNextPos;

	if (false == m_tDesc.bUseEasing)
		vNextPos = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_vFinalPos, m_tDesc.fPlaySpeed * (_float)TimeDelta);

	else
		vNextPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(XMLoadFloat3(&m_tDesc.vMovingDir)) * m_fMovingSpeed * (_float)TimeDelta;

	vNextPos = XMVectorSetW(vNextPos, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNextPos);

	auto vTest = DISTANCE(vNextPos - m_vFinalPos);

	if (0.1f > DISTANCE(vNextPos - m_vFinalPos))
	{
		m_tDesc.eMovingPattern = PATTERN_MOVE_NONE;

		if (m_tDesc.eLifeType == DISTANCE_LIMIT)
			m_bDead = true;
	}


	return RESULT_NOPROBLEM;
}

_int CEffect_Texture::Pattern_Spinning(_double TimeDelta)
{
	if (m_tDesc.bSpinning == false)
		return RESULT_NOPROBLEM;

	if (true == m_tDesc.bBillboarding)
	{
		// ? 코드가 왜 이모냥이야
		_vector		vLook = XMVector3Normalize(CGameInstance::GetInstance()->Get_CamPosition() - m_pTransformCom->Get_Position());
		_vector		vUp = __vector(0.f, 1.f, 0.f, 0.f);
		_vector		vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vecx(m_vCurrentScale));
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp * vecy(m_vCurrentScale));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * vecz(m_vCurrentScale));

		m_pTransformCom->NowRotation(vLook, XMConvertToRadians(m_fTimeAccum * 90.f * m_tDesc.fSpinSpeed));
	}

	else
		m_pTransformCom->Turn(m_tDesc.vSpinAxis, TimeDelta * m_tDesc.fSpinSpeed);

	return RESULT_NOPROBLEM;
}

void CEffect_Texture::Set_EasingFunction()
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

void CEffect_Texture::Use_EasingFunction()
{
	if (true == m_tDesc.bUseEasing_Scaling)
	{
		//점차 줄어드는 속도를 다이나믹하게 표현하기
		m_fScalingSpeed = m_tDesc.fScalingSpeed - m_pEasingFunction_Scale(m_tDesc.fScalingSpeed, m_fTimeAccum, m_tDesc.fTotalTime);
	}

	if (true == m_tDesc.bUseEasing)
	{
		m_fMovingSpeed = m_tDesc.fPlaySpeed - m_pEasingFunction(m_tDesc.fPlaySpeed, m_fTimeAccum, m_tDesc.fTotalTime);

		if (m_fMovingSpeed < 0.f)
			m_fMovingSpeed = 0.f;
	}
}

CEffect_Texture * CEffect_Texture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void* pArg)
{
	CEffect_Texture*	pInstance = new CEffect_Texture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Create CEffect_Texture(Proto)!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CEffect_Texture * CEffect_Texture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{

	CEffect_Texture*	pInstance = new CEffect_Texture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CEffect_Texture(Proto)!");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CGameObject * CEffect_Texture::Clone(void * pArg)
{
	CEffect_Texture*	pInstance = new CEffect_Texture(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CEffect_Texture!");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Texture::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSecondTextureCom);
}
