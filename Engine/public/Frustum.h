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

	//		����� �ִ� ������ ���÷� �̵���Ŵ
	HRESULT Transform_ToLocalSpace(class CTransform* pTransform);
public:
	//		���� ����ü�� �ִ��� �Ǵ�
	_bool	isIn_WorldSpace(_fvector vPoint, _float fRange);

	//		���� ����ü�� �ִ��� �Ǵ�
	_bool	isIn_LocalSpace(_fvector vPoint, _float fRange);
private:
	_float3	m_vPoint[8];
	_float3	m_vPointInWorld[8];
	_float3	m_vPointInLocal[8];

	_float4	m_PlaneInWorld[6];
	_float4	m_PlaneInLocal[6];
private:
	//		����ü ����� ������ִ� �Լ�
	void	Compute_Plane(const _float3* pPoints, _float4* pPlanes);
public:
	virtual void	Free() override;
};

END