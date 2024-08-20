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

class CUI_MiniMap final : public CUI_Parents
{
public:
	CUI_MiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_MiniMap(const CUI_MiniMap& rhs);
	virtual ~CUI_MiniMap() = default;

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

	_int iCom_Texture = 0;
	_int iPointer_Texture = 0;

	class CPlayer* m_pPlayer = nullptr;
	_bool m_bRender = false;

	_bool bStart = false;

	_vector	Player_Origin_Pos;
	_float4	m_vTargetPosition;

	_bool	bMiniMap_OnOff = false;

	/*===*/
	_float4 fPlayerOriPos;
	_vector vPlayerPos;
	_float4 fPlayerPos;
	_float Player_MovePosX = 0.f;
	_float Player_MovePosZ = 0.f;
	_vector final_position;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_MiniMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END
