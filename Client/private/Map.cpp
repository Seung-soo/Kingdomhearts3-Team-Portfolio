#include "stdafx.h"
#include "Map.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMap::CMap(const CMap& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMap::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap::NativeConstruct(void* pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;
	tTransformDesc.fSpeedPerSec    = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_MapDesc, pArg, sizeof(MAPDESC));
	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iPass = 31;

	m_fFrustumRange = 20.f;


	// For Shadow
	m_isStaticShadow = true;

	return S_OK;
}

_int CMap::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_bDead)
		return RESULT_DEAD;

#ifdef _DEBUG
	/*if (m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());*/
#endif

	return RESULT_NOPROBLEM;
}

_int CMap::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;
	
	// 절두체 안에 있다면 렌더그룹에 추가
	if (CGameInstance::GetInstance()->Get_LevelIndex() != LEVEL_LOADING)
	{
		if (true == m_bRenderShadow || TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

		if(true == m_bRenderShadow)
			m_bRenderShadow = false;
	}

	else
		m_bRenderShadow = true;


#ifdef _DEBUG
	/*if (m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL && CImGui_Manager::GetInstance()->show_mapcollider)
		m_pRendererCom->Add_Debug(m_pColliderCom);*/
#endif
	if (true == m_pGameInstance->Get_KeyEnter(DIK_NUMPAD2))
	{
		if (31 == m_iPass)
			m_iPass = 40;
		else if (40 == m_iPass)
			m_iPass = 31;
	}


	return RESULT_NOPROBLEM;
}

HRESULT CMap::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	_uint iPass = m_iPass;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		
		if (FAILED(m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture")))
			iPass = 40;

		_bool bBlur = true;
		if (FAILED(m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture")))
			bBlur = false;

		if (true == m_bCullnone)
			iPass = 47;

		if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &bBlur, sizeof(_bool))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pModelCom->Render(m_pShaderCom, iPass, i);
	}

	_bool IsBlur = false;
	if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &IsBlur, sizeof(_bool))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMap::Render_Shadow()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_STATIC).LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_STATIC).LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	_uint iPass = 43;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Render(m_pShaderCom, iPass, i);
	}

	return S_OK;
}

HRESULT CMap::Save_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// m_MapDesc.WorldMatrix 에 월드행렬 값 채워주기
	XMStoreFloat4x4(&m_MapDesc.WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	// MAPDESC 구조체 저장
	DWORD dwBytes = 0;
	WriteFile(hFile, &m_MapDesc, sizeof(MAPDESC), &dwBytes, nullptr);

	// m_fFrustumRange 저장
	WriteFile(hFile, &m_fFrustumRange, sizeof(_float), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CMap::Load_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 세이브데이터에서 MAPDESC 구조체 로드
	DWORD dwBytes = 0;
	ReadFile(hFile, &m_MapDesc, sizeof(MAPDESC), &dwBytes, nullptr);

	// 월드행렬 세팅
	m_pTransformCom->Set_WorldMatrix(m_MapDesc.WorldMatrix);

	// 세이브데이터에서 m_fFrustumRange 로드
	ReadFile(hFile, &m_fFrustumRange, sizeof(_float), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CMap::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_MapDesc.ModelPrototypeLevel, m_MapDesc.ModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	// 나무는 후면추려내기를 하지 않는다.
	char szModelTag[MAX_PATH] = "";

	WideCharToMultiByte(CP_UTF8, 0, m_MapDesc.ModelTag, MAX_PATH, szModelTag, IM_ARRAYSIZE(szModelTag), NULL, NULL);

	string str(szModelTag);

	if (string::npos != str.find("Tree"))
		m_bCullnone = true;


#ifdef _DEBUG
	///* For.Com_Collider */
	//CCollider::COLLIDERDESC ColliderDesc;
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	//ColliderDesc.vScale       = _float3(5.f, 15.f, 5.f);
	//ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	//ColliderDesc.vTranslation = _float4(0.f, 7.5f, 0.f, 1.f);
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
	//	return E_FAIL;
#endif

	return S_OK;
}

HRESULT CMap::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}

CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMap* pInstance = new CMap(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
	CMap* pInstance = new CMap(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
#ifdef _DEBUG
	//Safe_Release(m_pColliderCom);
#endif
}