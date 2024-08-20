#include "..\public\PipeLine.h"

READY_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Set_Transform(TRANSFORMSTATETYPE eStateType, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_PipeLineMatrix[eStateType], TransformMatrix);

	return S_OK;
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATETYPE eStateType)
{
	return XMLoadFloat4x4(&m_PipeLineMatrix[eStateType]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(TRANSFORMSTATETYPE eStateType)
{
	return m_PipeLineMatrix[eStateType];
}

_float4x4 CPipeLine::Get_TransformFloat4x4_TP(TRANSFORMSTATETYPE eStateType)
{
	_float4x4 TransposeMatrix;
	XMStoreFloat4x4(&TransposeMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_PipeLineMatrix[eStateType])));
	return TransposeMatrix;
}

_vector CPipeLine::Get_CamPosition() const
{
	return XMLoadFloat4(&m_vCamPosition);
}

void CPipeLine::Update()
{
	_matrix	ViewMatrix = XMLoadFloat4x4(&m_PipeLineMatrix[D3DTS_VIEW]);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	XMStoreFloat4(&m_vCamPosition, ViewMatrix.r[3]);
}

void CPipeLine::Free()
{
}
