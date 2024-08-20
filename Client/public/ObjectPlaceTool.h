#pragma once

/*
* ��Ʈ�� + ��Ŭ��	: ���̵�����Ʈ ���� (�ִ� 1��)
* ��Ʈ�� + q, w, e	: ���̵�����Ʈ �̵�
* ��Ʈ�� + z		: ���� �������� ������ ��ü ����
* Delete			: ��ü Ŭ���� ���·� Delete ������ ���ŵ�
*/

#include "Base.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)

class CObjectPlaceTool final : public CBase
{
	MAKE_SINGLETON(CObjectPlaceTool)

private:
	CObjectPlaceTool();
	virtual ~CObjectPlaceTool() = default;

#ifdef _DEBUG
public:
	HRESULT ObjectPlaceTool_SetUp();
	_int	ObjectPlaceTool_Tick(_double TimeDelta);
	HRESULT ObjectPlaceTool_Render();

private: // ���̵�����Ʈ ����
	void CreatePickingPoint(_double TimeDelta);
	void MoveGuidePoint(_double TimeDelta);

private: // ������Ʈ ����
	void CreateGameObject(_double TimeDelta);
	void MoveGameObject(_double TimeDelta);
	void DeleteGameObject(_double TimeDelta);

private: // ���̺�
	void Reset(_double TimeDelta);
	void Save(_double TimeDelta);
	void Load(_double TimeDelta);

private:
	void CreateImGuiButton(const char* pButtonTag, const _tchar* pLayerTag, const _tchar* pPrototypeTag);

private:
	ID3D11Device*			m_pDevice        = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	CGameInstance*			m_pGameInstance  = nullptr;
	class CImGui_Manager*	m_pImGui_Manager = nullptr;

	// ���̵�����Ʈ ����
	class CGameObject*	m_pGuidePoint   = nullptr;
	class CGameObject*	m_pPickingPoint = nullptr;
	_float				m_GuidePointY   = 0.f;
	
	// ���� �ֱٿ� ������ ������Ʈ, ������Ʈ �迭
	class CGameObject*			m_pSelectedObject;
	vector<class CGameObject*>	m_CreatedObjects;

	// �������� �̸� ����
	char m_SaveFileName[128]  = "DefaultSave.dat";
	char m_LoadFileName[128]  = "Layer_SkullSoldier.dat";

	const char* m_Objects[15] = { "Trigger", "SkullSoldier", "CrowSoldier", "ThinSoldier", "BowMan", "Cannon", "ShiningEgg", "Monkey", "Mower", "Nightmare", "Flower", "BigTree", "Icicle", "FrozenWolf", "Goat" };

	const _tchar* m_LayerTag[15] = { TEXT("Layer_Trigger"), TEXT("Layer_SkullSoldier"), TEXT("Layer_CrowSoldier"), TEXT("Layer_ThinSoldier"), TEXT("Layer_BowMan"), TEXT("Layer_Cannon"), TEXT("Layer_ShiningEgg"), TEXT("Layer_Monkey"), TEXT("Layer_Mower"), TEXT("Layer_Nightmare"), TEXT("Layer_Flower"), TEXT("Layer_BigTree"), TEXT("Layer_Icicle"), TEXT("Layer_FrozenWolf"), TEXT("Layer_Goat") };

	const _tchar* m_PrototypeTag[15] = { TEXT("Prototype_GameObject_Trigger"), TEXT("Prototype_GameObject_SkullSoldier"), TEXT("Prototype_GameObject_CrowSoldier"), TEXT("Prototype_GameObject_ThinSoldier"), TEXT("Prototype_GameObject_BowMan"), TEXT("Prototype_GameObject_Cannon"), TEXT("Prototype_GameObject_ShiningEgg"), TEXT("Prototype_GameObject_Monkey"), TEXT("Prototype_GameObject_Mower"), TEXT("Prototype_GameObject_Nightmare"), TEXT("Prototype_GameObject_Flower"), TEXT("Prototype_GameObject_BigTree"), TEXT("Prototype_GameObject_Icicle"), TEXT("Prototype_GameObject_FrozenWolf"), TEXT("Prototype_GameObject_Goat") };

#endif

public:
	void virtual Free() override;
};

END