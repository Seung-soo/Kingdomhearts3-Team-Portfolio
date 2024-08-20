#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	MAKE_SINGLETON(CFrustum)
private:
	CFrustum();
	~CFrustum() = default;
public:
	HRESULT	Initialize();
	void	Update();

	//		월드상에 있는 점들을 로컬로 이동시킴
	HRESULT Transform_ToLocalSpace(class CTransform* pTransform);
public:
	//		월드 절두체에 있는지 판단
	_bool	isIn_WorldSpace(_fvector vPoint, _float fRange);

	//		로컬 절두체에 있는지 판단
	_bool	isIn_LocalSpace(_fvector vPoint, _float fRange);
private:
	_float3	m_vPoint[8];
	_float3	m_vPointInWorld[8];
	_float3	m_vPointInLocal[8];

	_float4	m_PlaneInWorld[6];
	_float4	m_PlaneInLocal[6];
private:
	//		절두체 평면을 계산해주는 함수
	void	Compute_Plane(const _float3* pPoints, _float4* pPlanes);
public:
	virtual void	Free() override;
};

END