#pragma once#pragma once
#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;
END

BEGIN(Client)

class CUI_BossName final : public CUI_Parents
{
private:
	CUI_BossName(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_BossName(const CUI_BossName& rhs);
	virtual ~CUI_BossName() = default;

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

	_double		dTime = 0;
	_double		dSpeed = 0;
	_double		dSpeed2 = 0;

	_bool		bSound = false;
	_int		iCom_Texture_Set = 0;
	_int		iLevel_Name = 0;

	CGameObject* 	pBoss = nullptr;
	_int			iMAX_MonsterHP = 0;
	_int			iCnt = 0;
	_bool			bUI_Create = true;

public:
	static CUI_BossName* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END