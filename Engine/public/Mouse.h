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

public: // Client 에 보여줄 함수
	pair<_bool, _vector>	Check_Ray_VirtualPlane_Collision(_float fPlaneY); // 가상의 평면이랑 충돌했는지 체크하고, pair 로 충돌여부, 충돌지점을 반환
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