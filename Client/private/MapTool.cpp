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
    // 멤버변수 초기화
    m_pGameInstance  = CGameInstance::GetInstance();
    m_pDevice        = m_pGameInstance->Get_Device();
    m_pDeviceContext = m_pGameInstance->Get_Device_Context();
    m_pImGui_Manager = CImGui_Manager::GetInstance();
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
    Safe_AddRef(m_pImGui_Manager);

    // m_pGuidePoint 에 가이드용 피킹포인트 저장 
    m_pGuidePoint = m_pGameInstance->Get_GameObject(LEVEL_MAPTOOL, TEXT("Layer_GuidePoint"), 0);

    if (nullptr == m_pGameInstance || nullptr == m_pDevice || nullptr == m_pDeviceContext || nullptr == m_pImGui_Manager)
    {
        MSGBOX("ERROR :: CMapTool::MapTool_SetUp");
        return E_FAIL;
    }


	// 네비매쉬 랜더에 쓰일 컴포넌트들 초기화
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
    // ImGui_Manager 의 show_maptool_window 이 켜져있을 때만 MapTool Window 를 보여줌
    if (true == CImGui_Manager::GetInstance()->show_maptool_window)
    {
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("MapTool Window");

        if (ImGui::BeginTabBar("Select Type", ImGuiTabBarFlags_None))
        {
            // 맵 객체 생성 및 수정
            if (ImGui::BeginTabItem("Map"))
            {
                // 피킹 처리
                GuidePickingPointUpdate();  // 가이드피킹포인트 위치 업데이트
                CreatePickingPoint();       // PickingPoint 생성
                SetPickingPointY();         // m_fPickingPointY 값 설정                

                // 맵 처리
                SetGameObjectTransform(TAB_MAP);    // 선택된 게임오브젝트의 Transform 값을 변경
                m_pImGui_Manager->SmallSeparator();
                ShowCurrentMapList();               // 현재 설치된 맵 목록을 보여줌
                m_pImGui_Manager->SmallSeparator();
                CreateMapList();                    // 설치할 맵 모델 목록을 보여줌
                CreateContinuousMap();
                DeleteLatestGameObject(TAB_MAP);    // 가장 최근에 생성한 맵 객체 제거
                MoveToGuidePickingPoint(TAB_MAP);   // 선택된 맵 객체를 가이드피킹포인트 위치로
                m_pImGui_Manager->SmallSeparator();
                ResetSaveLoadMap();                 // 리셋, 세이브, 로드
				AutoSave(TimeDelta);
#ifdef _DEBUG
                //PickingMap();   // 맵 콜라이더랑 마우스레이 체크해서 m_pSelectedGameObject 설정
#endif
                DeleteMap();    // 맵 삭제

                ImGui::EndTabItem();
            }

            // 일반 게임오브젝트 생성 및 수정
            if (ImGui::BeginTabItem("GameObjects"))
            {
                // 피킹 처리
                GuidePickingPointUpdate();  // 가이드피킹포인트 위치 업데이트
                CreatePickingPoint();       // PickingPoint 생성
                SetPickingPointY();         // m_fPickingPointY 값 설정

               // 게임오브젝트 처리
                SetGameObjectTransform(TAB_GAMEOBJECT);     // 선택된 게임오브젝트의 Transform 값을 변경
                m_pImGui_Manager->SmallSeparator();
                ShowCurrentGameObjectList();                // 현재 설치된 게임오브젝트 목록을 보여줌
                m_pImGui_Manager->SmallSeparator();
                CreateGameObjectList();                     // 설치할 게임오브젝트 목록을 보여줌
                DeleteLatestGameObject(TAB_GAMEOBJECT);     // 가장 최근에 생성한 객체 제거
                MoveToGuidePickingPoint(TAB_GAMEOBJECT);    // 선택된 객체를 가이드피킹포인트 위치로

                ImGui::EndTabItem();
            }

            // 네비메쉬 생성 및 수정
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
    // 설치된 맵 리스트 목록을 보여주고 선택
    ImGui::Bullet(); ImGui::SameLine();
    m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Current Map List");

    if (ImGui::BeginListBox("Current Map List", ImVec2(-FLT_MIN, 5.f * ImGui::GetTextLineHeightWithSpacing())))
    {
        if (ImGui::BeginTable("Current Map Table", 5, ImGuiTableFlags_None))
        {
            // 0. 오브젝트에 부여할 번호 준비
            _uint iObjectNumber = 0;

            // 1. 맵 리스트를 가져옴
            list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

            if (nullptr == MapList)
            {
                ImGui::EndTable();
                ImGui::EndListBox();
                return S_OK;
            }

            // 2. MapList 의 원소들을 순차적으로 나열하고 선택 버튼 만들기
            auto iter = (*MapList).begin();
            for (_uint i = 0; i < (*MapList).size(); i++)
            {
                ImGui::TableNextColumn();

                char buf[128] = "";
                sprintf_s(buf, "Map_%d", iObjectNumber);

                // 버튼을 누르면 m_pSelectedGameObject 에 값을 넣고 Transform 설정할 수 있게함 / 쉐이더패스 변경
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
    // 맵 설치
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
    // 맵 리스트를 가져옴
    list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

    if (ImGui::Button("RESET"))
    {
        // 맵 레이어가 없거나 맵 리스트에 아무것도 없으면 패스
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

        // 전부 제거
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
        // 맵 레이어가 없거나 맵 리스트에 아무것도 없으면 패스
        if (MapList == nullptr || MapList->size() == 0)
        {
            MSGBOX("Map Doesn't Exist");
            return S_OK;
        }

        // 맵 레이어를 순회하면서 Save
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

        // 맵 레이어가 있고 맵 리스트에 원소가 있으면 모두 제거
        if (MapList != nullptr && MapList->size() != 0)
        {
            // m_pSelectedGameObject nullptr
            m_pSelectedGameObject[TAB_MAP] = nullptr;

            // m_pLatestGameObjects nullptr
            for (auto& pLatestMap : m_pLatestGameObjects[TAB_MAP])
                pLatestMap = nullptr;
            m_pLatestGameObjects->clear();

            // 전부 제거
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

            // 맵은 생성할 때도 pArg 받고, 세이브데이터 로드할 때도 pArg 받기 때문에 Load_ObjectData 를 호출하는 대신에 객체를 생성하고서 월드행렬과 frustumRange 를 따로 세팅해줌
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
        // 맵 생성 버튼 만들기
        _bstr_t ModelTag(pButtonTag);

        if (ImGui::Button(ModelTag, ImVec2(-FLT_MIN, 0.f)))
        {
            // 맵 사본객체에 모델 태그랑 모델 프로토타입이 있는 레벨을 저장하기 위해 구조체 준비
            CMap::MAPDESC MapDesc;
            lstrcpy(MapDesc.ModelTag, pModelTag);
            MapDesc.ModelPrototypeLevel = eLevel;

            // 객체 생성하고 m_pLatestGameObject 에 생성한 객체 저장
            CGameObject* pGameObejct = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &MapDesc);

            if (nullptr == pGameObejct)
            {
                BREAKPOINT;
                return E_FAIL;
            }

            m_pLatestGameObjects[TAB_MAP].push_back(pGameObejct);

            // PickingPoint 가 있다면 그 위치에 객체 설치하고 PickingPoint 제거(+ nullptr 로 초기화)
            if (nullptr != m_pPickingPoint)
            {
                _vector vPickingPointPos = m_pPickingPoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
                pGameObejct->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPickingPointPos);

                m_pPickingPoint->Set_Dead(true);
                m_pPickingPoint = nullptr;
            }

            // 생성된 객체는 바로 선택 처리해서 Transform 수정할 수 있도록 함
            m_pSelectedGameObject[TAB_MAP] = pGameObejct;
        }
    }
    // 연속설치
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
        // 객체 생성하고 m_pLatestGameObject 에 생성한 객체 저장
        CGameObject* pGameObejct = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &m_MapDesc);

        if (nullptr == pGameObejct)
        {
            BREAKPOINT;
            return E_FAIL;
        }

        m_pLatestGameObjects[TAB_MAP].push_back(pGameObejct);

        // PickingPoint 가 있다면 그 위치에 객체 설치하고 PickingPoint 제거(+ nullptr 로 초기화)
        if (nullptr != m_pPickingPoint)
        {
            _vector vPickingPointPos = m_pPickingPoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            pGameObejct->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPickingPointPos);

            m_pPickingPoint->Set_Dead(true);
            m_pPickingPoint = nullptr;
        }

        // 생성된 객체는 바로 선택 처리해서 Transform 수정할 수 있도록 함
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
    // 쉬프트 + 좌클릭
    if (m_pGameInstance->Get_KeyPressing(DIK_LSHIFT) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
    {
        // 맵 리스트를 가져옴
        list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

        // 맵 레이어가 없거나 맵 리스트에 아무것도 없으면 패스
        if (MapList == nullptr || MapList->size() == 0)
            return;

        // 맵 레이어가 있다면 순회하면서 피킹 체크
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
		// 맵 리스트를 가져옴
		list<CGameObject*>* MapList = m_pGameInstance->Get_ObjectList(LEVEL_MAPTOOL, TEXT("Layer_Map"));

		// 맵 레이어가 없거나 맵 리스트에 아무것도 없으면 패스
        if (MapList == nullptr || MapList->size() == 0)
        {
            m_TimeAccForAutoSave = 0.0;
			return S_OK;
        }

		// 맵 레이어를 순회하면서 Save
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
    // 설치된 게임오브젝트 목록 보여주고 선택 버튼 추가
    ImGui::Bullet(); ImGui::SameLine();
    m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Current GameObjects");

    if (ImGui::BeginListBox("Current GameObjects", ImVec2(-FLT_MIN, 15 * ImGui::GetTextLineHeightWithSpacing())))
    {
        if (ImGui::BeginTable("Current GameObjects", 2, ImGuiTableFlags_None))
        {
            // 0. 오브젝트에 부여할 번호 준비
            _uint iObjectNumber = 0;

            // 1. 오브젝트 매니저에서 map<const _tchar*, CLayer*> 를 가져옴
            map<const _tchar*, CLayer*> ObjectMap = m_pGameInstance->Get_ObjectMap(LEVEL_MAPTOOL);

            for (auto& Pair : ObjectMap)
            {
                // 2. 1 을 순회하면서 레이어태그를 가져오고 const char* 로 변환
                const _tchar* wLayerTag = Pair.first;
                _bstr_t LayerTag(wLayerTag);

                // (Axis, Camera, Map, PickingPointGuide, PickingPoint 는 패스)
                if (0 == lstrcmp(wLayerTag, TEXT("Layer_Axis")) || 0 == lstrcmp(wLayerTag, TEXT("Layer_Camera")) || 0 == lstrcmp(wLayerTag, TEXT("Layer_Map")) ||
                    0 == lstrcmp(wLayerTag, TEXT("Layer_PickingPointGuide")) || 0 == lstrcmp(wLayerTag, TEXT("Layer_PickingPoint")))
                    continue;

                ImGui::TableNextColumn();

                // 3. 2 의 레이어태그별로 Tree 생성
                if (ImGui::TreeNode(LayerTag))
                {
                    // 4. 각 레이어태그의 list<GameObject*> 를 GameObjectList 에 저장
                    CLayer* pLayer = Pair.second;
                    list<CGameObject*> GameObjectList = *(pLayer->Get_Object());

                    // 5. GameObjectList 의 원소들을 순차적으로 나열하고 선택 버튼 만들기
                    auto iter = GameObjectList.begin();
                    for (_uint i = 0; i < GameObjectList.size(); i++)
                    {
                        char buf[MAX_PATH];
                        sprintf_s(buf, "Object_%d", iObjectNumber);

                        // 버튼을 누르면 m_pSelectedGameObject 에 값을 넣고 Transform 설정할 수 있게함
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
            // (1열)
            ImGui::TableNextColumn();
            CreateObjectCreateButton("Create Player", TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player")); // 플레이어 생성

            // (2열)
            ImGui::TableNextColumn();
            CreateObjectCreateButton("Create TestMonster", TEXT("Layer_TestMonster"), TEXT("Prototype_GameObject_TestMonster")); // 테스트용 몬스터 생성

            ImGui::EndTable();
        }
        ImGui::EndListBox();
    }

    return S_OK;
}

HRESULT CMapTool::CreateObjectCreateButton(const char* pButtonTag, const _tchar* pLayerTag, const _tchar* pPrototypeTag)
{
    // 게임오브젝트 생성 버튼 만들기
    if (ImGui::Button(pButtonTag, ImVec2(-FLT_MIN, 0.f)))
    {
        // 객체 생성하고 m_pLatestGameObject 에 생성한 객체 저장
        CGameObject* pGameObejct = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, pLayerTag, pPrototypeTag);

        if (nullptr == pGameObejct)
        {
            MSGBOX("Clone Failed :: CMapTool::CreateObjectCreateButton")
                return E_FAIL;
        }

        m_pLatestGameObjects[TAB_GAMEOBJECT].push_back(pGameObejct);

        // PickingPoint 가 있다면 그 위치에 객체 설치하고 PickingPoint 제거(+ nullptr 로 초기화)
        if (nullptr != m_pPickingPoint)
        {
            _vector vPickingPointPos = m_pPickingPoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
            pGameObejct->Get_Transform()->Set_State(CTransform::STATE_POSITION, vPickingPointPos);

            m_pPickingPoint->Set_Dead(true);
            m_pPickingPoint = nullptr;
        }

        // 생성된 객체는 바로 선택 처리해서 Transform 수정할 수 있도록 함
        m_pSelectedGameObject[TAB_GAMEOBJECT] = pGameObejct;
    }

    return S_OK;
}

void CMapTool::SetGameObjectTransform(TAB eTab)
{
    // 선택된 게임오브젝트가 없다면 회색으로 표시할 디자인용 코드
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
    // 선택된 게임오브젝트가 있다면 Position, Scale, Rotation 값 변경
    else
    {
        ImGui::Bullet(); ImGui::SameLine();
        m_pImGui_Manager->ColoredText(_float3(1.f, 1.f, 0.f), "Transform GameObject");

        m_pImGui_Manager->SetGameObjectPosition(string("Position"), m_pSelectedGameObject[eTab]->Get_Transform()); // 위치
        m_pImGui_Manager->SetGameObjectScale(string("Scale"), m_pSelectedGameObject[eTab]->Get_Transform());       // 크기
        m_pImGui_Manager->SetGameObjectRotation(string("Rotation"), m_pSelectedGameObject[eTab]->Get_Transform()); // 회전

		_float fFrustumRange = m_pSelectedGameObject[eTab]->GetFrustumRange();
		if (ImGui::DragFloat("Frustum", &fFrustumRange, 1.f, 20.f, 1000.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
			m_pSelectedGameObject[eTab]->SetFrustumRange(fFrustumRange);
    }
}

void CMapTool::GuidePickingPointUpdate()
{
    // 가이드피킹포인트가 마우스 따라다니게 (y 위치는 m_fPickintPointY 으로)
    dynamic_cast<CPickingPoint*>(m_pGuidePoint)->FollowMouse(m_fPickintPointY);
}

HRESULT CMapTool::CreatePickingPoint()
{
    // 마우스 피킹 체크 (Ctrl + LB)
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
    {
        // y 값이 설정된 가상의 평면과 마우스 피킹지점이 닿았는지 체크
        pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_fPickintPointY);

        if (TRUE == CollisionCheck.first)
        {
            // 피킹이 제대로 됐다면 PickingPoint 생성 위치를 세팅
            _vector vPickingCreatePosition = XMVectorSetY(CollisionCheck.second, m_fPickintPointY);

            // 이미 생성된 PickingPoint 가 있다면 제거
            if (nullptr != m_pPickingPoint)
                m_pPickingPoint->Set_Dead(true);

            // PickingPoint 생성
            m_pPickingPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_PickingPoint"), TEXT("Prototype_GameObject_PickingPoint"), &vPickingCreatePosition);
        }
    }

    return S_OK;
}

void CMapTool::SetPickingPointY()
{
    // Ctrl + E 로 m_fPickintPointY 올리기
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_E))
        m_fPickintPointY += 0.1f;

    // Ctrl + Q 로 m_fPickintPointY 내리기
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_Q))
        m_fPickintPointY -= 0.1f;

    // Ctrl + W 로 m_fPickintPointY 0 으로 초기화
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_W))
        m_fPickintPointY = 0;
}

HRESULT CMapTool::MoveToGuidePickingPoint(TAB eTab)
{
    if (m_pGameInstance->Get_KeyPressing(DIK_SPACE))
    {
        // 선택된 객체가 없다면 패스
        if (nullptr == m_pSelectedGameObject[eTab])
            return S_OK;

        _vector vGuidePickingPointPos = m_pGuidePoint->Get_Transform()->Get_State(CTransform::STATE_POSITION);
        m_pSelectedGameObject[eTab]->Get_Transform()->Set_State(CTransform::STATE_POSITION, vGuidePickingPointPos);
    }

    return S_OK;
}

HRESULT CMapTool::DeleteLatestGameObject(TAB eTab)
{
    // Ctrl + Z 으로 가장 최근에 생성한 객체 제거
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_Z))
    {
        // m_pLatestGameObjects 에 아무것도 없다면 패스
        if (m_pLatestGameObjects[eTab].size() == 0)
            return S_OK;

        // nullptr 이면 패스
        if (m_pLatestGameObjects[eTab].back() == nullptr)
            return S_OK;

        // m_pLatestGameObjects 의 마지막 원소가 m_pSelectedGameObject 와 같다면 m_pSelectedGameObject 을 nullptr 로 초기화
        if (m_pSelectedGameObject[eTab] == m_pLatestGameObjects[eTab].back())
            m_pSelectedGameObject[eTab] = nullptr;

        // m_pLatestGameObjects 의 마지막 원소 Set_Dead, nullptr 하고 컨테이너에서 빼버리기
        m_pLatestGameObjects[eTab].back()->Set_Dead(true);
        m_pLatestGameObjects[eTab].back() = nullptr;
        m_pLatestGameObjects[eTab].pop_back();
    }

    return S_OK;
}

void CMapTool::NaviMesh_Make()
{

	// 셀을 생성 할 수있는모드
	ImGui::Checkbox("MakeCell_Mode", &m_bMakeCell_Mode);
	if (true == m_bMakeCell_Mode)
	{
		m_bTypeSetting_Mode = false;
		m_bTypeChange_Mode = false;
		// 테스트 포인트 생성
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


		// 우클릭으로 포인트 인덱스 전환가능
		if (true == m_pGameInstance->Get_KeyEnter(DIK_C))
		{
			m_iPointIndex++;
			if (2 < m_iPointIndex)
				m_iPointIndex = 0;
		}
		// 포인트 인덱스 보여줌
		ImGui::Text("PointIndex : %d", m_iPointIndex);

		// R + 좌클릭으로 터레인이나 마우스 피킹으로 포인트의 위치 특정해줌
		if ((m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80) && true == m_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
		{
			// 피킹된 지점 넣어줌
			pair<_bool, _vector> MouseInfo = m_pGameInstance->Check_Ray_VirtualPlane_Collision(0.f);
			if (true == MouseInfo.first)
			{
				m_Points[m_iPointIndex][0] = XMVectorGetX(MouseInfo.second);
				m_Points[m_iPointIndex][1] = XMVectorGetY(MouseInfo.second);
				m_Points[m_iPointIndex][2] = XMVectorGetZ(MouseInfo.second);
			}

			// 각 셀의 점들에 있는 콜라이더들
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


		// 드래그로도 포인트 이동 가능
		ImGui::DragFloat3("Points1", m_Points[0], 0.01f, -500.f, 500.f);
		ImGui::DragFloat3("Points2", m_Points[1], 0.01f, -500.f, 500.f);
		ImGui::DragFloat3("Points3", m_Points[2], 0.01f, -500.f, 500.f);


		// 이걸 눌러서 셀을 만들 수 있다.
		if (ImGui::Button("CreateCell"))
		{
			_float3	Points[3] = {
				_float3(m_Points[0][0], m_Points[0][1], m_Points[0][2]),
				_float3(m_Points[1][0], m_Points[1][1], m_Points[1][2]),
				_float3(m_Points[2][0], m_Points[2][1], m_Points[2][2]),
			};

			// 시계 방향 정렬
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


	// 이미 찍혀진 셀을 수정할 수 있는 모드

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

			// R + 좌클릭으로 터레인이나 마우스 피킹으로 포인트의 위치 특정해줌
			if ((m_pGameInstance->Get_KeyPressing(DIK_LCONTROL)) && true == m_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
			{
				_float3 vMousePos = _float3(0.f, 0.f, 0.f);

				CPicking* pPicking = GET_INSTANCE(CPicking);

				_vector Dir = pPicking->Get_RayDir();
				_vector Pos = pPicking->Get_RayPos();


				// 체크된 콜라이더를 다시 피킹한 콜라이더 위치로 갈 수 있게 함

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

	// 셀의 옵션 값 설정
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

	// 셀 없애기
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

	// 파일 목록 불러오기
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
    // 멤버변수 Free

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

    /* 맵툴에서는 따로 레퍼런스카운트를 관리하지 않기 때문에 포인터 객체들 Safe_Release 하지 않아도 됨 */
}
