#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CFrogBand final : public CGameObject
{
public:
	typedef struct tagFrogBandDesc
	{
		_tchar	ModelTag[MAX_PATH];	// ¸ðµ¨ ÅÂ±×
		_vector	vPos;
	}FROGBANDDESC;

private:
	CFrogBand(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFrogBand(const CFrogBand& rhs);
	virtual ~CFrogBand() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();
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

	FROGBANDDESC m_FrogBandDesc;

public:
	static	CFrogBand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END