#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Cube;
END

BEGIN(Client)

// Mouse Ŭ�������� ��ŷ�� ��ġ�� ǥ���ϱ� ���� ť�갴ü

class CPickingPoint final : public CGameObject
{
private:
	CPickingPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPickingPoint(const CPickingPoint& rhs);
	virtual ~CPickingPoint() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void FollowMouse(_float fPosY);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CShader*		m_pShaderCom   = nullptr;
	CTexture*		m_pTextureCom  = nullptr;
	CRenderer*		m_pRendererCom = nullptr;
	CVIBuffer_Cube* m_pVIBufferCom = nullptr;

	_vector m_vColor = XMVectorSet(0.f, 0.f, 1.f, 0.5f); // ���̴��� �Ѱܼ� �÷��� ���İ� ����

public:
	static	CPickingPoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END