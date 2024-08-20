#include "..\public\VIBuffer_Point_Instance.h"
#include "Shader.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance & rhs)
	: CVIBuffer(rhs)
	, m_VBInstDesc(rhs.m_VBInstDesc)
	, m_VBInstSubResource(rhs.m_VBInstSubResource)
	, m_iNumInstance(rhs.m_iNumInstance)
{
}

HRESULT CVIBuffer_Point_Instance::NativeConstruct_Prototype(_uint iNumInstance)
{
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 1;
	m_iNumVertices = 1;
	m_iNumVertexBuffers = 2;

	m_VBDesc.ByteWidth = sizeof(VTXPOINT) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXPOINT);

	VTXPOINT*		pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);
	pVertices[0].fPSize = 1.f;

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

#pragma endregion


#pragma region INDEX_BUFFER
	m_iNumInstance = iNumInstance;
	m_iNumPrimitive = m_iNumInstance;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = sizeof(_ushort) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	_ushort*	pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		pIndices[i] = 0;
	}

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

HRESULT CVIBuffer_Point_Instance::NativeConstruct(void * pArg)
{
#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstDesc.ByteWidth = sizeof(VTXMATRIX) * m_iNumInstance;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.StructureByteStride = sizeof(VTXMATRIX);
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	VTXMATRIX*		pInstanceMatrix = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceMatrix, sizeof(VTXMATRIX) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		pInstanceMatrix[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceMatrix[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceMatrix[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceMatrix[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}

	ZeroMemory(&m_VBInstSubResource, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBInstSubResource.pSysMem = pInstanceMatrix;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstDesc, &m_VBInstSubResource, &m_pVBInstance)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	Safe_Delete_Array(pInstanceMatrix);
#pragma endregion

	if (nullptr != pArg)
	{
		memcpy(&m_ParticleDesc, pArg, sizeof(m_ParticleDesc));
	}

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render(CShader * pShader, _uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	ID3D11Buffer*	pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint			iStrides[] = {
		m_VBDesc.StructureByteStride,
		m_VBInstDesc.StructureByteStride
	};

	_uint			iOffsets[] = {
		0,
		0
	};

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

	m_pDeviceContext->DrawIndexedInstanced(1, m_iNumInstance, 0, 0, 0);
	return S_OK;
}

void CVIBuffer_Point_Instance::UpdateParticle(vector<_float4x4> TransformMatrix, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
	{
		__debugbreak();
		return;
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		memcpy(&(((VTXMATRIX*)SubResource.pData)[i].vRight), &TransformMatrix[i]._11, sizeof(_float4));
		memcpy(&(((VTXMATRIX*)SubResource.pData)[i].vUp), &TransformMatrix[i]._21, sizeof(_float4));
		memcpy(&(((VTXMATRIX*)SubResource.pData)[i].vLook), &TransformMatrix[i]._31, sizeof(_float4));
		memcpy(&(((VTXMATRIX*)SubResource.pData)[i].vPosition), &TransformMatrix[i]._41, sizeof(_float4));
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Instance * CVIBuffer_Point_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumInstance)
{
	CVIBuffer_Point_Instance*	pInstance = new CVIBuffer_Point_Instance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iNumInstance)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Point_Instance::Clone(void * pArg)
{
	CVIBuffer_Point_Instance*	pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
	
}
