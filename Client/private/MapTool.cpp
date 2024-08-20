#include "stdafx.h"
#include "MapTool.h"
#include "ImGui_Manager.h"
#include "GameObject.h"
#include "PickingPoint.h"
#include "CellPoint.h"

READY_SINGLETON(CMapTool)

CMapTool::CMapTool()
{
}

#ifdef _DEBUG
HRESULT CMapTool::MapTool_SetUp()
{
    // ������� �ʱ�ȭ
    m_pGameInstance  = CGameInstance::GetInstance();
    m_pDevice        = m_pGameInstance->Get_Device();
    m_pDeviceContext = m_pGameInstance->Get_Device_Context();
    m_pImGui_Manager = CImGui_Manager::GetInstance();
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pImGui_Manager);

    // m_pGuidePoint �� ���̵�� ��ŷ����Ʈ ���� 
    m_pGuidePoint = m_pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("Layer_GuidePoint"), 0);

    if (nullptr == m_pGameInstance || nullptr == m_pDevice || nullptr == m_pDeviceContext || nullptr == m_pImGui_Manager)
    {
        MSGBOX("ERROR :: CMapTool::MapTool_SetUp");
        return E_FAIL;
    }


	// �׺�Ž� ������ ���� ������Ʈ�� �ʱ�ȭ
#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CellPoint"), CCellPoint::Create(m_pDevice, m_pDeviceContext))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pNaviVIBuffer = CVIBuffer_Triangle::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pNaviVIBuffer)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pNaviShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../ShaderFiles/Shader_Navigation.hlsl"), VTXCOL_DECLARATION::Elements, VTXCOL_DECLARATION::iNumElements);
	if (nullptr == m_pNaviShader)
	{
		BREAKPOINT;
		return E_FAIL;
	}
#endif

    return S_OK;
}

_int CMapTool::MapTool_Tick(_double TimeDelta)
{
    // ImGui_Manager �� show_maptool_window �� �������� ���� MapTool Window �� ������
    if (true == CImGui_Manager::GetInstance()->show_maptool_window)
    {
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("MapTool Window");

        if (ImGui::BeginTabBar("Select Type", ImGuiTabBarFlags_None))
        {
            // �� ��ü ���� �� ����
            if (ImGui::BeginTabItem("Map"))
            {
                // ��ŷ ó��
                GuidePickingPointUpdate();  // ���̵���ŷ����Ʈ ��ġ ������Ʈ
                CreatePickingPoint();       // PickingPoint ����
                SetPickingPointY();         // m_fPickingPointY �� ����                

                // �� ó��
                SetGameObjectTransform(TAB_MAP);    // ���õ� ���ӿ�����Ʈ�� Transform ���� ����
                m_pImGui_Manager->SmallSeparator();
                ShowCurrentMapList();               // ���� ��ġ�� �� ����� ������
                m_pImGui_Manager->SmallSeparator();
                CreateMapList();                    // ��ġ�� �� �� ����� ������
                CreateContinuousMap();
                DeleteLatestGameObject(TAB_MAP);    // ���� �ֱٿ� ������ �� ��ü ����
                MoveToGuidePickingPoint(TAB_MAP);   // ���õ� �� ��ü�� ���̵���ŷ����Ʈ ��ġ��
                m_pImGui_Manager->SmallSeparator();
                ResetSaveLoadMap();                 // ����, ���̺�, �ε�
				AutoSave(TimeDelta);
#ifdef _DEBUG
                //PickingMap();   // �� �ݶ��̴��� ���콺���� üũ�ؼ� m_pSelectedGameObject ����
#endif
                DeleteMap();    // �� ����

                ImGui::EndTabItem();
            }

            // �Ϲ� ���ӿ�����Ʈ ���� �� ����
            if (ImGui::BeginTabItem("GameObjects"))
            {
                // ��ŷ ó��
                GuidePickingPointUpdate();  // ���̵���ŷ����Ʈ ��ġ ������Ʈ
                CreatePickingPoint();       // PickingPoint ����
                SetPickingPointY();         // m_fPickingPointY �� ����

               // ���ӿ�����Ʈ ó��
                SetGameObjectTransform(TAB_GAMEOBJECT);     // ���õ� ���ӿ�����Ʈ�� Transform ���� ����
                m_pImGui_Manager->SmallSeparator();
                ShowCurrentGameObjectList();                // ���� ��ġ�� ���ӿ�����Ʈ ����� ������
                m_pImGui_Manager->SmallSeparator();
                CreateGameObjectList();                     // ��ġ�� ���ӿ�����Ʈ ����� ������
                DeleteLatestGameObject(TAB_GAMEOBJECT);     // ���� �ֱٿ� ������ ��ü ����
                MoveToGuidePickingPoint(TAB_GAMEOBJECT);    // ���õ� ��ü�� ���̵���ŷ����Ʈ ��ġ��

                ImGui::EndTabItem();
            }

            // �׺�޽� ���� �� ����
            if (ImGui::BeginTabItem("NaviMesh"))
            {
				NaviMesh_List();
				NaviMesh_Make();
				NaviMesh_Load_List();
				if (ImGui::Button("LoadNavi"))
				{
					Load_NaviMesh();
				}

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    return RESULT_NOPROBLEM;
}

HRESULT CMapTool::MapTool_Render()
{

	if (FAILED(NaviMesh_Render()))
	{
		BREAKPOINT;
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CMapTool::ShowCurrentMapList()
{
    // ��ġ�� �� ����Ʈ ����� �����ְ� ����
    ImGui::Bullet(); ImGui::SameLine();
    m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Current Map List");

    if (ImGui::BeginListBox("Current Map List", ImVec2(-FLT_MIN, 5.f * ImGui::GetTextLineHeightWithSpacing())))
    {
        if (ImGui::BeginTable("Current Map Table", 5, ImGuiTableFlags_None))
        {
            // 0. ������Ʈ�� �ο��� ��ȣ �غ�
            _uint iObjectNumber = 0;

            // 1. �� ����Ʈ�� ������
            list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

            if (nullptr == MapList)
            {
                ImGui::EndTable();
                ImGui::EndListBox();
                return S_OK;
            }

            // 2. MapList �� ���ҵ��� ���������� �����ϰ� ���� ��ư �����
            auto iter = (*MapList).begin();
            for (_uint i = 0; i < (*MapList).size(); i++)
            {
                ImGui::TableNextColumn();

                char buf[128] = "";
                sprintf_s(buf, "Map_%d", iObjectNumber);

                // ��ư�� ������ m_pSelectedGameObject �� ���� �ְ� Transform ������ �� �ְ��� / ���̴��н� ����
                if (ImGui::SmallButton(buf))
                {
                    m_pSelectedGameObject[TAB_MAP] = *iter;
                }

                if (m_pSelectedGameObject[TAB_MAP] == *iter)
                {
                    ImGui::SameLine();
                    m_pImGui_Manager->ColoredText(_float3(1.f, 0.5f, 1.f), "ON");
					(*iter)->Set_Pass(30);
                }
				else
					(*iter)->Set_Pass(31);


                ++iObjectNumber;
                ++iter;
            }
            ImGui::EndTable();
        }        
        ImGui::EndListBox();
    }

    return S_OK;
}

HRESULT CMapTool::CreateMapList()
{
    // �� ��ġ
    ImGui::Bullet(); ImGui::SameLine();
    m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Create Map");
    ImGui::SameLine();
    ImGui::Checkbox("Continuous", &m_bContinuous);
    LEVEL eLevel = LEVEL_END;
    if (ImGui::BeginListBox("Create Map", ImVec2(-FLT_MIN, 17.f * ImGui::GetTextLineHeightWithSpacing())))
	{
		if (ImGui::BeginTable("Map Create Table", 4, ImGuiTableFlags_None))
		{
            // TT
            //{
            //    ImGui::TableNextColumn();
            //    for (_uint i = 0; i < 2; ++i)
            //    {
            //        _tchar PrototypeTag[MAX_PATH] = TEXT("");
            //        wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_TT_BackGround_%d"), i);

            //        char ButtonTag[MAX_PATH] = "";
            //        sprintf_s(ButtonTag, "BackGround_%d", i);

            //        CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
            //        ImGui::TableNextColumn();
            //    }

            //    for (_uint i = 0; i < 2; ++i)
            //    {
            //        _tchar PrototypeTag[MAX_PATH] = TEXT("");
            //        wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_TT_Floor_%d"), i);

            //        char ButtonTag[MAX_PATH] = "";
            //        sprintf_s(ButtonTag, "Floor_%d", i);

            //        CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
            //        ImGui::TableNextColumn();
            //    }

            //    for (_uint i = 0; i < 52; ++i)
            //    {
            //        _tchar PrototypeTag[MAX_PATH] = TEXT("");
            //        wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_TT_Building_%d"), i);

            //        char ButtonTag[MAX_PATH] = "";
            //        sprintf_s(ButtonTag, "Building_%d", i);

            //        CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
            //        ImGui::TableNextColumn();
            //    }

            //    for (_uint i = 0; i < 74; ++i)
            //    {
            //        _tchar PrototypeTag[MAX_PATH] = TEXT("");
            //        wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_TT_Object_%d"), i);

            //        char ButtonTag[MAX_PATH] = "";
            //        sprintf_s(ButtonTag, "Object_%d", i);

            //        CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
            //        ImGui::TableNextColumn();
            //    }
            //}
            
            // TS_Andy
            //{
            //    
            //    ImGui::TableNextColumn();
            //    for (_uint i = 0; i < 61; ++i)
            //    {
            //        _tchar PrototypeTag[MAX_PATH] = TEXT("");
            //        wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_TS_Andy_%d"), i);

            //        char ButtonTag[MAX_PATH] = "";
            //        sprintf_s(ButtonTag, "AndyRoom_%d", i);

            //        CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
            //        ImGui::TableNextColumn();
            //    }

            //    for (_uint i = 0; i < 27; ++i)
            //    {
            //        _tchar PrototypeTag[MAX_PATH] = TEXT("");
            //        wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_TS_Duct_%d"), i);

            //        char ButtonTag[MAX_PATH] = "";
            //        sprintf_s(ButtonTag, "AndyDuct_%d", i);

            //        CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
            //        ImGui::TableNextColumn();
            //    }
            //    
            //}

            //// TS_Doll
            //{
            //    ImGui::TableNextColumn();
            //    CreateMapCreateButton(eLevel, "AndyDuct_4", TEXT("Prototype_Component_Model_TS_Duct_4"));

            //    ImGui::TableNextColumn();
            //    for (_uint i = 0; i < 126; ++i)
            //    {
            //        _tchar PrototypeTag[MAX_PATH] = TEXT("");
            //        wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_TS_Doll_%d"), i);

            //        char ButtonTag[MAX_PATH] = "";
            //        sprintf_s(ButtonTag, "Doll_%d", i);

            //        CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
            //        ImGui::TableNextColumn();
            //    }
            //}


			//RA_Map
			{
			eLevel = LEVEL_KEYGRAVE;
			ImGui::TableNextColumn();

			for (_uint i = 0; i < 8; ++i)
			{
				_tchar PrototypeTag[MAX_PATH] = TEXT("");
				wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapSwitch%d"), i);

				char ButtonTag[MAX_PATH] = "";
				sprintf_s(ButtonTag, "Object%d", i);

				CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
				ImGui::TableNextColumn();
			}

	



			////KG_Map
			//{
			//eLevel = LEVEL_KEYGRAVE;
			//ImGui::TableNextColumn();


			//for (_uint i = 0; i < 3; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_Area%d"), i);

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Area%d", i);

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			//for (_uint i = 0; i < 3; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapObject%d"), i);

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Object%d", i);

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			//for (_uint i = 0; i < 8; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapRock%d"), i);

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Rock%d", i);

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			//for (_uint i = 0; i < 4; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapStone%d"), i);

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Stone%d", i);

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			//for (_uint i = 0; i < 8; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapSwitch%d"), i);

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Switch%d", i);

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			//for (_uint i = 0; i < 1; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapMoon"));

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Moon");

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			//for (_uint i = 0; i < 1; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapGround"));

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Ground");

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			//for (_uint i = 0; i < 1; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_KG_MapSkydome"));

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Skydome");

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}


			//}

			////BT_Map
			//{
			//eLevel = LEVEL_FINALBOSS;
			//ImGui::TableNextColumn();


			//for (_uint i = 0; i < 1; ++i)
			//{
			//	_tchar PrototypeTag[MAX_PATH] = TEXT("");
			//	wsprintf(PrototypeTag, TEXT("Prototype_Component_Model_BT_MapObject"));

			//	char ButtonTag[MAX_PATH] = "";
			//	sprintf_s(ButtonTag, "Area%d", i);

			//	CreateMapCreateButton(eLevel, ButtonTag, PrototypeTag);
			//	ImGui::TableNextColumn();
			//}

			}

			ImGui::EndTable();
		}
        ImGui::EndListBox();
    }

    return S_OK;
}

HRESULT CMapTool::ResetSaveLoadMap()
{
    // �� ����Ʈ�� ������
    list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

    if (ImGui::Button("RESET"))
    {
        // �� ���̾ ���ų� �� ����Ʈ�� �ƹ��͵� ������ �н�
        if (MapList == nullptr || MapList->size() == 0)
        {
            MSGBOX("Map Doesn't Exist");
            return S_OK;
        }

        // m_pSelectedGameObject nullptr
        m_pSelectedGameObject[TAB_MAP] = nullptr;

        // m_pLatestGameObjects nullptr
        for (auto& pLatestMap : m_pLatestGameObjects[TAB_MAP])
            pLatestMap = nullptr;
        m_pLatestGameObjects->clear();

        // ���� ����
        auto iter = (*MapList).begin();
        for (_uint i = 0; i < (*MapList).size(); i++)
        {
            (*iter)->Set_Dead(true);
            ++iter;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("SAVE"))
    {
        // �� ���̾ ���ų� �� ����Ʈ�� �ƹ��͵� ������ �н�
        if (MapList == nullptr || MapList->size() == 0)
        {
            MSGBOX("Map Doesn't Exist");
            return S_OK;
        }

        // �� ���̾ ��ȸ�ϸ鼭 Save
        auto iter = (*MapList).begin();
        HANDLE hFile = CreateFile(TEXT("../SaveFiles/Map/NewMapData.dat"), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        for (_uint i = 0; i < (*MapList).size(); i++)
        {
            (*iter)->Save_ObjectData(hFile);
            ++iter;
        }

        CloseHandle(hFile);
    }

    ImGui::PushItemWidth(200.f);
    static char SaveFileName[128] = "NewMapData.dat";
    ImGui::InputText("FileName.dat", SaveFileName, 128);
    ImGui::PopItemWidth();

    ImGui::SameLine();

    if (ImGui::Button("LOAD"))
    {
        char SaveFilePath[MAX_PATH] = "../SaveFiles/Map/";
        strcat_s(SaveFilePath, SaveFileName);
        _bstr_t wSaveFilePath(SaveFilePath);

        HANDLE hFile = CreateFile(wSaveFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            MSGBOX("Check File Name!");
            return S_OK;
        }

        // �� ���̾ �ְ� �� ����Ʈ�� ���Ұ� ������ ��� ����
        if (MapList != nullptr && MapList->size() != 0)
        {
            // m_pSelectedGameObject nullptr
            m_pSelectedGameObject[TAB_MAP] = nullptr;

            // m_pLatestGameObjects nullptr
            for (auto& pLatestMap : m_pLatestGameObjects[TAB_MAP])
                pLatestMap = nullptr;
            m_pLatestGameObjects->clear();

            // ���� ����
            auto iter = (*MapList).begin();
            for (_uint i = 0; i < (*MapList).size(); i++)
            {
                (*iter)->Set_Dead(true);
                ++iter;
            }
        }

        DWORD dwBytes = 0;
        CMap::MAPDESC MapDesc;
        ZeroMemory(&MapDesc, sizeof(CMap::MAPDESC));
		_float fFrustumRange = 0.f;

        while (true)
        {
			ReadFile(hFile, &MapDesc, sizeof(CMap::MAPDESC), &dwBytes, nullptr);
			ReadFile(hFile, &fFrustumRange, sizeof(_float), &dwBytes, nullptr);

            if (dwBytes == 0)
                break;

            CGameObject* pNewMap = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &MapDesc);

            // ���� ������ ���� pArg �ް�, ���̺굥���� �ε��� ���� pArg �ޱ� ������ Load_ObjectData �� ȣ���ϴ� ��ſ� ��ü�� �����ϰ� ������İ� frustumRange �� ���� ��������
            pNewMap->Get_Transform()->Set_WorldMatrix(MapDesc.WorldMatrix);
			pNewMap->SetFrustumRange(fFrustumRange);
        }

        CloseHandle(hFile);
    }

    return S_OK;
}

HRESULT CMapTool::CreateMapCreateButton(LEVEL eLevel, const char* pButtonTag, const _tchar* pModelTag)
{
    if (m_bContinuous == false)
    {
        // �� ���� ��ư �����
        _bstr_t ModelTag(pButtonTag);

        if (ImGui::Button(ModelTag, ImVec2(-FLT_MIN, 0.f)))
        {
            // �� �纻��ü�� �� �±׶� �� ������Ÿ���� �ִ� ������ �����ϱ� ���� ����ü �غ�
            CMap::MAPDESC MapDesc;
            lstrcpy(MapDesc.ModelTag, pModelTag);
            MapDesc.ModelPrototypeLevel = eLevel;

            // ��ü �����ϰ� m_pLatestGameObject �� ������ ��ü ����
            CGameObject* pGameObejct = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &MapDesc);

            if (nullptr == pGameObejct)
            {
                BREAKPOINT;
                return E_FAIL;
            }

            m_pLatestGameObjects[TAB_MAP].push_back(pGameObejct);

            // PickingPoint �� �ִٸ� �� ��ġ�� ��ü ��ġ�ϰ� PickingPoint ����(+ nullptr �� �ʱ�ȭ)
            if (nullptr != m_pPickingPoint)
            {
                _vector vPickingPointPos = m_pPickingPoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
                pGameObejct->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPickingPointPos);

                m_pPickingPoint->Set_Dead(true);
                m_pPickingPoint = nullptr;
            }

            // ������ ��ü�� �ٷ� ���� ó���ؼ� Transform ������ �� �ֵ��� ��
            m_pSelectedGameObject[TAB_MAP] = pGameObejct;
        }
    }
    // ���Ӽ�ġ
    else
    {
        _bstr_t ModelTag(pButtonTag);
        if (ImGui::Button(ModelTag, ImVec2(-FLT_MIN, 0.f)))
        {
            ZeroMemory(&m_MapDesc, sizeof(CMap::MAPDESC));
            lstrcpy(m_MapDesc.ModelTag, pModelTag);
            m_MapDesc.ModelPrototypeLevel = eLevel;
        }
    }

    return S_OK;
}

HRESULT CMapTool::CreateContinuousMap()
{
    if (m_bContinuous == false)
        return S_OK;

    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
    {
        // ��ü �����ϰ� m_pLatestGameObject �� ������ ��ü ����
        CGameObject* pGameObejct = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &m_MapDesc);

        if (nullptr == pGameObejct)
        {
            BREAKPOINT;
            return E_FAIL;
        }

        m_pLatestGameObjects[TAB_MAP].push_back(pGameObejct);

        // PickingPoint �� �ִٸ� �� ��ġ�� ��ü ��ġ�ϰ� PickingPoint ����(+ nullptr �� �ʱ�ȭ)
        if (nullptr != m_pPickingPoint)
        {
            _vector vPickingPointPos = m_pPickingPoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            pGameObejct->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPickingPointPos);

            m_pPickingPoint->Set_Dead(true);
            m_pPickingPoint = nullptr;
        }

        // ������ ��ü�� �ٷ� ���� ó���ؼ� Transform ������ �� �ֵ��� ��
        m_pSelectedGameObject[TAB_MAP] = pGameObejct;
    }

    return S_OK;
}

void CMapTool::DeleteMap()
{
    if (m_pGameInstance->Get_KeyEnter(DIK_DELETE))
    {
        if (m_pSelectedGameObject[TAB_MAP] == nullptr)
            return;

        m_pSelectedGameObject[TAB_MAP]->Set_Dead(true);
        m_pSelectedGameObject[TAB_MAP] = nullptr;
    }
}

#ifdef _DEBUG
void CMapTool::PickingMap()
{
    // ����Ʈ + ��Ŭ��
    if (m_pGameInstance->Get_KeyPressing(DIK_LSHIFT) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
    {
        // �� ����Ʈ�� ������
        list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

        // �� ���̾ ���ų� �� ����Ʈ�� �ƹ��͵� ������ �н�
        if (MapList == nullptr || MapList->size() == 0)
            return;

        // �� ���̾ �ִٸ� ��ȸ�ϸ鼭 ��ŷ üũ
        auto iter = (*MapList).begin();
        for (_uint i = 0; i < (*MapList).size(); i++)
        {
            CCollider* pMapAABB = dynamic_cast<CCollider*>((*iter)->Get_Component(TEXT("Com_Collider")));

            BoundingBox* MapBoundingBox = pMapAABB->Get_AABB();
            _float fCollisionDist = 0.f;
            if (true == m_pGameInstance->Ray_Box_Collision(*MapBoundingBox, fCollisionDist))
                m_pSelectedGameObject[TAB_MAP] = (*iter);

            ++iter;
        }
    }
}
#endif

HRESULT CMapTool::AutoSave(_double TimeDelta)
{
	m_TimeAccForAutoSave += TimeDelta;

	if (m_TimeAccForAutoSave > 90.0)
	{
		// �� ����Ʈ�� ������
		list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

		// �� ���̾ ���ų� �� ����Ʈ�� �ƹ��͵� ������ �н�
        if (MapList == nullptr || MapList->size() == 0)
        {
            m_TimeAccForAutoSave = 0.0;
			return S_OK;
        }

		// �� ���̾ ��ȸ�ϸ鼭 Save
		auto iter = (*MapList).begin();
		HANDLE hFile = CreateFile(TEXT("../SaveFiles/Map/AutoSaveData.dat"), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		for (_uint i = 0; i < (*MapList).size(); i++)
		{
			(*iter)->Save_ObjectData(hFile);
			++iter;
		}

		CloseHandle(hFile);

		m_TimeAccForAutoSave = 0.0;
	}

	return S_OK;
}

HRESULT CMapTool::ShowCurrentGameObjectList()
{
    // ��ġ�� ���ӿ�����Ʈ ��� �����ְ� ���� ��ư �߰�
    ImGui::Bullet(); ImGui::SameLine();
    m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Current GameObjects");

    if (ImGui::BeginListBox("Current GameObjects", ImVec2(-FLT_MIN, 15 * ImGui::GetTextLineHeightWithSpacing())))
    {
        if (ImGui::BeginTable("Current GameObjects", 2, ImGuiTableFlags_None))
        {
            // 0. ������Ʈ�� �ο��� ��ȣ �غ�
            _uint iObjectNumber = 0;

            // 1. ������Ʈ �Ŵ������� map<const _tchar*, CLayer*> �� ������
            map<const _tchar*, CLayer*> ObjectMap = m_pGameInstance->Get_ObjectMap(LEVEL_MAPTOOL);

            for (auto& Pair : ObjectMap)
            {
                // 2. 1 �� ��ȸ�ϸ鼭 ���̾��±׸� �������� const char* �� ��ȯ
                const _tchar* wLayerTag = Pair.first;
                _bstr_t LayerTag(wLayerTag);

                // (Axis, Camera, Map, PickingPointGuide, PickingPoint �� �н�)
                if (0 == lstrcmp(wLayerTag, TEXT("Layer_Axis")) || 0 == lstrcmp(wLayerTag, TEXT("Layer_Camera")) || 0 == lstrcmp(wLayerTag, TEXT("Layer_Map")) ||
                    0 == lstrcmp(wLayerTag, TEXT("Layer_PickingPointGuide")) || 0 == lstrcmp(wLayerTag, TEXT("Layer_PickingPoint")))
                    continue;

                ImGui::TableNextColumn();

                // 3. 2 �� ���̾��±׺��� Tree ����
                if (ImGui::TreeNode(LayerTag))
                {
                    // 4. �� ���̾��±��� list<GameObject*> �� GameObjectList �� ����
                    CLayer* pLayer = Pair.second;
                    list<CGameObject*> GameObjectList = *(pLayer->Get_Object());

                    // 5. GameObjectList �� ���ҵ��� ���������� �����ϰ� ���� ��ư �����
                    auto iter = GameObjectList.begin();
                    for (_uint i = 0; i < GameObjectList.size(); i++)
                    {
                        char buf[MAX_PATH];
                        sprintf_s(buf, "Object_%d", iObjectNumber);

                        // ��ư�� ������ m_pSelectedGameObject �� ���� �ְ� Transform ������ �� �ְ���
                        if (ImGui::SmallButton(buf))
                        {
                            m_pSelectedGameObject[TAB_GAMEOBJECT] = *iter;
                        }

                        if (m_pSelectedGameObject[TAB_GAMEOBJECT] == *iter)
                        {
                            ImGui::SameLine();
                            m_pImGui_Manager->ColoredText(_float3(1.f, 0.5f, 1.f), "CLICKED");
                            (*iter)->Set_Pass(8);
                        }
                        else
                            (*iter)->Set_Pass(0);

                        ++iObjectNumber;
                        ++iter;
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndListBox();
    }

    return S_OK;
}

HRESULT CMapTool::CreateGameObjectList()
{
    ImGui::Bullet(); ImGui::SameLine();
    m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Create GameObject");
    if (ImGui::BeginListBox("Create GameObject", ImVec2(-FLT_MIN, 8.5f * ImGui::GetTextLineHeightWithSpacing())))
    {
        if (ImGui::BeginTable("CreateObj 1 / CreateObj 2", 2, ImGuiTableFlags_None))
        {
            // (1��)
            ImGui::TableNextColumn();
            CreateObjectCreateButton("Create Player", TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player")); // �÷��̾� ����

            // (2��)
            ImGui::TableNextColumn();
            CreateObjectCreateButton("Create TestMonster", TEXT("Layer_TestMonster"), TEXT("Prototype_GameObject_TestMonster")); // �׽�Ʈ�� ���� ����

            ImGui::EndTable();
        }
        ImGui::EndListBox();
    }

    return S_OK;
}

HRESULT CMapTool::CreateObjectCreateButton(const char* pButtonTag, const _tchar* pLayerTag, const _tchar* pPrototypeTag)
{
    // ���ӿ�����Ʈ ���� ��ư �����
    if (ImGui::Button(pButtonTag, ImVec2(-FLT_MIN, 0.f)))
    {
        // ��ü �����ϰ� m_pLatestGameObject �� ������ ��ü ����
        CGameObject* pGameObejct = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, pLayerTag, pPrototypeTag);

        if (nullptr == pGameObejct)
        {
            MSGBOX("Clone Failed :: CMapTool::CreateObjectCreateButton")
                return E_FAIL;
        }

        m_pLatestGameObjects[TAB_GAMEOBJECT].push_back(pGameObejct);

        // PickingPoint �� �ִٸ� �� ��ġ�� ��ü ��ġ�ϰ� PickingPoint ����(+ nullptr �� �ʱ�ȭ)
        if (nullptr != m_pPickingPoint)
        {
            _vector vPickingPointPos = m_pPickingPoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            pGameObejct->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPickingPointPos);

            m_pPickingPoint->Set_Dead(true);
            m_pPickingPoint = nullptr;
        }

        // ������ ��ü�� �ٷ� ���� ó���ؼ� Transform ������ �� �ֵ��� ��
        m_pSelectedGameObject[TAB_GAMEOBJECT] = pGameObejct;
    }

    return S_OK;
}

void CMapTool::SetGameObjectTransform(TAB eTab)
{
    // ���õ� ���ӿ�����Ʈ�� ���ٸ� ȸ������ ǥ���� �����ο� �ڵ�
    if (nullptr == m_pSelectedGameObject[eTab])
    {
        ImGui::Bullet(); ImGui::SameLine();
        m_pImGui_Manager->ColoredText(_float3(1.f, 0.5f, 1.f), "GameObjects is not selected");

        ImGui::BeginDisabled(true);

        _float fTemp[3] = { 0.f, 0.f, 0.f };
        ImGui::DragFloat3("Position", fTemp, 0.1f, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragFloat3("Scale", fTemp, 0.1f, 1.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragFloat("RotationX", &fTemp[0], 0.1f, 0.f, 1.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragFloat("RotationY", &fTemp[0], 0.1f, 0.f, 1.f, "%.f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragFloat("RotationZ", &fTemp[0], 0.1f, 0.f, 1.f, "%.f", ImGuiSliderFlags_AlwaysClamp);		

        ImGui::EndDisabled();
    }
    // ���õ� ���ӿ�����Ʈ�� �ִٸ� Position, Scale, Rotation �� ����
    else
    {
        ImGui::Bullet(); ImGui::SameLine();
        m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Transform GameObject");

        m_pImGui_Manager->SetGameObjectPosition(string("Position"), m_pSelectedGameObject[eTab]->Get_Transform()); // ��ġ
        m_pImGui_Manager->SetGameObjectScale(string("Scale"), m_pSelectedGameObject[eTab]->Get_Transform());       // ũ��
        m_pImGui_Manager->SetGameObjectRotation(string("Rotation"), m_pSelectedGameObject[eTab]->Get_Transform()); // ȸ��

		_float fFrustumRange = m_pSelectedGameObject[eTab]->GetFrustumRange();
		if (ImGui::DragFloat("Frustum", &fFrustumRange, 1.f, 20.f, 1000.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
			m_pSelectedGameObject[eTab]->SetFrustumRange(fFrustumRange);
    }
}

void CMapTool::GuidePickingPointUpdate()
{
    // ���̵���ŷ����Ʈ�� ���콺 ����ٴϰ� (y ��ġ�� m_fPickintPointY ����)
    dynamic_cast<CPickingPoint*>(m_pGuidePoint)->FollowMouse(m_fPickintPointY);
}

HRESULT CMapTool::CreatePickingPoint()
{
    // ���콺 ��ŷ üũ (Ctrl + LB)
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
    {
        // y ���� ������ ������ ���� ���콺 ��ŷ������ ��Ҵ��� üũ
        pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_fPickintPointY);

        if (TRUE == CollisionCheck.first)
        {
            // ��ŷ�� ����� �ƴٸ� PickingPoint ���� ��ġ�� ����
            _vector vPickingCreatePosition = XMVectorSetY(CollisionCheck.second, m_fPickintPointY);

            // �̹� ������ PickingPoint �� �ִٸ� ����
            if (nullptr != m_pPickingPoint)
                m_pPickingPoint->Set_Dead(true);

            // PickingPoint ����
            m_pPickingPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_PickingPoint"), TEXT("Prototype_GameObject_PickingPoint"), &vPickingCreatePosition);
        }
    }

    return S_OK;
}

void CMapTool::SetPickingPointY()
{
    // Ctrl + E �� m_fPickintPointY �ø���
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_E))
        m_fPickintPointY += 0.1f;

    // Ctrl + Q �� m_fPickintPointY ������
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_Q))
        m_fPickintPointY -= 0.1f;

    // Ctrl + W �� m_fPickintPointY 0 ���� �ʱ�ȭ
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_W))
        m_fPickintPointY = 0;
}

HRESULT CMapTool::MoveToGuidePickingPoint(TAB eTab)
{
    if (m_pGameInstance->Get_KeyPressing(DIK_SPACE))
    {
        // ���õ� ��ü�� ���ٸ� �н�
        if (nullptr == m_pSelectedGameObject[eTab])
            return S_OK;

        _vector vGuidePickingPointPos = m_pGuidePoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        m_pSelectedGameObject[eTab]->Get_Transform()->Set_State(CTransform::STATE_POSITION, vGuidePickingPointPos);
    }

    return S_OK;
}

HRESULT CMapTool::DeleteLatestGameObject(TAB eTab)
{
    // Ctrl + Z ���� ���� �ֱٿ� ������ ��ü ����
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_Z))
    {
        // m_pLatestGameObjects �� �ƹ��͵� ���ٸ� �н�
        if (m_pLatestGameObjects[eTab].size() == 0)
            return S_OK;

        // nullptr �̸� �н�
        if (m_pLatestGameObjects[eTab].back() == nullptr)
            return S_OK;

        // m_pLatestGameObjects �� ������ ���Ұ� m_pSelectedGameObject �� ���ٸ� m_pSelectedGameObject �� nullptr �� �ʱ�ȭ
        if (m_pSelectedGameObject[eTab] == m_pLatestGameObjects[eTab].back())
            m_pSelectedGameObject[eTab] = nullptr;

        // m_pLatestGameObjects �� ������ ���� Set_Dead, nullptr �ϰ� �����̳ʿ��� ��������
        m_pLatestGameObjects[eTab].back()->Set_Dead(true);
        m_pLatestGameObjects[eTab].back() = nullptr;
        m_pLatestGameObjects[eTab].pop_back();
    }

    return S_OK;
}

void CMapTool::NaviMesh_Make()
{

	// ���� ���� �� ���ִ¸��
	ImGui::Checkbox("MakeCell_Mode", &m_bMakeCell_Mode);
	if (true == m_bMakeCell_Mode)
	{
		m_bTypeSetting_Mode = false;
		m_bTypeChange_Mode = false;
		// �׽�Ʈ ����Ʈ ����
		if (false == m_bMakeTestPoint)
		{
			for (_uint i = 0; i < 3; i++)
			{
				if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_TestPoint"), TEXT("Prototype_GameObject_CellPoint"), &m_Points[i])))
				{
					BREAKPOINT;
					return;
				}
			}
			m_bMakeTestPoint = true;
			m_bFirstSetting = false;
		}


		// ��Ŭ������ ����Ʈ �ε��� ��ȯ����
		if (true == m_pGameInstance->Get_KeyEnter(DIK_C))
		{
			m_iPointIndex++;
			if (2 < m_iPointIndex)
				m_iPointIndex = 0;
		}
		// ����Ʈ �ε��� ������
		ImGui::Text("PointIndex : %d", m_iPointIndex);

		// R + ��Ŭ������ �ͷ����̳� ���콺 ��ŷ���� ����Ʈ�� ��ġ Ư������
		if ((m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80) && true == m_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
		{
			// ��ŷ�� ���� �־���
			pair<_bool, _vector> MouseInfo = m_pGameInstance->Check_Ray_VirtualPlane_Collision(0.f);
			if (true == MouseInfo.first)
			{
				m_Points[m_iPointIndex][0] = XMVectorGetX(MouseInfo.second);
				m_Points[m_iPointIndex][1] = XMVectorGetY(MouseInfo.second);
				m_Points[m_iPointIndex][2] = XMVectorGetZ(MouseInfo.second);
			}

			// �� ���� ���鿡 �ִ� �ݶ��̴���
			CPicking* pPicking = GET_INSTANCE(CPicking);

			_vector Dir = pPicking->Get_RayDir();
			_vector Pos = pPicking->Get_RayPos();

			list<CCollider*>* ColliderList = m_pGameInstance->Get_ColliderList(string("CellPoint"));
			if (nullptr != ColliderList)
				for (auto& pCollider : *ColliderList)
				{
					_float Dist = 0.f;

					BoundingBox* ColliderBox = pCollider->Get_AABB();
					
					if (true == m_pGameInstance->Ray_Box_Collision(*ColliderBox, Dist))
					{
						m_Points[m_iPointIndex][0] = ColliderBox->Center.x;
						m_Points[m_iPointIndex][1] = ColliderBox->Center.y;
						m_Points[m_iPointIndex][2] = ColliderBox->Center.z;
					}
				}

			 

			m_iPointIndex++;
			if (2 < m_iPointIndex)
				m_iPointIndex = 0;
		}


		// �巡�׷ε� ����Ʈ �̵� ����
		ImGui::DragFloat3("Points1", m_Points[0], 0.01f, -500.f, 500.f);
		ImGui::DragFloat3("Points2", m_Points[1], 0.01f, -500.f, 500.f);
		ImGui::DragFloat3("Points3", m_Points[2], 0.01f, -500.f, 500.f);


		// �̰� ������ ���� ���� �� �ִ�.
		if (ImGui::Button("CreateCell"))
		{
			_float3	Points[3] = {
				_float3(m_Points[0][0], m_Points[0][1], m_Points[0][2]),
				_float3(m_Points[1][0], m_Points[1][1], m_Points[1][2]),
				_float3(m_Points[2][0], m_Points[2][1], m_Points[2][2]),
			};

			// �ð� ���� ����
			if (Points[0].x > Points[1].x)
			{
				_float3 Temp = Points[0];
				Points[0] = Points[1];
				Points[1] = Temp;
			}
			if (Points[0].x > Points[2].x)
			{
				_float3 Temp = Points[0];
				Points[0] = Points[2];
				Points[2] = Temp;
			}

			_float B = -((Points[1].z - Points[0].z) / (Points[1].x - Points[0].x) * Points[0].x - Points[0].z);
			_float Z = (Points[1].z - Points[0].z) / (Points[1].x - Points[0].x) * Points[2].x + B;
			if (Z < Points[2].z)
			{
				_float3 Temp = Points[1];
				Points[1] = Points[2];
				Points[2] = Temp;
			}


			CCell*	pCell = CCell::Create(m_pDevice, m_pDeviceContext, Points, (_uint)m_Cells.size());
			m_Cells.push_back(pCell);

			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_A))))
			{
				BREAKPOINT;
				return;
			}
			pCell->Set_CellPoint(CCell::POINT_A, m_pGameInstance->Get_Back(LEVEL_MAPTOOL, TEXT("Layer_Point")));

			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_B))))
			{
				BREAKPOINT;
				return;
			}
			pCell->Set_CellPoint(CCell::POINT_B, m_pGameInstance->Get_Back(LEVEL_MAPTOOL, TEXT("Layer_Point")));

			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_C))))
			{
				BREAKPOINT;
				return;
			}
			pCell->Set_CellPoint(CCell::POINT_C, m_pGameInstance->Get_Back(LEVEL_MAPTOOL, TEXT("Layer_Point")));

			ZeroMemory(m_Points, sizeof(_float) * 9);

			m_iCurrentCellIndex = (_uint)m_Cells.size() - 1;
		}
	}


	// �̹� ������ ���� ������ �� �ִ� ���

	ImGui::Checkbox("Fix_Mode", &m_bTypeSetting_Mode);
	if (true == m_bTypeSetting_Mode)
	{
		m_bTypeChange_Mode = false;
		if (true == m_bMakeCell_Mode)
		{
			if (FAILED(m_pGameInstance->Release_LayerObject(LEVEL_MAPTOOL, TEXT("Layer_TestPoint"))))
			{
				BREAKPOINT;
				return;
			}
			m_bFirstSetting = true;
			m_bMakeCell_Mode = false;
			m_bMakeTestPoint = false;
		}

		if (0 <= m_iCurrentCellIndex)
		{
			_float3* vPointA = m_Cells[m_iCurrentCellIndex]->Get_Points(CCell::POINT_A);
			_float3* vPointB = m_Cells[m_iCurrentCellIndex]->Get_Points(CCell::POINT_B);
			_float3* vPointC = m_Cells[m_iCurrentCellIndex]->Get_Points(CCell::POINT_C);
			_float	fPoint_A[3] = { vPointA->x, vPointA->y, vPointA->z };
			_float	fPoint_B[3] = { vPointB->x, vPointB->y, vPointB->z };
			_float	fPoint_C[3] = { vPointC->x, vPointC->y, vPointC->z };

			ImGui::DragFloat3("Point_A", fPoint_A, 0.01f, -500.f, 500.f);
			ImGui::SameLine();
			ImGui::Checkbox("A", &m_bPointA);
			if (true == m_bPointA)
			{
				m_bPointB = false;
				m_bPointC = false;
			}

			ImGui::DragFloat3("Point_B", fPoint_B, 0.01f, -500.f, 500.f);
			ImGui::SameLine();
			ImGui::Checkbox("B", &m_bPointB);
			if (true == m_bPointB)
			{
				m_bPointA = false;
				m_bPointC = false;
			}

			ImGui::DragFloat3("Point_C", fPoint_C, 0.01f, -500.f, 500.f);
			ImGui::SameLine();
			ImGui::Checkbox("C", &m_bPointC);
			if (true == m_bPointC)
			{
				m_bPointB = false;
				m_bPointA = false;
			}

			// R + ��Ŭ������ �ͷ����̳� ���콺 ��ŷ���� ����Ʈ�� ��ġ Ư������
			if ((m_pGameInstance->Get_KeyPressing(DIK_LCONTROL)) && true == m_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
			{
				_float3 vMousePos = _float3(0.f, 0.f, 0.f);

				CPicking* pPicking = GET_INSTANCE(CPicking);

				_vector Dir = pPicking->Get_RayDir();
				_vector Pos = pPicking->Get_RayPos();


				// üũ�� �ݶ��̴��� �ٽ� ��ŷ�� �ݶ��̴� ��ġ�� �� �� �ְ� ��

				list<CCollider*>* ColliderList = m_pGameInstance->Get_ColliderList(string("CellPoint"));
				if (nullptr != ColliderList)
					for (auto& pCollider : *ColliderList)
					{
						_float Dist = 0.f;

						BoundingBox* ColliderBox = pCollider->Get_AABB();

						if (true == m_pGameInstance->Collision_RayToAABB(Pos, Dir, pCollider, &Dist))
						{
							if (true == m_bPointA)
							{
								fPoint_A[0] = ColliderBox->Center.x;
								fPoint_A[1] = ColliderBox->Center.y;
								fPoint_A[2] = ColliderBox->Center.z;
							}
							else if (true == m_bPointB)
							{
								fPoint_B[0] = ColliderBox->Center.x;
								fPoint_B[1] = ColliderBox->Center.y;
								fPoint_B[2] = ColliderBox->Center.z;
							}
							else if (true == m_bPointC)
							{
								fPoint_C[0] = ColliderBox->Center.x;
								fPoint_C[1] = ColliderBox->Center.y;
								fPoint_C[2] = ColliderBox->Center.z;
							}
						}
					}

				 
			}

			vPointA->x = fPoint_A[0];
			vPointA->y = fPoint_A[1];
			vPointA->z = fPoint_A[2];
			vPointB->x = fPoint_B[0];
			vPointB->y = fPoint_B[1];
			vPointB->z = fPoint_B[2];
			vPointC->x = fPoint_C[0];
			vPointC->y = fPoint_C[1];
			vPointC->z = fPoint_C[2];
		}

	}

	// ���� �ɼ� �� ����
	ImGui::Checkbox("TypeChange_Mode", &m_bTypeChange_Mode);
	if (true == m_bTypeChange_Mode)
	{
		if (true == m_bMakeCell_Mode)
		{
			if (FAILED(m_pGameInstance->Release_LayerObject(LEVEL_MAPTOOL, TEXT("Layer_TestPoint"))))
			{
				BREAKPOINT;
				return;
			}
			m_bFirstSetting = true;
			m_bMakeCell_Mode = false;
			m_bMakeTestPoint = false;
		}
		m_bTypeSetting_Mode = false;

		char szType[64] = "";
		switch (m_iCellType)
		{
		case 0:
			strcpy_s(szType, "TYPE_TERRAIN");
			break;
		case 1:
			strcpy_s(szType, "TYPE_ROAD");
			break;
		case 2:
			strcpy_s(szType, "TYPE_NOTROAD");
			break;
		case 3:
			strcpy_s(szType, "TYPE_RUNNING");
			break;
		case 4:
			strcpy_s(szType, "TYPE_FLOW");
			break;
		}


		if (ImGui::Button("Change Type"))
		{
			m_iCellType++;
			if (4 < m_iCellType)
				m_iCellType = 0;
		}
		ImGui::SameLine();
		ImGui::Text(szType);

		CPicking* pPicking = GET_INSTANCE(CPicking);
		if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_DIMouseButtonState(CInput_Device::MBS_LBUTTON))
		{
			for (auto& pCell : m_Cells)
			{
				_vector CellPoint[3] = { pCell->Get_Point(CCell::POINT_A), pCell->Get_Point(CCell::POINT_B), pCell->Get_Point(CCell::POINT_C) };

				if (true == pPicking->IsNaviPick(CellPoint))
				{
					pCell->Set_Type(CCell::TYPE(m_iCellType));
					break;
				}
			}
		}		 
	}

	ImGui::InputText("SaveFileName", m_szSaveNaviDataFileName, MAX_PATH);
	if (ImGui::Button("Save_Navi"))
	{
		Save_NaviMesh();
	}
}

_int CMapTool::NaviMesh_List()
{
	if (0 > m_iCurrentCellIndex)
		return 0;

	ImGui::BeginChild("top pane", ImVec2(250.f, 150.f), true);
	ImGui::Text("Cell List");
	ImGui::BeginChild("top in pane", ImVec2(235.f, 90.f), true);

	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		char label[128];
		sprintf_s(label, "Cell%d", i);

		if (ImGui::Selectable(label, i == m_iCurrentCellIndex))
		{
			for (_uint i = 0; i < 3; i++)
				static_cast<CCellPoint*>(m_Cells[m_iCurrentCellIndex]->Get_CellPoints()[i])->IsPickCell(false);

			m_iCurrentCellIndex = i;
			for (_uint i = 0; i < 3; i++)
				static_cast<CCellPoint*>(m_Cells[m_iCurrentCellIndex]->Get_CellPoints()[i])->IsPickCell(true);
			break;
		}


	}
	ImGui::EndChild();

	// �� ���ֱ�
	if (ImGui::Button("Cell_Delete") && 0 < m_Cells.size())
	{
		auto iter = m_Cells.begin();

		for (_int i = 0; i < m_iCurrentCellIndex; i++)
			iter++;

		m_Cells[m_iCurrentCellIndex]->CellPointsDead();
		Safe_Release(m_Cells[m_iCurrentCellIndex]);
		m_Cells.erase(iter);
		m_iCurrentCellIndex = (_uint)m_Cells.size() - 1;
	}

	ImGui::EndChild();

	if (ImGui::Button("NaviRender"))
	{
		m_bNaviMeshRender = !m_bNaviMeshRender;
	}

	return _int();
}

_int CMapTool::NaviMesh_Load_List()
{
	ImGui::Text("Load List");
	ImGui::BeginChild("LoadList", ImVec2(150.f, 300.f), true);

	// ���� ��� �ҷ�����
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../NaviData/*.*", &fd);
	if (handle == -1 || handle == 0)
	{
		ImGui::EndChild();
		return -1;
	}

	static _int iSelect = 0;
	int iResult = 0;
	int i = 0;
	char szFilename[64];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		if (!strcmp(szFilename, ".") || !strcmp(szFilename, ".."))
		{
			iResult = _tfindnext64(handle, &fd);
			continue;
		}

		sprintf_s(szFilename, szFilename);


		if (ImGui::Selectable(szFilename, iSelect == i))
		{
			iSelect = i;
			strcpy_s(m_szNaviDataFilePath, "../NaviData/");
			strcat_s(m_szNaviDataFilePath, szFilename);
		}
		i++;

		iResult = _tfindnext64(handle, &fd);
	}

	_findclose(handle);

	ImGui::EndChild();

	return iSelect;
}

HRESULT CMapTool::NaviMesh_Render()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pNaviShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
 		return E_FAIL;
	}
	if (FAILED(m_pNaviShader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	if (FAILED(m_pNaviShader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == m_bNaviMeshRender)
		for (auto& pCell : m_Cells)
		{
			pCell->Render(m_pNaviVIBuffer, m_pNaviShader, _float4(0.f, 1.f, 0.f, 1.f));
		}

	return S_OK;
}

void CMapTool::Save_NaviMesh()
{
	char szPath[MAX_PATH] = "../NaviData/";
	strcat_s(szPath, m_szSaveNaviDataFileName);
	strcat_s(szPath, ".dat");
	_tchar			wszStr[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, szPath, IM_ARRAYSIZE(szPath), wszStr, IM_ARRAYSIZE(wszStr));

	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(wszStr, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;

	_float3			vPoints[3];
	_uint			iType = 0;
	for (auto& pCell : m_Cells)
	{
		memcpy(vPoints, pCell->Get_Points(CCell::POINT_A), sizeof(_float3) * 3);
		WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		iType = pCell->Get_Type();
		WriteFile(hFile, &iType, sizeof(_uint), &dwByte, nullptr);
	}

	if (0 == dwByte)
	{
		MSGBOX("Failed Save");
		return;
	}

	MSGBOX("Success Save");

	CloseHandle(hFile);
}

void CMapTool::Load_NaviMesh()
{
	_ulong			dwByte = 0;
	_tchar			wszStr[MAX_PATH] = L"";
	MultiByteToWideChar(CP_ACP, 0, m_szNaviDataFilePath, IM_ARRAYSIZE(m_szNaviDataFilePath), wszStr, IM_ARRAYSIZE(wszStr));
	HANDLE			hFile = CreateFile(wszStr, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;

	_float3			vPoints[3];
	_uint			iType = 0;
	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		ReadFile(hFile, &iType, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, (_uint)m_Cells.size());
		if (nullptr == pCell)
			return;
		pCell->Set_Type((CCell::TYPE)iType);

		m_Cells.push_back(pCell);

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_A))))
		{
			BREAKPOINT;
			return;
		}
		pCell->Set_CellPoint(CCell::POINT_A, m_pGameInstance->Get_Back(LEVEL_MAPTOOL, TEXT("Layer_Point")));

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_B))))
		{
			BREAKPOINT;
			return;
		}
		pCell->Set_CellPoint(CCell::POINT_B, m_pGameInstance->Get_Back(LEVEL_MAPTOOL, TEXT("Layer_Point")));

		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_MAPTOOL, TEXT("Layer_Point"), TEXT("Prototype_GameObject_CellPoint"), pCell->Get_Points(CCell::POINT_C))))
		{
			BREAKPOINT;
			return;
		}
		pCell->Set_CellPoint(CCell::POINT_C, m_pGameInstance->Get_Back(LEVEL_MAPTOOL, TEXT("Layer_Point")));

		m_iCurrentCellIndex = (_uint)m_Cells.size() - 1;
	}

	CloseHandle(hFile);
}
#endif

void CMapTool::Free()
{
    // ������� Free

#ifdef _DEBUG
	Safe_Release(m_pNaviShader);
	Safe_Release(m_pNaviVIBuffer);
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();

    if (nullptr != m_pDevice)
        Safe_Release(m_pDevice);
    if (nullptr != m_pDeviceContext)
        Safe_Release(m_pDeviceContext);
    if (nullptr != m_pGameInstance)
        Safe_Release(m_pGameInstance);
    if (nullptr != m_pImGui_Manager)
        Safe_Release(m_pImGui_Manager);
#endif

    /* ���������� ���� ���۷���ī��Ʈ�� �������� �ʱ� ������ ������ ��ü�� Safe_Release ���� �ʾƵ� �� */
}
