#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CRenderer;
END

BEGIN(Client)

class CMap_Object_KGWall final : public CGameObject
{
private:
	CMap_Object_KGWall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMap_Object_KGWall(const CMap_Object_KGWall& rhs);
	virtual ~CMap_Object_KGWall() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;

	_int				iCom_Object = 0;
	_vector				vPosition;
	_float4				fPosition = _float4(0.f, 0.f, 0.f, 0.f);

	_int				iRandomWall = 0;

	_double				dRandomTime = 0;
	_int				iRandom = 0;
	_bool				bPattern = false;

	/* MoveWall */
	_int				iCnt = 0;
	_int				iPatternCnt = 0;

	_int				iSwitch = 0;

	_double				dTime = 0.0;
	_double				dTime2 = 0.0;
	_float				SettingPosY = 0.f;
	_float				fSpeed = 0.f;

	_bool				bPattern1 = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMap_Object_KGWall*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END