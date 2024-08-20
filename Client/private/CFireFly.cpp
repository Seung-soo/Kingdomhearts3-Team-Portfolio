#include "stdafx.h"
#include "CFireFly.h"
#include "GameInstance.h"
#include "Progress_Manager.h"

CFireFly::CFireFly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject(pDevice, pDeviceContext)
{
}

CFireFly::CFireFly(const CFireFly& rhs)
    : CGameObject(rhs)
{
}

HRESULT CFireFly::NativeConstruct_Prototype()
{
    if(FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CFireFly::NativeConstruct(void* pArg)
{
    // 부모 초기화할 때 트랜스폼 초기화
    CTransform::TRANSFORMDESC TransformDesc;
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = 1.5f;
    TransformDesc.fScalingPerSec = 5.f;

    if (FAILED(__super::NativeConstruct(&TransformDesc)))
        return E_FAIL;

    if(nullptr != pArg)
        memcpy(&m_tDesc, (FIREFLYDESC*)pArg, sizeof(FIREFLYDESC));

    if(FAILED(SetUp_Components()))
        return E_FAIL;

    m_pTransformCom->Scaled(__vector(0.05f, 0.05f, 0.05f, 0.f));

    m_fPreDir = m_tDesc.fDir;
    return S_OK;
}

_int CFireFly::Tick(_double TimeDelta)
{
    if (true == PM->Get_MinigameWaiting())
        return RESULT_NOPROBLEM;

    if (FAILED(__super::Tick(TimeDelta)))
        return E_FAIL;

    m_TimeAccum += TimeDelta;

    Update_Action(TimeDelta);

    // UV 좌표 움직이기
    m_fMovingU += (_float)TimeDelta * 0.25f;
    m_fMovingV += (_float)TimeDelta * 0.5f;

    _vector vAimRange = __vector(m_fMaxRange, m_fMaxRange, m_fMaxRange, 0.f);

    if (false == m_isReverse)
    {
        if (m_fRange >= m_fMaxRange - 1.f)
        {
            m_isReverse = true;
        }
    }

    else
    {
        vAimRange = __vector(m_fMinRange, m_fMinRange, m_fMinRange, 0.f);

        if(m_fRange <= m_fMinRange + 1.f)
            m_isReverse = false;
    }
   

    m_fRange = vecx(XMVectorLerp(__vector(m_fRange, m_fRange, m_fRange, 0.f), vAimRange, (_float)TimeDelta));

    auto LightDesc = CGameInstance::GetInstance()->Get_LightDesc(m_tDesc.iLightIndex);
    XMStoreFloat4(&LightDesc->vPosition, m_pTransformCom->Get_Position());


    LightDesc->fRange = m_fRange;

    return _int();
}

_int CFireFly::LateTick(_double TimeDelta)
{
    if (true == PM->Get_MinigameWaiting())
        return RESULT_NOPROBLEM;

    if (FAILED(__super::LateTick(TimeDelta)))
        return E_FAIL;

    if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && false == m_bDead)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

    return _int();
}

HRESULT CFireFly::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    _uint iNumMaterial = m_pModelCom->Get_NumberOfMaterials();

    for (_uint i = 0; i < iNumMaterial; ++i)
    {
        m_pModelCom->Render(m_pShaderCom, 35, i);
    }

	return S_OK;
}

HRESULT CFireFly::SetUp_Components()
{
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
    {
        BREAKPOINT;
        MSGBOX("Failed to Create CFireFly's Shader!");
        return E_FAIL;
    }

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
    {
        BREAKPOINT;
        MSGBOX("Failed to Create CFireFly's Texture!");
        return E_FAIL;
    }

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sphere04"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
    {
        BREAKPOINT;
        MSGBOX("Failed to Create CEffect_Mesh's Model!");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CFireFly::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", 346)))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CParticle_Effect::SetUp_ConstantTable");
		return E_FAIL;
	}

	//	m_tDesc.vUVMovingSpeed.x

	_float			fMovingUVX = m_fMovingU;
	_float			fMovingUVY = m_fMovingV;

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
	if (FAILED(m_pShaderCom->Set_RawValue("g_isPlayOnce", &IsPlayOnce, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

    _float fTransparency = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &fTransparency, sizeof(_float))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_fMoveUV_Y)");
		return E_FAIL;
	}

    _bool isUsingBlur = true;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &isUsingBlur, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_iBlur)");
		return E_FAIL;
	}
    m_vColor = _float4(0.5f, -0.3f, -0.5f, -0.1f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_vColor)");
		return E_FAIL;
	}


	_bool bDistortion = false;

	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &bDistortion, sizeof(_bool))))
	{
		MSGBOX("m_pShaderCom->Set_RawValue returned E_FAIL in CParticle_Effect::SetUp_ConstantTable(g_iRGBNum)");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFireFly::Update_Action(_double TimeDelta)
{
    if (nullptr == m_pPlayer)
    {
		m_pPlayer = CGameInstance::GetInstance()->Get_GameObject(CGameInstance::GetInstance()->Get_LevelIndex(), TEXT("Layer_Player"), 0);

		if (m_pPlayer == nullptr)
			return E_FAIL;

		m_pPlayerTransform = m_pPlayer->Get_Transform();

		if (m_pPlayerTransform == nullptr)
			return E_FAIL;

		Safe_AddRef(m_pPlayer);
		Safe_AddRef(m_pPlayerTransform);

        // 오프셋 포즈 랜덤으로 정하기
        m_vOffsetPos = __vector(_float(rand() % 360 - 180), 1.f, _float(rand() % 360 - 180), 0.f);

        m_pTransformCom->Set_Position(m_pPlayerTransform->Get_Position() + m_vOffsetPos);
    }

    if (m_TimeAccum > 0.6 && m_TimeAccum < 1.0)
    {
        m_isLoosingSpeed = true; 
    }

    else
        m_isLoosingSpeed = false;

    if (m_TimeAccum > 1.0)
    {
        m_tDesc.fDir = -m_tDesc.fDir;

        if(vecy(m_pTransformCom->Get_Position()) <= 0.f)
            m_tDesc.fDir = 1.f;

        m_TimeAccum = 0.0;
        m_isLoosingSpeed = false;
    }


    // 일정 범위 밖에 가면 플레이어 트레이싱
    auto fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransform);
    
    if (fDistance > 4.f && m_bChase == false)
    {
        m_bChase = true;

        m_vTargetPos = m_pPlayerTransform->Get_Position();

        m_vTargetPos = XMVectorSetY(m_vTargetPos, vecy(m_vTargetPos) + 1.f);
    }

    if (m_bChase == true)
    {
        auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetPos, (_float)TimeDelta * 2.f);

        m_pTransformCom->Set_Position(vPos);

        if(m_pTransformCom->Get_Distance(m_vTargetPos) <= 1.f)
            m_bChase = false;
    }
    
    else
    {
        if (fDistance <= 1.f)
        {
            auto vDir = m_pTransformCom->Get_Dir(m_pPlayerTransform);

            vDir = -vDir;


            m_pTransformCom->Go_VectorXZ(vDir, TimeDelta * 0.2);
        }


        auto vTargetPos = m_pPlayerTransform->Get_Position();

        // 플레이어 공전 함수
        m_pTransformCom->Revolution(__vector(0.f, 1.f, 0.f, 0.f), m_pPlayerTransform->Get_Position(), TimeDelta * 0.5 * m_tDesc.fRevolutionDir);

        
        // 위아래로 살짝 움직임
        _double Speed = (_double)m_tDesc.fDir * 0.2;

        if (true == m_isLoosingSpeed)
        {
            Speed = (_double)m_tDesc.fDir * 0.1;
        }

        m_pTransformCom->Go_Up(TimeDelta * Speed);
    }

 

    return S_OK;
}

CFireFly* CFireFly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CFireFly* pInstance = new CFireFly(pDevice, pDeviceContext);

    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        MSGBOX("Failed to Create CFireFly!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFireFly::Clone(void* pArg)
{
    CFireFly* pInstance = new CFireFly(*this);

    if (FAILED(pInstance->NativeConstruct(pArg)))
    {
        MSGBOX("Failed to Clone CFireFly!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFireFly::Free()
{
    __super::Free();

    Safe_Release(m_pRendererCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pPlayer);
    Safe_Release(m_pPlayerTransform);
    Safe_Release(m_pTextureCom);
}
