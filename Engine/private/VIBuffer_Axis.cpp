#include "VIBuffer_Axis.h"

CVIBuffer_Axis::CVIBuffer_Axis(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Axis::CVIBuffer_Axis(const CVIBuffer_Axis& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Axis::NativeConstruct_Prototype()
{
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 2;
	m_iNumVertices            = 6;
	m_iNumVertexBuffers       = 1;

	m_VBDesc.ByteWidth           = sizeof(VTXPOS) * m_iNumVertices;
	m_VBDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXPOS);

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

	pVertices[0].vPosition = _float4(-1000.0f, 0.0f, 0.0f, 1.f);
	pVertices[1].vPosition = _float4(+1000.0f, 0.0f, 0.0f, 1.f);

	pVertices[2].vPosition = _float4(0.0f, -1000.0f, 0.0f, 1.f);
	pVertices[3].vPosition = _float4(0.0f, +1000.0f, 0.0f, 1.f);

	pVertices[4].vPosition = _float4(0.0f, 0.0f, -1000.0f, 1.f);
	pVertices[5].vPosition = _float4(0.0f, 0.0f, +1000.0f, 1.f);

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
	{
		Safe_Delete_Array(pVertices);
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = 3;
	m_eIndexFormat  = DXGI_FORMAT_R16_UINT;
	m_eTopology     = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(LINEINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage     = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	LINEINDICES16* pIndices = new LINEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(LINEINDICES16) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;

	pIndices[1]._0 = 2;
	pIndices[1]._1 = 3;

	pIndices[2]._0 = 4;
	pIndices[2]._1 = 5;

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
	{
		Safe_Delete_Array(pIndices);
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Axis::NativeConstruct(void* pArg)
{
	return S_OK;
}

CVIBuffer_Axis* CVIBuffer_Axis::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CVIBuffer_Axis* pInstance = new CVIBuffer_Axis(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Axis::Clone(void* pArg)
{
	CVIBuffer_Axis* pInstance = new CVIBuffer_Axis(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Axis::Free()
{
	__super::Free();
}