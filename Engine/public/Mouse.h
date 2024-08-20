#pragma once

#include "Base.h"

BEGIN(Engine)

class CMouse final : public CBase
{
	MAKE_SINGLETON(CMouse)

private:
	CMouse();
	virtual ~CMouse() = default;

public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hWnd);
	_vector MouseDir_To_WorldSpace();

public: // Client �� ������ �Լ�
	pair<_bool, _vector>	Check_Ray_VirtualPlane_Collision(_float fPlaneY); // ������ ����̶� �浹�ߴ��� üũ�ϰ�, pair �� �浹����, �浹������ ��ȯ
	_vector					Get_MouseWorldDir() { return m_MouseWorldDir; }
	_bool					Ray_Box_Collision(const BoundingBox& box, _Out_ float& Dist);
	_bool					Ray_Sphere_Collision(const BoundingSphere& sphere, _Out_ float& Dist);

private:
	HWND					m_hWnd;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	_Ray m_MouseRay;

	_vector m_MouseWorldPos;
	_vector m_MouseWorldDir;

public:
	virtual void Free() override;
};

END