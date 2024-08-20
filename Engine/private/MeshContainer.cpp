#include "..\public\MeshContainer.h"
#include "HierarchyNode.h"
#include "Shader.h"
CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMeshContainer::NativeConstruct_Prototype(MESHCONTAINERDESC* pMeshContainer, _uint iIndex)
{
	strcpy_s(m_szName, pMeshContainer->szMeshName);
	m_iMaterialIndex = pMeshContainer->iMeshIndex;	// 매쉬 인덱스
	m_iNumBones = pMeshContainer->iNumBones;			// 뼈 갯수
	m_pMeshContainerDesc = pMeshContainer;
	m_iMeshIndex = iIndex;

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Ready_VertexIndexBuffer(CModel::MODELTYPE eMeshType, _fmatrix TransformMatrix)
{
#pragma region VERTICES
	m_iNumIndicesPerPrimitive = m_pMeshContainerDesc->iNumIndicesPerPrimitive;
	m_iNumVertices = m_pMeshContainerDesc->iNumVertices;
	m_iNumVertexBuffers = m_pMeshContainerDesc->iNumVertexBuffers;

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eMeshType)
		hr = Ready_NonAnimMeshContainer((vector<VTXMODEL>*)(&m_pMeshContainerDesc->NonAnimInfo), TransformMatrix);
	else
		hr = Ready_AnimMeshContainer(&m_pMeshContainerDesc->AnimInfo, false);
#pragma endregion

#pragma region INDICES
	m_iNumPrimitive = m_pMeshContainerDesc->iNumPrimitive;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	memcpy(pIndices, &m_pMeshContainerDesc->Indices[0], sizeof(FACEINDICES32) * m_iNumPrimitive);

	//for (_uint i = 0; i < m_iNumPrimitive; i++)
	//{
	//	memcpy(&pIndices[i], &m_pMeshContainerDesc->Indices[i], sizeof(FACEINDICES32));
	//}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Add_HierarChyNode(_uint iNodeIndex)
{
	m_HierarchyNodeIndices.push_back(iNodeIndex);

	return S_OK;
}

HRESULT CMeshContainer::SetUp_Matrices(_float4x4 * pBoneMatrices, const vector<CHierarchyNode*>* pNodes, _fmatrix TransformMatrix)
{
	_uint	iIndex = 0;

	if (0 == m_iNumBones)		// 뼈의 갯수가 0개이면
	{
		auto iter = find_if(pNodes->begin(), pNodes->end(), [&](CHierarchyNode* pHierarchyNode) 
		{
			return !strcmp(m_szName, pHierarchyNode->Get_Name());	// 매쉬의 이름과 하이어라키 노드의 이름이 같다면 그 부분을 iter에 저장
		});

		if (iter == pNodes->end())
			XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());	// 본 매트릭스를 항등으로
		else
		{
			_matrix	OffsetMatrix = (*iter)->Get_OffsetMatrix();
			_matrix CombinedTransformationMatrix = (*iter)->Get_CombinedTransformationMatrix();
			_matrix BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;

			XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(BoneMatrix));
		}

	}

	for (auto& iNodeIndex : m_HierarchyNodeIndices)	// 현재 매쉬에 영향을 주는 모든 노드를 순회
	{
		_matrix		OffsetMatrix = (*pNodes)[iNodeIndex]->Get_OffsetMatrix();
		_matrix		CombinedTransformationMatrix = (*pNodes)[iNodeIndex]->Get_CombinedTransformationMatrix();

		_matrix		BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;	// 오프셋 매트릭스와 컴바인 매트릭스를 곱해서 뼈 매트릭스를 만듬

		XMStoreFloat4x4(&pBoneMatrices[iIndex++], XMMatrixTranspose(BoneMatrix));	// 셰이더에 던질 것이기 때문에 본 매트릭스 행렬을 전치해서 던짐
		//if (pBoneMatrices[iIndex - 1]._44 < 0.8f)
		//	int a = 1;
	}

	//if (pBoneMatrices[iIndex - 1]._44 < 0.8f)
	//	int a = 1;

	return S_OK;
}

_float4x4* CMeshContainer::Ready_SetUp_Matrices(_int iSize, const vector<CHierarchyNode*>* pNodes, _fmatrix TransformMatrix)
{
	_uint	iIndex = 0;

	_float4x4* pBoneMatrix = new _float4x4[iSize];

	if (0 == m_iNumBones)		// 뼈의 갯수가 0개이면
	{
		auto iter = find_if(pNodes->begin(), pNodes->end(), [&](CHierarchyNode* pHierarchyNode)
		{
			return !strcmp(m_szName, pHierarchyNode->Get_Name());	// 매쉬의 이름과 하이어라키 노드의 이름이 같다면 그 부분을 iter에 저장
		});

		if (iter == pNodes->end())
			XMStoreFloat4x4(&pBoneMatrix[0], XMMatrixIdentity());	// 본 매트릭스를 항등으로
		else
		{
			_matrix	OffsetMatrix = (*iter)->Get_OffsetMatrix();
			_matrix CombinedTransformationMatrix = (*iter)->Get_CombinedTransformationMatrix();
			_matrix BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;

			XMStoreFloat4x4(&pBoneMatrix[iIndex++], XMMatrixTranspose(BoneMatrix));
		}

	}

	for (auto& iNodeIndex : m_HierarchyNodeIndices)	// 현재 매쉬에 영향을 주는 모든 노드를 순회
	{
		_matrix		OffsetMatrix = (*pNodes)[iNodeIndex]->Get_OffsetMatrix();
		_matrix		CombinedTransformationMatrix = (*pNodes)[iNodeIndex]->Get_CombinedTransformationMatrix();

		_matrix		BoneMatrix = OffsetMatrix * CombinedTransformationMatrix * TransformMatrix;	// 오프셋 매트릭스와 컴바인 매트릭스를 곱해서 뼈 매트릭스를 만듬

		XMStoreFloat4x4(&pBoneMatrix[iIndex++], XMMatrixTranspose(BoneMatrix));	// 셰이더에 던질 것이기 때문에 본 매트릭스 행렬을 전치해서 던짐
	}

	return pBoneMatrix;
}

HRESULT CMeshContainer::Ready_NonAnimMeshContainer(vector<VTXMODEL>* pModelInfo, _fmatrix TransformMatrix)
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXMODEL*	pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	memcpy(pVertices, &(*pModelInfo)[0], sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformMatrix), sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), TransformMatrix), sizeof(_float3));
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

	return S_OK;
}


HRESULT CMeshContainer::Ready_AnimMeshContainer(vector<VTXANIMMODEL>* pModelInfo, _bool b)
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXANIMMODEL) * m_iNumVertices;
	m_VBDesc.StructureByteStride = sizeof(VTXANIMMODEL);
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	VTXANIMMODEL*	pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	memcpy(pVertices, &(*pModelInfo)[0], sizeof(VTXANIMMODEL) * m_iNumVertices);

	//for (_uint i = 0; i < m_iNumVertices; i++)
	//{
	//	memcpy(&pVertices[i].vPosition, &(*pModelInfo)[i].vPosition, sizeof(_float3));
	//	memcpy(&pVertices[i].vNormal, &(*pModelInfo)[i].vNormal, sizeof(_float3));
	//	memcpy(&pVertices[i].vTexUV, &(*pModelInfo)[i].vTexUV, sizeof(_float2));
	//	memcpy(&pVertices[i].vTangent, &(*pModelInfo)[i].vTangent, sizeof(_float3));
	//	memcpy(&pVertices[i].vBlendIndex, &(*pModelInfo)[i].vBlendIndex, sizeof(XMUINT4));
	//	memcpy(&pVertices[i].vBlendWeight, &(*pModelInfo)[i].vBlendWeight, sizeof(_float4));

	//}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

	return S_OK;
}


CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, MESHCONTAINERDESC * pMeshContainer, _uint iIndex)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pMeshContainer, iIndex)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	m_HierarchyNodeIndices.clear();
}
