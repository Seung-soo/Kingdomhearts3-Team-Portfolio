#include "..\public\Channel.h"
#include "HierarchyNode.h"

void CChannel::Set_LastUsedKeyFrame(_fvector vScale, _fvector vRotation, _fvector vPosition)
{
	XMStoreFloat3(&m_LastUsedKeyFrame.vScale, vScale);
	XMStoreFloat4(&m_LastUsedKeyFrame.vRotation, vRotation);
	XMStoreFloat3(&m_LastUsedKeyFrame.vPosition, vPosition);
	m_LastUsedKeyFrame.Time = 0.0;
}

void CChannel::Set_TransformationMatrix(_fmatrix TransformationMatrix)
{
	if (nullptr == m_pNode)
		return;

	m_pNode->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Set_HierarchyNodePtr(CHierarchyNode * pNode)
{
	m_pNode = pNode;

	Safe_AddRef(m_pNode);
}

HRESULT CChannel::NativeConstruct(const char * pName)
{
	strcpy_s(m_szName, pName);
	ZeroMemory(&m_LastUsedKeyFrame, sizeof(KEYFRAME));
	m_LastUsedKeyFrame.vScale = _float3(1.f, 1.f, 1.f);

	return S_OK;
}

CChannel * CChannel::Create(const char * pName)
{
	CChannel*	pInstance = new CChannel();

	if (FAILED(pInstance->NativeConstruct(pName)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);

	m_KeyFrames.clear();

	Safe_Release(m_pNode);
}