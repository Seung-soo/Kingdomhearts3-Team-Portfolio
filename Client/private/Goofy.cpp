#include "stdafx.h"
#include "..\public\Goofy.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameManager.h"
#include "Goofy_Weapon.h"
#include "Effect_Manager.h"
#include "MotionTrail.h"
#include "Player.h"

CGoofy::CGoofy(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CGoofy::CGoofy(const CGoofy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGoofy::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoofy::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec    = 15.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec  = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(Separate_Channels()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* Setup RootNode Index */
	_int iRootNodeIndex = m_pModelCom->Get_NodeIndexByName("kh_sk");
	if (UINT_MAX == iRootNodeIndex)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Set_RootNodeIndex(iRootNodeIndex);

	m_pModelCom->Add_OnceListAll();
	m_pModelCom->Add_LerpList(STATE_PUSH);
	m_pModelCom->Add_LerpList(STATE_RUN);
	m_pModelCom->Add_LerpList(STATE_THROWSTART);

	if (FAILED(NativeConstruct_Pool(pArg)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGoofy::NativeConstruct_Pool(void * pArg)
{
	for (_uint i = 0; i < STATE_END; i++)
	{
		m_pModelCom->ResetAnimation(i);
	}

	if (nullptr != m_pNavigationCom)
	{
		Safe_Release(m_pNavigationCom);
		Safe_Release(m_pNavigationCom);
		m_Components.erase(TEXT("Com_Navigation"));
	}

	if (LEVEL_TWILIGHT == GM->Get_CurrentLevel())
	{
		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Navigation_Twilight"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else if (LEVEL_RAPUNZEL == GM->Get_CurrentLevel())
	{
		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Navigation_Rapunzel"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
	{
		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Navigation_Rapunzel_Boss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else if (LEVEL_FROZEN == GM->Get_CurrentLevel() || LEVEL_TEST == GM->Get_CurrentLevel())
	{
		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_FROZEN, TEXT("Prototype_Component_Navigation_Frozen"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel())
	{
		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Navigation_KeyGrave"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else if (LEVEL_FINALBOSS == GM->Get_CurrentLevel())
	{
		CNavigation::NAVIDESC tNaviDesc;
		tNaviDesc.iCurrentIndex = 0;
		tNaviDesc.iPreviousIndex = 0;
		tNaviDesc.iPreviousFrame = 0;
		tNaviDesc.pParent = m_pTransformCom;

		/* For.Com_Navigation */
		if (FAILED(__super::Add_Component(LEVEL_FINALBOSS, TEXT("Prototype_Component_Navigation_FinalBoss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	_vector vPlayerPosition = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_Position();
	_vector vPlayerLook     = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_Look();

	_vector	vPosition = vPlayerPosition + vPlayerLook * 3.f;


	m_pTransformCom->MakeSameDirection(vPlayerLook);
	m_pNavigationCom->Find_Cell(vPosition);
	if (LEVEL_FINALBOSS != GM->Get_CurrentLevel())
	{
		_float	vPosY = m_pNavigationCom->Compute_Y(vPosition);
		vecy(vPosition) = vPosY;
	}
	m_pTransformCom->Set_Position(vPosition);

	m_pModelCom->Set_TempIndex_EachBody(0, STATE_APPEAR);
	m_pModelCom->Set_TempIndex_EachBody(1, STATE_EYE);

	m_bDead = false;
	m_bMakeWeapon = false;
	m_eCurrentState = STATE_APPEAR;
	m_pModelCom->Update_CombinedTransformationMatrices(0.0);

	m_AttackMonster.clear();

	if (nullptr != pArg)
		m_eSkill = STATE(*(_uint*)pArg);

	m_bMotionTrail = true;

	static_cast<CStatus*>(GM->Get_Player()->Get_Component(TEXT("Com_Status")))->DamagedMP(50.f);

	return S_OK;
}

_int CGoofy::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return RESULT_POOL;

	}
	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (FAILED(State_Manage(TimeDelta)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * m_dAnimationSpeed, m_pTransformCom);

	return _int();
}

_int CGoofy::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 5.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CGoofy::Render()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(SetUp_ConstantTable()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

	vector<CModel::MESHDESC*> MeshDescVector;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		if (true == m_bMotionTrail)
			m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices", &MeshDescVector);
		else
			m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	if (true == m_bMotionTrail)
	{
		CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
		tMotionTrailDesc.iSize = 280;
		tMotionTrailDesc.pMeshDescVector = &MeshDescVector;
		tMotionTrailDesc.vColor = _float4(1.f, 1.f, 0.7f, 1.f);
		tMotionTrailDesc.fRimPower = 0.8f;
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"), TEXT("Prototype_GameObject_MotionTrail"), &tMotionTrailDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"))->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
		MeshDescVector.clear();
	}

	return S_OK;
}

HRESULT CGoofy::Add_AttackMonster(CMonster * pMonster)
{
	if (nullptr == pMonster)
		return E_FAIL;

	m_AttackMonster.push_back(pMonster);

	return S_OK;
}

HRESULT CGoofy::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Goofy"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoofy::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoofy::Separate_Channels()
{
	if (FAILED(m_pModelCom->Make_BodyPartInfo(2)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	const char* part_name1 = "Face";
	const char* bone_name_want_to_search1[] = { "facial" };

	if (FAILED(m_pModelCom->Fill_BodyPartInfo(part_name1, bone_name_want_to_search1, 1)))
	{
		BREAKPOINT;
		return E_FAIL;
	}


	const char* part_name2 = "Body";
	const char* bone_name_want_to_search2[] = { m_pModelCom->Get_RootName() };

	if (FAILED(m_pModelCom->Fill_BodyPartInfo(part_name2, bone_name_want_to_search2, 1)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGoofy::State_Manage(_double TimeDelta)
{
	switch (m_eCurrentState)
	{
	case STATE_APPEAR:
		Appear(TimeDelta);
		break;
	case STATE_PUSH:
		Push(TimeDelta);
		break;
	case STATE_RUN:
		Run(TimeDelta);
		break;
	case STATE_THROWSTART:
		ThrowStart(TimeDelta);
		break;
	case STATE_THROWING:
		Throwing(TimeDelta);
		break;
	case STATE_THROWEND:
		ThrowEnd(TimeDelta);
		break;
	case STATE_EYE:
		Eye(TimeDelta);
		break;
	}

	return S_OK;
}

void CGoofy::Appear(_double TimeDelta)
{
	if (true == m_bMotionTrail && 7.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
		m_bMotionTrail = false;

	if (false == m_bMakeWeapon)
	{
		// 무기 생성
		CGoofy_Weapon::WEAPON_DESC tWeaponDesc;
		tWeaponDesc.pModel      = m_pModelCom;
		tWeaponDesc.pParent     = this;
		tWeaponDesc.pSocketName = "R_buki";
		tWeaponDesc.pTransform  = m_pTransformCom;

		switch (m_eSkill)
		{
		case STATE_RUN:
			m_pGameInstance->LetsPlaySound(L"Goofy_Start.wav", CSoundMgr::CHANNELID::GOOFY_C, 1.f);
			tWeaponDesc.eMode = CGoofy_Weapon::MODE_RUN;
			break;
		case STATE_THROWSTART:
			m_pGameInstance->LetsPlaySound(L"Goofy_Start.wav", CSoundMgr::CHANNELID::GOOFY_C, 1.f);
			tWeaponDesc.eMode = CGoofy_Weapon::MODE_THROW;
			break;
		}

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_GoofyWeapon"), TEXT("Prototype_GameObject_GoofyWeapon"), &tWeaponDesc)))
		{
			BREAKPOINT;
			return;
		}

		m_pWeapon = static_cast<CGoofy_Weapon*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_GoofyWeapon")));



		m_bMakeWeapon = true;
	}


	if (STATE_THROWSTART == m_eSkill)
	{
		m_pWeapon->Set_Turn(true);
	}


	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_pModelCom->Set_TempIndex_EachBody(0, m_eSkill);
		m_eCurrentState = m_eSkill;

		m_pWeapon->Set_Turn(false);

		if (STATE_RUN == m_eCurrentState)
		{
			m_pWeapon->Set_ScaleUp(true);
		}
	}
}

void CGoofy::Push(_double TimeDelta)
{
	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
	}
}

void CGoofy::Run(_double TimeDelta)
{
	if (14.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0) && 34.0 > m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		if (false == m_bEffectStart)
		{
			m_pGameInstance->LetsPlaySound(L"Goofy_Rush.wav", CSoundMgr::CHANNELID::GOOFY_A, 1.f);

			m_pWeapon->Set_Effect(true);
			m_pWeapon->Set_Run(true);
			m_bEffectStart = true;
		}
		m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
		if (LEVEL_FINALBOSS != GM->Get_CurrentLevel())
		{
			_vector vPos = m_pTransformCom->Get_Position();
			vecy(vPos) = m_pNavigationCom->Compute_Y(vPos);
			m_pTransformCom->Set_Position(vPos);
		}
	}

	if (false == m_bWeaponScaleDown && 41.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		m_pWeapon->Set_ScaleUp(false);
		m_pWeapon->Set_ScaleDown(true);
		m_pWeapon->Set_Effect(false);
		m_bWeaponScaleDown = true;
	}


	if (51.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0) && false == m_bThrowEnd)
	{
		CEffect_Manager::GetInstance()->Create_GoofyBye(m_pTransformCom->Get_Position());
		m_bThrowEnd = true;
	}

	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
		m_bWeaponScaleDown = false;
		m_pWeapon->Set_ScaleDown(false);
		m_bEffectStart = false;
		m_bThrowEnd = false;
	}
}

void CGoofy::ThrowStart(_double TimeDelta)
{
	// 이 때 던지는 상태로 전환
	if (30.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		m_pGameInstance->LetsPlaySound(L"Goofy_Throw.wav", CSoundMgr::CHANNELID::GOOFY_B, 1.f);

		m_pWeapon->Set_Throw(true);
		m_pWeapon->Set_Effect(true);
	}
	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_pModelCom->Set_TempIndex_EachBody(0, STATE_THROWING);
		m_eCurrentState = STATE_THROWING;
	}
}

void CGoofy::Throwing(_double TimeDelta)
{
	if (true == m_pModelCom->Get_CurrStopped_EachBody(0) || true == m_bThrowEnd)
	{
		m_pModelCom->Set_TempIndex_EachBody(0, STATE_THROWEND);
		m_eCurrentState = STATE_THROWEND;
		m_pWeapon->Set_ComeBack(true);
		m_bThrowEnd = false;
		m_pWeapon->Set_Effect(false);
	}
}

void CGoofy::ThrowEnd(_double TimeDelta)
{
	if (5.0 > m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		m_pWeapon->Set_ComeBackLerp(_float(m_pModelCom->Get_CurrTimeAcc_EachBody(0) / 5.0));
	}
	else
		m_pWeapon->Set_Throw(false);

	if (26.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0) && false == m_bThrowEnd)
	{
		CEffect_Manager::GetInstance()->Create_GoofyBye(m_pTransformCom->Get_Position());
		m_bThrowEnd = true;
	}
		

	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
		m_bThrowEnd = false;
	}
}

void CGoofy::Eye(_double TimeDelta)
{
	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
	}
}

CGoofy * CGoofy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGoofy*	pInstance = new CGoofy(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGoofy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGoofy::Clone(void * pArg)
{
	CGoofy*	pInstance = new CGoofy(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGoofy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGoofy::Free()
{
	__super::Free();

	m_AttackMonster.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
