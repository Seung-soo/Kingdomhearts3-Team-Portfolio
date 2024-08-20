#pragma once

#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_MonsterHP final : public CUI_Parents
{
	enum UI_MONSTER_SIZE
	{
		UI_MONSTER_SIZE_TEXT,
		UI_MONSTER_SIZE_LARGE,
		UI_MONSTER_SIZE_MIDDLE,
		UI_MONSTER_SIZE_SMALL,
		UI_MONSTER_SIZE_END
	};
private:
	CUI_MonsterHP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_MonsterHP(const CUI_MonsterHP& rhs);
	virtual ~CUI_MonsterHP() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	//CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom[UI_MONSTER_SIZE_END] = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	class CGameObject*	target_object = nullptr;
	//class CStatus*	target_status_com = nullptr;

private:
	_int	Fnc_MonsterHp(_double TimeDelta);

	_float		fMAX_MonsterHP = 0.f;
	_float		fMonsterHP = 0.f;
	_float		fSetMonsterHP_First = 0.f;
	_float		fSetMonsterHP_Second = 0.f;

	_bool		bMonsterHP_Change = false;
	_double		dMonsterHP_Change_Time = 0.0;
	_float		fMonsterHP_Back = 0.f;

	_bool		bHpRender = false;

// Monster=
	class CPlayer* m_pPlayer = nullptr;

public:
	static CUI_MonsterHP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END

#define	SIZE_LARGE	0
#define	SIZE_MIDDLE	1
#define	SIZE_SMALL	2