#include "stdafx.h"
#include "LittleWolf.h"

#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"

// State
#include "State_MonsterIdle.h"
#include "State_MonsterChase.h"
#include "State_MonsterDynamicMove.h"
#include "State_MonsterSpawn.h"
#include "Effect_Manager.h"

CLittleWolf::CLittleWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CMonster(pDevice, pDeviceContext)
{
}

CLittleWolf::CLittleWolf(const CLittleWolf& rhs)
    : CMonster(rhs)
{
}

HRESULT CLittleWolf::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLittleWolf::NativeConstruct(void* pArg)
{
    //  ============================== Loader에서 실행 됨===========================================

    CTransform::TRANSFORMDESC TransformDesc;
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = 1.5f;
    TransformDesc.fScalingPerSec = 5.f;

    if (FAILED(__super::NativeConstruct(&TransformDesc)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        WOLFDESC* tDesc = (WOLFDESC*)pArg;

        m_vFlyUpDir = XMVector3Normalize(tDesc->vStartDir);
        m_vFlyUpDir = XMVectorSetW(m_vFlyUpDir, 0.f);

        m_pTransformCom->MakeSameDirection(m_vFlyUpDir);

        m_pTransformCom->Set_Position(tDesc->vStartPos);

        if (true == tDesc->bRain)
        {
            m_bRain = true;
            m_pTransformCom->Rotation(XMVector3Normalize(m_pTransformCom->Get_Right()), XMConvertToRadians(90.f));
        }
    }

    if (FAILED(SetUp_Components()))
        return E_FAIL;

    if (FAILED(SetUp_RootNodeIndex()))
        return E_FAIL;

    // 애니메이션 설정
    if (FAILED(SetUp_Animations()))
        return E_FAIL;

    // 콜라이더 정보 초기화
    if (FAILED(Ready_Collider_Info()))
        return E_FAIL;

    // 쉐이더패스 설정
    m_iPass = 0;

    //m_bImGuiController = true;

    return S_OK;
}

HRESULT CLittleWolf::NativeConstruct_Pool(void* pArg)
{
    //  ================================== 실제 인게임에서 실행 됨 =============================================

    // 게임매니저가 게임 내 모든 몬스터들을 따로 관리하고 있기 때문에 오브젝트풀에서 꺼내올 때 게임매니저에 등록



 
    return S_OK;
}

_int CLittleWolf::Tick(_double TimeDelta)
{
    if (FAILED(__super::Tick(TimeDelta)))
        return RESULT_ERROR;

	if (true == m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_FrozenWolf"))->Get_Dead())
		m_bDead = true;

    m_TimeAccum_Life += TimeDelta;

    if (true == m_bDead)
        return RESULT_DEAD;

    m_TimeAccum += TimeDelta;
    m_TimeAccum_Turn += TimeDelta;
    m_TimeAccum_ChasePlayer += TimeDelta;

    if (m_TimeAccum_Life > 30.0)
    {
        CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 4.5f, 0.f);

        // 충돌하면 m_bReadyToDie 를 true 로
        m_bReadyToDie = true;
        m_bDead = true;
    }


    if (FAILED(ChangeState(TimeDelta)))
        return RESULT_ERROR;

    if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
        m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

    else
        m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);

    if (FAILED(Collider_Transform_Update()))
        return RESULT_ERROR;

    if (m_bDead == false)
    {
        m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom);
    }

    // 공격 콜라이더는 공격 상태(m_bIsAttacking)일 때만 등록
    // m_bIsAttacking 는 공격 상태로 들어갈 때 직접 켜주면 됨 (SetAttackTime 함수 사용)
    if (m_bIsAttacking == true)
    {
       m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom);

    }

    auto fY = vecy(m_pTransformCom->Get_Position());

    if(fY < 1.0f)
        m_pTransformCom->Set_PositionY(1.0f);

    return _int();
}

_int CLittleWolf::LateTick(_double TimeDelta)
{
    if (FAILED(__super::LateTick(TimeDelta)))
        return RESULT_ERROR;

    if (FAILED(OnCollision(TimeDelta)))
        return RESULT_ERROR;

    if (true == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && false == m_bDead)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
    if (m_bReadyToDie == false && m_bDead == false)
    {
        m_pRendererCom->Add_Debug(m_pHitColliderCom);
    }
    // 공격 콜라이더
    if (m_bIsAttacking == true)
    {
       m_pRendererCom->Add_Debug(m_pAtkColliderCom);
    }
#endif

    return RESULT_NOPROBLEM;
}

HRESULT CLittleWolf::Render()
{
    if (nullptr == m_pModelCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    _uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

    for (_uint i = 0; i < iNumMaterials; ++i)
    {
        m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
        m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
        m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");
        m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
    }

    return S_OK;
}

HRESULT CLittleWolf::Render_Shadow()
{
    if (nullptr == m_pModelCom)
    {
        BREAKPOINT;
        return E_FAIL;
    }

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
    {
        BREAKPOINT;
        return E_FAIL;
    }
    _float4x4 LightViewMatrix = pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightViewMatrix;
    if (FAILED(m_pShaderCom->Set_RawValue("g_LightViewMatrix", &LightViewMatrix, sizeof(_float4x4))))
    {
        BREAKPOINT;
        return E_FAIL;
    }

    _float4x4 LightProjMatrix = pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightProjMatrix;
    if (FAILED(m_pShaderCom->Set_RawValue("g_LightProjMatrix", &LightProjMatrix, sizeof(_float4x4))))
    {
        BREAKPOINT;
        return E_FAIL;
    }

    _uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
    _uint iPass = 14;

    for (_uint i = 0; i < iNumMaterials; ++i)
    {
        m_pModelCom->Render(m_pShaderCom, iPass, i, "g_BoneMatrices");
    }

    return S_OK;
}

void CLittleWolf::PostRender(_double TimeDelta)
{
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGui::Begin("Object Control");

    CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

    // 애니메이션 테스트
    pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
    pImGui_Manager->SmallSeparator();

    // 월드행렬 테스트
    pImGui_Manager->SetGameObjectPosition(string("LW Pos"), m_pTransformCom);
    pImGui_Manager->SetGameObjectScale(string("FW Scale"), m_pTransformCom);
    pImGui_Manager->SmallSeparator();

    // 쉐이더 테스트
    pImGui_Manager->SetGameObjectShaderPass(string("LW Shader"), m_pShaderCom, (_int*)(&m_iPass));
    pImGui_Manager->SmallSeparator();

    // 상태 체크
    ImGui::Text("AnimIndex : %d", m_pModelCom->Get_CurrAnimIndex());

    // 플레이어와의 거리
    ImGui::Text("Distance From Player : %.1f", m_pTransformCom->Get_Distance(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0)->Get_Transform()));
    pImGui_Manager->SmallSeparator();

    // OnlyIdle 상태로 고정
    ImGui::Checkbox("ONLYIDLE", &m_bOnlyIdle);
    pImGui_Manager->SmallSeparator();

    // Dead 처리
    pImGui_Manager->ColoredButtonBegin(200, 50, 0);
    if (ImGui::Button("DEAD"))
    {
        m_bDead = true;
    }
    pImGui_Manager->ColoredButtonEnd();

    ImGui::End();
}

HRESULT CLittleWolf::Save_ObjectData(HANDLE hFile)
{
    return S_OK;
}

HRESULT CLittleWolf::Load_ObjectData(HANDLE hFile)
{
    return S_OK;
}

_bool CLittleWolf::Damaged(CCollider* pAtkCollider, _float fDamaged)
{
    // 죽기 직전 상태라면 충돌검사 패스
    if (m_bReadyToDie == true)
        return false;

    // 데미지 받음
    if (false == m_pStatusCom->Damaged(fDamaged))
        return false;

    // 죽는 이펙트
    CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 4.5f, 0.f);
    CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("LittleWolf_Dead.wav"), 0.5f, true, m_pTransformCom->Get_Position());


    // 충돌하면 m_bReadyToDie 를 true 로
    m_bReadyToDie = true;
    m_bDead = true;

    return true;
}

HRESULT CLittleWolf::Ready_Collider_Info()
{
    // 피격 콜라이더
    const char* HitBone = "ago";

    m_HitCollider_BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr(HitBone);
    m_HitCollider_SocketPivotMatrix = m_pModelCom->Get_TransformMatrix();


    // 공격 콜라이더
    const char* AtkBones = "ago";

	m_AtkCollider_BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr(AtkBones);
	m_AtkCollider_SocketPivotMatrix = m_pModelCom->Get_TransformMatrix();

    return S_OK;
}

HRESULT CLittleWolf::Collider_Transform_Update()
{
	m_HitCollider_TransformMatrix = XMLoadFloat4x4(m_HitCollider_BoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_HitCollider_SocketPivotMatrix) * m_pTransformCom->Get_WorldMatrix();
	m_pHitColliderCom->Update_Transform(m_HitCollider_TransformMatrix);

	m_AtkCollider_TransformMatrix = XMLoadFloat4x4(m_AtkCollider_BoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_AtkCollider_SocketPivotMatrix) * m_pTransformCom->Get_WorldMatrix();
	m_pAtkColliderCom->Update_Transform(m_AtkCollider_TransformMatrix);

    return S_OK;
}

HRESULT CLittleWolf::SetUp_Components()
{
    /* For.Com_Status */
    CStatus::STATUSDESC StatusDesc;
    StatusDesc.fMaxHP = 200.f;
    StatusDesc.fHP = StatusDesc.fMaxHP;
    StatusDesc.fDamaged = 5.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
        return E_FAIL;

    /* For.Com_MonsterCooler */
    // 여기서 각 행동들의 쿨타임을 설정
    CMonsterCooler::MONSTERCOOLERDESC MonsterCoolerDesc;
    MonsterCoolerDesc.MaxAttackCool = 3.0;
    MonsterCoolerDesc.AttackCool = 6.0;
    MonsterCoolerDesc.MaxRunAwayCool = 6.5;
    MonsterCoolerDesc.RunAwayCool = 8.0;
    MonsterCoolerDesc.MaxChaseCool = 15.0;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MonsterCooler"), TEXT("Com_MonsterCooler"), (CComponent**)&m_pMonsterCoolerCom, &MonsterCoolerDesc)))
        return E_FAIL;


    /* For.Com_Collider */
    {
        CCollider::COLLIDERDESC ColliderDesc;

        // 피격 콜라이더
        const _tchar* HitColliderTag = L"HitCollider";

        ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
        ColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
        ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
        ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

        if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), HitColliderTag, (CComponent**)&m_pHitColliderCom, &ColliderDesc)))
            return E_FAIL;


        // 공격 콜라이더
        const _tchar* AtkColliderTag = L"AttackCollider";

        ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
        ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
        ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
        ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);


		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), AtkColliderTag, (CComponent**)&m_pAtkColliderCom, &ColliderDesc)))
			return E_FAIL;
    }

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;


    /* For.Com_Navigation */
    CNavigation::NAVIDESC tNaviDesc;
    tNaviDesc.iCurrentIndex = 0;
    tNaviDesc.pParent = m_pTransformCom;

    if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Navigation_KeyGrave"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
    {
        BREAKPOINT;
        return E_FAIL;
    }
    m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());


    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;


    /* Dissolve_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
        return E_FAIL;


    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_LittleWolf"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLittleWolf::SetUp_RootNodeIndex()
{
	_int iRootNodeIndex = m_pModelCom->Get_NodeIndexByName("kh_sk");
	if (UINT_MAX == iRootNodeIndex)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Set_RootNodeIndex(iRootNodeIndex);

    return S_OK;
}

HRESULT CLittleWolf::SetUp_Animations()
{
    m_pModelCom->Add_OnceListAll();

    m_pModelCom->Rmv_OnceList(1);
    m_pModelCom->Rmv_OnceList(8);

    m_pModelCom->Add_LerpListAll();

    // Spawn 애니메이션 바인드
    Set_AnimAndReset(m_pModelCom, 0);

    // 애니메이션 속도 조정
    m_pModelCom->Set_TargetPlaySpeed(8, 50.0);   
    m_pModelCom->Set_TargetPlaySpeed(0, 40.0);

    // Bite애니메이션 속도 저장
    m_fAnimOriginSpeed = (_float)m_pModelCom->Get_TargetPlaySpeed(11);

    return S_OK;
}

HRESULT CLittleWolf::SetUp_ConstantTable()
{
    if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
        return E_FAIL;

    _bool       isBlur = false;

    if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &isBlur, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_isRim", &isBlur, sizeof(_bool))))
        return E_FAIL;

    if (m_iPass == 4)
    {
        // 디졸브 텍스처 바인딩
        if (FAILED(m_pDissolveTexture->SetUp_OnShader(m_pShaderCom, "g_DissolveTexture", 99)))
            return E_FAIL;

        // 디졸브 Alpha 바인딩
        if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveAmount, sizeof(_float))))
            return E_FAIL;
    }   

    return S_OK;
}

HRESULT CLittleWolf::OnCollision(_double TimeDelta)
{
    // 죽기 직전 상태라면 충돌검사 패스
    if (m_bReadyToDie == true)
        return S_OK;

    // 몬스터끼리 밀어내기
    auto pColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
    for (auto& pDestCollider : *pColliderList)
    {
        if (pDestCollider == m_pHitColliderCom)
            continue;

        if (m_pGameInstance->Collision_Sphere(m_pHitColliderCom, pDestCollider))
        {
            CGameObject* pDestObject = pDestCollider->Get_Parent();
            CTransform* pDestTransform = pDestObject->Get_Transform();
            CCollider* pDestCollider = pDestObject->Get_HitCollider();

            _float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;
            _float fSourColliderRadius = m_pHitColliderCom->Get_Sphere()->Radius;

            // 겹친부분 크기
            _float fCollisionSize = fDestColliderRadius + fSourColliderRadius - m_pTransformCom->Get_Distance(pDestTransform);

            // 방향계산
            _vector vDir = m_pTransformCom->Get_Dir(pDestTransform);

            // 겹치부분만큼 밀려나기
            _vector vPos = m_pTransformCom->Get_Position();
            m_pTransformCom->Set_Position(vPos - vDir * fCollisionSize);
        }
    }

    return S_OK;
}

HRESULT CLittleWolf::ChangeState(_double TimeDelta)
{
    if (m_pPlayer == nullptr)
    {
        m_pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0);

        if (m_pPlayer == nullptr)
            return E_FAIL;

        m_pPlayerTransform = m_pPlayer->Get_Transform();

        if (m_pPlayerTransform == nullptr)
            return E_FAIL;
    }

    if (m_pModelCom->Get_CurrAnimIndex() == 1)
    {
        if (m_TimeAccum > m_fCoolTime)
        {
            m_pModelCom->Set_TempIndex(8);
            
            m_bReadyToAttack = false;

            m_pModelCom->Set_TargetPlaySpeed(11, m_fAnimOriginSpeed);

            m_isTurnFinish = false;

            m_isClosed = false;
        }

        return S_OK;
    }

    if (m_isBiting == true)
    {
        // 공격시 라이프타임 리밋 초기화
        m_TimeAccum_Life = 0.0;

        if(m_pModelCom->Check_CurAnim_And_Finished(11) == true)
            m_pModelCom->Set_TempIndex(8);

        auto   vPos        = XMVectorLerp(m_pTransformCom->Get_Position(), m_vTargetPos, (_float)TimeDelta * 5.0f);
        _float fDistance   = m_pTransformCom->Get_Distance(m_vTargetPos);

        if (fDistance > 2.f)
            m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pNavigationCom, TimeDelta * 5.0);

        else
        {

           m_pModelCom->Set_TempIndex(1);

            m_TimeAccum = 0.0;

            m_isBiting = false;

            Set_IsAttacking(false);

            m_TimeAccum_Turn = 0.0;

            m_TimeAccum_ChasePlayer = 0.0;
         
        }

        return S_OK;
    }

    if (m_pModelCom->Check_CurAnim_And_Finished(0) == true)
    {
        m_pModelCom->Set_TempIndex(8);
       // Set_AnimAndReset(m_pModelCom, 8);
    }

    // Spawn Motion이 끝나면 정해진 방향으로 이동
    if (m_pModelCom->Get_CurrAnimIndex() == 8)
    {
        // 위에서 아래로 떨어지는 몬스터들 땅에 도착할때까지에 처리
        if (true == m_bRain)
        {
            if (true == m_isFocusingOnPlayer)
            {
              /*  if(m_pTransformCom->Get_Distance(m_vTargetPos) <= 5.f)
                {
                    auto vDir = m_pTransformCom->Get_Dir(m_vTargetPos);
                    vDir = XMVectorSetY(vDir, 0.f);
                    m_pTransformCom->MakeSameDirection(vDir);

                    m_bRain = false;
                }

                else
                {*/

                    _bool isTurnFinish = m_pTransformCom->Turn_To_Direction(m_pTransformCom->Get_Dir(m_vTargetPos), TimeDelta * 5.0, 0.3f);
                    _bool isFinish = m_pTransformCom->Turn_To_Direction_AxisX(m_vTargetPos, TimeDelta * 3.0, 0.2f);

                    if (isFinish && isTurnFinish)
                        m_bRain = false;

                    else
                        m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
                //}
        
                

                m_TimeAccum = 0.0;
                m_TimeAccum_ChasePlayer = 0.0;
            }

            else
            {
                _float fY = XMVectorGetY(m_pTransformCom->Get_Position());

                m_pTransformCom->Go_Straight(TimeDelta * 5.0);

                if (fY < 1.5f)
                {
                    m_pTransformCom->Set_PositionY(1.5f);

					m_vTargetPos = m_pPlayerTransform->Get_Position();
                    m_vTargetPos = XMVectorSetY(m_vTargetPos, 1.5f);
					m_isFocusingOnPlayer = true;
                }
            }
            return S_OK;
        }
        // 플레이어가 범위 밖에 있을 때
        if (false == m_bReadyToAttack)
        {
            _float fY = XMVectorGetY(m_pTransformCom->Get_Position());

            if (fY < 1.5f)
            {
                m_pTransformCom->Set_PositionY(1.5f);
            }

            _float fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransform->Get_Position());

            // 10 초 경과하면 플레이어로 방향 전환
            if (m_TimeAccum_ChasePlayer > 5.0 && fDistance > 10.f)
            {
                if (m_isChasingPlayer == false)
                {
                    m_isChasingPlayer = true;
                    m_vTargetPos = m_pPlayerTransform->Get_Position();
                }

                m_TimeAccum_ChasePlayer = 0.0;
            }

            if(true == m_isChasingPlayer)
            {
                auto vDir = m_pTransformCom->Get_Dir(m_vTargetPos);

                _bool isTurnFinish = m_pTransformCom->Turn_To_Direction(vDir, TimeDelta * 5.0, 0.3f);
                _bool isTurnFinish_UpDown = m_pTransformCom->Turn_To_Direction_AxisX(m_vTargetPos, TimeDelta * 5.0, 0.3f);

                if (isTurnFinish && isTurnFinish_UpDown)
                {
                    m_isChasingPlayer = false;
                    m_TimeAccum_ChasePlayer = 0.0;
                }

                m_pTransformCom->Go_Straight(TimeDelta, m_pNavigationCom);
            }

            else
            {
                auto isFacingWall = m_pTransformCom->Go_Straight_Check_NaviEnd(TimeDelta * 3.0, m_pNavigationCom);

                // 네비에 끝을 가거나
                if (true == isFacingWall || m_TimeAccum_Turn > 3.0)
                {
                    m_pTransformCom->Turn_Left(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * 5.0);

                    if (m_TimeAccum_Turn > 3.0)
                        m_TimeAccum_Turn = 0.0;
                }

                _float fDistance = m_pTransformCom->Get_Distance(m_pPlayerTransform);

                // 플레이어가 범위 안에 있는지 체크
                if (fDistance < 15.f && fDistance > 7.f)
                {
                    auto vTargetPos = m_pPlayerTransform->Get_Position();

                    if (vecy(vTargetPos) < 1.5f)
                        vTargetPos = XMVectorSetY(vTargetPos, 1.5f);

                    m_vTargetPos = vTargetPos;

                    m_bReadyToAttack = true;
                }
            }
          
        }

        // 플레이어가 공격 범위 안에 있을 때 공격 전 준비 동작
        else
        {
            if (false == m_isTurnFinish || false == m_isClosed)
            {
                if (false == m_isTurnFinish)
                {
                    _bool isTurnFinish = m_pTransformCom->Turn_To_Direction(m_pTransformCom->Get_Dir(m_vTargetPos), TimeDelta * 5.0, 0.3f);
                    _bool isTurnFinish_UpDown = m_pTransformCom->Turn_To_Direction_AxisX(m_vTargetPos, TimeDelta * 5.0, 0.3f);

                    if(isTurnFinish == true && isTurnFinish_UpDown == true)
                        m_isTurnFinish = true;
                }

                if (false == m_isClosed)
                {
                    auto vHeight = XMVectorSetY(m_pTransformCom->Get_Position(), vecy(m_vTargetPos));

                    auto vPos = XMVectorLerp(m_pTransformCom->Get_Position(), vHeight, (_float)(TimeDelta * 5.0));

                    auto fDist = m_pTransformCom->Get_Distance(vPos);

                    if(fDist < 0.2f)
                        m_isClosed = true;

                    else
                        m_pTransformCom->Sliding_Vector(vPos, m_pTransformCom->Get_Position(), m_pNavigationCom, TimeDelta * 5.0);
                }
               
            }

            else
            {
                Set_AnimAndReset(m_pModelCom, 11);
            }
        }
    }

    // 공격 중일 시
    if (m_pModelCom->Get_CurrAnimIndex() == 11)
    {
        if (m_pModelCom->Get_CurrTimeAcc() > 26.f)
        {
            m_isBiting = true;
            m_pModelCom->Set_TargetPlaySpeed(11, 90.0);

            Set_IsAttacking(true);
        }
    }
  

    if (XMVectorGetY(m_pTransformCom->Get_Position()) < 1.5f)
    {
        m_pTransformCom->Set_PositionY(1.5f);

       // m_pTransformCom->Turn_Left(XMVector3Normalize(m_pTransformCom->Get_Right()), TimeDelta * 5.0);

        //auto vLook = XMVector3Normalize(m_pTransformCom->Get_Look());

        //vLook = XMVectorSetY(vLook, 1.5f);

        //m_pTransformCom->MakeSameDirection(vLook);
    }

    else if (XMVectorGetY(m_pTransformCom->Get_Position()) >= 51.f)
    {
        m_pTransformCom->Set_PositionY(51.f);
        
       /* auto vLook = XMVector3Normalize(m_pTransformCom->Get_Look());
        vLook = XMVectorSetY(vLook, -1.f);

        m_pTransformCom->Turn_To_Direction(vLook, TimeDelta * 10.0);*/

        //m_pTransformCom->Turn_Left(__vector(0.f, 1.f, 0.f, 0.f), TimeDelta * 5.0);
        m_pTransformCom->NowRotation(XMVector3Normalize(m_pTransformCom->Get_Right()), XMConvertToRadians(90.f));
    }

    return S_OK;
}

CLittleWolf* CLittleWolf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CLittleWolf* pInstance = new CLittleWolf(pDevice, pDeviceContext);

    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        MSGBOX("Failed to Created CLittleWolf(Prototype)!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLittleWolf::Clone(void* pArg)
{
    CLittleWolf* pInstance = new CLittleWolf(*this);

    if (FAILED(pInstance->NativeConstruct(pArg)))
    {
        MSGBOX("Failed to Clone CLittleWolf!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLittleWolf::Free()
{
    __super::Free();

    Safe_Release(m_pStateMachineCom);
    Safe_Release(m_pMonsterCoolerCom);
    Safe_Release(m_pStatusCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pHitColliderCom);
    Safe_Release(m_pAtkColliderCom);
}
