#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;
public:
	void				Set_Parent(class CGameObject* pParent);
	class CGameObject*	Get_Parent() { return m_pParent; }
public:
	virtual HRESULT		NativeConstruct_Prototype();
	virtual HRESULT		NativeConstruct(void* pArg);
//#ifdef _DEBUG
public:
	virtual HRESULT		Render() { return S_OK; }
//#endif
protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	_bool					m_isCloned = false;
	class CGameObject*		m_pParent = nullptr;
public:
	virtual CComponent*	Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END