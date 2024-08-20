#include "..\public\Cell.h"
#include "VIBuffer_Triangle.h"
#include "Shader.h"
#include "GameObject.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CGameObject** CCell::Get_CellPoints()
{
	return m_pCellPoint;
}

HRESULT CCell::NativeConstruct(const _float3 * pPoints, _uint iIndex)
{
	// �ε��� ����
	m_iIndex = iIndex;

	// ����Ʈ �����ؼ� �־��ִ°� ����
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	// ���� ����
	XMStoreFloat3(&m_vLineDir[LINE_AB], _vec3norm(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A])));
	XMStoreFloat3(&m_vLineDir[LINE_BC], _vec3norm(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B])));
	XMStoreFloat3(&m_vLineDir[LINE_CA], _vec3norm(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C])));

	// ���, ����Ʈ ����
	XMStoreFloat3(&m_vNormal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vLineDir[LINE_AB]), XMLoadFloat3(&m_vLineDir[LINE_BC]))));
	XMStoreFloat3(&m_vRight, _vec3norm(XMVector3Cross(XMLoadFloat3(&m_vNormal), _axisY)));

	// �� ���� �ְ����, �������̸� ����
	_float highest_height = max(max(m_vPoints[POINT_A].y, m_vPoints[POINT_B].y), m_vPoints[POINT_C].y);
	_float lowest_height = min(min(m_vPoints[POINT_A].y, m_vPoints[POINT_B].y), m_vPoints[POINT_C].y);

	for (_uint i = 0; i < POINTS::POINT_END; ++i)
	{
		if(highest_height == m_vPoints[i].y)
			m_eHighestPoint = (POINTS)i;

		if (lowest_height == m_vPoints[i].y)
			m_eLowestPoint = (POINTS)i;
	}

	// ���� ���� ������ �����صд�
	_float height_ab = (m_vPoints[POINT_A].y + m_vPoints[POINT_B].y) * 0.5f;
	_float height_bc = (m_vPoints[POINT_B].y + m_vPoints[POINT_C].y) * 0.5f;
	_float height_ca = (m_vPoints[POINT_C].y + m_vPoints[POINT_A].y) * 0.5f;

	_float highest_line_height = max(max(height_ab, height_bc), height_ca);
	if (highest_line_height == height_ab)
		m_eHighestLine = LINE_AB;
	else if(highest_line_height == height_bc)
		m_eHighestLine = LINE_BC;
	else if (highest_line_height == height_ca)
		m_eHighestLine = LINE_CA;

	// ���� ���� �������� ���صд�
	XMStoreFloat3(&m_vCenter, 
		(XMLoadFloat3(&m_vPoints[POINT_A]) + 
		XMLoadFloat3(&m_vPoints[POINT_B]) + 
		XMLoadFloat3(&m_vPoints[POINT_C])) * 0.3f);

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;
	}
	return false;
}

_bool CCell::isIn(_fvector vPosition, _int * pNeighborIndex, _float3** pOut, _bool bClimb)
{
	// ���⼭ �� Ÿ�Ժ��� isIn ���ǰ˻縦 �ٸ��� �����ؾ��Ѵ�.
	// FLOW�� �ٴڱ������� ��´ٸ� ���ǰ˻縦 �ٸ��� ������� Ÿ���� ���� RUNNING ���̴�.

	// XZ�� �Ǻ� �Ұ����� ���γ׺�(��)�� ��
	if (TYPE::TYPE_RUNNING == m_eType)
	{
		for (_uint i = 0; i < LINE_END; i++)
		{
			_vector		vDestDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
			_vector		vSourDir = XMVector3Normalize(XMLoadFloat3(&m_vLineDir[i]));

			// Up(vNormal)�� Look(m_vLineDir[i])�� �����ؼ� Right�� ���ϰ� -1�� ���ؼ� Left ���·� ����
			// �ؿ��� �ڵ� �����ϱ� ���ؼ� Left ���. Right���ؼ� ��ȣ �ٲ㼭 �����ص� ��
			_vector		vRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vNormal), vSourDir));
			_vector		vLeft = vRight * -1.f;

			if (true == bClimb)
			{
				if (true == isUpperSide(vPosition))
				{
					if (vecy(vPosition) < HighestY())
					{
						if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
						{
							if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
								*pNeighborIndex = m_iNeighbor[i];

							// ��� ���� ������ �������� ����
							if (nullptr != pOut)
								*pOut = &m_vLineDir[i];

							return false;
						}
					}
				}

				else
				{
					if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
					{
						if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
							*pNeighborIndex = m_iNeighbor[i];

						// ��� ���� ������ �������� ����
						if (nullptr != pOut)
							*pOut = &m_vLineDir[i];

						return false;
					}
				}
			}

			else
			{
				if (vecy(vPosition) < HighestY())
				{
					if (0 <= XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
					{
						if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
							*pNeighborIndex = m_iNeighbor[i];

						// ��� ���� ������ �������� ����
						if (nullptr != pOut)
							*pOut = &m_vLineDir[i];

						return false;
					}
				}
			}

#pragma region Save
			//if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
			//{
			//	if(0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
			//		*pNeighborIndex = m_iNeighbor[i];

			//	// ��� ���� ������ �������� ����
			//	if (nullptr != pOut)
			//		*pOut = &m_vLineDir[i];

			//	return false;
			//}
#pragma endregion

		}
	}

	// �׿� XZ�� �Ǻ� ������ �׺��� ��
	else
	{
		/* ppOut�� �����̵� ���Ϳ� ���� ������ ������ ���� ������ ���� */
		for (_uint i = 0; i < LINE_END; i++)
		{
			_vector		vDestDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
			_vector		vSourDir = XMVector3Normalize(XMLoadFloat4(&_float4(m_vLineDir[i].z * -1.f, 0.f, m_vLineDir[i].x, 0.f)));

			// �ۿ� �ִ��� Ȯ��
			if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vSourDir)))
			{
				if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
					*pNeighborIndex = m_iNeighbor[i];

				// ��� ���� ������ �������� ����
				if (nullptr != pOut)
					*pOut = &m_vLineDir[i];

				return false;
			}
		}
	}

	return true;
}

_bool CCell::isIn_Return_Direction_And_Point(_fvector vPosition, _int * pNeighborIndex, _float3 * pDir, _float3 * pPos, _bool bClimb)
{
	// ���⼭ �� Ÿ�Ժ��� isIn ���ǰ˻縦 �ٸ��� �����ؾ��Ѵ�.
	// FLOW�� �ٴڱ������� ��´ٸ� ���ǰ˻縦 �ٸ��� ������� Ÿ���� ���� RUNNING ���̴�.

	// XZ�� �Ǻ� �Ұ����� ���γ׺�(��)�� ��
	if (TYPE::TYPE_RUNNING == m_eType)
	{
		for (_uint i = 0; i < LINE_END; i++)
		{
			_vector		vDestDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
			_vector		vSourDir = XMVector3Normalize(XMLoadFloat3(&m_vLineDir[i]));

			// Up(vNormal)�� Look(m_vLineDir[i])�� �����ؼ� Right�� ���ϰ� -1�� ���ؼ� Left ���·� ����
			// �ؿ��� �ڵ� �����ϱ� ���ؼ� Left ���. Right���ؼ� ��ȣ �ٲ㼭 �����ص� ��
			_vector		vRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&m_vNormal), vSourDir));
			_vector		vLeft = vRight * -1.f;

			if (true == bClimb)
			{
				if (true == isUpperSide(vPosition))
				{
					if (vecy(vPosition) < HighestY())
					{
						if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
						{
							if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
								*pNeighborIndex = m_iNeighbor[i];

							// ��� ���� ������ �������� ����
								pDir = &m_vLineDir[i];
								pPos = &m_vPoints[i];

							return false;
						}
					}
				}

				else
				{
					if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
					{
						if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
							*pNeighborIndex = m_iNeighbor[i];

						// ��� ���� ������ �������� ����
							pDir = &m_vLineDir[i];
							pPos = &m_vPoints[i];

						return false;
					}
				}
			}

			else
			{
				if (vecy(vPosition) < HighestY())
				{
					if (0 <= XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
					{
						if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
							*pNeighborIndex = m_iNeighbor[i];

						// ��� ���� ������ �������� ����
							pDir = &m_vLineDir[i];
							pPos = &m_vPoints[i];

						return false;
					}
				}
			}

#pragma region Save
			//if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vLeft)))
			//{
			//	if(0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
			//		*pNeighborIndex = m_iNeighbor[i];

			//	// ��� ���� ������ �������� ����
			//	if (nullptr != pOut)
			//		*pOut = &m_vLineDir[i];

			//	return false;
			//}
#pragma endregion

		}
	}

	// �׿� XZ�� �Ǻ� ������ �׺��� ��
	else
	{
		/* ppOut�� �����̵� ���Ϳ� ���� ������ ������ ���� ������ ���� */
		for (_uint i = 0; i < LINE_END; i++)
		{
			_vector		vDestDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
			_vector		vSourDir = XMVector3Normalize(XMLoadFloat4(&_float4(m_vLineDir[i].z * -1.f, 0.f, m_vLineDir[i].x, 0.f)));

			// �ۿ� �ִ��� Ȯ��
			if (0 < XMVectorGetX(XMVector3Dot(vDestDir, vSourDir)))
			{
				if (0 <= m_iNeighbor[i] && nullptr != pNeighborIndex)
					*pNeighborIndex = m_iNeighbor[i];

				// ��� ���� ������ �������� ����
					pDir = &m_vLineDir[i];
					pPos = &m_vPoints[i];

				return false;
			}
		}
	}

	return true;
}

_float CCell::Compute_Y(_fvector vPoint)
{
	_float4 plane;

	XMStoreFloat4(&plane, XMPlaneFromPoints(
		Get_Point(POINT_A), Get_Point(POINT_B), Get_Point(POINT_C)));

	return (-plane.x * XMVectorGetX(vPoint) - plane.z * XMVectorGetZ(vPoint) - plane.w) / plane.y;
}

_bool CCell::isAbove(_fvector vPoint, _float* pOut)
{
	if (nullptr != pOut)
		*pOut = vecy(vPoint) - Compute_Y(vPoint);

	return Compute_Y(vPoint) <= vecy(vPoint) ? true : false;
}

_bool CCell::isUpperSide(_fvector vPoint, _float* pOut)
{
	_float4 plane;

	_vector pointA, pointB, pointC;
	pointA = Get_Point(POINT_A);
	pointB = Get_Point(POINT_B);
	pointC = Get_Point(POINT_C);

	XMStoreFloat4(&plane, XMPlaneFromPoints(pointA, pointB, pointC));

	_float ax = vecx(vPoint) * plane.x;
	_float by = vecy(vPoint) * plane.y;
	_float cz = vecz(vPoint) * plane.z;
	_float d = plane.w;

	if (nullptr != pOut)
	{
		// ������ ���� ���� �Ÿ�(90�� ����)
		vecy(pointA) = 0.f;
		vecy(pointB) = 0.f;
		vecy(pointC) = 0.f;

		_float distAB = DISTANCE(pointA - pointB);
		_float distBC = DISTANCE(pointB - pointC);
		_float distCA = DISTANCE(pointC - pointA);

		_float longest = max(max(distAB, distBC), distCA);

		LINES line_type = LINES::LINE_END;
		if (longest == distAB)
			line_type = LINES::LINE_AB;
		else if (longest == distBC)
			line_type = LINES::LINE_BC;
		else if (longest == distCA)
			line_type = LINES::LINE_CA;

		// A == �÷��̾� ������, B, C ����� ������ point ������
		_vector positionA, positionB, positionC;
		positionA = vPoint;
		vecy(positionA) = 0.f;

		switch (line_type)
		{
		case Engine::CCell::LINE_AB:

			positionB = Get_Point(POINT_A);
			positionC = Get_Point(POINT_B);

			break;
		case Engine::CCell::LINE_BC:

			positionB = Get_Point(POINT_B);
			positionC = Get_Point(POINT_C);

			break;
		case Engine::CCell::LINE_CA:

			positionB = Get_Point(POINT_C);
			positionC = Get_Point(POINT_A);

			break;
		}

		_vector dot = XMVector3Length(XMVector3Cross(positionA - positionB, positionC - positionB));
		dot /= XMVector3Length(positionC - positionB);

		*pOut = vecx(dot);
	}
	
	return 0 <= ax + by + cz + d ? true : false;
}

_float CCell::Get_CloseLine(_fvector vPoint, LINES* pOut, _bool bXZ)
{
	_float distance = FLT_MAX;
	LINES closest = LINES::LINE_END;

	_float temp = 0.f;
	for (_uint i = 0; i < LINE_END; ++i)
	{
		switch ((LINES)i)
		{
		case Engine::CCell::LINE_AB:

			temp = Compute_Distance(vPoint, LINE_AB, bXZ);
			if (distance > temp)
			{
				distance = temp;
				closest = LINE_AB;
			}

			break;
		case Engine::CCell::LINE_BC:

			temp = Compute_Distance(vPoint, LINE_BC, bXZ);
			if (distance > temp)
			{
				distance = temp;
				closest = LINE_BC;
			}

			break;
		case Engine::CCell::LINE_CA:

			temp = Compute_Distance(vPoint, LINE_CA, bXZ);
			if (distance > temp)
			{
				distance = temp;
				closest = LINE_CA;
			}

			break;
		}
	}

	if (nullptr != pOut)
		*pOut = closest;

	return distance;
}

_float CCell::Compute_Distance(_fvector vPoint, LINES eLine, _bool bXZ)
{
	_vector point_A, point_B, point_C;
	point_A = point_B = point_C = XMVectorZero();

	point_A = vPoint;

	switch (eLine)
	{
	case Engine::CCell::LINE_AB:

		point_B = XMLoadFloat3(&m_vPoints[POINT_A]);
		point_C = XMLoadFloat3(&m_vPoints[POINT_B]);

		break;
	case Engine::CCell::LINE_BC:

		point_B = XMLoadFloat3(&m_vPoints[POINT_B]);
		point_C = XMLoadFloat3(&m_vPoints[POINT_C]);

		break;
	case Engine::CCell::LINE_CA:

		point_B = XMLoadFloat3(&m_vPoints[POINT_C]);
		point_C = XMLoadFloat3(&m_vPoints[POINT_A]);

		break;
	}

	if (true == bXZ)
	{
		vecy(point_A) = 0.f;
		vecy(point_B) = 0.f;
		vecy(point_C) = 0.f;
	}

	_vector dot = XMVector3Length(XMVector3Cross(point_A - point_B, point_C - point_B));
	dot /= XMVector3Length(point_C - point_B);

	return vecx(dot);
}

//#ifdef _DEBUG
void CCell::CellPointsDead()
{
	for (_uint i = 0; i < POINT_END; i++)
	{
		m_pCellPoint[i]->Set_Dead(true);
	}
}

HRESULT CCell::Render(CVIBuffer_Triangle * pVIBuffer, CShader * pShader, _float4 vColor)
{
	if (FAILED(pVIBuffer->Update(m_vPoints)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == XMVector4Equal(XMLoadFloat4(&vColor), XMVectorSet(1.f, 0.f, 0.f, 1.f)))
		vColor = _hotpink4;

	else
	{
		switch (m_eType)
		{
		case Engine::CCell::TYPE_TERRAIN:

			vColor = _green4;

			break;
		case Engine::CCell::TYPE_ROAD:

			vColor = _white4;

			break;
		case Engine::CCell::TYPE_NOTROAD:

			vColor = _black4;

			break;
		case Engine::CCell::TYPE_RUNNING:

			vColor = _yellow4;

			break;
		case Engine::CCell::TYPE_FLOW:

			vColor = _cyan4;

			break;

		case Engine::CCell::TYPE_END:

			vColor = _blue4;

			break;
		}
	}

	if (FAILED(pShader->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(pVIBuffer->Render(pShader, 0)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}
//#endif
CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _float3 * pPoints, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pPoints, iIndex)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
