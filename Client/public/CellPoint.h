#pragma once

/*
IMGUI에서 네비매쉬 깔 때 쓰이는 클래스

*/

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CCellPoint final : public CGameObject
{
private:
	CCellPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCellPoint(const CCellPoint& rhs);
	virtual ~CCellPoint() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
public:
	void			IsPickCell(_bool bPick);
private:
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
private:
	_float3*			m_pPoint = nullptr;
private:
	HRESULT SetUp_Components();
public:
	static CCellPoint*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END