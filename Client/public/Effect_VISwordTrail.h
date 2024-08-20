#pragma once
#include "GameObject.h"
#include "VIBuffer_Trail.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CEffect_VISwordTrail final: public CGameObject
{
public:
	enum TYPE {TYPE_PRE, TYPE_CUR, TYPE_END};

public:
	CEffect_VISwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_VISwordTrail(const CEffect_VISwordTrail& rhs);
	virtual ~CEffect_VISwordTrail() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void	PostRender(_double TimeDelta);

	virtual void Set_Active(_bool isActive) override; 

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CEffect_VISwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	static CEffect_VISwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Trail*			m_pBufferCom = nullptr;
	CTransform*					m_pTargetTransform = nullptr;


	_float						m_fTimeAccum = 0.f;
	_float						m_fInterval = 0.f;

	_matrix						m_vWorld[TYPE_END];

	_bool						m_isEnding = false;

	float						m_fTransparency = 0.f;
};

END