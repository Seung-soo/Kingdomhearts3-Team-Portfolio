#pragma once

#include "Base.h"


BEGIN(Engine)
class CComponent;

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	CComponent*					Get_Component(const _tchar* pComponentTag, _uint iIndex);
	class CGameObject*			Get_GameObject(_uint iIndex);				// 오브젝트 하나 반환
	list<class CGameObject*>*	Get_Object() { return &m_Objects; }			// 오브젝트 리스트 반환
	list<class CGameObject*>*	Get_ObjectPool() { return &m_ObjectPool; }	// 오브젝트 리스트 반환

	void						Set_TimeSpeed(_double TimeSpeed) { m_TimeSpeed = TimeSpeed; }
public:
	HRESULT				Add_GameObject(class CGameObject* pGameObject);
	void				FrontChange();
	HRESULT				Clear_Layer();
	HRESULT				Save_Layer(HANDLE hFile);
	HRESULT				Add_ObjectPool(class CGameObject* pGameObject);
	HRESULT				ThrowToLayer(void* pArg);
	class CGameObject*	ThrowToLayer_And_Return(void* pArg);

	_bool Find_Object(class CGameObject* pGameObject);
	_bool Delete_Object(class CGameObject* pGameObject);

	_int			Tick(_double dTimeDelta);
	_int			LateTick(_double dTimeDelta);
	void			PostRender(_double TimeDelta);
private:
	list<class CGameObject*>			m_Objects;
	typedef list<class CGameObject*>	OBJECTS;

	list<class CGameObject*>			m_ObjectPool;
	typedef list<class CGameObject*>	OBJECTPOOLLIST;

	_double								m_TimeSpeed = 1.0;
public:
	static CLayer*	Create();
	virtual void	Free() override;
};

END