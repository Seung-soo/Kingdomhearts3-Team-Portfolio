#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client) 

class CFlower final : public CMonster
{
private:
	CFlower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFlower(const CFlower& rhs);
	virtual ~CFlower() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
	virtual void	PostRender(_double TimeDelta);

public:
	virtual HRESULT Save_ObjectData(HANDLE hFile) override;
	virtual HRESULT Load_ObjectData(HANDLE hFile) override;

private:
	void ChangeAnim(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_Animations();
	HRESULT SetUp_ConstantTable();

private:
	CShader*	m_pShaderCom   = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CModel*		m_pModelCom    = nullptr;

	_uint m_iAnimAcc      = 0;
	_bool m_bSummonNobody = false;

public:
	static	CFlower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END