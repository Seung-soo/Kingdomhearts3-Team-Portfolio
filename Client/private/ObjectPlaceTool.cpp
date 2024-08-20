#include "stdafx.h"
#include "ObjectPlaceTool.h"
#include "ImGui_Manager.h"
#include "GameObject.h"
#include "PickingPoint.h"
#include "FileLoad_Manager.h"
#include "Trigger.h"

READY_SINGLETON(CObjectPlaceTool)

CObjectPlaceTool::CObjectPlaceTool()
{
}

#ifdef _DEBUG

HRESULT CObjectPlaceTool::ObjectPlaceTool_SetUp()
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
        MSGBOX("ERROR :: CObjectPlaceTool::ObjectPlaceTool_SetUp");
        return E_FAIL;
    }

	return S_OK;
}

_int CObjectPlaceTool::ObjectPlaceTool_Tick(_double TimeDelta)
{
    if (true == m_pImGui_Manager->show_objectplacetool_window)
    {
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("ObjectPlaceTool Window");

        ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "GuidePoint Y : %.1f", m_GuidePointY);
        m_pImGui_Manager->SmallSeparator();

        // 가이드포인트 관리
        CreatePickingPoint(TimeDelta);
        MoveGuidePoint(TimeDelta);

        // 오브젝트 관리
        CreateGameObject(TimeDelta);
        MoveGameObject(TimeDelta);
        DeleteGameObject(TimeDelta);

        m_pImGui_Manager->SmallSeparator();

        // 리셋, 세이브, 로드
        Save(TimeDelta);
        m_pImGui_Manager->BigSeparator();
        Load(TimeDelta);
        m_pImGui_Manager->BigSeparator();
        Reset(TimeDelta);
        m_pImGui_Manager->BigSeparator();

        // 저장파일 로드
        ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Load Objects");
        if (ImGui::Button("Load TT_FIRST", ImVec2(-FLT_MIN, 20.f)))
            CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_TT_FIRST, &m_CreatedObjects);
        if (ImGui::Button("Load TT_SECOND", ImVec2(-FLT_MIN, 20.f)))
            CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_TT_SECOND, &m_CreatedObjects);
        if (ImGui::Button("Load TT_BOSS", ImVec2(-FLT_MIN, 20.f)))
            CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_TT_BOSS, &m_CreatedObjects);
        if (ImGui::Button("Load RA_FIRST", ImVec2(-FLT_MIN, 20.f)))
            CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_FIRST, &m_CreatedObjects);
        if (ImGui::Button("Load RA_SECOND", ImVec2(-FLT_MIN, 20.f)))
            CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_SECOND, &m_CreatedObjects);
        if (ImGui::Button("Load RA_THIRD", ImVec2(-FLT_MIN, 20.f)))
            CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_THIRD, &m_CreatedObjects);
        if (ImGui::Button("Load RA_Boss_BIGTREE", ImVec2(-FLT_MIN, 20.f)))
            CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_BOSS_BIGTREE, &m_CreatedObjects);

        ImGui::End();
    }

	return _int();
}

HRESULT CObjectPlaceTool::ObjectPlaceTool_Render()
{
    return S_OK;
}

void CObjectPlaceTool::CreatePickingPoint(_double TimeDelta)
{
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Mouse_Pressing(CInput_Device::MBS_LBUTTON))
    {
        // 가상의 평면과 마우스 피킹지점이 닿았는지 체크
        pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(m_GuidePointY);

        if (TRUE == CollisionCheck.first)
        {
            // 이미 생성된 PickingPoint 가 있다면 제거
            if (nullptr != m_pPickingPoint)
                m_pPickingPoint->Set_Dead(true);

            // m_pPickingPoint 에 새로운 PickingPoint 저장
            m_pPickingPoint = m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, TEXT("Layer_PickingPoint"), TEXT("Prototype_GameObject_PickingPoint"), &CollisionCheck.second);
        }
    }
}

void CObjectPlaceTool::MoveGuidePoint(_double TimeDelta)
{
    if (m_pGuidePoint == nullptr)
        return;

    // Ctrl + E 로 m_GuidePointY 올리기
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_E))
        m_GuidePointY += 0.1f;

    // Ctrl + Q 로 m_GuidePointY 내리기
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyPressing(DIK_Q))
        m_GuidePointY -= 0.1f;

    // Ctrl + W 로 m_GuidePointY 0 으로 초기화
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyUp(DIK_W))
        m_GuidePointY = 0;

    // 가이드포인트가 마우스 따라다니게 (y 위치는 m_GuidePointY 으로)
    dynamic_cast<CPickingPoint*>(m_pGuidePoint)->FollowMouse(m_GuidePointY);
}

void CObjectPlaceTool::CreateGameObject(_double TimeDelta)
{
	if (ImGui::CollapsingHeader("CREATE OBJECT", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("ObjectPlace Tab", 2, ImGuiTableFlags_None))
		{
			// ================================================================================
			ImGui::TableNextColumn();

            CreateImGuiButton("Trigger", TEXT("Layer_Trigger"), TEXT("Prototype_GameObject_Trigger"));
            CreateImGuiButton("SkullSoldier", TEXT("Layer_SkullSoldier"), TEXT("Prototype_GameObject_SkullSoldier"));
			CreateImGuiButton("CrowSoldier", TEXT("Layer_CrowSoldier"), TEXT("Prototype_GameObject_CrowSoldier"));
			CreateImGuiButton("ThinSoldier", TEXT("Layer_ThinSoldier"), TEXT("Prototype_GameObject_ThinSoldier"));
			CreateImGuiButton("BowMan", TEXT("Layer_BowMan"), TEXT("Prototype_GameObject_BowMan"));
			CreateImGuiButton("Cannon", TEXT("Layer_Cannon"), TEXT("Prototype_GameObject_Cannon"));
			CreateImGuiButton("ShiningEgg", TEXT("Layer_ShiningEgg"), TEXT("Prototype_GameObject_ShiningEgg"));
			CreateImGuiButton("Monkey", TEXT("Layer_Monkey"), TEXT("Prototype_GameObject_Monkey"));
			CreateImGuiButton("Mower", TEXT("Layer_Mower"), TEXT("Prototype_GameObject_Mower"));
			CreateImGuiButton("Icicle", TEXT("Layer_Icicle"), TEXT("Prototype_GameObject_Icicle"));
			// ================================================================================
			ImGui::TableNextColumn();

			CreateImGuiButton("Nightmare", TEXT("Layer_Nightmare"), TEXT("Prototype_GameObject_Nightmare"));
			CreateImGuiButton("Flower", TEXT("Layer_Flower"), TEXT("Prototype_GameObject_Flower"));
			CreateImGuiButton("BigTree", TEXT("Layer_BigTree"), TEXT("Prototype_GameObject_BigTree"));
			CreateImGuiButton("IceMan", TEXT("Layer_IceMan"), TEXT("Prototype_GameObject_IceMan"));
			CreateImGuiButton("FrozenWolf", TEXT("Layer_FrozenWolf"), TEXT("Prototype_GameObject_FrozenWolf"));
			CreateImGuiButton("Goat", TEXT("Layer_Goat"), TEXT("Prototype_GameObject_Goat"));

			ImGui::EndTable();
		}
	}
}

void CObjectPlaceTool::MoveGameObject(_double TimeDelta)
{
    // 객체 선택
    if (m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
    {
        // 현재 레벨에 있는 레이어들 불러옴
        map<const _tchar*, CLayer*> ObjectMap = m_pGameInstance->Get_ObjectMap(m_pGameInstance->Get_LevelIndex());

        for (auto& Pair : ObjectMap)
        {
            // 레이어 태그를 가져옴
            const _tchar* wLayerTag = Pair.first;
            _bstr_t LayerTag(wLayerTag);

            // 건너뛸 레이어들
            if (lstrcmp(wLayerTag, TEXT("Layer_Axis")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_BackGround")) == 0 ||
                lstrcmp(wLayerTag, TEXT("Layer_Camera")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_GuidePoint")) == 0 ||
                lstrcmp(wLayerTag, TEXT("Layer_PickingPoint")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_Map")) == 0)
                continue;

            list<CGameObject*> GameObjectList = *(Pair.second->Get_Object());

            for (auto& pGameObject : GameObjectList)
            {
                CCollider* pCollider = pGameObject->Get_HitCollider();
                if (pCollider == nullptr)
                    return;

                BoundingSphere* pBoundingSphere = pCollider->Get_Sphere();
                _float fDist = 0.f;
                if (pBoundingSphere == nullptr)
                    return;

                if (m_pGameInstance->Ray_Sphere_Collision(*pBoundingSphere, fDist) == true)
                {
                    m_pSelectedObject = pGameObject;
                    break;
                }
            }
        }
    }
    
    if (m_pSelectedObject == nullptr)
        return;

    // 객체 이동 (가이드포인트 위치로)
    if (m_pGameInstance->Mouse_Pressing(CInput_Device::MBS_LBUTTON))
    {
        _vector vGuidePointPos = m_pGuidePoint->Get_Transform()->Get_Position();
        m_pSelectedObject->Get_Transform()->Set_Position(vGuidePointPos);
    }

    // 객체 해제
    if (m_pGameInstance->Get_Mouse_Up(CInput_Device::MBS_LBUTTON))
    {
        m_pSelectedObject = nullptr;
    }
}

void CObjectPlaceTool::DeleteGameObject(_double TimeDelta)
{
    if (m_CreatedObjects.size() == 0)
        return;

    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_Z))
    {
        m_CreatedObjects.back()->Set_Dead(true);
        m_CreatedObjects.pop_back();
        m_pSelectedObject = nullptr;
    }

    if (m_pSelectedObject == nullptr)
        return;

    if (m_pGameInstance->Get_KeyEnter(DIK_DELETE))
    {
        auto iter = m_CreatedObjects.begin();

        for (; iter != m_CreatedObjects.end(); )
        {
            if ((*iter) == m_pSelectedObject)
            {
                (*iter)->Set_Dead(true);
                iter = m_CreatedObjects.erase(iter);
                break;
            }
            ++iter;
        }

        m_pSelectedObject = nullptr;
    }
}

void CObjectPlaceTool::Reset(_double TimeDelta)
{
    m_pImGui_Manager->ColoredButtonBegin(150, 0, 0);
    if (ImGui::Button("Reset", ImVec2(-FLT_MIN, 40.f)))
    {
        for (auto& pGameObject : m_CreatedObjects)
            pGameObject->Set_Dead(true);

        m_CreatedObjects.clear();
        m_pSelectedObject = nullptr;
    }
    m_pImGui_Manager->ColoredButtonEnd();
}

void CObjectPlaceTool::Save(_double TimeDelta)
{
    if (ImGui::CollapsingHeader("SAVE LAYER", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 데이터 로드하기 위해 파일이름 입력받기
        ImGui::PushItemWidth(175.f);
        ImGui::InputText("Save File Name", m_SaveFileName, 128);

        if (ImGui::BeginTable("Save Layer Table", 2, ImGuiTableFlags_None)) 
        {
            // 현재 레벨에 있는 레이어들 불러옴
            map<const _tchar*, CLayer*> ObjectMap = m_pGameInstance->Get_ObjectMap(m_pGameInstance->Get_LevelIndex());

            for (auto& Pair : ObjectMap)
            {
                // 레이어 태그를 가져옴
                const _tchar* wLayerTag = Pair.first;
                _bstr_t LayerTag(wLayerTag);

                // 건너뛸 레이어들
                if (lstrcmp(wLayerTag, TEXT("Layer_Axis")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_BackGround")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_Camera")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_GuidePoint")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_PickingPoint")) == 0 || lstrcmp(wLayerTag, TEXT("Layer_Map")) == 0)
                    continue;

                if (Pair.second->Get_Object()->size() == 0)
                    continue;

                // 각 레이어태그별로 버튼 생성
                ImGui::TableNextColumn();
                m_pImGui_Manager->ColoredButtonBegin(160, 70, 160);
                if (ImGui::Button(LayerTag, ImVec2(-FLT_MIN, 20.f)))
                {
                    // 저장파일 이름 설정
                    wstring FilePath;
                    FilePath = TEXT("../SaveFiles/Objects/");
                    FilePath += bstr_t(m_SaveFileName);

                    HANDLE hFile = CreateFile(FilePath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                    m_pGameInstance->Save_Layer(m_pGameInstance->Get_LevelIndex(), wLayerTag, hFile);

                    CloseHandle(hFile);
                }
                m_pImGui_Manager->ColoredButtonEnd();
            }

            ImGui::EndTable();
        }       
    }    
}

void CObjectPlaceTool::Load(_double TimeDelta)
{
    // 데이터 로드하기 위해 파일이름 입력받기
    ImGui::PushItemWidth(175.f);
    ImGui::InputText("Load File Name", m_LoadFileName, 128);

    // 어떤 Object 불러올지 선택
    static int iSelect = -1;
    ImGui::Combo("Select Object", &iSelect, m_Objects, IM_ARRAYSIZE(m_Objects));

    m_pImGui_Manager->ColoredButtonBegin(50, 150, 50); 
    if (ImGui::Button("Load", ImVec2(-FLT_MIN, 40.f)))
    {
        if (iSelect == -1)
        {
            m_pImGui_Manager->ColoredButtonEnd();
            return;
        }

        // 저장파일 경로로 변환
        char SaveFilePath[MAX_PATH] = "../SaveFiles/Objects/";
        strcat_s(SaveFilePath, m_LoadFileName);
        _bstr_t wSaveFilePath(SaveFilePath);

        HANDLE hFile = CreateFile(wSaveFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hFile == INVALID_HANDLE_VALUE)
        {
            MSGBOX("Check File Name!");
            iSelect = -1;
            m_pImGui_Manager->ColoredButtonEnd();
            return;
        }

        // 기존 오브젝트 리셋
        {
            for (auto& pGameObject : m_CreatedObjects)
                pGameObject->Set_Dead(true);

            m_CreatedObjects.clear();
            m_pSelectedObject = nullptr;
        }

        if (iSelect == 0) // 트리거일 경우
        {
            // 생성하고 월드행렬이랑 타입 세팅
            DWORD               dwBytes = 0;
            _float4x4           WorldMatrix;
            CTrigger::TRIGGER   eTrigger;

            while (true)
            {
                ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwBytes, nullptr);
                if (dwBytes == 0)
                    break;

                ReadFile(hFile, &eTrigger, sizeof(CTrigger::TRIGGER), &dwBytes, nullptr);
                if (dwBytes == 0)
                    break;

                CGameObject* pGameObject = CGameInstance::GetInstance()->Add_GameObject_And_Return(LEVEL_MAPTOOL, m_LayerTag[iSelect], m_PrototypeTag[iSelect]);
                pGameObject->Get_Transform()->Set_WorldMatrix(WorldMatrix);
                dynamic_cast<CTrigger*>(pGameObject)->Set_TriggerType(eTrigger);
                m_CreatedObjects.push_back(pGameObject);
            }
        }
        else // 트리거가 아닐 경우
        {
            // 생성하고 월드행렬 세팅
            DWORD       dwBytes = 0;
            _float4x4   WorldMatrix;
            while (true)
            {
                ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

                if (dwBytes == 0)
                    break;

                CGameObject* pGameObject = CGameInstance::GetInstance()->Add_GameObject_And_Return(LEVEL_MAPTOOL, m_LayerTag[iSelect], m_PrototypeTag[iSelect]);
                pGameObject->Get_Transform()->Set_WorldMatrix(WorldMatrix);
                m_CreatedObjects.push_back(pGameObject);
            }
        }      

        iSelect = -1;
        CloseHandle(hFile);
    }
    m_pImGui_Manager->ColoredButtonEnd();
}

void CObjectPlaceTool::CreateImGuiButton(const char* pButtonTag, const _tchar* pLayerTag, const _tchar* pPrototypeTag)
{
    if (ImGui::Button(pButtonTag, ImVec2(-FLT_MIN, 20.f)))
    {
        if (m_pPickingPoint == nullptr)
            return;

        _vector vPos = m_pPickingPoint->Get_Transform()->Get_Position();
        m_CreatedObjects.push_back(m_pGameInstance->Add_GameObject_And_Return(LEVEL_MAPTOOL, pLayerTag, pPrototypeTag, &vPos));

        m_pPickingPoint->Set_Dead(true);
        m_pPickingPoint = nullptr;
    }
}

#endif

void CObjectPlaceTool::Free()
{
#ifdef _DEBUG
    // 멤버변수 Free
    if (nullptr != m_pDevice)
        Safe_Release(m_pDevice);
    if (nullptr != m_pDeviceContext)
        Safe_Release(m_pDeviceContext);
    if (nullptr != m_pGameInstance)
        Safe_Release(m_pGameInstance);
    if (nullptr != m_pImGui_Manager)
        Safe_Release(m_pImGui_Manager);
#endif
}
