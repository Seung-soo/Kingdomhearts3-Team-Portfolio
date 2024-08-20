#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Cube;
class CCollider;
END

BEGIN(Client)

class CCameraPoint final : public CGameObject
{
public:
	enum CAMERAPOINTTYPE { CAMERAPOINTTYPE_POS, CAMERAPOINTTYPE_LOOKAT, CAMERAPOINTTYPE_VIA, CAMERAPOINTTYPE_END};

public:
	typedef struct tagCameraPointDesc
	{
		CAMERAPOINTTYPE eType;
		_vector			vPos;
		_uint			iIndex;
	}CAMERAPOINTDESC;

private:
	CCameraPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCameraPoint(const CCameraPoint& rhs);
	virtual ~CCameraPoint() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	CCollider*	GetCollider() { return m_pColliderCom; }
	void		FollowMouse(_float fPosY);
	void		SetCameraPointDesc(CAMERAPOINTDESC CameraPointDesc) { m_CameraPointDesc = CameraPointDesc; }
	void		SetCameraPointDescPos(_fvector vPos) { m_CameraPointDesc.vPos = vPos; }

public:
	virtual HRESULT Save_ObjectData(HANDLE hFile) override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CShader*		m_pShaderCom   = nullptr;
	CTexture*		m_pTextureCom  = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CCollider*		m_pColliderCom = nullptr;
	CVIBuffer_Cube* m_pVIBufferCom = nullptr;

	CAMERAPOINTDESC m_CameraPointDesc;

	_vector m_vColor = XMVectorSet(0.f, 0.f, 0.f, 1.f);

public:
	static	CCameraPoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END