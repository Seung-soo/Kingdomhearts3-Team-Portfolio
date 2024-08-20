#include "stdafx.h"
#include "Flower.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CFlower::CFlower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
} 

CFlower::CFlower(const CFlower& rhs)
	: CMonster(rhs)
{
}

HRESULT CFlower::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlower::NativeConstruct(void* pArg)
{
	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 3.5f;
	TransformDesc.fRotationPerSec = 1.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 몬스터 타입 설정
	m_MonsterType = MONSTERTYPE_FLOWER;

	// 컴포넌트 초기화
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// 애니메이션 설정
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// 쉐이더패스 설정
	m_iPass = 0;

	// 위치설정
	if (pArg != nullptr)
	{
		_vector vPos = _zeroPoint;
		memcpy(&vPos, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPos);
	}

	return S_OK;
}

_int CFlower::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// 죽으면 RESULT_DEAD 반환
	if (true == m_bDead)
		return RESULT_DEAD;

	// 애니메이션 0번->1번(3번 반복)->2번->3번 변경
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0);

	if (pPlayer != nullptr)
		ChangeAnim(TimeDelta);

	// 모델 업데이트
	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta);

	return RESULT_NOPROBLEM;
}

_int CFlower::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// 절두체 안에 있다면 + m_bDead 상태가 아니면 렌더그룹에 추가
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) &&
		FALSE == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CFlower::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	// 쉐이더 전역 상수에 값 전달
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	// 모델이 갖고있는 머테리얼 수 가져와서 쉐이더의 텍스처 전역 상수에 값 전달하고 렌더링
	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CFlower::Render_Shadow()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	_uint iPass = 14;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Render(m_pShaderCom, iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CFlower::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// 쉐이더 테스트
	pImGui_Manager->SetGameObjectShaderPass(string("FB Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	ImGui::End();
}

HRESULT CFlower::Save_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 월드행렬 저장
	_float4x4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, m_pTransformCom->Get_WorldMatrix());

	DWORD dwBytes = 0;
	WriteFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CFlower::Load_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 세이브데이터에서 월드행렬을 로드
	DWORD		dwBytes = 0;
	_float4x4	worldMatrix;

	ReadFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	// 월드행렬 세팅
	m_pTransformCom->Set_WorldMatrix(worldMatrix);

	return S_OK;
}

void CFlower::ChangeAnim(_double TimeDelta)
{
	if (m_pModelCom->Get_CurrAnimIndex() != 0)
	{
		if (m_iAnimAcc % 2 == 0)
			m_pTransformCom->Turn_Right(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta);
		else
			m_pTransformCom->Turn_Left(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta);
	}

	// 0:등장, 1:Idle, 2:Bloom, 3:Dead
	if (m_pModelCom->Check_CurAnim_And_Finished(0))
		m_pModelCom->Set_TempIndex(1);

	if (m_pModelCom->Check_CurAnim_And_Finished(1))
		++m_iAnimAcc;

	if (m_pModelCom->Get_CurrAnimIndex() == 1)
		m_pModelCom->Set_CurrPlaySpeed(10.0);

	if (m_pModelCom->Get_CurrAnimIndex() == 1 && m_iAnimAcc == 3)
	{
		m_pModelCom->ResetAnimation(2);
		m_pModelCom->Set_TempIndex(2);
	}

	if (m_pModelCom->Get_CurrAnimIndex() == 2 && m_pModelCom->Check_ProgressRate(0.6f) && m_bSummonNobody == false)
	{
		_vector vPos = Get_Transform()->Get_Position() + __vector(0.f, 5.f, 0.f, 0.f);

		// LEVEL_RAPUNZEL 레벨일때는 오브젝트풀에서 가져오고 아닐때는 사본생성
		if (m_pGameInstance->Get_LevelIndex() == LEVEL_RAPUNZEL)
			m_pGameInstance->ThrowToLayer_And_Return(LEVEL_RAPUNZEL, TEXT("Layer_Nobody"), &vPos);
		else
			m_pGameInstance->Add_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Nobody"), TEXT("Prototype_GameObject_Nobody"), &vPos);

		m_bSummonNobody = true;
	}

	if (m_pModelCom->Check_CurAnim_And_Finished(2))
		m_pModelCom->Set_TempIndex(3);

	if (m_pModelCom->Check_CurAnim_And_Finished(3))
		m_bDead = true;
}

HRESULT CFlower::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_Flower"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlower::SetUp_Animations()
{
	m_pModelCom->Add_OnceListAll();

	m_pModelCom->Rmv_OnceList(1);

	m_pModelCom->Add_LerpListAll();

	m_pModelCom->Set_TempIndex(0);

	return S_OK;
}

HRESULT CFlower::SetUp_ConstantTable()
{
	// 월드행렬
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// 뷰행렬
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	// 투영행렬
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}

CFlower* CFlower::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFlower* pInstance = new CFlower(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CFlower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFlower::Clone(void* pArg)
{
	CFlower* pInstance = new CFlower(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CFlower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFlower::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}