#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CItem : public CGameObject
{
public:
	enum Item_List {
		ITEM_POTION, ITEM_HIGH_POTION , ITEM_ELIXIR ,
		ITEM_END
	};

protected:
	explicit	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END