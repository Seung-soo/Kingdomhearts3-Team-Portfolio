#pragma once

#include "Client_Defines.h"
#include "UI_Parents.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer;
END

BEGIN(Client)

class CUI_MiniMap_Monster final : public CUI_Parents
{
public:
	typedef struct MonsterPointer
	{
		_float4*	fMonsterPos = nullptr;
		_bool*		bMonsterDead = nullptr;
		_int		ComTexture = 0;

	}MONSTERPOINTER;
public:
	CUI_MiniMap_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_MiniMap_Monster(const CUI_MiniMap_Monster& rhs);
	virtual ~CUI_MiniMap_Monster() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer*					m_pVIBufferCom = nullptr;

	MONSTERPOINTER				pMonsterPointer;
	class CPlayer* m_pPlayer = nullptr;

	_bool	bMiniMap_OnOff = false;

	_vector vPlayerPos;
	_float4 fPlayerPos = _float4(0.f, 0.f, 0.f, 0.f);
	_float4 fMonsterPos = _float4(0.f, 0.f, 0.f, 0.f);
	_float4	fSrdPos = _float4(0.f, 0.f, 0.f, 0.f);
	_vector final_position;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_MiniMap_Monster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
