#include "stdafx.h"
#include "..\public\Money_Object.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "UI_Num_Manager.h"
#include "InvenManager.h"
#include "SoundManager.h"
#include "Easing_Function.h"

#include "Player.h"


CMoney_Object::CMoney_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CMoney_Object::CMoney_Object(const CMoney_Object & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoney_Object::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMoney_Object::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));
	CHECK_FAILED(SetUp_Components());

	LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();

	CTransform* pMonsterTransform = (CTransform*)pArg;

	_vector vMonster_Position = pMonsterTransform->Get_Position();
	_vector	ObjectUP = XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

	_vector vObject_Scale = XMVectorSet(0.3f, 0.3f, 0.3f, 1.f);

	_float fRadian = (_float)(rand() % 9);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRadian * 30.f));
	m_pTransformCom->Scaled(vObject_Scale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMonster_Position + (ObjectUP * 0.5f));

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	m_iPass = 13;

	return S_OK;
}

_int CMoney_Object::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	if (false == GM->Get_UIRender())
		return 0;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();

	dTime += TimeDelta;
	if (1.5 < dTime)
		bPattern = true;

	/* ========= */

	dSpeedTime += TimeDelta;
	if (0.01 < dSpeedTime && 0.f <= fSpeed)
	{
		fSpeed -= 0.02f;
		dSpeedTime = 0.0;
	}
	if (0 < fSpeed)
	{
		m_pTransformCom->Go_Straight(TimeDelta * fSpeed, m_pNavigationCom);
	}
	else if (0 >= fSpeed)
	{
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(10.f));
	}

	/* === */

	dSpeedTime2 += TimeDelta;
	if (false == bChaseOn && 0.01 < dSpeedTime2 && -1.f <= fSpeed2)
	{
		fSpeed2 -= 0.06f;
		dSpeedTime2 = 0.0;
	}
	_float fNaviY = m_pNavigationCom->Compute_Y(m_pTransformCom->Get_Position());

	if (false == bChaseOn && fNaviY + 0.3f < vecy(m_pTransformCom->Get_Position()))
	{
		m_pTransformCom->Go_Up(TimeDelta * fSpeed2, m_pNavigationCom);
	}
	if (fNaviY + 0.3f > vecy(m_pTransformCom->Get_Position()))
		m_pTransformCom->Set_PositionY(fNaviY + 0.3f);

	/* ========= */

	/* 플레이어와의 거리 계산========== */
	_vector Player_Position = m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Player"))->Get_Transform()->Get_Position();
	_vector Player_Up = XMVector4Normalize(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Player"))->Get_Transform()->Get_State(CTransform::STATE_UP));
	_vector	ItemPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector Total_Length = XMVector4Length((Player_Position + (Player_Up * 1.f)) - ItemPosition);
	_float4	fTotal_Length;
	XMStoreFloat4(&fTotal_Length, Total_Length);

	if (true == bPattern)
	{
		if (fTotal_Length.x <= 3.f
			|| fTotal_Length.y <= 3.f)
			bChaseOn = true;

		if (fTotal_Length.x <= 0.5f
			|| fTotal_Length.y <= 0.5f)
		{
			_int iMoney = (_int)rand() % 9 + 5;
			Inven->Math_Money(iMoney);

			NM->Set_MoneyBar_Render(true); // Num_Manager::Update() 에서 동작확인
			NM->Set_MoneyBar_Render_Cnt(true);
			SM->Sound_MoneyGet(L"UI_MoneyGet.wav", 0.5f);
			m_bDead = true;
		}
	}
	

	/* Chase */
	if (true == bChaseOn)
		m_pTransformCom->Chase(Player_Position + (Player_Up * 1.f), 3.0 * TimeDelta, m_pNavigationCom);
	/* ============================== */


	return RESULT_NOPROBLEM;
}

_int CMoney_Object::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	if (true == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.5f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	return RESULT_NOPROBLEM;
}

HRESULT CMoney_Object::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i);
	}

	return S_OK;
}

HRESULT CMoney_Object::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Model */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Money_Object"), TEXT("m_pModelCom"), (CComponent**)&m_pModelCom));

	CNavigation::NAVIDESC tNaviDesc;
	tNaviDesc.iCurrentIndex = 0;
	tNaviDesc.iPreviousIndex = 0;
	tNaviDesc.iPreviousFrame = 0;
	tNaviDesc.pParent = m_pTransformCom;

	_tchar wszNaviPrototypeTag[128] = TEXT("");

	switch (GM->Get_CurrentLevel())
	{
	case LEVEL_TWILIGHT:
		lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Twilight"));
		break;
	case LEVEL_RAPUNZEL:
		lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Rapunzel"));
		break;
	case LEVEL_RAPUNZEL_BOSS:
		lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Rapunzel_Boss"));
		break;
	case LEVEL_FROZEN:
		lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_Frozen"));
		break;
	case LEVEL_KEYGRAVE:
		lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_KeyGrave"));
		break;
	case LEVEL_FINALBOSS:
		lstrcpy(wszNaviPrototypeTag, TEXT("Prototype_Component_Navigation_FinalBoss"));
		break;
	}

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(GM->Get_CurrentLevel(), wszNaviPrototypeTag, TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMoney_Object::SetUp_ConstantTable()
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

	// 쉐이더에 던져야 하는 변수들
	_bool		bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float		fAlpha = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &fAlpha, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_bool		bDistortion = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &bDistortion, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CMoney_Object * CMoney_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoney_Object*	pInstance = new CMoney_Object(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMoney_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMoney_Object::Clone(void * pArg)
{
	CMoney_Object*	pInstance = new CMoney_Object(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMoney_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoney_Object::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
