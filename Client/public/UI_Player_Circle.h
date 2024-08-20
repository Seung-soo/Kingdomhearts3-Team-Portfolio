#pragma once

#include"Client_Defines.h"
#include"UI_Parents.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Rect;
class CShader;
class CTexture;

// 플레이어가 가지고있는 컴포넌트 가져오기 위한 CStatus 컴포넌트 전방선언
class CStatus;
END

BEGIN(Client)

class CUI_Player_Circle	final : public CUI_Parents
{
private:
	CUI_Player_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Player_Circle(const CUI_Player_Circle& rhs);
	virtual ~CUI_Player_Circle() = default;

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
	_int		Fnc_Hp(_double TimeDelta);
	_int		Fnc_MP(_double TimeDelta);
	_int		Fnc_FOCUS(_double TimeDelta);

//HP======
	_float		fMAX_HP = 0.f;
	_float		fHP = 0.f;
	_float		fSetHP_First = 0.f;
	_float		fSetHP_Second = 0.f;

	_bool		bHP_Change = false;
	_double		dHP_Change_Time = 0.0;
	_float		fHP_Back = 0.f;
	
//MP======
	_float		fMAX_MP = 0.f;
	_float		fMP = 0.f;
	_float		fSetMP_First = 0.f;
	_float		fSetMP_Second = 0.f;

	_bool		bMP_Change = false;
	_double		dMP_Change_Time = 0.0;
	_float		fMP_Back = 0.f;

//FOCUS===
	_float		fMAX_FOCUS = 0.f;
	_float		fFOCUS = 0.f;
	_float		fSetFOCUS_First = 0.f;
	_float		fSetFOCUS_Second = 0.f;

	_bool		bFOCUS_Change = false;
	_double		dFOCUS_Change_Time = 0.0;
	_float		fFOCUS_Back = 0.f;

// Player=
	class CPlayer* m_pPlayer = nullptr;
	CStatus* m_pPlayerStatus = nullptr;

public:
	static CUI_Player_Circle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END