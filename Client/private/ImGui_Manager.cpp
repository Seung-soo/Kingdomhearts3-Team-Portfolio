#include "stdafx.h"
#include "ImGui_Manager.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "AssimpLoad.h"
#include "GameManager.h"
#include "Monster.h"

READY_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

void CImGui_Manager::ImGui_SetUp()
{
    // ������� �ʱ�ȭ
    m_pGameInstance  = CGameInstance::GetInstance();
    m_pDevice        = m_pGameInstance->Get_Device();
    m_pDeviceContext = m_pGameInstance->Get_Device_Context();
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);

    // ImGui �ʱ�ȭ
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.Fonts->AddFontFromFileTTF("../../ImGui/D2Coding.ttf", 13.5f, NULL);
    ImGui::StyleColorsDark();
    ImGui::GetStyle().Colors[ImGuiCol_Separator] = ImVec4(1.f, 1.f, 1.f, 0.5f);

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);
}

void CImGui_Manager::ImGui_Start()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImGui_Manager::ImGui_Tick()
{
    // Basic ������ �Ѱ� ����
    OnOffBasicWindow();

    // �⺻ ������
    if (show_basic_winodw)
    {
        ImGui::SetNextWindowBgAlpha(0.7f);
        ImGui::Begin("Basic Window");

        // ������ε�
        AssimpLoad();

        // ���� ����
        SelectLevel();

        SmallSeparator();

        ImGui::BulletText("(CTRL + BACKSPACE) ON/OFF Basic Window");

        // �ý��� ����(������, ���콺��ġ) ���
        ShowSystemInfo();

        SmallSeparator();

        // ���ӽ��ǵ� ����
        SetGameSpeed();

        // ���콺��
        MouseLock();    

        // ���� ����
        PauseSound();

        SmallSeparator();

        // ������ �ƽ�
        SetWeaponDamageMax();

        // üũ�ڽ�
        CheckWindowCheckbox();
#ifdef _DEBUG
        OnOffDebugRender();
#endif
        SmallSeparator();

        // ����ü Range ����
        SetFrustumRange();

        SmallSeparator();

        // ��ü���� ImGui ��Ʈ�ѷ� ����
        ShowGameObjectImGuiController();
        
        ImGui::End();
    }

    // ���� ������
    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }
}

void CImGui_Manager::ImGui_Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGui_Manager::SmallSeparator()
{
    ImGui::Dummy(ImVec2(0.0f, 2.5f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 2.5f));
}

void CImGui_Manager::BigSeparator()
{
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
}

void CImGui_Manager::ColoredText(_float3 color, const char* text)
{
    ImGui::TextColored(ImVec4(color.x, color.y, color.z, 1.0f), text);
}

void CImGui_Manager::ColoredButtonBegin(int r, int g, int b)
{
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(r, g, b));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor(r - 50, g + 50, b));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor(r - 100, g, b));
}

void CImGui_Manager::ColoredButtonEnd()
{
    ImGui::PopStyleColor(3);
}

void CImGui_Manager::SetGameObjectPosition(string Tag, CTransform* pTransform, _float fMin, _float fMax)
{
    if (nullptr == pTransform)
        return;

    // ������ ����
    _vector vPosition   = pTransform->Get_State(CTransform::STATE_POSITION);
    _float  fPosition[3] = { XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition) };
    
    if (ImGui::DragFloat3(Tag.c_str(), fPosition, 0.1f, fMin, fMax, "%.1f", ImGuiSliderFlags_AlwaysClamp))
        pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f));
}

void CImGui_Manager::SetGameObjectScale(string Tag, CTransform* pTransform, _float fMin, _float fMax)
{
    if (nullptr == pTransform)
        return;

    // ������ ����
    _float vRightScale = XMVectorGetX(pTransform->Get_Scale(CTransform::STATE_RIGHT));
    _float vUpScale    = XMVectorGetX(pTransform->Get_Scale(CTransform::STATE_UP));
    _float vLookScale  = XMVectorGetX(pTransform->Get_Scale(CTransform::STATE_LOOK));
    _float fScale[3]   = { vRightScale, vUpScale, vLookScale };

    if (ImGui::DragFloat3(Tag.c_str(), fScale, 0.1f, fMin, fMax, "%.1f", ImGuiSliderFlags_AlwaysClamp))
        pTransform->Scaled(XMVectorSet(fScale[0], fScale[1], fScale[2], 1.f));
}

void CImGui_Manager::SetGameObjectRotation(string Tag, CTransform* pTransform)
{
    if (nullptr == pTransform)
        return;

    // ȸ��
    Tag += 'X';
    static _float fDegreeX = 0.f;
    if (ImGui::DragFloat(Tag.c_str(), &fDegreeX, 0.1f, 0.f, 359.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(fDegreeX));

    Tag.back() = 'Y';
    static _float fDegreeY = 0.f;
    if (ImGui::DragFloat(Tag.c_str(), &fDegreeY, 0.1f, 0.f, 359.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_UP), XMConvertToRadians(fDegreeY));

    Tag.back() = 'Z';
    static _float fDegreeZ = 0.f;
    if (ImGui::DragFloat(Tag.c_str(), &fDegreeZ, 0.1f, 0.f, 359.f, "%.f", ImGuiSliderFlags_AlwaysClamp))
        pTransform->Rotation(pTransform->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(fDegreeZ));
}

void CImGui_Manager::SetGameObjectAnimation(CModel* pModel)
{
    if (pModel == nullptr)
        return;

    _int iNumberOfAnimations = (_int)pModel->Get_NumberOfAnimations();
    _int iCurrentAnimation   = (_int)pModel->Get_CurrAnimIndex();

    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##left", ImGuiDir_Left))
    {
        if (--iCurrentAnimation < 0)
            iCurrentAnimation = iNumberOfAnimations - 1;
        pModel->Set_TempIndex(iCurrentAnimation);
    }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##right", ImGuiDir_Right))
    {
        if (++iCurrentAnimation > iNumberOfAnimations - 1)
            iCurrentAnimation = 0;
        pModel->Set_TempIndex(iCurrentAnimation);
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text("Animation ( %d / %d )", iCurrentAnimation, iNumberOfAnimations - 1);
}

void CImGui_Manager::SetGameObjectShaderPass(string Tag, CShader* pShader, _int* iCurPass)
{
    if (pShader == nullptr)
        return;

    _uint iNumberOfPass = pShader->Get_NumberOfPasses();
    ImGui::DragInt(Tag.c_str(), iCurPass, 1.f, 0, iNumberOfPass - 1, "%d", ImGuiSliderFlags_AlwaysClamp);
}

void CImGui_Manager::SetGameObjectCollider(string Tag, CCollider* pCollider)
{
    if (pCollider == nullptr)
        return;

    // �ݶ��̴� ��ġ
    _float4 vColliderPos = pCollider->Get_Translation();
    _float ColliderPos[3];
    memcpy(ColliderPos, &vColliderPos, sizeof(_float3));

    Tag += 'P';
    if (ImGui::DragFloat3(Tag.c_str(), ColliderPos, 0.1f, -10.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
    {
        memcpy(&vColliderPos, ColliderPos, sizeof(_float3));
        pCollider->Set_Translation(vColliderPos);
    }

    // �ݶ��̴� ũ��
    _float3 vColliderScale = pCollider->Get_Scale();
    _float ColliderScale[3];
    memcpy(ColliderScale, &vColliderScale, sizeof(_float3));

    Tag.back() = 'S';
    if (ImGui::DragFloat3(Tag.c_str(), ColliderScale, 0.1f, 0.1f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
    {
        memcpy(&vColliderScale, ColliderScale, sizeof(_float3));
        pCollider->Set_Scale(vColliderScale);
    }
}

void CImGui_Manager::CheckMonsterState(string Tag, CStateMachine* pStateMachine)
{
    ImGui::Text(Tag.c_str());
    ImGui::SameLine();
    ImGui::Text(" :");
    ImGui::SameLine();

    string state = pStateMachine->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE);
    if (state == string(""))
        state = pStateMachine->Get_StateName(CStateMachine::STATETYPE_IDLE);
    ImGui::Text(state.c_str());
}

void CImGui_Manager::GetDistanceFromMouse(CTransform* pTransform)
{
    ImGui::Text("Distance From Mouse : %.1f", pTransform->Get_Distance_From_Mouse());
}

void CImGui_Manager::Set_LUTTexIndex(CRenderer* pRenderer)
{
    if (pRenderer == nullptr)
        return;

    _int iTexIndex = (_int)pRenderer->Get_LUTTexIndex();
    _uint iLUTMaxCnt = 97;

    float spacing = ImGui::GetStyle().ItemInnerSpacing.x; 
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##left", ImGuiDir_Left))
    {
        if (--iTexIndex < 0)
        {
            iTexIndex = iLUTMaxCnt - 1;
        }

        pRenderer->Set_LUTTexIndex((_uint)iTexIndex);
    }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::ArrowButton("##right", ImGuiDir_Right))
    {
        if (++iTexIndex > (_int)iLUTMaxCnt - 1)
            iTexIndex = iLUTMaxCnt - 1;
        pRenderer->Set_LUTTexIndex((_uint)iTexIndex);
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text("LUT Tex Index ( %d / %d )", iTexIndex, iLUTMaxCnt - 1);
}

void CImGui_Manager::OnOffBasicWindow()
{
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_BACKSPACE))
    {
        show_basic_winodw = !show_basic_winodw;
        return;
    }
}

void CImGui_Manager::ShowSystemInfo()
{
    ImGui::BulletText("FRAME : %.1f FPS", ImGui::GetIO().Framerate);
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsMousePosValid())
    {
        ImGui::BulletText("MOUSE POS(VIEWPORT) : (%g, %g)", io.MousePos.x, io.MousePos.y); // ����Ʈ ���� ���콺 ��ġ ǥ��
        auto CollisionCheck = m_pGameInstance->Check_Ray_VirtualPlane_Collision(0.f);
        if (CollisionCheck.first == true)
            ImGui::BulletText("MOUSE POS(INGAME) : (%.0f, %.0f)", vecx(CollisionCheck.second), vecz(CollisionCheck.second));
    }
    else
        ImGui::BulletText("Mouse pos: <INVALID>");

    CGameObject* pPlayer = m_pGameInstance->Get_GameObject(m_iCurLevelIndex, TEXT("Layer_Player"), 0);
    if (pPlayer == nullptr)
        return;
    _vector vPlayerPosition = pPlayer->Get_Transform()->Get_Position();
    ImGui::BulletText("Player Pos : (%.1f, %.1f, %.1f)", vecx(vPlayerPosition), vecy(vPlayerPosition), vecz(vPlayerPosition));
}

void CImGui_Manager::SetGameSpeed()
{
    _bool bDisabled = false;
    if (m_iCurLevelIndex == LEVEL_LOGO)
        bDisabled = true;

    ImGui::BeginDisabled(bDisabled);
    ImGui::BulletText("SPEED ");
    ImGui::SameLine();
    ImGui::SliderFloat(" ", &g_fGameSpeed, 0.1f, 5.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::EndDisabled();
}

void CImGui_Manager::SelectLevel()
{
    // ���� ����
    m_iCurLevelIndex = m_pGameInstance->Get_LevelIndex();
    ImGui::BulletText("CURRENT LEVEL:");
    ImGui::SameLine();
    ColoredText(_float3(1.f, 1.f, 0.f), m_levels[m_iCurLevelIndex]);

    // �� �̿ܿ��� ���� ������ ���� ����
   if (m_iCurLevelIndex >= LEVEL_TWILIGHT && m_iCurLevelIndex < LEVEL_FINALBOSS)
    {
        if (ImGui::Button("Go To Next Level", ImVec2(-FLT_MIN, 0.f)))
        {
            // ���� �����Ϸ��� �� �� �������� �� �������� ���� ����ϱ� ������ ����ֱ�
            CRenderer* pRenderer = (CRenderer*)m_pGameInstance->Clone_Component(
                LEVEL_STATIC, TEXT("Prototype_Component_Renderer"));
            pRenderer->Clear_Renderer();
            Safe_Release(pRenderer);

            // ���� ����
            if (m_pGameInstance->Get_LevelIndex() == LEVEL_TWILIGHT)
            {
                GM->Set_MoveOnToNextLevel(true);
                GM->Set_NextLevel(LEVEL_RAPUNZEL);
            }
            else if (m_pGameInstance->Get_LevelIndex() == LEVEL_RAPUNZEL)
            {
                GM->Set_MoveOnToNextLevel(true);
                GM->Set_NextLevel(LEVEL_RAPUNZEL_BOSS);
            }
            else if (m_pGameInstance->Get_LevelIndex() == LEVEL_RAPUNZEL_BOSS)
            {
                GM->Set_MoveOnToNextLevel(true);
                GM->Set_NextLevel(LEVEL_FROZEN);
            }
			else if (m_pGameInstance->Get_LevelIndex() == LEVEL_FROZEN)
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_KEYGRAVE);
			}
			else if (m_pGameInstance->Get_LevelIndex() == LEVEL_KEYGRAVE)
			{
				GM->Set_MoveOnToNextLevel(true);
				GM->Set_NextLevel(LEVEL_FINALBOSS);
			}
        }
    }
}

void CImGui_Manager::CheckWindowCheckbox()
{
    ImGui::Checkbox("DEMO", &show_demo_window);
    ImGui::Checkbox("LEVEL CONTROL", &show_levelcontrol_window);
    ImGui::Checkbox("PAUSE SOUND", &m_bPauseSound);

    ImGui::Checkbox("MOUSE LOCK(CTRL + L)", &m_bMouseLock);
    if (m_pGameInstance->Get_KeyPressing(DIK_LCONTROL) && m_pGameInstance->Get_KeyEnter(DIK_L))
        m_bMouseLock = !m_bMouseLock;

    if (m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
        ImGui::Checkbox("MAP COLLIDER", &show_mapcollider);

    if (ImGui::Checkbox("IGNORE MONSTER NAVI", &m_bIgnoreNavi))
        CGameManager::GetInstance()->IgnoreMonsterNavigation(m_bIgnoreNavi);

    // ���� �������� ����� �͵�
    if (LEVEL_MAPTOOL == m_pGameInstance->Get_LevelIndex())
    {
        if (ImGui::Checkbox("MAPTOOL", &show_maptool_window))
        {
            // ���� ���������� �ͷ���, ī�޶��� ������
			if (show_maptool_window)
			{
				show_cameratool_window      = false;
				show_terraintool_window     = false;
                show_objectplacetool_window = false;
			}
        }
		if (ImGui::Checkbox("TERRAINTOOL", &show_terraintool_window))
		{
			// �ͷ����� ���������� ī�޶�, ���� ������
			if (show_terraintool_window)
			{
				show_maptool_window         = false;
				show_cameratool_window      = false;
                show_objectplacetool_window = false;
			}
		}
        if (ImGui::Checkbox("CAMERATOOL", &show_cameratool_window))
        {
            // ī�޶��� ���������� ����, �ͷ����� ������
			if (show_cameratool_window)
			{
				show_maptool_window         = false;
				show_terraintool_window     = false;
                show_objectplacetool_window = false;
			}
        }
        if (ImGui::Checkbox("OBJECTTOOL", &show_objectplacetool_window))
        {
            if (show_objectplacetool_window)
            {
                show_maptool_window     = false;
                show_terraintool_window = false;
                show_cameratool_window  = false;
            }
        }
    }

    // ���ͻ����� â
    if (LEVEL_TEST == m_pGameInstance->Get_LevelIndex())
    {
        ImGui::Checkbox("MONSTER GENERATOR", &show_monstergenerator_window);
    }
}

void CImGui_Manager::ShowGameObjectImGuiController()
{
    if (ImGui::BeginTable("GameObjects", 2, ImGuiTableFlags_None))
    {
        // 0. ������Ʈ�� �ο��� ��ȣ �غ�
        _uint iObjectNumber = 0;

        // 1. ������Ʈ �Ŵ������� map<const _tchar*, CLayer*> �� ������
        map<const _tchar*, CLayer*> ObjectMap = m_pGameInstance->Get_ObjectMap(m_pGameInstance->Get_LevelIndex());

        for (auto& Pair : ObjectMap)
        {
            // 2. 1 �� ��ȸ�ϸ鼭 ���̾��±׸� �������� const char* �� ��ȯ
            const _tchar* wLayerTag = Pair.first;
            _bstr_t LayerTag(wLayerTag);

            // (��Ʈ���ϱ� ���� ���̾���� �н�)
            if (0 == lstrcmp(wLayerTag, TEXT("Layer_Axis")) || 0 == lstrcmp(wLayerTag, TEXT("Layer_Camera")) ||
                0 == lstrcmp(wLayerTag, TEXT("Layer_Map")) ||
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

                    // ��ư�� ������ ImGui ��Ʈ�ѷ��� Ű�ų� ��
                    if (ImGui::SmallButton(buf))
                    {
                        _bool bCurImGuiController = (*iter)->Get_ImGuiController();
                        (*iter)->Set_ImGuiController(!bCurImGuiController);                        
                    }
                    ++iObjectNumber;
                    ++iter;
                }
                ImGui::TreePop();
            }
        }
        ImGui::EndTable();
    }
}

void CImGui_Manager::SetFrustumRange()
{
    ImGui::BulletText("Set Frustum Range");

    ImGui::PushItemWidth(150.f);
    static char LayerTag[128] = "";
    ImGui::InputText("Input LayerTag", LayerTag, 128);
    ImGui::PopItemWidth();

    _bstr_t wLayerTag(LayerTag);

    list<CGameObject*>* ObjectList = m_pGameInstance->Get_ObjectList(m_pGameInstance->Get_LevelIndex(), wLayerTag);
    if (ObjectList == nullptr)
        return;

    if (ObjectList->size() == 0)
        return;

    static _float fRange = 5.f;
    if (ImGui::DragFloat("Frustum Range", &fRange, 0.5f, 0.5f, 100.f, "%.1f", ImGuiSliderFlags_AlwaysClamp))
    {
        for (auto& pObject : (*ObjectList))
        {
            pObject->SetFrustumRange(fRange);
        }        
    }
}

void CImGui_Manager::AssimpLoad()
{
    if (m_pGameInstance->Get_LevelIndex() != LEVEL_LOGO)
        return;

    ColoredButtonBegin(150, 50, 0);
    if (ImGui::Button("ASSIMP LOAD", ImVec2(-FLT_MIN, 0.f)))
    {
        CAssimpLoad* pAssimpLoad = new CAssimpLoad;
        pAssimpLoad->NonAnim_NativeConstruct(TEXT("../Bin/Resources/NonAnimMeshes"));
        pAssimpLoad->Anim_NativeConstruct(TEXT("../Bin/Resources/AnimMeshes"));
        Safe_Delete(pAssimpLoad);
    }
    ColoredButtonEnd();

    SmallSeparator();
}

void CImGui_Manager::MouseLock()
{
    if (m_bMouseLock)
    {
        if (false == m_bIsMouseLocked)
        {
            RECT	rc;
            POINT	pt1, pt2;
            GetClientRect(g_hWnd, &rc);

            pt1.x = rc.left;
            pt1.y = rc.top;
            pt2.x = rc.right;
            pt2.y = rc.bottom;

            ClientToScreen(g_hWnd, &pt1);
            ClientToScreen(g_hWnd, &pt2);

            rc.left = pt1.x;
            rc.top = pt1.y;
            rc.right = pt2.x;
            rc.bottom = pt2.y;

            ClipCursor(&rc);

            m_bIsMouseLocked = true;
        }
    }
    else
    {
        if (m_bIsMouseLocked)
        {
            ClipCursor(nullptr);

            m_bIsMouseLocked = FALSE;
        }
    }
}

void CImGui_Manager::PauseSound()
{
    m_pGameInstance->PauseAll(m_bPauseSound);
}

void CImGui_Manager::SetWeaponDamageMax()
{
    if (ImGui::Button("Damage Max", ImVec2(-FLT_MIN, 0.f)))
    {
        CComponent* pStatus = m_pGameInstance->Get_Component(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_PlayerWeapon"), TEXT("Com_Status"), 0);
        if (pStatus == nullptr)
            return;

        static_cast<CStatus*>(pStatus)->Set_Damage(9999.f);
    }

    if (ImGui::Button("Return Damage", ImVec2(-FLT_MIN, 0.f)))
    {
        CComponent* pStatus = m_pGameInstance->Get_Component(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_PlayerWeapon"), TEXT("Com_Status"), 0);
        if (pStatus == nullptr)
            return;

        static_cast<CStatus*>(pStatus)->Set_Damage(5.f);
    }
}

#ifdef _DEBUG
void CImGui_Manager::OnOffDebugRender()
{
    CRenderer* pRenderer = (CRenderer*)m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"));

    _bool* DebugRenderController = pRenderer->Get_DebugRenderController();

    ImGui::Checkbox("DEBUG RENDER", DebugRenderController);

    Safe_Release(pRenderer);
}
#endif

void CImGui_Manager::Free()
{
    // ������� Free
    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);
    Safe_Release(m_pGameInstance);

    // ImGui Free
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}