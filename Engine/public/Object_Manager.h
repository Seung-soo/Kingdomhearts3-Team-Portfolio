#pragma once

#include "Base.h"
#include "GameObject.h"
#include "Layer.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	MAKE_SINGLETON(CObject_Manager)
public:
	CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	map<const _tchar*, CLayer*> Get_ObjectMap(_uint iLevelIndex);
	CComponent*					Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);
	size_t						Get_Size(_uint iLevelIndex, const _tchar* pLayerTag) { return Find_Layer(iLevelIndex, pLayerTag)->Get_Object()->size(); }
	CGameObject*				Get_Front(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*				Get_Back(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*				Get_ObjectPoolBack(_uint iLevelIndex, const _tchar* pLayerTag);
	list<CGameObject*>*			Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject*				Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex);

	void						Set_TimeSpeed(_uint iLevelIndex, const _tchar* pLayerTag, _double TimeSpeed);
	HRESULT						Save_Layer(_uint iLevelIndex, const _tchar* pLayerTag, HANDLE hFile);

public:
	HRESULT			Reserve_Container(_uint iNumLevels);
	HRESULT			Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT			Rmv_Prototype(const _tchar* pPrototypeTag);
	HRESULT			Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg); // 복사 생성
	HRESULT			Rmv_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, CGameObject* pObject);
	CGameObject*	Add_GameObject_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg); // 복사 생성하고 생성한 객체 리턴
	HRESULT			Clear_LevelObject(_uint iLevelIndex);
	HRESULT			Release_LayerObject(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT			Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT			Add_ObjectPool(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);
	HRESULT			FrontChange(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT			ThrowToLayer(_uint iLevelIndex, const _tchar* pLayerTag, void* pArg);
	CGameObject*	ThrowToLayer_And_Return(_uint iLevelIndex, const _tchar* pLayerTag, void* pArg);

public:
	_int	Tick(_double dTimeDelta);
	_int	LateTick(_double dTimeDelta);
	void	GameObject_PostRender(_double TimeDelta);

private:
	map<const _tchar*, CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, CGameObject*>	PROTOTYPES;

private:
	map<const _tchar*, CLayer*>*				m_pLayers;
	typedef map<const _tchar*, CLayer*>			LAYERS;

private:
	_uint m_iNumLevels = 0;

public:
	CGameObject*	Find_Prototype(const _tchar* pPrototypeTag);
	CLayer*			Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void	Free() override;
};

END