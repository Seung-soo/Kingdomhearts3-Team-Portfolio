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
class CUI_BossHP final : public CUI_Parents
{
private:
	CUI_BossHP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_BossHP(const CUI_BossHP& rhs);
	virtual ~CUI_BossHP() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

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
	_bool		bHp_Frist = false;

	CGameObject*		pBoss = nullptr;
	_int	iCnt_MaxHp = 0;
	_int	iCnt_Hp = 0;
	_int	iHp_Cnt = 0;

	_int iFirst = 0;
	_int iSecond = 0;

public:
	static CUI_BossHP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END