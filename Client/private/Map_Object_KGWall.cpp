#include "stdafx.h"
#include "..\public\Map_Object_KGWall.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Easing_Function.h"


CMap_Object_KGWall::CMap_Object_KGWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMap_Object_KGWall::CMap_Object_KGWall(const CMap_Object_KGWall & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMap_Object_KGWall::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap_Object_KGWall::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 0.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	iCom_Object = (*(_int*)pArg);
	iRandomWall = rand() % 4;
	iRandom = rand() % 9;
	if (FAILED(SetUp_Components()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	switch (iCom_Object) /* 포지션 잡기 */
	{
	case 0:
		vPosition = XMVectorSet(58.f, 0.f, 66.5f, 1.f);
		break;
	case 1:
		vPosition = XMVectorSet(46.5f, 0.f, 66.5f, 1.f);
		break;
	case 2:
		vPosition = XMVectorSet(35.f, 0.f, 66.5f, 1.f);
		break;
	case 3:
		vPosition = XMVectorSet(23.5f, 0.f, 66.5f, 1.f);
		break;
	case 4:
		vPosition = XMVectorSet(12.f, 0.f, 66.5f, 1.f);
		break;
	case 5:
		vPosition = XMVectorSet(0.5f, 0.f, 66.5f, 1.f);
		break;
	case 6:
		vPosition = XMVectorSet(-11.f, 0.f, 66.5f, 1.f);
		break;
	case 7:
		vPosition = XMVectorSet(-22.5f, 0.f, 66.5f, 1.f);
		break;
	case 8:
		vPosition = XMVectorSet(-34.f, 0.f, 66.5f, 1.f);
		break;
	case 9:
		vPosition = XMVectorSet(-45.5f, 0.f, 66.5f, 1.f);
		break;
	case 10:
		vPosition = XMVectorSet(-57.f, 0.f, 66.5f, 1.f);
		break;
	case 11:
		vPosition = XMVectorSet(58.f, 0.f, -66.5f, 1.f);
		break;
	case 12:
		vPosition = XMVectorSet(46.5f, 0.f, -66.5f, 1.f);
		break;
	case 13:
		vPosition = XMVectorSet(35.f, 0.f, -66.5f, 1.f);
		break;
	case 14:
		vPosition = XMVectorSet(23.5f, 0.f, -66.5f, 1.f);
		break;
	case 15:
		vPosition = XMVectorSet(12.f, 0.f, -66.5f, 1.f);
		break;
	case 16:
		vPosition = XMVectorSet(0.5f, 0.f, -66.5f, 1.f);
		break;
	case 17:
		vPosition = XMVectorSet(-11.f, 0.f, -66.5f, 1.f);
		break;
	case 18:
		vPosition = XMVectorSet(-22.5f, 0.f, -66.5f, 1.f);
		break;
	case 19:
		vPosition = XMVectorSet(-34.f, 0.f, -66.5f, 1.f);
		break;
	case 20:
		vPosition = XMVectorSet(-45.5f, 0.f, -66.5f, 1.f);
		break;
	case 21:
		vPosition = XMVectorSet(-57.f, 0.f, -66.5f, 1.f);
		break;
	case 22:
		vPosition = XMVectorSet(66.5, 0.f, 58.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 23:
		vPosition = XMVectorSet(66.5, 0.f, 46.5, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 24:
		vPosition = XMVectorSet(66.5, 0.f, 35.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 25:
		vPosition = XMVectorSet(66.5, 0.f, 23.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 26:
		vPosition = XMVectorSet(66.5, 0.f, 12.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 27:
		vPosition = XMVectorSet(66.5, 0.f, 0.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 28:
		vPosition = XMVectorSet(66.5, 0.f, -11.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 29:
		vPosition = XMVectorSet(66.5, 0.f, -22.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 30:
		vPosition = XMVectorSet(66.5, 0.f, -34.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 31:
		vPosition = XMVectorSet(66.5, 0.f, -45.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 32:
		vPosition = XMVectorSet(66.5, 0.f, -57.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 33:
		vPosition = XMVectorSet(-66.5, 0.f, 58.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 34:
		vPosition = XMVectorSet(-66.5, 0.f, 46.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 35:
		vPosition = XMVectorSet(-66.5, 0.f, 35.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 36:
		vPosition = XMVectorSet(-66.5, 0.f, 23.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 37:
		vPosition = XMVectorSet(-66.5, 0.f, 12.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 38:
		vPosition = XMVectorSet(-66.5, 0.f, 0.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 39:
		vPosition = XMVectorSet(-66.5, 0.f, -11.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 40:
		vPosition = XMVectorSet(-66.5, 0.f, -22.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 41:
		vPosition = XMVectorSet(-66.5, 0.f, -34.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 42:
		vPosition = XMVectorSet(-66.5, 0.f, -45.5f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	case 43:
		vPosition = XMVectorSet(-66.5, 0.f, -57.f, 1.f);
		m_pTransformCom->NowRotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(90.f));
		break;
	default:
		break;
	}


	XMStoreFloat4(&fPosition, vPosition);
	m_pTransformCom->Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pTransformCom->Set_Position(vPosition);

	return S_OK;
}

_int CMap_Object_KGWall::Tick(_double TimeDelta)
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

	if (false == bPattern)
	{
		dRandomTime += TimeDelta;
		if (3 < dRandomTime)
		{
			iRandom = rand() % 9;
			dRandomTime = 0.0;
		}
	}
		
	if (0 == iRandom)
		bPattern = true;

	if (true == bPattern)
	{
		switch (iCnt)
		{
		case 0:
			dTime += TimeDelta;
			if (0.01 < dTime)
			{
				if (false == bPattern1)
				{
					bPattern1 = true;
				}
				else
				{
					bPattern1 = false;
				}
				++iPatternCnt;
				dTime = 0.0;
			}

			if (false == bPattern1)
			{
				SettingPosY = 0.f;
			}
			else
			{
				SettingPosY = -0.5f;
			}

			if (60 <= iPatternCnt)
			{
				iPatternCnt = 0;
				iCnt = 1;
			}
				
			break;
		case 1:
			dTime += TimeDelta;
			if (0.01 < dTime && 5.f > fSpeed)
			{
				fSpeed += 0.05f;
				dTime = 0.0;
			}

			SettingPosY = -fSpeed;

			if (5.f <= fSpeed)
			{
				dTime2 += TimeDelta;
				if (2 < dTime2)
				{
					dTime2 = 0.0;
					iCnt = 2;
				}
			}
				
			break;
		case 2:
			dTime += TimeDelta;
			if (0.01 < dTime && 0.f != fSpeed)
			{
				fSpeed -= 0.01f;
				dTime = 0.0;
			}

			SettingPosY = -fSpeed;

			if (0.f >= fSpeed)
			{
				fSpeed = 0.f;
				iCnt = 3;
			}
			break;
		case 3:
			bPattern = false;
			iCnt = 0;
			break;
		default:
			break;
		}

	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition.x, fPosition.y + SettingPosY, fPosition.z, 1.f));

	return RESULT_NOPROBLEM;
}

_int CMap_Object_KGWall::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CMap_Object_KGWall::Render()
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
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CMap_Object_KGWall::SetUp_Components()
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
	switch (iRandomWall)
	{
	case 0:
		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		break;
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch5"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch6"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_KG_MapSwitch7"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		break;

	default:
		break;
	}



	return S_OK;
}

HRESULT CMap_Object_KGWall::SetUp_ConstantTable()
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

	// 쉐이더에 던져야 하는 변수들
	_bool		bBlur = false;
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

CMap_Object_KGWall * CMap_Object_KGWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMap_Object_KGWall*	pInstance = new CMap_Object_KGWall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMap_Object_KGWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMap_Object_KGWall::Clone(void * pArg)
{
	CMap_Object_KGWall*	pInstance = new CMap_Object_KGWall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMap_Object_KGWall");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap_Object_KGWall::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
