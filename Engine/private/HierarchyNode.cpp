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

	XMStoreFloat4x4(&m_CombineTransformationMatrix, XMMatrixIdentity());	// �Ĺ��� ��� �׵� ��ķ� �ʱ�ȭ.

	m_iDepth = iDepth;

	m_pParent = m_pModel->Find_HierarchyNode(m_iParentIndex);

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	_matrix	TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	_matrix CombinedTransformationMatrix;

	if (-1 == m_iParentIndex)									// �ֻ��� ����� ��
		CombinedTransformationMatrix = TransformationMatrix;	// �� Ʈ�������� �� �����ε� Ʈ������
	else	// �ֻ��� ��尡 �ƴ� ��
			// �θ��� �Ĺ��� ��İ� ������
		CombinedTransformationMatrix = TransformationMatrix * XMLoadFloat4x4(&m_pParent->m_CombineTransformationMatrix);

	XMStoreFloat4x4(&m_CombineTransformationMatrix, CombinedTransformationMatrix);		// ���ŵ� �Ĺ��� ����� ����־���
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

