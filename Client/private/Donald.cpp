#include "stdafx.h"
#include "..\public\Donald.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "GameManager.h"
#include "Donald_Weapon.h"
#include "Donald_Icicle.h"
#include "HierarchyNode.h"
#include "Donald_IceFlower.h"
#include "Effect_Particle.h"
#include "Donald_Ice.h"
#include "Monster.h"
#include "Effect_Manager.h"
#include "MotionTrail.h"
#include "Player.h"

#define NUMBER_OF_TARGET 8

CDonald::CDonald(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CDonald::CDonald(const CDonald & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDonald::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDonald::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 20.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

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

	m_pModelCom->Add_OnceListAll();
	m_pModelCom->Add_LerpList(STATE_ATTACK);
	m_pModelCom->Add_LerpList(STATE_HEAL);
	m_pModelCom->Add_LerpList(STATE_DANCE);

	if (FAILED(NativeConstruct_Pool(pArg)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDonald::NativeConstruct_Pool(void * pArg)
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
	_vector vPlayerLook = static_cast<CTransform*>(m_pGameInstance->Get_Component(GM->Get_CurrentLevel(), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_Look();

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

	m_bDead         = false;
	m_bMakeWeapon   = false;
	m_eCurrentState = STATE_APPEAR;

	m_pNode = m_pModelCom->Find_HierarchyNode("R_buki");

	m_AttackMonster.clear();

	if (nullptr != pArg)
		m_eSkill = STATE(*(_uint*)pArg);

	m_bMotionTrail = true;

	static_cast<CStatus*>(GM->Get_Player()->Get_Component(TEXT("Com_Status")))->DamagedMP(50.f);

	return S_OK;
}

_int CDonald::Tick(_double TimeDelta)
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
	
	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * m_dAnimationSpeed);

	return _int();
}

_int CDonald::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (false == m_bFire && STATE_ATTACK == m_eCurrentState && 14.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		CDonald_Icicle::ICICLEDESC tIcicleDesc;

		auto ColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));

		_int iTargetNum = NUMBER_OF_TARGET;

		for (auto& pCollider : *ColliderList)
		{
			tIcicleDesc.pTargetCollider = pCollider;

			_matrix WorldMatrix   = XMMatrixTranslation(-0.5f, 0.f, 0.f) 
				* XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) 
				* m_pNode->Get_CombinedTransformationMatrix() 
				* XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) 
				* m_pTransformCom->Get_WorldMatrix();
			tIcicleDesc.vPosition = WorldMatrix.r[3];

			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_DonaldIcicle")
				, TEXT("Prototype_GameObject_DonaldIcicle"), &tIcicleDesc)))
			{
				BREAKPOINT;
				return RESULT_ERROR;
			}
			iTargetNum--;
			if (0 >= iTargetNum)
				break;
		}


		m_bFire = true;
	}

	auto iter = m_AttackMonster.begin();
	for (auto iter = m_AttackMonster.begin(); iter < m_AttackMonster.end();)
	{
		if (true == (*iter)->Get_ReadyToDie())
			iter = m_AttackMonster.erase(iter);
		else
			iter++;
	}
	

	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 1.5f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}




HRESULT CDonald::Render()
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
		tMotionTrailDesc.vColor = _float4(0.5f, 0.7f, 1.f, 1.f);
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

HRESULT CDonald::Add_AttackMonster(CMonster * pMonster)
{
	if (nullptr == pMonster)
		return E_FAIL;

	m_AttackMonster.push_back(pMonster);

	return S_OK;
}

HRESULT CDonald::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Donald"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDonald::SetUp_ConstantTable()
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

HRESULT CDonald::Separate_Channels()
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

HRESULT CDonald::State_Manage(_double TimeDelta)
{
	switch (m_eCurrentState)
	{
	case STATE_APPEAR:
		Appear(TimeDelta);
		break;
	case STATE_HEAL:
		Heal(TimeDelta);
		break;
	case STATE_ATTACK:
		Attack(TimeDelta);
		break;
	case STATE_DANCE:
		Dance(TimeDelta);
		break;
	case STATE_EYE:
		Eye(TimeDelta);
		break;
	}

 	return S_OK;
}

void CDonald::Appear(_double TimeDelta)
{
	if (true == m_bMotionTrail && 6.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
		m_bMotionTrail = false;

	if (false == m_bMakeWeapon)
	{
		// 公扁 积己
		CDonald_Weapon::WEAPON_DESC tWeaponDesc;
		tWeaponDesc.pModel = m_pModelCom;
		tWeaponDesc.pParent = this;
		tWeaponDesc.pSocketName = "R_buki";
		tWeaponDesc.pTransform = m_pTransformCom;

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_DonaldWeapon"), TEXT("Prototype_GameObject_DonaldWeapon"), &tWeaponDesc)))
		{
			BREAKPOINT;
			return;
		}

		m_bMakeWeapon = true;
	}
	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_pModelCom->Set_TempIndex_EachBody(0, m_eSkill);
		m_eCurrentState = m_eSkill;
	}
}

void CDonald::Heal(_double TimeDelta)
{
	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
	}
}

void CDonald::Attack(_double TimeDelta)
{
	if (12.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0) && 13.0 > m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		m_pGameInstance->LetsPlaySound(L"Donald_Freeze2.wav", CSoundMgr::CHANNELID::DONALD_B, 1.f);
		m_pGameInstance->LetsPlaySound(L"Donald_Magic.wav", CSoundMgr::CHANNELID::DONALD_C, 1.f);
	}
		/*===*/
	if (29.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0) && false == m_bLastEffect)
	{
		CEffect_Manager::GetInstance()->Create_DonaldBye(m_pTransformCom->Get_Position());
		m_bLastEffect = true;
	}

	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
		m_bFire = false;
		m_bLastEffect = false;
		m_eCurrentState = STATE_APPEAR;
	}
}

void CDonald::Dance(_double TimeDelta)
{
	_matrix WorldMatrix = XMMatrixTranslation(-10.f, 0.f, 0.f) * XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * m_pNode->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();
	m_vPosition = XMStoreFloat4RT(WorldMatrix.r[3]);

	if (false == m_bDance[0] && 14.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		m_pGameInstance->LetsPlaySound(L"Donald_Ice2.wav", CSoundMgr::CHANNELID::DONALD_B, 1.f);

		CDonald_IceFlower::ICEFLOWERDESC tIceFlowerDesc;
		tIceFlowerDesc.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		tIceFlowerDesc.pDead       = &m_bDead;

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_DonaldFlower"), &tIceFlowerDesc)))
		{
			BREAKPOINT;
			return;
		}
		m_Ice.push_back(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));

		m_bDance[0] = true;
	}

	if (false == m_bDance[1] && 29.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
	

		CDonald_IceFlower::ICEFLOWERDESC tIceFlowerDesc;
		XMStoreFloat4x4(&tIceFlowerDesc.WorldMatrix, XMMatrixRotationAxis(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.f)) * m_pTransformCom->Get_WorldMatrix());
		tIceFlowerDesc.pDead = &m_bDead;

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_DonaldFlower"), &tIceFlowerDesc)))
		{
			BREAKPOINT;
			return;
		}
		static_cast<CDonald_IceFlower*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")))->Set_Color(_float4(0.45f, 0.5f, 1.f, 0.9f));
		m_Ice.push_back(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));

		m_bDance[1] = true;
	}

	if (42.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0) && 52.0 > m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		m_fMakeIceTime++;

		if (0.03f < m_fMakeIceTime)
		{
			_float4 vPos;
			WorldMatrix = XMMatrixTranslation(4.f, 0.f, 0.f) * WorldMatrix;
			memcpy(&vPos, &WorldMatrix.r[3], sizeof(_float4));
			vPos.y = vecy(m_pTransformCom->Get_Position()) + 0.7f;
			
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_DonaldIce"), &vPos)))
			{
				BREAKPOINT;
				return;
			}
			m_Ice.push_back(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Effect")));

			m_fMakeIceTime = 0.f;
		}

	}

	if (false == m_bDance[2] && 42.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		// 颇萍努 积己
		CEffect_Particle::PARTICLEDESC tParticleDesc;
		tParticleDesc.bChase          = false;
		tParticleDesc.bLoop           = false;
		tParticleDesc.bTrail          = true;
		tParticleDesc.bUseEasing      = false;
		tParticleDesc.eDir            = CEffect_Particle::DIR_ONE;
		tParticleDesc.eParticleNumber = CEffect_Particle::NUM_10;

		_float	fAddColor[4] = { -1.f, -0.18f, -0.56f, 0.f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);

		_float	fMulColor[4] = { 0.03f, 0.63f, 5.47f, 1.f };
		memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

		tParticleDesc.fGravityPower   = 0.f;
		tParticleDesc.fLiveTime       = 0.4f;
		tParticleDesc.fMakeRange      = 3.f;
		tParticleDesc.fMaxDistance    = 300.f;
		tParticleDesc.fMinSpeed       = 0.f;
		tParticleDesc.fMaxSpeed       = 0.f;
		tParticleDesc.fScaleSpeed     = 2.f;
		tParticleDesc.fOriginScale    = 0.5f;
		tParticleDesc.fTrailInterval  = 0.f;
		tParticleDesc.iLevelIndex     = 0;
		tParticleDesc.iTexIndex       = 2;
		tParticleDesc.pDead           = &m_bDead;
		tParticleDesc.pParentPosition = &m_vPosition;
		tParticleDesc.vDir            = __vector(0.f, 1.f, 0.f, 0.f);

		for (_uint i = 0; i < 30; i++)
		{
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
			{
				BREAKPOINT;
				return;
			}
		}

		_float	AddColor[4] = { -0.43f, -0.35f, 1.f, 0.f };
		memcpy(tParticleDesc.fAddColor, AddColor, sizeof(_float) * 4);

		_float	MulColor[4] = { 1.f, 1.f, 1.f, 1.f };
		memcpy(tParticleDesc.fMulColor, MulColor, sizeof(_float) * 4);

		for (_uint i = 0; i < 30; i++)
		{
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
			{
				BREAKPOINT;
				return;
			}
		}
		m_bDance[2] = true;
	}


	if (false == m_bDance[3] && 73.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0))
	{
		m_pGameInstance->LetsPlaySound(L"Donald_Ice3.wav", CSoundMgr::CHANNELID::DONALD_C, 1.f);
		m_pGameInstance->LetsPlaySound_Monster(L"Donald_Skill2_3.wav", 0.5f);

		// 颇萍努 积己
		CEffect_Particle::PARTICLEDESC tParticleDesc;
		tParticleDesc.bChase          = false;
		tParticleDesc.bLoop           = false;
		tParticleDesc.bTrail          = false;
		tParticleDesc.bUseEasing      = false;
		tParticleDesc.eDir            = CEffect_Particle::DIR_RANDOM;
		tParticleDesc.eParticleNumber = CEffect_Particle::NUM_100;

		_float	fAddColor[4] = { -1.f, -0.18f, -0.56f, 0.f };
		memcpy(tParticleDesc.fAddColor, fAddColor, sizeof(_float) * 4);
		_float	fMulColor[4] = { 0.03f, 0.63f, 5.47f, 1.f };
		memcpy(tParticleDesc.fMulColor, fMulColor, sizeof(_float) * 4);

		tParticleDesc.fGravityPower   = 0.f;
		tParticleDesc.fLiveTime       = 0.5f;
		tParticleDesc.fMakeRange      = 8.f;
		tParticleDesc.fMaxDistance    = 300.f;
		tParticleDesc.fMinSpeed       = 50.f;
		tParticleDesc.fMaxSpeed       = 80.f;
		tParticleDesc.fScaleSpeed     = 0.8f;
		tParticleDesc.fOriginScale    = 0.5f;
		tParticleDesc.fTrailInterval  = 0.f;
		tParticleDesc.iLevelIndex     = 0;
		tParticleDesc.iTexIndex       = 2;
		tParticleDesc.pDead           = &m_bDead;
		tParticleDesc.pParentPosition = &XMStoreFloat4RT(m_pTransformCom->Get_Position());
		tParticleDesc.vDir            = __vector(0.f, 1.f, 0.f, 0.f);

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return;
		}

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return;
		}

		_float	AddColor[4] = { -0.43f, -0.35f, 1.f, 0.f };
		memcpy(tParticleDesc.fAddColor, AddColor, sizeof(_float) * 4);

		_float	MulColor[4] = { 1.f, 1.f, 1.f, 1.f };
		memcpy(tParticleDesc.fMulColor, MulColor, sizeof(_float) * 4);

		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return;
		}
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Particle"), &tParticleDesc)))
		{
			BREAKPOINT;
			return;
		}

		for (size_t i = 0; i < m_Ice.size(); i++)
		{
			m_Ice[i]->Set_Dead(true);
		}
		m_Ice.clear();

		m_bDance[3] = true;
	}

	if (95.0 < m_pModelCom->Get_CurrTimeAcc_EachBody(0) && false == m_bLastEffect)
	{
		
		CEffect_Manager::GetInstance()->Create_DonaldBye(m_pTransformCom->Get_Position());
		m_bLastEffect = true;
	}

	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
		m_bLastEffect = false;
		for (_uint i = 0; i < 4; i++)
		{
			m_bDance[i] = false;
		}
	}
}

void CDonald::Eye(_double TimeDelta)
{
	if (true == m_pModelCom->Get_CurrStopped_EachBody(0))
	{
		m_bDead = true;
	}
}

CDonald * CDonald::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDonald*	pInstance = new CDonald(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDonald");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDonald::Clone(void * pArg)
{
	CDonald*	pInstance = new CDonald(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDonald");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDonald::Free()
{
	__super::Free();

	m_AttackMonster.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
