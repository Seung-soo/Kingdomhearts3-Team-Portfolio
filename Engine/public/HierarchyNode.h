#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;
public:

	HRESULT NativeConstruct(const char* pName, _float4x4 TransformationMatrix, _uint iDepth, _int iParentIndex, _int iNodeIndex, _float4x4 OffsetMatrix, class CModel* pModel);

	void	Update_CombinedTransformationMatrix();		// ���� Ʈ������ ��� ���� �Լ�

	_bool	Search_Ancestry(const char* pName);
public:
	_uint			Get_Depth() { return m_iDepth; }
	const char*		Get_Name() { return m_szName; }
	_fmatrix		Get_OffsetMatrix() { return XMLoadFloat4x4(&m_OffsetMatrix); }	// ������ ��Ʈ���� ��ȯ
	_float4x4*		Get_OffsetFloat4x4() { return &m_OffsetMatrix; }	// ������ ��Ʈ���� ��ȯ
	_fmatrix		Get_CombinedTransformationMatrix() { return XMLoadFloat4x4(&m_CombineTransformationMatrix); }	// �Ĺ��� ��Ʈ���� ��ȯ
	_float4x4*		Get_CombinedTransformationFloat4x4() { return &m_CombineTransformationMatrix; }	// �Ĺ��� ��Ʈ���� ��ȯ
	_fmatrix		Get_TransformationMatrix() { return XMLoadFloat4x4(&m_TransformationMatrix); }	// ��Ʈ���� ��ȯ
	_int			Get_NodeIndex() { return m_iNodeIndex; }
public:
	void			Set_OffsetMatrix(_float4x4 OffsetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix))); }
	void			Set_TransformationMatrix(_fmatrix TransformationMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}
	void			Set_CombineTransformationMatrix(_fmatrix CombineTransformationMatrix) {
		XMStoreFloat4x4(&m_CombineTransformationMatrix, CombineTransformationMatrix);
	}
private:

	char			m_szName[MAX_PATH] = "";
	_float4x4		m_OffsetMatrix;					// �Ž��� ���ð� ���� ������ �ٸ� �� �ִ�. �̰� �������ִ� ���.
	_float4x4		m_TransformationMatrix;			// ���� ���� ���
	_float4x4		m_CombineTransformationMatrix;	// ���� ���
	_uint			m_iDepth;

	_int			m_iParentIndex = 0;
	_int			m_iNodeIndex = 0;
	class CModel*	m_pModel = nullptr;
	CHierarchyNode*	m_pParent = nullptr;
public:

	static CHierarchyNode* Create(const char* pName, _float4x4 TransformationMatrix, _uint iDepth, _int iParentIndex, _int iNodeIndex, _float4x4 OffsetMatrix, class CModel* pModel);
	virtual void Free() override;
};

END