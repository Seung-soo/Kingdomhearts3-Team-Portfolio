#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSummonMovingParticle final : public CGameObject
{
public:
	typedef struct tagSummonParticleDesc
	{
		_float3	vLocalPosition = _float3(0.f, 0.f, 0.f);
		CTransform*	pTargetTransform = nullptr;
	}SUMPARTDESC;
private:
	CSummonMovingParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSummonMovingParticle(const CSummonMovingParticle& rhs);
	virtual ~CSummonMovingParticle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CTransform*		m_pTargetTransform = nullptr;

public:
	static CSummonMovingParticle*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END