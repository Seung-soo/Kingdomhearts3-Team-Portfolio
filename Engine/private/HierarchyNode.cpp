#include "..\public\HierarchyNode.h"
#include "Model.h"

CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char * pName, _float4x4 TransformationMatrix, _uint iDepth, _int iParentIndex, _int iNodeIndex, _float4x4 OffsetMatrix, CModel* pModel)
{
	m_iParentIndex = iParentIndex;
	m_pModel = pModel;
	m_iNodeIndex = iNodeIndex;

	strcpy_s(m_szName, pName);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

	memcpy(&m_TransformationMatrix, &TransformationMatrix, sizeof(_float4x4));

	XMStoreFloat4x4(&m_CombineTransformationMatrix, XMMatrixIdentity());	// 컴바인 행렬 항등 행렬로 초기화.

	m_iDepth = iDepth;

	m_pParent = m_pModel->Find_HierarchyNode(m_iParentIndex);

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	_matrix	TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	_matrix CombinedTransformationMatrix;

	if (-1 == m_iParentIndex)									// 최상위 노드일 때
		CombinedTransformationMatrix = TransformationMatrix;	// 내 트랜스폼이 곧 컴파인드 트랜스폼
	else	// 최상위 노드가 아닐 때
			// 부모의 컴바인 행렬과 곱해줌
		CombinedTransformationMatrix = TransformationMatrix * XMLoadFloat4x4(&m_pParent->m_CombineTransformationMatrix);

	XMStoreFloat4x4(&m_CombineTransformationMatrix, CombinedTransformationMatrix);		// 갱신된 컴바인 행렬을 집어넣어줌
}

_bool CHierarchyNode::Search_Ancestry(const char * pName)
{
	if (!strcmp(m_szName, pName))
		return true;

	if (-1 == m_iParentIndex)
		return false;

	return m_pParent->Search_Ancestry(pName);
}

CHierarchyNode * CHierarchyNode::Create(const char * pName, _float4x4 TransformationMatrix, _uint iDepth, _int iParentIndex, _int iNodeIndex, _float4x4 OffsetMatrix, class CModel* pModel)
{
	CHierarchyNode* pInstance = new CHierarchyNode;

	if (FAILED(pInstance->NativeConstruct(pName, TransformationMatrix, iDepth, iParentIndex, iNodeIndex, OffsetMatrix, pModel)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHierarchyNode::Free()
{
}

