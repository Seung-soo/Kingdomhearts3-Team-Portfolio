#include "..\public\VIBuffer_OrthoWin.h"
#include "GameInstance.h"

CVIBuffer_OrthoWin::CVIBuffer_OrthoWin(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_OrthoWin::CVIBuffer_OrthoWin(const CVIBuffer_OrthoWin & rhs)
	: CVIBuffer(rhs)
	, m_fHeight(rhs.m_fHeight)
	, m_fWidth(rhs.m_fWidth)
{

}

HRESULT CVIBuffer_OrthoWin::NativeConstruct_Prototype(_int iWidth, _int iHeight)
{
	m_fWidth = (_float)iWidth;
	m_fHeight = (_float)iHeight;

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = 4;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXTEX);

	//윈도우 창 위치 잡아주기(정중앙의 좌표는 (0,0))
	_float		fLeft, fRight, fTop, fBottom;
	
	fLeft = (_float)((iWidth * 0.5f) * -1);
	fRight = fLeft + (_float)iWidth;
	fTop = (_float)(iHeight * 0.5f);
	fBottom = fTop - (_float)iHeight;


	VTXTEX*		pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(fLeft, fTop, 0.f);
	pVertices[0].vTexUV = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(fRight, fTop, 0.f);
	pVertices[1].vTexUV = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(fRight, fBottom, 0.f);
	pVertices[2].vTexUV = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(fLeft, fBottom, 0.f);
	pVertices[3].vTexUV = _float2(0.f, 1.f);

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES16*	pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_OrthoWin::NativeConstruct(void * pArg)
{


	return S_OK;
}

HRESULT CVIBuffer_OrthoWin::Render(CShader * pShader, _uint iPassIndex)
{
	_uint				iNumViewports = 1;
	D3D11_VIEWPORT		Viewport;

	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	if (FAILED(pShader->Set_RawValue("ScreenWidth", &m_fWidth, sizeof(_float))))
	{
		BREAKPOINT;

		 

		return E_FAIL;
	}

	if (FAILED(pShader->Set_RawValue("ScreenHeight", &m_fHeight, sizeof(_float))))
	{
		BREAKPOINT;

		 

		return E_FAIL;
	}

	_float4x4		ProjMatrix;
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(m_fWidth, m_fHeight, 0.f, 1.f)));
	if (FAILED(pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;

		 

		return E_FAIL;
	}

	if (FAILED(__super::Render(pShader, iPassIndex)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

CVIBuffer_OrthoWin * CVIBuffer_OrthoWin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _int iWidth, _int iHeight)
{
	CVIBuffer_OrthoWin*	pInstance = new CVIBuffer_OrthoWin(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iWidth, iHeight)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_OrthoWin::Clone(void * pArg)
{
	CVIBuffer_OrthoWin*	pInstance = new CVIBuffer_OrthoWin(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_OrthoWin::Free()
{
	__super::Free();
}
