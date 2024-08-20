#include "..\public\VIBuffer_Terrain.h"
#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_fTileNumX(rhs.m_fTileNumX)
	, m_fTileNumZ(rhs.m_fTileNumZ)
	, m_pVerticesSplatting(rhs.m_pVerticesSplatting)
{
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(_uint iNumX, _uint iNumZ)
{
	m_iNumVerticesX = iNumX;
	m_iNumVerticesZ = iNumZ;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXNORMULTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXNORMULTEX);

	VTXNORMULTEX*		pVertices = new VTXNORMULTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORMULTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	m_pVerticesSplatting = new _float4[m_iNumVertices];
	ZeroMemory(m_pVerticesSplatting, sizeof(_float4) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = _float3((_float)j, 0.0f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTexUV = _float2((_float)j / (m_iNumVerticesX - 1) * m_fTileNumX, (_float)i / (m_iNumVerticesZ - 1) * m_fTileNumZ);
			pVertices[iIndex].vSplatting = m_pVerticesSplatting[iIndex] = _float4(1.f, 0.f, 0.f, 0.f);
		}
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

#pragma endregion

#pragma region INDEX_BUFFER

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	m_iIndicesSize = sizeof(FACEINDICES32);
	
	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, m_iIndicesSize * m_iNumPrimitive);
	_uint		iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;
			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;


			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumPrimitive]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

#pragma endregion

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar * pFilePath)
{
	HANDLE			hFile = CreateFile(
		pFilePath, GENERIC_READ, NULL, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSGBOX("Failed to Create File for Load Terrain Data");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_bool bBreak = false;

#pragma region Vertex

	ReadFile(hFile, &m_iNumVerticesX, sizeof(_uint), &dwByte, NULL);
	ReadFile(hFile, &m_iNumVerticesZ, sizeof(_uint), &dwByte, NULL);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertexBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = sizeof(VTXNORMULTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.StructureByteStride = sizeof(VTXNORMULTEX);

	VTXNORMULTEX*		pVertices = new VTXNORMULTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORMULTEX) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	m_pVerticesSplatting = new _float4[m_iNumVertices];
	ZeroMemory(m_pVerticesSplatting, sizeof(_float4) * m_iNumVertices);

	_float3 position;
	ZeroMemory(&position, sizeof(_float3));

	_float4 splatting;
	ZeroMemory(&splatting, sizeof(_float4));

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			ReadFile(hFile, &position, sizeof(_float3), &dwByte, NULL);
			ReadFile(hFile, &splatting, sizeof(_float4), &dwByte, NULL);

			if (0 == dwByte)
			{
				bBreak = true;
				break;
			}

			_uint		iIndex = i * m_iNumVerticesX + j;
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = position;

			pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			pVertices[iIndex].vSplatting = m_pVerticesSplatting[iIndex] = splatting;

			_int iDebug = 0;
		}

		if (bBreak)
			break;
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

#pragma endregion

#pragma region Index

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFace = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[1];
			pIndices[iNumFace]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;
			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));

			++iNumFace;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[2];
			pIndices[iNumFace]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._0]);
			vDest = XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._2]) - XMLoadFloat3(&m_pVerticesPos[pIndices[iNumFace]._1]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));

			++iNumFace;
		}
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

#pragma endregion

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	CloseHandle(hFile);
	OutputDebugString(L"File Loaded");

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Y(_fvector vPosition)
{
	_float3 position;
	XMStoreFloat3(&position, vPosition);

	_uint index = (_uint)position.z * m_iNumVerticesX + (_uint)position.x;

	_uint indices[4] = {
		index + m_iNumVerticesX,
		index + m_iNumVerticesX + 1,
		index + 1,
		index
	};

	_float width = position.x - m_pVerticesPos[indices[0]].x;
	_float depth = m_pVerticesPos[indices[0]].z - position.z;

	_vector result_v;
	if (width > depth)
	{
		result_v = XMPlaneFromPoints(
			XMLoadFloat3(&m_pVerticesPos[indices[0]]),
			XMLoadFloat3(&m_pVerticesPos[indices[1]]),
			XMLoadFloat3(&m_pVerticesPos[indices[2]])
		);
	}
	else
	{
		result_v = XMPlaneFromPoints(
			XMLoadFloat3(&m_pVerticesPos[indices[0]]),
			XMLoadFloat3(&m_pVerticesPos[indices[2]]),
			XMLoadFloat3(&m_pVerticesPos[indices[3]])
		);
	}

	_float4 result;
	XMStoreFloat4(&result, result_v);

	return -(result.x * position.x + result.z * position.z + result.w) / result.y;
}

_float CVIBuffer_Terrain::Compute_Y(const _float3& vPoint)
{
	_uint		iIndex = (_uint)vPoint.z * m_iNumVerticesX + (_uint)vPoint.x;

	if ((_uint)vPoint.x < 0 || (_uint)vPoint.x >= m_iNumVerticesX - 1 ||
		(_uint)vPoint.z < 0 || (_uint)vPoint.z >= m_iNumVerticesZ - 1)
		return 0.f;

	_uint		iIndices[4] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	_float		fWidth = vPoint.x - m_pVerticesPos[iIndices[0]].x;
	_float		fDepth = m_pVerticesPos[iIndices[0]].z - vPoint.z;

	_float4		vPlane;

	if (fWidth > fDepth)
	{
		XMStoreFloat4(&vPlane,XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[1]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]])));
	}
	else
	{
		XMStoreFloat4(&vPlane, XMPlaneFromPoints(XMLoadFloat3(&m_pVerticesPos[iIndices[0]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[2]]),
			XMLoadFloat3(&m_pVerticesPos[iIndices[3]])));
	}

	return (-vPlane.x * vPoint.x - vPlane.z * vPoint.z - vPlane.w) / vPlane.y;
}

void CVIBuffer_Terrain::Edit_Position(POS_BRUSH_DESC _posBrush, _float3 * out)
{
	POS_BRUSH_DESC brush = _posBrush;

	_vector brush_position = XMVectorZero();
	_vector vertex_position = XMVectorZero();

	// XZ거리를 재기 위한 변경
	brush_position = XMLoadFloat3(&brush.brush_position);
	vecy(brush_position) = 0.f;

	D3D11_MAPPED_SUBRESOURCE subResource = { 0 };
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	VTXNORMULTEX* pVertices = (VTXNORMULTEX*)subResource.pData;

	// 브러쉬 범위 안에 들어오는 정점들의 인덱스를 보관하기 위한 컨테이너
	vector<_uint> index_vector;
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			// 인덱스 계산
			_uint		iIndex = i * m_iNumVerticesX + j;

			// 모든 정점을 초기화 시키는 브러시에 대한 처리
			if (BRUSH_WORK::WORK_INIT == brush.brush_work)
			{
				pVertices[iIndex].vPosition.y = 0.f;

				m_pVerticesPos[iIndex].y = pVertices[iIndex].vPosition.y;

				m_pVerticesSplatting[iIndex].x = pVertices[iIndex].vSplatting.x = 1.f;
				m_pVerticesSplatting[iIndex].y = pVertices[iIndex].vSplatting.y = 0.f;
				m_pVerticesSplatting[iIndex].z = pVertices[iIndex].vSplatting.z = 0.f;
				m_pVerticesSplatting[iIndex].w = pVertices[iIndex].vSplatting.w = 0.f;

				continue;
			}

			// 계산한 인덱스로 iIndex번째 정점의 위치값에 접근
			vertex_position = XMLoadFloat3(&pVertices[iIndex].vPosition);
			vecy(vertex_position) = 0.f;

			_float length = DISTANCE(brush_position - vertex_position);
			if (brush.brush_range < length)
				continue;

			index_vector.push_back(iIndex);
		}
	}

	switch (brush.brush_work)
	{
	case Engine::CVIBuffer_Terrain::WORK_UP:
	case Engine::CVIBuffer_Terrain::WORK_DOWN:

		for (auto& index : index_vector)
		{
			// 저장한 인덱스로 index번째 정점의 위치값에 접근
			vertex_position = XMLoadFloat3(&pVertices[index].vPosition);
			vecy(vertex_position) = 0.f;

			// 브러쉬 포지션과 정점의 거리 계산
			_float distance = DISTANCE(brush_position - vertex_position);
			_float factor = (brush.brush_range - distance) / brush.brush_range;

			for (_uint i = 0; i < 2; ++i)
				factor = sqrtf(factor);

			_float amount = brush.brush_value * factor;

			if (CVIBuffer_Terrain::WORK_UP == brush.brush_work)
				pVertices[index].vPosition.y += amount;

			if (CVIBuffer_Terrain::WORK_DOWN == brush.brush_work)
				pVertices[index].vPosition.y -= amount;

			m_pVerticesPos[index].y = pVertices[index].vPosition.y;
		}

		break;
	case Engine::CVIBuffer_Terrain::WORK_FLAT:
	{
		_float max_y = pVertices[index_vector[0]].vPosition.y;
		_float min_y = pVertices[index_vector[0]].vPosition.y;

		// 범위 안에 들어온 정점들을 순회하면서 최대값과 최소값을 구해둔다.
		for (auto& index : index_vector)
		{
			if (max_y < pVertices[index].vPosition.y)
				max_y = pVertices[index].vPosition.y;
			else if (min_y > pVertices[index].vPosition.y)
				min_y = pVertices[index].vPosition.y;
		}

		// 최소, 최대에 대한 차이값과 중간값을 구해둔다.
		_float gap_between_min_max = fabsf(max_y - min_y);
		_float middle_y = (max_y + min_y) * 0.5f;

		// 중간값을 기준으로 평탄화할 때 사용할 변수. 중간값으로부터 가장 먼 값과의 차이를 담아둔다.
		_float bigger_gap_from_middle = max(fabsf(max_y - middle_y), fabsf(min_y - middle_y));

		// 브러쉬 스타일에대한 처리(Higher, Lower, middle)
		switch (brush.brush_style)
		{
		case Engine::CVIBuffer_Terrain::STYLE_HIGHER:

			for (auto& index : index_vector)
			{
				_float factor = (max_y - pVertices[index].vPosition.y) / gap_between_min_max;

				if (true == isnan(factor))
					factor = 0.f;

				_float amount = factor * brush.brush_value;

				pVertices[index].vPosition.y += amount;
				m_pVerticesPos[index].y = pVertices[index].vPosition.y;
			}

			break;
		case Engine::CVIBuffer_Terrain::STYLE_LOWER:

			for (auto& index : index_vector)
			{
				_float factor = (pVertices[index].vPosition.y - min_y) / gap_between_min_max;

				if (true == isnan(factor))
					factor = 0.f;

				_float amount = factor * brush.brush_value;

				pVertices[index].vPosition.y -= amount;
				m_pVerticesPos[index].y = pVertices[index].vPosition.y;
			}

			break;
		case Engine::CVIBuffer_Terrain::STYLE_MIDDLE:

			for (auto& index : index_vector)
			{
				_float factor = (pVertices[index].vPosition.y - middle_y) / bigger_gap_from_middle;

				if (0.f == bigger_gap_from_middle)
					factor = 0.f;

				_float amount = factor * brush.brush_value;

				pVertices[index].vPosition.y -= amount;
				m_pVerticesPos[index].y = pVertices[index].vPosition.y;
			}

			break;
		case Engine::CVIBuffer_Terrain::STYLE_END:
		default:

			// 예외 발생. brush.style이 end이거나 다른 값이 들어가있음.
			// assert(SUCCEEDED(E_FAIL));

			break;
		}
	}
		break;
	case Engine::CVIBuffer_Terrain::WORK_INIT:

		// 위에서 이미 해당 조건에 대한 처리가 되어있어서 여기까지 내려올 일이 없음.

		break;
	case Engine::CVIBuffer_Terrain::WORK_END:
	default:
		
		// 예외 발생. brush.work가 end이거나 다른 값이 들어가있음.
		// assert(SUCCEEDED(E_FAIL));
		
		break;
	}

	m_pDeviceContext->Unmap(m_pVB, 0);

	if (nullptr != out)
	{
		vecy(brush_position) = Compute_Y(brush_position);
		XMStoreFloat3(out, brush_position);
	}
}

void CVIBuffer_Terrain::Edit_Texture(TEX_BRUSH_DESC _texBrush, _float3 * out)
{
	TEX_BRUSH_DESC brush = _texBrush;

	_vector brush_position = XMVectorZero();
	_vector vertex_position = XMVectorZero();

	// XZ거리를 재기 위한 변경
	brush_position = XMLoadFloat3(&brush.brush_position);
	vecy(brush_position) = 0.f;

	D3D11_MAPPED_SUBRESOURCE subResource = { 0 };
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	VTXNORMULTEX* pVertices = (VTXNORMULTEX*)subResource.pData;

	// 브러쉬 범위 안에 들어오는 정점들의 인덱스를 보관하기 위한 컨테이너
	vector<_uint> index_vector;
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			// 인덱스 계산
			_uint		iIndex = i * m_iNumVerticesX + j;

			// 계산한 인덱스로 iIndex번째 정점의 위치값에 접근
			vertex_position = XMLoadFloat3(&pVertices[iIndex].vPosition);
			vecy(vertex_position) = 0.f;

			_float length = DISTANCE(brush_position - vertex_position);
			if (brush.brush_range < length)
				continue;

			index_vector.push_back(iIndex);
		}
	}

	for (auto& index : index_vector)
	{
		vertex_position = XMLoadFloat3(&pVertices[index].vPosition);
		vecy(vertex_position) = 0.f;

		// 브러쉬 포지션과 정점의 거리 계산
		_float distance = DISTANCE(brush_position - vertex_position);
		_float factor = (brush.brush_range - distance) / brush.brush_range;

		for (_uint i = 0; i < 2; ++i)
			factor = sqrtf(factor);

		_float amount = brush.brush_value * factor;

		switch (brush.brush_tex)
		{
		case Engine::CVIBuffer_Terrain::TEX_FIRST:
			pVertices[index].vSplatting.x += amount;
			break;
		case Engine::CVIBuffer_Terrain::TEX_SECOND:
			pVertices[index].vSplatting.y += amount;
			break;
		case Engine::CVIBuffer_Terrain::TEX_THIRD:
			pVertices[index].vSplatting.z += amount;
			break;
		case Engine::CVIBuffer_Terrain::TEX_FORTH:
			pVertices[index].vSplatting.w += amount;
			break;
		case Engine::CVIBuffer_Terrain::TEX_END:
		default:

			// 예외 발생. brush.brush_tex가 end이거나 다른 값이 들어가있음.
			assert(SUCCEEDED(E_FAIL));

			break;
		}

		_float mag = pVertices[index].vSplatting.x + pVertices[index].vSplatting.y + pVertices[index].vSplatting.z + pVertices[index].vSplatting.w;
		pVertices[index].vSplatting.x /= mag;
		pVertices[index].vSplatting.y /= mag;
		pVertices[index].vSplatting.z /= mag;
		pVertices[index].vSplatting.w /= mag;

		_vector splatting_norm = XMLoadFloat4(&pVertices[index].vSplatting);
		XMStoreFloat4(&pVertices[index].vSplatting, splatting_norm);
		XMStoreFloat4(&m_pVerticesSplatting[index], splatting_norm);
	}

	m_pDeviceContext->Unmap(m_pVB, 0);

	if (nullptr != out)
	{
		vecy(brush_position) = Compute_Y(brush_position);
		XMStoreFloat3(out, brush_position);
	}
}

void CVIBuffer_Terrain::Calculate_Normal(VTXNORMULTEX * pVertices)
{
	_bool wasNull = false;

	if (nullptr == pVertices)
	{
		wasNull = true;

		D3D11_MAPPED_SUBRESOURCE subResource = { 0 };
		m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

		pVertices = (VTXNORMULTEX*)subResource.pData;
	}

	FACEINDICES32*	pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFace = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[1];
			pIndices[iNumFace]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;
			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));

			++iNumFace;

			pIndices[iNumFace]._0 = iIndices[0];
			pIndices[iNumFace]._1 = iIndices[2];
			pIndices[iNumFace]._2 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFace]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFace]._2].vNormal) + vNormal));

			++iNumFace;
		}
	}

	Safe_Delete_Array(pIndices);

	if (wasNull)
		m_pDeviceContext->Unmap(m_pVB, 0);
}

HRESULT CVIBuffer_Terrain::Load_Terrain(const _tchar * pFilePath)
{
	if (nullptr == pFilePath)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	HANDLE			hFile = CreateFile(
		pFilePath,
		GENERIC_READ, NULL, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		//MSGBOX("Failed to Create File for Load Terrain Data");

		BREAKPOINT;
		return E_FAIL;
	}

	DWORD dwByte = 0;
	_bool bBreak = false;

	_uint vertices_num_x = 0;
	_uint vertices_num_z = 0;
	ReadFile(hFile, &vertices_num_x, sizeof(_uint), &dwByte, NULL);
	ReadFile(hFile, &vertices_num_z, sizeof(_uint), &dwByte, NULL);

	_float3 position;
	ZeroMemory(&position, sizeof(_float3));

	_float4 splatting;
	ZeroMemory(&splatting, sizeof(_float4));

	D3D11_MAPPED_SUBRESOURCE subResource = { 0 };
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	VTXNORMULTEX* pVertices = (VTXNORMULTEX*)subResource.pData;
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			ReadFile(hFile, &position, sizeof(_float3), &dwByte, NULL);
			ReadFile(hFile, &splatting, sizeof(_float4), &dwByte, NULL);

			if (0 == dwByte)
			{
				bBreak = true;
				break;
			}

			_uint		iIndex = i * m_iNumVerticesX + j;
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = position;
			pVertices[iIndex].vSplatting = m_pVerticesSplatting[iIndex] = splatting;

			_bool bDebug = false;
		}

		if (bBreak)
			break;
	}

	Calculate_Normal(pVertices);
	m_pDeviceContext->Unmap(m_pVB, 0);

	CloseHandle(hFile);
	OutputDebugString(L"File Loaded");

	return S_OK;
}

_bool CVIBuffer_Terrain::Terrain_Pick(const _matrix & WorldMatrixInverse, _fvector * pOut)
{
	CPicking*		pPicking = GET_INSTANCE(CPicking);

	pPicking->Transform_ToLocalSpace(WorldMatrixInverse);

	_uint		iIndexByte = 0;

	if (m_eIndexFormat == DXGI_FORMAT_R16_UINT)
		iIndexByte = 2;
	else
		iIndexByte = 4;

	_bool		isPick = false;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
	VTXNORMULTEX*	pVertices = (VTXNORMULTEX*)mappedResource.pData;
	m_pDeviceContext->Unmap(m_pVB, 0);

	D3D11_MAPPED_SUBRESOURCE mappedResourceIndex;
	ZeroMemory(&mappedResourceIndex, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResourceIndex);
	FACEINDICES32*	pIndices = (FACEINDICES32*)mappedResourceIndex.pData;
	m_pDeviceContext->Unmap(m_pIB, 0);

	_vector vRay[2] = { pPicking->Get_RayDirLocal(), pPicking->Get_RayPosLocal() };

	_vector vNowPos;

	_uint StartX;
	_uint StartZ;
	_uint EndX;
	_uint EndZ;

	for (_int i = 0; i < 200; i++)
	{
		vNowPos = vRay[1] + vRay[0] * (_float)i;
		if ((XMVectorGetZ(vNowPos) < 0.f || XMVectorGetZ(vNowPos) > _float(m_iNumVerticesZ - 1)) || (XMVectorGetX(vNowPos) < 0.f || XMVectorGetX(vNowPos) > _float(m_iNumVerticesX - 1)))
			continue;
		_int	iIndex = (_int)XMVectorGetZ(vNowPos) * m_iNumVerticesX + (_int)XMVectorGetX(vNowPos);
		if (0 > iIndex || iIndex >= (_int)m_iNumVertices)
			continue;
		if (pVertices[iIndex].vPosition.y >= XMVectorGetY(vNowPos))
		{
			_vector vPastPos = vRay[1] + vRay[0] * (_float)(i - 1);

			StartX = XMVectorGetX(vPastPos) < pVertices[iIndex].vPosition.x ? (_int)XMVectorGetX(vPastPos) - 1 : (_int)pVertices[iIndex].vPosition.x - 1;
			EndX = XMVectorGetX(vPastPos) < pVertices[iIndex].vPosition.x ? (_int)pVertices[iIndex].vPosition.x + 2 : (_int)XMVectorGetX(vPastPos) + 2;
			StartZ = XMVectorGetZ(vPastPos) < pVertices[iIndex].vPosition.z ? (_int)XMVectorGetZ(vPastPos) - 1 : (_int)pVertices[iIndex].vPosition.z - 1;
			EndZ = XMVectorGetZ(vPastPos) < pVertices[iIndex].vPosition.z ? (_int)pVertices[iIndex].vPosition.z + 2 : (_int)XMVectorGetZ(vPastPos) + 2;

			if (StartZ < 0.f)
			{
				StartZ = 0;
				EndZ = 1;
			}
			if (EndZ > _float(m_iNumVerticesZ - 1))
			{
				StartZ = m_iNumVerticesZ - 2;
				EndZ = m_iNumVerticesZ - 1;
			}
			if (StartX < 0.f)
			{
				StartX = 0;
				EndX = 1;
			}
			if (EndX > _float(m_iNumVerticesX - 1))
			{
				StartX = m_iNumVerticesX - 2;
				EndX = m_iNumVerticesX - 1;
			}

			break;
		}
		if (i == 199)
		{
			 
			return false;
		}
	}

	for (_uint i = StartZ; i < EndZ; ++i)
	{
		for (_uint j = StartX * 2; j < EndX * 2; j++)
		{
			_uint iIndex = i * (m_iNumVerticesX - 1) * 2 + j;
			_uint	iIndices[3] = { 0 };


			iIndices[0] = ((FACEINDICES32*)pIndices)[iIndex]._0;
			iIndices[1] = ((FACEINDICES32*)pIndices)[iIndex]._1;
			iIndices[2] = ((FACEINDICES32*)pIndices)[iIndex]._2;



			_vector		vPoint[3] = {
				XMLoadFloat3((_float3*)(((_byte*)pVertices) + m_VBDesc.StructureByteStride * iIndices[0])),
				XMLoadFloat3((_float3*)(((_byte*)pVertices) + m_VBDesc.StructureByteStride * iIndices[1])),
				XMLoadFloat3((_float3*)(((_byte*)pVertices) + m_VBDesc.StructureByteStride * iIndices[2]))
			};

			if (isPick = pPicking->isPick(vPoint, pOut))
			{
				_matrix		WorldMatrix = XMMatrixInverse(nullptr, WorldMatrixInverse);
				*(_vector*)pOut = XMVector3TransformCoord(*pOut, WorldMatrix);
				 
				return true;
			}
		}
	}

	 

	return isPick;
}

HRESULT CVIBuffer_Terrain::Save_Y(const char* szTerrainSaveFileName)
{
	char szFileName[MAX_PATH] = "../Data/";
	strcat_s(szFileName, szTerrainSaveFileName);
	strcat_s(szFileName, ".dat");
	_tchar wszTerrainSaveFileName[MAX_PATH] = L"";

	MultiByteToWideChar(CP_ACP, 0, szFileName, MAX_PATH, wszTerrainSaveFileName, sizeof(_tchar) * MAX_PATH);

	HANDLE		hFile = CreateFile(wszTerrainSaveFileName,
		GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// 파일을 저장할 수 없으면
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSGBOX("Can't Save");
		return E_FAIL;
	}
	DWORD		dwByte = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(m_pDeviceContext->Map(m_pVB, D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
	{
		MSGBOX("!");
		return E_FAIL;
	}
	VTXNORMULTEX*		pVertices = (VTXNORMULTEX*)mappedResource.pData;

	WriteFile(hFile, &(m_iNumVerticesX), sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &(m_iNumVerticesZ), sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &(m_fTileNumX), sizeof(_float), &dwByte, NULL);
	WriteFile(hFile, &(m_fTileNumZ), sizeof(_float), &dwByte, NULL);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			WriteFile(hFile, &(pVertices[iIndex].vPosition.y), sizeof(_float), &dwByte, NULL);
		}
	}

	m_pDeviceContext->Unmap(m_pVB, 0);

	CloseHandle(hFile);

	return S_OK;
}

void CVIBuffer_Terrain::Save_Terrain()
{
	HANDLE			hFile = CreateFile(TEXT("../SaveFiles/Terrain/SaveData.dat"),
		GENERIC_WRITE, NULL, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSGBOX("Failed to Create File for Save Terrain Data");
		return;
	}

	DWORD dwByte = 0;

	WriteFile(hFile, &m_iNumVerticesX, sizeof(_uint), &dwByte, NULL);
	WriteFile(hFile, &m_iNumVerticesZ, sizeof(_uint), &dwByte, NULL);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint index = i * m_iNumVerticesX + j;

			WriteFile(hFile, &m_pVerticesPos[index], sizeof(_float3), &dwByte, NULL);
			WriteFile(hFile, &m_pVerticesSplatting[index], sizeof(_float4), &dwByte, NULL);

			_bool bDebug = false;
		}
	}

	CloseHandle(hFile);
	OutputDebugString(L"File Saved");
}

void CVIBuffer_Terrain::Load_Terrain()
{
	HANDLE			hFile = CreateFile(
		TEXT("../SaveFiles/Terrain/SaveData.dat"),
		GENERIC_READ, NULL, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSGBOX("Failed to Create File for Load Terrain Data");
		return;
	}

	DWORD dwByte = 0;
	_bool bBreak = false;

	_uint vertices_num_x = 0;
	_uint vertices_num_z = 0;
	ReadFile(hFile, &vertices_num_x, sizeof(_uint), &dwByte, NULL);
	ReadFile(hFile, &vertices_num_z, sizeof(_uint), &dwByte, NULL);

	_float3 position;
	ZeroMemory(&position, sizeof(_float3));

	_float4 splatting;
	ZeroMemory(&splatting, sizeof(_float4));

	D3D11_MAPPED_SUBRESOURCE subResource = { 0 };
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subResource);

	VTXNORMULTEX* pVertices = (VTXNORMULTEX*)subResource.pData;
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			ReadFile(hFile, &position, sizeof(_float3), &dwByte, NULL);
			ReadFile(hFile, &splatting, sizeof(_float4), &dwByte, NULL);

			if (0 == dwByte)
			{
				bBreak = true;
				break;
			}

			_uint		iIndex = i * m_iNumVerticesX + j;
			pVertices[iIndex].vPosition = m_pVerticesPos[iIndex] = position;
			pVertices[iIndex].vSplatting = m_pVerticesSplatting[iIndex] = splatting;

			_bool bDebug = false;
		}

		if (bBreak)
			break;
	}

	Calculate_Normal(pVertices);
	m_pDeviceContext->Unmap(m_pVB, 0);

	CloseHandle(hFile);
	OutputDebugString(L"File Loaded");
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumX, _uint iNumZ)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iNumX, iNumZ)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TERRAIN_BUFFER_DESC _Desc)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	pInstance->m_fTileNumX = (_float)_Desc.tileX;
	pInstance->m_fTileNumZ = (_float)_Desc.tileZ;

	if (FAILED(pInstance->NativeConstruct_Prototype(_Desc.numX, _Desc.numZ)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFilePath)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pFilePath)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pVerticesSplatting);
}
