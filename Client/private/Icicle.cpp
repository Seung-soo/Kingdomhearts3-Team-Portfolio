#include "stdafx.h"
#include "Icicle.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "Effect_Manager.h"
#include "Monster.h"

CIcicle::CIcicle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CIcicle::CIcicle(const CIcicle& rhs)
	: CGameObject(rhs)
{
}

HRESULT CIcicle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIcicle::NativeConstruct(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;

	TransformDesc.fSpeedPerSec    = 10.f;
	TransformDesc.fRotationPerSec = 15.f;
	TransformDesc.fScalingPerSec  = 5.f;

	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// ������Ʈ �ʱ�ȭ
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// �ִϸ��̼� ����
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// ���̴��н� ����
	m_iPass = 0;

	// ���ݻ��� �ѵα�
	m_bIsAttacking = true;

	if (pArg != nullptr)
	{
		_vector vPos;
		memcpy(&vPos, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPos);
	}

	// �ʱ� ������ �۰�
	m_pTransformCom->Scaled(__vector(0.01f, 0.01f, 0.01f, 0.f));

	// ����Ʈ
	CEffect_Manager::GetInstance()->Create_IcicleEffect(m_pTransformCom);

	return S_OK;
}

_int CIcicle::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// �ð� �������� ������ Ŀ����
	m_TimeAcc += TimeDelta * 10.0;
	if (vecx(m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT)) < 1.f)
	{
		if (m_TimeAcc >= 1.0)
			m_TimeAcc = 1.0;

		m_pTransformCom->Scaled(__vector((_float)m_TimeAcc, (_float)m_TimeAcc, (_float)m_TimeAcc, 0.f));
	}

	// m_bPlayAnim �� false �� ������ �ִϸ��̼� ��� x
	if (m_bPlayAnim == false)
	{
		m_pModelCom->Set_TargetTimeAcc(0, 0.0);
	}
	// m_bPlayAnim �� true �� ��쿡 0 �� �ִϸ��̼�(�ı��Ǵ� �ִϸ��̼�) ������ ������
	else
	{
		if (m_pModelCom->Get_TargetTimeAcc(0) >= 70.0)
		{
			StartDissolve(0.8f);
			m_iPass = 4;
		}
	}

	// ������ ������ Dead
	if (m_fDissolveAmount >= 1.0)
		m_bDead = true;

	// ������ RESULT_DEAD ��ȯ
	if (true == m_bDead)
		return RESULT_DEAD;

	// �� ������Ʈ
	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

	// �ݶ��̴� ��ġ ������Ʈ
	m_pColliderCom[0]->Update_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom[1]->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// �ݸ����Ŵ����� �ݶ��̴� ��� (Tick �� ���������� ����ؾ���)
	if (m_bIsAttacking == true)
	{
		m_pGameInstance->Add_Collider(string("Icicle"), m_pColliderCom[0]);
		m_pGameInstance->Add_Collider(string("Icicle"), m_pColliderCom[1]);
	}

	// ����Ʈ�� ��ġ����
	m_pTransformCom->Get_PositionFloat4_Pointer();

	return RESULT_NOPROBLEM;
}

_int CIcicle::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// �浹 �˻� (LateTick ���� ������ ��)
	OnCollision(TimeDelta);

	// ����ü �ȿ� �ִٸ� + m_bDead ���°� �ƴϸ� �����׷쿡 �߰�
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && FALSE == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// �ݶ��̴� ����� ������
	if (true == m_bIsAttacking)
	{
		m_pRendererCom->Add_Debug(m_pColliderCom[0]);
		m_pRendererCom->Add_Debug(m_pColliderCom[1]);
	}
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CIcicle::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	// ���̴� ���� ����� �� ����
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	// ���� �����ִ� ���׸��� �� �����ͼ� ���̴��� �ؽ�ó ���� ����� �� �����ϰ� ������
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

HRESULT CIcicle::Render_Shadow()
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

void CIcicle::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();
	pImGui_Manager->SetGameObjectScale(string("Icicle Scale"), m_pTransformCom);

	ImGui::End();
}

_bool CIcicle::IsIcicleInRange(_vector vPos, _float fRange)
{
	_float fDistance = m_pTransformCom->Get_Distance(vPos);

	if (fDistance <= fRange)
		return true;

	return false;
}

HRESULT CIcicle::Save_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ������� ����
	_float4x4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, m_pTransformCom->Get_WorldMatrix());

	DWORD dwBytes = 0;
	WriteFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CIcicle::Load_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���̺굥���Ϳ��� ��������� �ε�
	DWORD		dwBytes = 0;
	_float4x4	worldMatrix;

	ReadFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	// ������� ����
	m_pTransformCom->Set_WorldMatrix(worldMatrix);

	return S_OK;
}

HRESULT CIcicle::SetUp_Animations()
{
	m_pModelCom->Add_OnceListAll();

	m_pModelCom->ResetAnimation(0);
	m_pModelCom->Set_TempIndex(0);

	m_pModelCom->Set_TargetPlaySpeed(0, 35.0);

	return S_OK;
}

HRESULT CIcicle::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Dissolve_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	_uint iRandNum = rand() % 4;
	if (FAILED(__super::Add_Component(LEVEL_FROZEN, m_pPrototypeTag[iRandNum], TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(4.f, 4.f, 4.f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 4.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_U"), (CComponent**)&m_pColliderCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(4.f, 4.f, 4.f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 2.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_D"), (CComponent**)&m_pColliderCom[1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIcicle::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (m_iPass == 4)
	{
		// ������ �ؽ�ó ���ε�
		if (FAILED(m_pDissolveTexture->SetUp_OnShader(m_pShaderCom, "g_DissolveTexture", 99)))
			return E_FAIL;

		// ������ Alpha ���ε�
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveAmount, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

void CIcicle::OnCollision(_double TimeDelta)
{
	// ���Ϳ�(IceMan) �浹 �˻�
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		// �Ϲ��浹, Enter, Stay, Exit �浹���� ���� �ٸ� �Լ� �����ų �� ����
		for (_uint i = 0; i < 2; ++i)
		{
			if (m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom[i], pDestCollider))
			{
				// �浹�ϸ� ���ݻ��� ���� �μ����� �ִϸ��̼� ����
				m_bIsAttacking = false;
				m_bPlayAnim    = true;

				if (m_bSoundEnd == false)
				{
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_IcicleBreak_33.wav"), 0.7f, true, m_pTransformCom->Get_Position());

					// ����Ʈ
					CEffect_Manager::GetInstance()->Create_IcicleEffect(m_pTransformCom);

					m_bSoundEnd = true;
				}

				break;
			}
		}
	}

	if (m_bPlayAnim == true)
		return;

	// ���Ϳ�(IceMan) �浹 �˻�
	pColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Atk"));
	for (auto& pDestCollider : *pColliderList)
	{
		// �Ϲ��浹, Enter, Stay, Exit �浹���� ���� �ٸ� �Լ� �����ų �� ����
		for (_uint i = 0; i < 2; ++i)
		{
			if (m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom[i], pDestCollider))
			{
				// �浹�ϸ� ���ݻ��� ���� �μ����� �ִϸ��̼� ����
				m_bIsAttacking = false;
				m_bPlayAnim    = true;

				if (m_bSoundEnd == false)
				{
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_IcicleBreak_33.wav"), 0.7f, true, m_pTransformCom->Get_Position());

					// ����Ʈ
					CEffect_Manager::GetInstance()->Create_IcicleEffect(m_pTransformCom);

					m_bSoundEnd = true;
				}

				break;
			}
		}
	}

	if (m_bPlayAnim == true)
		return;

	// �÷��̾�� �浹 �˻�
	pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Atk"));
	for (auto& pDestCollider : *pColliderList)
	{
		// �Ϲ��浹, Enter, Stay, Exit �浹���� ���� �ٸ� �Լ� �����ų �� ����
		for (_uint i = 0; i < 2; ++i)
		{
			if (m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom[i], pDestCollider))
			{
				if (m_bSoundEnd == false)
				{
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_OnDamaged_7.wav"), 0.7f, true, m_pTransformCom->Get_Position());

					// ����Ʈ
					CEffect_Manager::GetInstance()->Create_IcicleEffect(m_pTransformCom);

					m_bSoundEnd = true;
				}

				break;
			}
		}
	}

	if (m_bPlayAnim == true)
		return;

	// �÷��̾� �о��
	pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		for (_uint i = 0; i < 2; ++i)
		{
			if (m_pGameInstance->Collision_Sphere(m_pColliderCom[i], pDestCollider))
			{
				CGameObject* pDestObject    = pDestCollider->Get_Parent();
				CTransform*	 pDestTransform = pDestObject->Get_Transform();

				// ������
				_vector vDir = m_pTransformCom->Get_Dir(pDestTransform);

				// ���������� �з�����
				pDestTransform->Go_VectorXZ(vDir, TimeDelta, nullptr);
			}
		}
	}
}

CIcicle* CIcicle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CIcicle* pInstance = new CIcicle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CIcicle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CIcicle::Clone(void* pArg)
{
	CIcicle* pInstance = new CIcicle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CIcicle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CIcicle::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom[0]);
	Safe_Release(m_pColliderCom[1]);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}