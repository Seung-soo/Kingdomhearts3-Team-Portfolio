#include "stdafx.h"
#include "..\public\Donald_Ice.h"
#include "GameInstance.h"
#include "Easing_Function.h"
#include "Effect_Particle.h"
#include "GameManager.h"
#include "Donald.h"
#include "Monster.h"

CDonald_Ice::CDonald_Ice(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CDonald_Ice::CDonald_Ice(const CDonald_Ice & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDonald_Ice::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDonald_Ice::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 10.f;
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

	_float4 vPosition = *(_float4*)pArg;

	m_pTransformCom->Set_Position(XMLoadFloat4(&vPosition));

	m_fScale = 1.5f + _float(rand() % 201 - 100) / 500.f;
	m_pTransformCom->Scaled(__vector(m_fScale, m_fScale, m_fScale, 0.f));
	m_fEndScale = m_fScale + 1.f;

	m_pTransformCom->Rotation(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(_float(rand() % 90)));

	m_pDonald = static_cast<CDonald*>(m_pGameInstance->Get_Back(LEVEL_STATIC, TEXT("Layer_Donald")));

	//m_pGameInstance->LetsPlaySound_Monster(L"Donald_Skill2_1.wav", 0.5f);

	return S_OK;
}

_int CDonald_Ice::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		return RESULT_DEAD;
	}

	if (0 > __super::Tick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	m_fScale += (_float)TimeDelta;
	if (m_fEndScale < m_fScale)
		m_fScale = m_fEndScale;
	m_pTransformCom->Scaled(__vector(m_fScale, m_fScale, m_fScale, 0.f));

	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	return _int();
}

_int CDonald_Ice::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	//	몬스터 콜라이더 리스트
	auto pDestColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
	//	한번 충돌한 몬스터 벡터 컨테이너
	auto pAttackMonsterContainer = m_pDonald->Get_AttackMonsterContainer();

	//	몬스터 콜라이더 리스트를 순회
	for (auto& pDestCollider : *pDestColliderList)
	{
		//	충돌 검사
		if (true == m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, pDestCollider))
		{

			CGameObject*	pDestParent = pDestCollider->Get_Parent();
			_bool			bInSameMonster = false;
			static_cast<CMonster*>(pDestParent)->Set_Stun(1.5f);
			//	이미 같은 몬스터를 얼렸는지 검사
			for (size_t i = 0; i < pAttackMonsterContainer->size(); i++)
			{
				if ((*pAttackMonsterContainer)[i] == pDestParent)
				{
					bInSameMonster = true;
					break;
				}
			}
			//	같은 몬스터가 없다면 충돌한 몬스터 벡터 컨테이너에 집어 넣어준다.
			if (false == bInSameMonster)
			{
				// 얼리기
				m_pDonald->Add_AttackMonster(static_cast<CMonster*>(pDestParent));
			}

		}

	}


	if (m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_Position(), 2.f))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
#ifdef _DEBUG
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif
	}

	return RESULT_NOPROBLEM;
}

HRESULT CDonald_Ice::Render()
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

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, 33, i);
	}

	return S_OK;
}

void CDonald_Ice::PostRender(_double TimeDelta)
{
}

HRESULT CDonald_Ice::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_DonaldIce"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Collider */
	CCollider::COLLIDERDESC tColliderDesc;
	tColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	tColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	tColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tColliderDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDonald_Ice::SetUp_ConstantTable()
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

	_float4 vColor = _float4(0.5f, 0.6f, 1.f, 0.8f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_iBlur", &bBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	_bool bDistortion = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_bDistortion", &bDistortion, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float fTransparency = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fTransparency", &fTransparency, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	

	return S_OK;
}

CDonald_Ice * CDonald_Ice::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDonald_Ice*	pInstance = new CDonald_Ice(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CDonald_Ice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDonald_Ice::Clone(void * pArg)
{
	CDonald_Ice*	pInstance = new CDonald_Ice(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CDonald_Ice");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDonald_Ice::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
