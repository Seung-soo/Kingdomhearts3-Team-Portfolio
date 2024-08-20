#pragma once
#include "GameObject.h"

class CEffect_Base abstract: public CGameObject
{
protected:
	explicit CEffect_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Base(const CEffect_Base& rhs);
	virtual ~CEffect_Base() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual CGameObject*	Clone(void* pArg)PURE;
	virtual void			Free() override;
};

