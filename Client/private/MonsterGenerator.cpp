#include "stdafx.h"
#include "MonsterGenerator.h"
#include "ImGui_Manager.h"
#include "PickingPoint.h"
#include "FrogBand.h"
#include "GameManager.h"
#include "MonsterSkill.h"
#include "PillarOfFire.h"
#include "Effect_Manager.h"

READY_SINGLETON(CMonsterGenerator)

CMonsterGenerator::CMonsterGenerator()
{
}

#ifdef _DEBUG
HRESULT CMonsterGenerator::CMonsterGenerator_SetUp()
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

    if (nullptr == m_pGameInstance || nullptr == m_pDevice ||
        nullptr == m_pDeviceContext || nullptr == m_pImGui_Manager)
    {
        BREAKPOINT;
        return E_FAIL;
    }

    return S_OK;
}

_int CMonsterGenerator::CMonsterGenerator_Tick(_double TimeDelta)
{
    if (true == CImGui_Manager::GetInstance()->show_monstergenerator_window)
    {
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("Monster Generator Window");

        // 가이드포인트 위치 업데이트
        CreateGuidePoint();

        // 가이드포인트 위치에 몬스터 생성
        CreateMonster();

        // 몬스터제거
        //DeleteGameObject();

        HiddenGenerator();

        ImGui::End();
    }
	return RESULT_NOPROBLEM;
}

HRESULT CMonsterGenerator::CMonsterGenerator_Render()
{
	return S_OK;
}

void CMonsterGenerator::CreateGuidePoint()
{
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
    {
        // 가상의 평면과 마우스 피킹지점이 닿았는지 체크
        pair<_bool, _vector> CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(0.f);

        if (TRUE == CollisionCheck.first)
        {
            // PickingPoint 생성
            if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TEST, TEXT("Layer_PickingPoint"), TEXT("Prototype_GameObject_PickingPoint"), &CollisionCheck.second)))
                return;

            // 이미 생성된 PickingPoint 가 있다면 제거하고 m_pGuidePoint 에 PickingPoint 저장
            if (nullptr != m_pGuidePoint)
            {
                m_pGuidePoint->Set_Dead(true);
                // Tick 다 돌기 전까지는 Layer_PickingPoint 레이어에서 Dead 처리된 PickingPoint 가 빠지지 않기 때문에 1번을 저장해야함
                m_pGuidePoint = m_pGameInstance->Get_GameObject(LEVEL_TEST, TEXT("Layer_PickingPoint"), 1);
            }
            else
                // m_pPickingPoint 에 PickingPoint 저장
                m_pGuidePoint = m_pGameInstance->Get_GameObject(LEVEL_TEST, TEXT("Layer_PickingPoint"), 0);
        }
    }
}

void CMonsterGenerator::CreateMonster()
{
    if (m_pGuidePoint == nullptr)
        return;

    if (ImGui::BeginTable("MonsterGenerator Tab", 3, ImGuiTableFlags_None))
    {
        // ================================================================================
        ImGui::TableNextColumn();

        CreateButton("SkullSoldier", TEXT("Layer_SkullSoldier"), TEXT("Prototype_GameObject_SkullSoldier"));
        CreateButton("CrowSoldier", TEXT("Layer_CrowSoldier"), TEXT("Prototype_GameObject_CrowSoldier"));
        CreateButton("ThinSoldier", TEXT("Layer_ThinSoldier"), TEXT("Prototype_GameObject_ThinSoldier"));
        CreateButton("BowMan", TEXT("Layer_BowMan"), TEXT("Prototype_GameObject_BowMan"));
        CreateButton("Cannon", TEXT("Layer_Cannon"), TEXT("Prototype_GameObject_Cannon"));
        CreateButton("ShiningEgg", TEXT("Layer_ShiningEgg"), TEXT("Prototype_GameObject_ShiningEgg"));
        CreateButton("Monkey", TEXT("Layer_Monkey"), TEXT("Prototype_GameObject_Monkey"));
        CreateButton("Mower", TEXT("Layer_Mower"), TEXT("Prototype_GameObject_Mower"));
        CreateButton("Crow", TEXT("Layer_Crow"), TEXT("Prototype_GameObject_Crow"));
        CreateButton("Icicle", TEXT("Layer_Icicle"), TEXT("Prototype_GameObject_Icicle"));
        CreateButton("FrutiZone", TEXT("Layer_FrutiZone"), TEXT("Prototype_GameObject_DropFruitZone"));
       
        // ================================================================================
        ImGui::TableNextColumn();

        CreateButton("Nightmare", TEXT("Layer_Nightmare"), TEXT("Prototype_GameObject_Nightmare"));
        CreateButton("Flower", TEXT("Layer_Flower"), TEXT("Prototype_GameObject_Flower"));
        CreateButton("BigTree", TEXT("Layer_BigTree"), TEXT("Prototype_GameObject_BigTree"));
        CreateButton("IceMan", TEXT("Layer_IceMan"), TEXT("Prototype_GameObject_IceMan"));
        CreateButton("FrozenWolf", TEXT("Layer_FrozenWolf"), TEXT("Prototype_GameObject_FrozenWolf"));
        CreateButton("Goat", TEXT("Layer_Goat"), TEXT("Prototype_GameObject_Goat")); 
        CreateButton("LittleWolf", TEXT("Layer_LittleWolf"), TEXT("Prototype_GameObject_LittleWolf"));

        // ================================================================================
        ImGui::TableNextColumn();

        // 개구리 합창단~
        CreateFrogBandButton("Cello",   TEXT("Prototype_Component_Model_FrogBand_Cello"));
        CreateFrogBandButton("Flute",   TEXT("Prototype_Component_Model_FrogBand_Flute"));
        CreateFrogBandButton("Horn",    TEXT("Prototype_Component_Model_FrogBand_Horn"));
        CreateFrogBandButton("Master",  TEXT("Prototype_Component_Model_FrogBand_Master"));
        CreateFrogBandButton("Timpani", TEXT("Prototype_Component_Model_FrogBand_Timpani"));
        CreateFrogBandButton("Trumpet", TEXT("Prototype_Component_Model_FrogBand_Trumpet"));
        CreateFrogBandButton("Tuba",    TEXT("Prototype_Component_Model_FrogBand_Tuba"));
        CreateFrogBandButton("Violin",  TEXT("Prototype_Component_Model_FrogBand_Violin"));
        CreateFrogBandButton("Vocal",   TEXT("Prototype_Component_Model_FrogBand_Vocal"));

        // 불기둥(끌어당기기)
        if (ImGui::Button("PillarOfFire"))
        {
            CPillarOfFire::PILLARDESC PillarDesc;
            PillarDesc.vPosition    = m_pGuidePoint->Get_Transform()->Get_Position();
            PillarDesc.bGravitation = true;

            m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

            m_pGuidePoint->Set_Dead(true);
            m_pGuidePoint = nullptr;
        }
        
        // 불기둥(공전)
        if (ImGui::Button("PillarOfFires"))
        {
            _float fDistance = 10.f;

            _vector vPos = m_pGuidePoint->Get_Transform()->Get_Position();
            CPillarOfFire::PILLARDESC PillarDesc;
            PillarDesc.vPosition    = vPos + __vector(fDistance, 0.f, 0.f, 0.f);
            PillarDesc.bGravitation = false;

            m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

            PillarDesc.vPosition = vPos + __vector(-fDistance, 0.f, 0.f, 0.f);
            m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

            PillarDesc.vPosition = vPos + __vector(0.f, 0.f, fDistance, 0.f);
            m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

            PillarDesc.vPosition = vPos + __vector(0.f, 0.f, -fDistance, 0.f);
            m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_PillarOfFire"), TEXT("Prototype_GameObject_PillarOfFire"), &PillarDesc);

            m_pGuidePoint->Set_Dead(true);
            m_pGuidePoint = nullptr;
        }

        ImGui::EndTable();
    }
}

void CMonsterGenerator::DeleteGameObject()
{
    if (m_pGameInstance->Get_KeyEnter(DIK_DELETE))
        GM->Release_Monsters();
}

void CMonsterGenerator::HiddenGenerator()
{
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_H))
    {
        for (_uint i = 0; i < 20; ++i)
        {
            _vector vPos = __vector((_float)(rand() % 16) - 7.5f, 0.f, (_float)(rand() % 16) - 7.5f, 1.f);
            m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_ShiningEgg"), TEXT("Prototype_GameObject_ShiningEgg"), &vPos);
        }
    }
}

void CMonsterGenerator::CreateButton(const char* ButtonTag, _tchar* pLayerTag, _tchar* pPrototypeTag)
{
    if (ImGui::Button(ButtonTag))
    {
        _vector vPos = m_pGuidePoint->Get_Transform()->Get_Position();
        m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, pLayerTag, pPrototypeTag, &vPos);

        m_pGuidePoint->Set_Dead(true);
        m_pGuidePoint = nullptr;
    }
}

void CMonsterGenerator::CreateFrogBandButton(const char* ButtonTag, _tchar* pModelTag)
{
    if (ImGui::Button(ButtonTag))
    {
        CFrogBand::FROGBANDDESC FrogBandDesc;
        lstrcpy(FrogBandDesc.ModelTag, pModelTag);
        FrogBandDesc.vPos = m_pGuidePoint->Get_Transform()->Get_Position();

        m_pSelectedGameObject = m_pGameInstance->Add_GameObject_And_Return(LEVEL_TEST, TEXT("Layer_FrogBand"), TEXT("Prototype_GameObject_FrogBand"), &FrogBandDesc);

        m_pGuidePoint->Set_Dead(true);
        m_pGuidePoint = nullptr;
    }
}

#endif

void CMonsterGenerator::Free()
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