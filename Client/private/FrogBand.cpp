#include "stdafx.h"
#include "FrogBand.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

CFrogBand::CFrogBand(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CFrogBand::CFrogBand(const CFrogBand& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFrogBand::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrogBand::NativeConstruct(void* pArg)
{
	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 3.5f;
	TransformDesc.fRotationPerSec = 10.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 밴드 태그 설정, 위치설정
	if (pArg != nullptr)
	{
		memcpy(&m_FrogBandDesc, pArg, sizeof(FROGBANDDESC));
		m_pTransformCom->Set_Position(m_FrogBandDesc.vPos);
	}

	// 컴포넌트 초기화
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// 애니메이션 설정
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// 쉐이더패스 설정
	m_iPass = 0;

	return S_OK;
}

_int CFrogBand::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// 죽으면 RESULT_DEAD 반환
	if (true == m_bDead)
		return RESULT_DEAD;

	// 애니메이션 1번 2번 계속 변경
	ChangeAnim(TimeDelta);

	// 모델 업데이트
	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta);

	return RESULT_NOPROBLEM;
}

_int CFrogBand::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// 절두체 안에 있다면 + m_bDead 상태가 아니면 렌더그룹에 추가
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) &&
		FALSE == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	return RESULT_NOPROBLEM;
}

HRESULT CFrogBand::Render()
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
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CFrogBand::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// 쉐이더 테스트
	pImGui_Manager->SetGameObjectShaderPass(string("FB Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	ImGui::End();
}

HRESULT CFrogBand::Save_ObjectData(HANDLE hFile)
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

HRESULT CFrogBand::Load_ObjectData(HANDLE hFile)
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

void CFrogBand::ChangeAnim(_double TimeDelta)
{
	if (m_pModelCom->CheckFinished_Input(0))
	{
		m_pModelCom->ResetAnimation(1);
		m_pModelCom->Set_TempIndex(1);
	}
	else if (m_pModelCom->CheckFinished_Input(1))
	{
		m_pModelCom->ResetAnimation(0);
		m_pModelCom->Set_TempIndex(0);
	}
}

HRESULT CFrogBand::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TEST, m_FrogBandDesc.ModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrogBand::SetUp_Animations()
{
	m_pModelCom->Add_OnceListAll();

	m_pModelCom->Add_LerpListAll();

	m_pModelCom->Set_TempIndex(0);

	return S_OK;
}

HRESULT CFrogBand::SetUp_ConstantTable()
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

CFrogBand* CFrogBand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFrogBand* pInstance = new CFrogBand(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CFrogBand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFrogBand::Clone(void* pArg)
{
	CFrogBand* pInstance = new CFrogBand(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CFrogBand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFrogBand::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}