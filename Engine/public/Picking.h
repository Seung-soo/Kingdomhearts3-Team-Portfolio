#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPicking final : public CBase
{
	MAKE_SINGLETON(CPicking)
private:
	CPicking();
	virtual ~CPicking() = default;
public:
	HRESULT	NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hWnd);
public:
	_vector  Get_RayDirLocal();
	_vector  Get_RayPosLocal();
	_float4  Get_RayDirLocal_Float4();
	_float4  Get_RayPosLocal_Float4();
	_vector	 Get_RayDir() { return m_vRayDir; }
	_vector  Get_RayPos() { return m_vRayPos; }
public:
	HRESULT	Transform_ToWorldSpace();
	HRESULT	Transform_ToLocalSpace(_matrix WorldMatrixInverse);
	_bool	isPick(_fvector* pLocalPoints, _fvector* pOut = nullptr);
	_bool	IsNaviPick(_fvector* pLocalPoints);
	_bool	isPick_AABB(BoundingBox tAABB);
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	HWND					m_hWnd;

private:
	class CPipeLine*		m_pPipeLine = nullptr;
	_vector					m_vRayDir;
	_vector					m_vRayPos;

	_vector					m_vRayDirInLocal;
	_vector					m_vRayPosInLocal;
public:
	virtual void	Free() override;
};

END