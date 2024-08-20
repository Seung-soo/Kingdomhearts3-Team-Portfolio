#pragma once
#include "Item.h"

BEGIN(Engine)

class CPotion final : public CItem
{
private:
	CPotion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPotion(const CPotion& rhs);
	virtual ~CPotion() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	HRESULT ITEM_Potion(void* pArg);
	virtual _int Tick(_double TimeDelta);

	//===
	_int	Potion_Cnt = 0;

public:
	static	CPotion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END