#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CMap final : public CGameObject
{
public:
	typedef struct tagMapDesc
	{
		_tchar		ModelTag[MAX_PATH];		// �� �±�
		LEVEL		ModelPrototypeLevel;	// ���� ������ �ִ� ����
		_float4x4	WorldMatrix;			// �����Ʈ����(������ �� ä���ֱ�)
	}MAPDESC;

private:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int	Tick(_double TimeDelta);
	virtual _int	LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual HRESULT Render_Shadow()override;

public:
	virtual HRESULT Save_ObjectData(HANDLE hFile) override;
	virtual HRESULT Load_ObjectData(HANDLE hFile) override;

public:
	void ChangeModelPrototypeLevel(LEVEL eLevel) { m_MapDesc.ModelPrototypeLevel = eLevel; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	CShader*	m_pShaderCom       = nullptr;
	CRenderer*	m_pRendererCom     = nullptr;
	CModel*		m_pModelCom        = nullptr;

	// For Shadow
	_bool		m_bRenderShadow	   = false;
#ifdef _DEBUG
	//CCollider* m_pColliderCom = nullptr;
#endif

	MAPDESC m_MapDesc;

	_bool	m_bCullnone = false;
public:
	static	CMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END