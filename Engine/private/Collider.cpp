#include "..\public\Collider.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "DebugDraw.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
#ifdef _DEBUG
	, m_pBasicEffect(rhs.m_pBasicEffect)
	, m_pBatch(rhs.m_pBatch)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
	, m_eType(rhs.m_eType)
{
	for (_uint i = 0; i < BOUNDING_END; i++)
	{
		if (nullptr != rhs.m_pAABB[BOUNDING_ORIGINAL])
			m_pAABB[i] = new BoundingBox(*rhs.m_pAABB[BOUNDING_ORIGINAL]);

		if (nullptr != rhs.m_pOBB[BOUNDING_ORIGINAL])
			m_pOBB[i] = new BoundingOrientedBox(*rhs.m_pOBB[BOUNDING_ORIGINAL]);

		if (nullptr != rhs.m_pSphere[BOUNDING_ORIGINAL])
			m_pSphere[i] = new BoundingSphere(*rhs.m_pSphere[BOUNDING_ORIGINAL]);
	}
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif

}

void CCollider::Set_ColliderDesc(COLLIDERDESC tColliderDesc)
{
	memcpy(&m_ColliderDesc, &tColliderDesc, sizeof(COLLIDERDESC));

	Make_TransformMatrix();
}

void CCollider::Set_Scale(_float3 vScale)
{
	m_ColliderDesc.vScale = vScale;

	Make_TransformMatrix();
}

void CCollider::Set_Rotation(_float4 vRotation)
{
	m_ColliderDesc.vRotation = vRotation;

	Make_TransformMatrix();
}

void CCollider::Set_Translation(_float4 vTranslation)
{
	m_ColliderDesc.vTranslation = vTranslation;

	Make_TransformMatrix();
}

void CCollider::Set_Color(_float4 vColor)
{
	m_vColor = vColor;
	m_bCustomColor = true;
}

HRESULT CCollider::Add_Colliding(STATE eState, CCollider * pCollider)
{
	if (nullptr == pCollider)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	for (_uint i = 0; i < TYPE::TYPE_END; ++i)
	{
		for (auto& collider : m_Colliding[i])
		{
			if (pCollider->Get_Parent() == collider->Get_Parent())
			{
				Add_OvalapingParent_Colliding(eState, pCollider);
				return S_OK;
			}
		}
	}

	m_Colliding[eState].push_back(pCollider);

	return S_OK;
}

void CCollider::Add_OvalapingParent_Colliding(STATE eState, CCollider * pCollider)
{
	for (_uint i = 0; i < TYPE::TYPE_END; ++i)
	{
		for (auto& collider : m_OverlappingParent_Colliding[i])
		{
			if (collider == pCollider)
				return;
		}
	}
	m_OverlappingParent_Colliding[eState].push_back(pCollider);
}

_vector CCollider::Get_Center()
{
	_vector cetner = XMVectorZero();
	switch (m_eType)
	{
	case Engine::CCollider::TYPE_AABB:
		cetner = XMVectorSetW(XMLoadFloat3(&Get_AABB()->Center), 1.f);
		break;

	case Engine::CCollider::TYPE_OBB:
		cetner = XMVectorSetW(XMLoadFloat3(&Get_OBB()->Center), 1.f);
		break;

	case Engine::CCollider::TYPE_SPHERE:
		cetner = XMVectorSetW(XMLoadFloat3(&Get_Sphere()->Center), 1.f);
		break;
	}

	return cetner;
}

void CCollider::Set_Center(_float3 vCenter)
{
	switch (m_eType)
	{
	case Engine::CCollider::TYPE_AABB:
		Get_AABB()->Center = vCenter;
		break;
	case Engine::CCollider::TYPE_OBB:
		Get_OBB()->Center = vCenter;
		break;
	case Engine::CCollider::TYPE_SPHERE:
		Get_Sphere()->Center = vCenter;
		break;
	}
}

void CCollider::Set_Center(_vector vCenter)
{
	switch (m_eType)
	{
	case Engine::CCollider::TYPE_AABB:
		XMStoreFloat3(&Get_AABB()->Center, vCenter);
		break;
	case Engine::CCollider::TYPE_OBB:
		XMStoreFloat3(&Get_OBB()->Center, vCenter);
		break;
	case Engine::CCollider::TYPE_SPHERE:
		XMStoreFloat3(&Get_Sphere()->Center, vCenter);
		break;
	}
}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eType)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSGBOX("__super::NativeConstruct_Prototype returned E_FAIL in CCollider::NativeConstruct_Prototype");
		return E_FAIL;
	}

	m_eType = eType;

	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB[BOUNDING_ORIGINAL] = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		break;
	case TYPE_OBB:
		m_pOBB[BOUNDING_ORIGINAL] = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		break;
	case TYPE_SPHERE:
		m_pSphere[BOUNDING_ORIGINAL] = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		break;
	}

#ifdef _DEBUG
	m_pBasicEffect = new BasicEffect(m_pDevice);
	m_pBasicEffect->SetVertexColorEnabled(true);	// 색을 받아오기 위한 셰이더를 사용할 것이다.

	const void*	pShaderByteCode = nullptr;
	size_t		iShaderByteCodeLength = 0;

	m_pBasicEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
	{
		MSGBOX("m_pDevice->CreateInputLayout returned E_FAIL in CCollider::NativeConstruct_Prototype");
		return E_FAIL;
	}

	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pDeviceContext);

#endif


	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		MSGBOX("__super::NativeConstruct returned E_FAIL in CCollider::NativeConstruct");
		return E_FAIL;
	}

	_matrix		TransformMatrix;

	if (nullptr != pArg)
	{
		memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDESC));
		TransformMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&m_ColliderDesc.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&m_ColliderDesc.vRotation), XMLoadFloat4(&m_ColliderDesc.vTranslation));
	}
	else
		TransformMatrix = XMMatrixIdentity();

	if (nullptr != m_pAABB[BOUNDING_ORIGINAL])
		m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_TRANSFORM], TransformMatrix);
	if (nullptr != m_pOBB[BOUNDING_ORIGINAL])
		m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_TRANSFORM], TransformMatrix);
	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_TRANSFORM], TransformMatrix);

	ZeroMemory(&m_vColor, sizeof(_float4));

	return S_OK;
}

void CCollider::Update_Transform(_fmatrix TransformMatrix)
{
	if (nullptr != m_pAABB[BOUNDING_TRANSFORM])
		m_pAABB[BOUNDING_TRANSFORM]->Transform(*m_pAABB[BOUNDING_WORLD_TRANSFORM], Remove_Rotation(TransformMatrix));

	if (nullptr != m_pOBB[BOUNDING_TRANSFORM])
		m_pOBB[BOUNDING_TRANSFORM]->Transform(*m_pOBB[BOUNDING_WORLD_TRANSFORM], TransformMatrix);

	if (nullptr != m_pSphere[BOUNDING_TRANSFORM])
		m_pSphere[BOUNDING_TRANSFORM]->Transform(*m_pSphere[BOUNDING_WORLD_TRANSFORM], TransformMatrix);

	_vector vPos = Get_Center();
	memcpy(&m_vPosition, &vPos, sizeof(_float4));
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pBasicEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pBasicEffect->SetView(XMLoadFloat4x4(&pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	m_pBasicEffect->SetProjection(XMLoadFloat4x4(&pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));

	 

	m_pBasicEffect->Apply(m_pDeviceContext);


	_vector		vColliderColor = XMVectorZero();

	_bool collided = false;
	/*for (_uint i = 0; i < STATE::STATE_END; ++i)
	{
		if (0 < m_Colliding[i].size())
		{
			collided = true;
			break;
		}
	}*/

	if (0 < m_Colliding[STATE::STATE_ENTER].size())
		collided = true;

	if (true == collided)
	{
		vColliderColor = _redV;
	}

	else
	{
		vColliderColor = _greenV;

		if (true == m_bCustomColor)
			vColliderColor = XMLoadFloat4(&m_vColor);
	}

	m_pBatch->Begin();

	if (nullptr != m_pAABB[BOUNDING_WORLD_TRANSFORM])
		DX::Draw(m_pBatch, *m_pAABB[BOUNDING_WORLD_TRANSFORM], vColliderColor);
	if (nullptr != m_pOBB[BOUNDING_WORLD_TRANSFORM])
		DX::Draw(m_pBatch, *m_pOBB[BOUNDING_WORLD_TRANSFORM], vColliderColor);
	if (nullptr != m_pSphere[BOUNDING_WORLD_TRANSFORM])
		DX::Draw(m_pBatch, *m_pSphere[BOUNDING_WORLD_TRANSFORM], vColliderColor);

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return Matrix;
}

void CCollider::Make_TransformMatrix()
{
	_matrix TransformMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&m_ColliderDesc.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&m_ColliderDesc.vRotation), XMLoadFloat4(&m_ColliderDesc.vTranslation));

	if (nullptr != m_pAABB[BOUNDING_ORIGINAL])
		m_pAABB[BOUNDING_ORIGINAL]->Transform(*m_pAABB[BOUNDING_TRANSFORM], Remove_Rotation(TransformMatrix));

	if (nullptr != m_pOBB[BOUNDING_ORIGINAL])
		m_pOBB[BOUNDING_ORIGINAL]->Transform(*m_pOBB[BOUNDING_TRANSFORM], TransformMatrix);

	if (nullptr != m_pSphere[BOUNDING_ORIGINAL])
		m_pSphere[BOUNDING_ORIGINAL]->Transform(*m_pSphere[BOUNDING_TRANSFORM], TransformMatrix);
}


CCollider::STATE CCollider::Find_Colliding(CCollider * pCollider)
{
	if (nullptr == pCollider)
	{
		BREAKPOINT;
		return STATE_END;
	}

	for (_uint i = 0; i < STATE_END; i++)
	{
		for (auto& pColliding : m_Colliding[i])
		{
			if (pColliding == pCollider)
				return (STATE)i;
		}
	}

	return STATE_END;
}

CCollider::STATE CCollider::Erase_Colliding(CCollider * pCollider)
{
	if (nullptr == pCollider)
	{
		BREAKPOINT;
		return STATE_END;
	}

	for (_uint i = 0; i < STATE_END; i++)
	{
		auto iter = m_Colliding[i].begin();
		for (auto& pColliding : m_Colliding[i])
		{
			if (pColliding == pCollider)
			{
				m_Colliding[i].erase(iter);
				return (STATE)i;
			}
			else
				iter++;

		}

		auto OverlapIter = m_OverlappingParent_Colliding[i].begin();
		for (auto& pColliding : m_OverlappingParent_Colliding[i])
		{
			if (pColliding == pCollider)
			{
				m_OverlappingParent_Colliding[i].erase(OverlapIter);
				return (STATE)i;
			}
			else
				OverlapIter++;

		}
	}

	return STATE_END;
}

void CCollider::Erase_Colliding_All(CCollider * pCollider)
{
	if (nullptr == pCollider)
	{
		BREAKPOINT;
		return;
	}

	for (_uint i = 0; i < STATE_END; i++)
	{
		auto iter = m_Colliding[i].begin();
		for (auto& pColliding : m_Colliding[i])
		{
			if (pColliding == pCollider)
			{
				m_Colliding[i].erase(iter);
				break;
			}
			else
				iter++;
		}

		auto OverlapIter = m_OverlappingParent_Colliding[i].begin();
		for (auto& pColliding : m_OverlappingParent_Colliding[i])
		{
			if (pColliding == pCollider)
			{
				m_OverlappingParent_Colliding[i].erase(OverlapIter);
				break;
			}
			else
				OverlapIter++;

		}
	}
}

void CCollider::Clear_Colliding_All()
{
	for (_uint i = 0; i < STATE_END; i++)
	{
		m_Colliding[i].clear();
		m_OverlappingParent_Colliding[i].clear();
	}
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eType)
{
	CCollider*	pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*	pInstance = new CCollider(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	for (size_t i = 0; i < STATE_END; i++)
	{
		for (size_t j = 0; j < m_Colliding[i].size(); j++)
		{
			m_Colliding[i][j]->Erase_Colliding(this);
		}
		for (size_t j = 0; j < m_OverlappingParent_Colliding[i].size(); j++)
		{
			m_OverlappingParent_Colliding[i][j]->Erase_Colliding(this);
		}
	}
	Clear_Colliding_All();

	for (_uint i = 0; i < BOUNDING_END; i++)
	{
		Safe_Delete(m_pAABB[i]);
		Safe_Delete(m_pOBB[i]);
		Safe_Delete(m_pSphere[i]);
	}

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBasicEffect);
		Safe_Delete(m_pBatch);
	}
#endif
}
