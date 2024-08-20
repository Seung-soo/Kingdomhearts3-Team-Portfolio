#include "..\public\VIBuffer_Trail.h"
#include "GameInstance.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
	{
		TRAILDESC		tDesc;
		memcpy(&tDesc, pArg, sizeof(TRAILDESC));

		m_iNumVertices = tDesc.iNumVerticies;
		m_fAliveTime   = tDesc.fLifeTime;
		m_iLerpCnt     = (_uint)tDesc.fLerpCnt; 
		m_fDuration    = tDesc.fDuration;
	}

	else
	{
		m_iNumVertices = 1000;
	}

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iNumIndicesPerPrimitive = 3;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = sizeof(VTXTEX) * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.StructureByteStride = sizeof(VTXTEX);
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	VTXTEX*		pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);
	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	m_iNumPrimitive = m_iNumVertices - 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = sizeof(FACEINDICES16) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	FACEINDICES16*	pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);
	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

void CVIBuffer_Trail::Update(_float fTimeDelta, _matrix* pWorldMat)
{
	if (m_tTrailData.empty() )
	{
		return;
	}

	// #1. ���ӽð� �ʰ��� Ʈ���� ���� ��������.
	while (!m_tTrailData.empty())
	{
		auto frontData = m_tTrailData.front(); // ���� �տ� �ִ� ���̴� ���ӽð��� ���� ū ���� �̴�. 
		
		if (frontData.timecount + fTimeDelta >= m_fAliveTime)
			m_tTrailData.pop_front();
		
		else
			break;
	}

	// #2. ����� 1�� ���ϸ� ������.
	if (m_tTrailData.size() <= 1)
		return;

	// #3. ��Ƴ��� Ʈ���ϵ��� ���ӽð��� ����.
	for (auto iter = m_tTrailData.begin(); iter != m_tTrailData.end(); ++iter)
	{
		iter->timecount += fTimeDelta;
	}

	D3D11_MAPPED_SUBRESOURCE		SubResourceDataVB;
	D3D11_MAPPED_SUBRESOURCE		SubResourceDataIB;

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataVB);
	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataIB);



	//for (_uint i = 0; i < m_iNumVertices; ++i)  
	//{
	//	((VTXTEX*)SubResourceDataVB.pData)[i].vTexUV = _float2(0.f, 0.f);
	//	((VTXTEX*)SubResourceDataVB.pData)[i].vPosition = _float3(0.f, 0.f, 0.f);
	//}


	// #4. ���� ���Ϳ� ��� ����� � ó��.
	_uint				iDataCnt = (_uint)m_tTrailData.size();
	unsigned long		iCurVertexIndex = 0;				//���� ������Ʈ ��(������ �ִ�) ���� �ε���

	for (_uint i = 0; i < iDataCnt; ++i)
	{
		SplineTrailPosition((VTXTEX*)SubResourceDataVB.pData, i, iCurVertexIndex, pWorldMat);

		if (iCurVertexIndex >= m_iNumVertices)
			break;
	}

	// �� ���鿡 �־��� uv����
	_float			fUVRate = 1.f / _float(iCurVertexIndex - 2);

	// �� �տ� �ִ� ģ���� ���� ����
	for (_uint i = 0; i < iCurVertexIndex; i += 2)
	{
		//0���� up 1���� down
		((VTXTEX*)SubResourceDataVB.pData)[i].vTexUV = _float2((iCurVertexIndex  - fUVRate * i), 0.f);
		((VTXTEX*)SubResourceDataVB.pData)[i + 1].vTexUV = _float2((iCurVertexIndex - fUVRate * i), 1.f);
	}

	// ���� �ﰢ�� ����
	_uint		iCurTriCnt = iCurVertexIndex - 2;

	//��ȸ�ϸ鼭 �ٽ� �ε��� ����
	for (_uint i = 0; i < iCurTriCnt; i += 2)
	{
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._0 = i;
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._1 = i + 3;
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._2 = i + 1;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._0 = i;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._1 = i + 2;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._2 = i + 3;
	}

	m_pDeviceContext->Unmap(m_pVB, 0);
	m_pDeviceContext->Unmap(m_pIB, 0);
}

void CVIBuffer_Trail::Update_EndMotion(_float fTimeDelta, _matrix* pWorldMat)
{
	if (m_tTrailData.empty())
	{
		return;
	}

	// #1. ���ӽð� �ʰ��� Ʈ���� ���� ��������.
	while (!m_tTrailData.empty())
	{
		auto frontData = m_tTrailData.front(); // ���� �տ� �ִ� ���̴� ���ӽð��� ���� ū ���� �̴�. 

		if (frontData.timecount + fTimeDelta >= m_fEndingAliveTime)
			m_tTrailData.pop_front();

		else
			break;
	}

	// #2. ����� 1�� ���ϸ� ������.
	if (m_tTrailData.size() <= 1)
	{
		m_isFinished = true;
		return;
	}

	// #3. ��Ƴ��� Ʈ���ϵ��� ���ӽð��� ����.
	for (auto iter = m_tTrailData.begin(); iter != m_tTrailData.end(); ++iter)
	{
		iter->timecount += fTimeDelta;
	}

	D3D11_MAPPED_SUBRESOURCE		SubResourceDataVB;
	D3D11_MAPPED_SUBRESOURCE		SubResourceDataIB;

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataVB);
	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataIB);



	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		((VTXTEX*)SubResourceDataVB.pData)[i].vTexUV = _float2(0.f, 0.f);
		((VTXTEX*)SubResourceDataVB.pData)[i].vPosition = _float3(0.f, 0.f, 0.f);
	}


	// #4. ���� ���Ϳ� ��� ����� � ó��.
	_uint				iDataCnt = (_uint)m_tTrailData.size();
	unsigned long		iCurVertexIndex = 0;				//���� ������Ʈ ��(������ �ִ�) ���� �ε���

	for (_uint i = 0; i < iDataCnt; ++i)
	{
		SplineTrailPosition((VTXTEX*)SubResourceDataVB.pData, i, iCurVertexIndex, pWorldMat);

		if (iCurVertexIndex >= m_iNumVertices)
			break;
	}

	// �� ���鿡 �־��� uv����
	_float			fUVRate = 1.f / _float(iCurVertexIndex - 2);

	// �� �տ� �ִ� ģ���� ���� ����
	for (_uint i = 0; i < iCurVertexIndex; i += 2)
	{
		//0���� up 1���� down
		((VTXTEX*)SubResourceDataVB.pData)[i].vTexUV = _float2((iCurVertexIndex - fUVRate * i), 0.f);
		((VTXTEX*)SubResourceDataVB.pData)[i + 1].vTexUV = _float2((iCurVertexIndex - fUVRate * i), 1.f);
	}

	// ���� �ﰢ�� ����
	_uint		iCurTriCnt = iCurVertexIndex - 2;

	//��ȸ�ϸ鼭 �ٽ� �ε��� ����
	for (_uint i = 0; i < iCurTriCnt; i += 2)
	{
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._0 = i;
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._1 = i + 3;
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._2 = i + 1;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._0 = i;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._1 = i + 2;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._2 = i + 3;
	}

	m_pDeviceContext->Unmap(m_pVB, 0);
	m_pDeviceContext->Unmap(m_pIB, 0);
}

HRESULT CVIBuffer_Trail::Render(CShader * pShader, _uint iPassIndex)
{
	__super::Render(pShader, iPassIndex);

	return S_OK;
}

void CVIBuffer_Trail::SplineTrailPosition(OUT VTXTEX * vtx, const size_t & dataindex, unsigned long & index, _matrix * pWorldMat)
{
	auto		iWorldInverse = XMMatrixInverse(nullptr, *pWorldMat);

	// iIndex�� ���� ���� ������ �ִ� �������� ��ȸ�ϴ� �ε�����
	// dataIndex �� ���� ���� ��ȸ�ϰ� �ִ� m_tTrailData�� �ε�����
	// ��� traildataũ�⿡ ���缭 ������ �������ִ� �ʳ�

	// ���� �ε����� �ش��ϴ� ������ ���� ��ǥ�� �о��ֱ� (���� �����Ҷ��� ���� ����� �����༭ ���ؼ� ����ϴ� �����ε�)
	_vector		vLocal[2];					//�� ������ ���� dataindex�� ���� �������� �Ǵ� �����̴�

	vLocal[0] = XMVector3TransformCoord(XMLoadFloat3(&m_tTrailData[dataindex].position[0]), iWorldInverse);
	vLocal[1] = XMVector3TransformCoord(XMLoadFloat3(&m_tTrailData[dataindex].position[1]), iWorldInverse);

	XMStoreFloat3(&vtx[index].vPosition, vLocal[0]);
	++index;

	// ����ó���� �ʼ�
	if (index >= m_iNumVertices)
		return;

	XMStoreFloat3(&vtx[index].vPosition, vLocal[1]);
	++index;

	if (index >= m_iNumVertices)
		return;

	_uint		iDataSize = (_uint)m_tTrailData.size();
	_vector		vLerpPos[2];

	//LerpCnt�� ��� ���� �����ϰ� ���� ������ ������
	for (_uint i = 0; i < m_iLerpCnt; ++i)
	{
		_uint		iPreIndex       = (_uint)(dataindex < 1 ? 0 : dataindex - 1);
		_uint		iCurIndex       = (_uint)dataindex;
		_uint		iNextIndex      = (_uint)(dataindex + 1 >= iDataSize ? dataindex : dataindex + 1);
		_uint		iAfterNextIndex = (_uint)(dataindex + 2 >= iDataSize ? iNextIndex : dataindex + 2);
		_float		fWeight         = i / _float(m_iLerpCnt);

		// Weight ���� 0�� �������� iCurIndex�����̶� ���������, 1�� �������� iNextIndex�����̶� ���������

		//���ʶ���
		vLerpPos[0] = XMVectorCatmullRom(XMLoadFloat3(&m_tTrailData[iPreIndex].position[0]),
										 XMLoadFloat3(&m_tTrailData[iCurIndex].position[0]),
										 XMLoadFloat3(&m_tTrailData[iNextIndex].position[0]),
										 XMLoadFloat3(&m_tTrailData[iAfterNextIndex].position[0]),
										 fWeight);

		//�ؿ�����
		vLerpPos[1] = XMVectorCatmullRom(XMLoadFloat3(&m_tTrailData[iPreIndex].position[1]),
										 XMLoadFloat3(&m_tTrailData[iCurIndex].position[1]),
										 XMLoadFloat3(&m_tTrailData[iNextIndex].position[1]),
										 XMLoadFloat3(&m_tTrailData[iAfterNextIndex].position[1]),
										 fWeight);

		_vector		vToLocal = XMVector3TransformCoord(vLerpPos[0], iWorldInverse);
		XMStoreFloat3(&vtx[index].vPosition, vToLocal);
		++index;

		if (index >= m_iNumVertices)
			return;

		vToLocal = XMVector3TransformCoord(vLerpPos[1], iWorldInverse);
		XMStoreFloat3(&vtx[index].vPosition, vToLocal);
		++index;

		if (index >= m_iNumVertices)
			return;
	}
}

void CVIBuffer_Trail::AddNewTrail(const _float3& upposition, const _float3& downposition, _float fTimeDelta)
{
	m_fTimeAccum += fTimeDelta;

	if ((double)m_fTimeAccum > m_fDuration)
	{
		TrailData data(upposition, downposition);
		m_tTrailData.emplace_back(data);
		m_fTimeAccum = 0;
	}
}

void CVIBuffer_Trail::UpdateTrail(const _float3& upposition, const _float3& downposition, _float fTimeDelta)
{
	//m_fEndingTimeAccum += fTimeDelta;
	//m_fTimeAccum += fTimeDelta;

	//if (m_fEndingTimeAccum <= m_fEndingTime)
	//{
	//	if ((double)m_fTimeAccum > 0.05f)
	//	{
	//		TrailData data(upposition, downposition);
	//		m_tTrailData.emplace_back(data);
	//		m_fTimeAccum = 0;
	//	}
	//}
}


void CVIBuffer_Trail::Reset_Trail()
{
	D3D11_MAPPED_SUBRESOURCE		SubResourceDataVB;
	//D3D11_MAPPED_SUBRESOURCE		SubResourceDataIB;

	//m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataVB);
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceDataVB);
	//m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResourceDataIB);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		((VTXTEX*)SubResourceDataVB.pData)[i].vTexUV = _float2(0.f, 0.f);
		((VTXTEX*)SubResourceDataVB.pData)[i].vPosition = _float3(0.f, 0.f, 0.f);
	}

	//��ȸ�ϸ鼭 �ٽ� �ε��� ����
	//for (_uint i = 0; i < (m_iNumVertices - 2); i += 2)
	//{
	//	((FACEINDICES16*)SubResourceDataIB.pData)[i]._0 = 0;
	//	((FACEINDICES16*)SubResourceDataIB.pData)[i]._1 = 0;
	//	((FACEINDICES16*)SubResourceDataIB.pData)[i]._2 = 0;
	//}

	m_pDeviceContext->Unmap(m_pVB, 0);
//	m_pDeviceContext->Unmap(m_pIB, 0);

	if(m_tTrailData.size() >= 1)
		m_tTrailData.clear();

	m_fTimeAccum = 0.f;

	m_fEndingTimeAccum = 0.f;

	m_isFinished = false;
}

CVIBuffer_Trail * CVIBuffer_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Trail::Clone(void * pArg)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}
