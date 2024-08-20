#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CDonald_Icicle final : public CGameObject
{
public:
	typedef struct tagIcicleDesc
	{
		CCollider*		pTargetCollider = nullptr;
		_vector			vPosition = __vector(0.f, 0.f, 0.f, 1.f);
	}ICICLEDESC;
public:
	CDonald_Icicle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDonald_Icicle(const CDonald_Icicle& rhs);
	virtual ~CDonald_Icicle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CModel*					m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	ICICLEDESC				m_tIcicleDesc;
	_float					m_fDist = 0.f;
	_vector					m_vTargetPosition;
	_float					m_fLerpTime = 0.f;
	_vector					m_vCenterPosition = __vector(0.f, 0.f, 0.f, 1.f);

	_bool					m_bHalf = false;

	_float4					m_vPosition = _float4(0.f, 0.f, 0.f, 1.f);
public:
	static	CDonald_Icicle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END