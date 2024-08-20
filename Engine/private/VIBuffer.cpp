#include "..\public\VIBuffer.h"
#include "Shader.h"
#include "Picking.h"
#include "GameInstance.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_VBDesc(rhs.m_VBDesc)
	, m_VBSubResourceData(rhs.m_VBSubResourceData)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_IBDesc(rhs.m_IBDesc)
	, m_IBSubResourceData(rhs.m_IBSubResourceData)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iNumIndicesPerPrimitive(rhs.m_iNumIndicesPerPrimitive)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_eTopology(rhs.m_eTopology)
	, m_pVerticesPos(rhs.m_pVerticesPos)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);

}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render(CShader* pShader, _uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*	pVertexBuffers[] = { m_pVB };

	_uint			iStrides[] = { m_VBDesc.StructureByteStride };

	_uint			iOffsets[] = { 0, };

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_eTopology);

	if (FAILED(pShader->Set_InputLayout(iPassIndex)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pShader->Apply(iPassIndex)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubResourceData, &m_pVB);
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	return m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubResourceData, &m_pIB);
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
