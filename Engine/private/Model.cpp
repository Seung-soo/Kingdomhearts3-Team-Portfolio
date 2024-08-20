#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Shader.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_iNumMeshContainers(rhs.m_iNumMeshContainers)
	, m_pMeshContainers(rhs.m_pMeshContainers)
	, m_eModelType(rhs.m_eModelType)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_TransformMatrix(rhs.m_TransformMatrix)
	//, m_iNumAnimations(rhs.m_iNumAnimations)				
	//, m_Animations(rhs.m_Animations)							나의 역사
	//, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
	, m_tAssimpDesc(rhs.m_tAssimpDesc)
{
	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(pMaterial->pTexture[i]);
	}

	if (nullptr != m_pMeshContainers)
	{
		for (_uint i = 0; i < m_iNumMaterials; i++)
		{
			for (auto& pMeshContainer : m_pMeshContainers[i])
				Safe_AddRef(pMeshContainer);
		}
	}

	/*for (auto& pAnimation : m_Animations)
	{
		Safe_AddRef(pAnimation);
	} 승수의 역사 2 */
}

_bool CModel::CheckStop()
{
	return m_Animations[m_iCurrAnim]->Get_Stopped();
}

_bool CModel::CheckStop_Input(_uint iAnimationIndex)
{
	return m_Animations[iAnimationIndex]->Get_Stopped();
}

_bool CModel::CheckFinished()
{
	return m_Animations[m_iCurrAnim]->Get_Finished();
}

_bool CModel::CheckFinished_Input(_uint iAnimationIndex)
{
	return m_Animations[iAnimationIndex]->Get_Finished();
}

_uint CModel::Get_TempIndex_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return pair->second.iTemp_Index;
}

_bool CModel::Check_CurAnim_And_Finished(_uint iAnimIndex)
{
	return ((Get_CurrAnimIndex() == iAnimIndex) && (Get_CurrFinished()));
}

_uint CModel::Get_CurrAnimIndex_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return pair->second.iCurr_Index;
}

char * CModel::Get_CurrName_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return nullptr;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_Name();
}

_double CModel::Get_CurrDuration_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_Duration();
}

_double CModel::Get_CurrPlaySpeed_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_PlaySpeed();
}

void CModel::Set_CurrPlaySpeed_EachBody(BODY_ENUM eBodyEnum, _double dPlaySpeed)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iCurr_Index]->Set_PlaySpeed(dPlaySpeed);
}

void CModel::Set_CurrPlaySpeed_Rate_EachBody(BODY_ENUM eBodyEnum, _double dPlaySpeedRate)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iCurr_Index]->Set_PlaySpeed_Rate(dPlaySpeedRate);
}

_double CModel::Get_CurrTimeAcc_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_TimeAcc();
}

void CModel::Set_CurrTimeAcc_EachBody(BODY_ENUM eBodyEnum, _double dTimeAcc)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Set_TimeAcc(dTimeAcc);
}

CAnimation::ANIM_TYPE CModel::Get_CurrAnimType_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return CAnimation::ANIM_TYPE::ANIM_END;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_AnimType();
}

void CModel::Set_CurrAnimType_EachBody(BODY_ENUM eBodyEnum, CAnimation::ANIM_TYPE eAnimType)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iCurr_Index]->Set_AnimType(eAnimType);
}

_bool CModel::Get_CurrFinished_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_Finished();
}

void CModel::Set_CurrFinished_EachBody(BODY_ENUM eBodyEnum, _bool bFinished)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iCurr_Index]->Set_Finished(bFinished);
}

_bool CModel::Get_CurrStopped_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_Stopped();
}

void CModel::Set_CurrStopped_EachBody(BODY_ENUM eBodyEnum, _bool bStopped)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iCurr_Index]->Set_Stopped(bStopped);
}

_bool CModel::Get_CurrSeparated_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_Separated();
}

void CModel::Set_CurrSeparated_EachBody(BODY_ENUM eBodyEnum, _bool bSeparated)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iCurr_Index]->Set_Separated(bSeparated);
}

_float CModel::Get_CurrMovementAmountXZ_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_MovementAmountXZ();
}

_float CModel::Get_CurrMovementAmountY_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_MovementAmountY();
}

_float CModel::Get_CurrMovementRatioXZ(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_MovementRatioXZ();
}

void CModel::Set_CurrMovementRatioXZ(BODY_ENUM eBodyEnum, _float fRatioXZ)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return;
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Set_MovementRatioXZ(fRatioXZ);
}

_float CModel::Get_CurrMovementRatioY(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_MovementRatioY();
}

void CModel::Set_CurrMovementRatioY(BODY_ENUM eBodyEnum, _float fRatioY)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return;
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Set_MovementRatioY(fRatioY);
}

_uint CModel::Get_PrevAnimIndex_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return pair->second.iPrev_Index;
}

char * CModel::Get_PrevName_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return nullptr;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_Name();
}

_double CModel::Get_PrevDuration_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_Duration();
}

_double CModel::Get_PrevPlaySpeed_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_PlaySpeed();
}

void CModel::Set_PrevPlaySpeed_EachBody(BODY_ENUM eBodyEnum, _double dPlaySpeed)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Set_PlaySpeed(dPlaySpeed);
}

_double CModel::Get_PrevTimeAcc_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return -1.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_TimeAcc();
}

void CModel::Set_PrevTimeAcc_EachBody(BODY_ENUM eBodyEnum, _double dTimeAcc)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Set_TimeAcc(dTimeAcc);
}

CAnimation::ANIM_TYPE CModel::Get_PrevAnimType_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return CAnimation::ANIM_TYPE::ANIM_END;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_AnimType();
}

void CModel::Set_PrevAnimType_EachBody(BODY_ENUM eBodyEnum, CAnimation::ANIM_TYPE eAnimType)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Set_AnimType(eAnimType);
}

_bool CModel::Get_PrevFinished_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_Finished();
}

void CModel::Set_PrevFinished_EachBody(BODY_ENUM eBodyEnum, _bool bFinished)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iPrev_Index]->Set_Finished(bFinished);
}

_bool CModel::Get_PrevStopped_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_Stopped();
}

void CModel::Set_PrevStopped_EachBody(BODY_ENUM eBodyEnum, _bool bStopped)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iPrev_Index]->Set_Stopped(bStopped);
}

_bool CModel::Get_PrevSeparated_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_Separated();
}

void CModel::Set_PrevSeparated_EachBody(BODY_ENUM eBodyEnum, _bool bSeparated)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	m_Animations[pair->second.iPrev_Index]->Set_Separated(bSeparated);
}

_float CModel::Get_PrevMovementAmountXZ_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_MovementAmountXZ();
}

_float CModel::Get_PrevMovementAmountY_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_MovementAmountY();
}

_float CModel::Get_PrevMovementRatioXZ(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_MovementRatioXZ();
}

void CModel::Set_PrevMovementRatioXZ(BODY_ENUM eBodyEnum, _float fRatioXZ)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return;
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Set_MovementRatioXZ(fRatioXZ);
}

_float CModel::Get_PrevMovementRatioY(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return _float();
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Get_MovementRatioY();
}

void CModel::Set_PrevMovementRatioY(BODY_ENUM eBodyEnum, _float fRatioY)
{
	if (0 >= m_AnimationGroup.size())
	{
		BREAKPOINT;
		return;
	}

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iPrev_Index]->Set_MovementRatioY(fRatioY);
}

_bool CModel::CheckStop_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_Stopped();
}

_bool CModel::CheckFinished_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return m_Animations[pair->second.iCurr_Index]->Get_Finished();
}

_bool CModel::Get_AnimationChanged_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return false;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return pair->second.bAnimationChanged;
}

_double CModel::Get_LerpTimer_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return 0.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return pair->second.dLerpTimer;
}

_double CModel::Get_LerpDuration_EachBody(BODY_ENUM eBodyEnum)
{
	if (0 >= m_AnimationGroup.size())
		return 0.0;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	return pair->second.dLerpDuration;
}

void CModel::Set_LerpDuration_EachBody(BODY_ENUM eBodyEnum, _double dDuration)
{
	if (0 >= m_AnimationGroup.size())
		return;

	auto pair = m_AnimationGroup.find(eBodyEnum);

	pair->second.dLerpDuration = dDuration;
}

const char * CModel::Get_RootName()
{
	return m_HierarchyNodes[0]->Get_Name();
}

_int CModel::Get_NodeIndexByName(const char * pName)
{
	_int iIndex = INT_MAX;
	for (auto& pNode : m_HierarchyNodes)
	{
		if (!strcmp(pName, pNode->Get_Name()))
		{
			iIndex = pNode->Get_NodeIndex();
			break;
		}
	}

	return iIndex;
}

CModel::BONEMATRIX_PTR CModel::Get_BoneMatrixPtr(const char * pBoneName)
{
	BONEMATRIX_PTR		BoneMatrixPtr;
	ZeroMemory(&BoneMatrixPtr, sizeof(BONEMATRIX_PTR));

	CHierarchyNode*		pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return BONEMATRIX_PTR();

	BoneMatrixPtr.pOffsetMatrix = pNode->Get_OffsetFloat4x4();
	BoneMatrixPtr.pCombinedMatrix = pNode->Get_CombinedTransformationFloat4x4();

	return BoneMatrixPtr;
}

void CModel::Set_RootNodeIndex(_uint iRootNodeIndex)
{
	for (auto& animation : m_Animations)
		animation->Set_RootNodeIndex(iRootNodeIndex);
}

void CModel::Set_RootNodeY(_uint iRootNodeY)
{
	for (auto& animation : m_Animations)
	{
		animation->Set_RootNodeY(iRootNodeY);
		animation->Set_RootChannelY();
	}
}

HRESULT CModel::NativeConstruct_Prototype(const _tchar * pModelFilePath, _fmatrix TransformMatrix)
{
	CHECK_FAILED(Load_Data(pModelFilePath, TransformMatrix));

	// 머테리얼 세팅
	CHECK_FAILED(Ready_Materials(m_tAssimpDesc.iNumMaterials, &m_tAssimpDesc.Materials));

	if (TYPE_ANIM == m_tAssimpDesc.iModelType)
	{
		//레디 하이어라키
		CHECK_FAILED(Ready_HierarchyNodes(m_tAssimpDesc.iNumNodes, &m_tAssimpDesc.HierarchyNodes));

		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
		{
			return pSour->Get_Depth() < pDest->Get_Depth();
		});
	}

	// 매쉬컨테이너
	CHECK_FAILED(Ready_MeshContainers(m_tAssimpDesc.iNumMeshContainers, &m_tAssimpDesc.MeshContainers));

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		for (auto& pMeshContainer : m_pMeshContainers[i])
		{
			pMeshContainer->Ready_VertexIndexBuffer((MODELTYPE)m_tAssimpDesc.iModelType, TransformMatrix);
		}
	}

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	m_eModelType = (MODELTYPE)m_tAssimpDesc.iModelType;

	CHECK_FAILED(Ready_HierarchyNodes(m_tAssimpDesc.iNumNodes, &m_tAssimpDesc.HierarchyNodes));


	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	// 애니메이션 정보 저장
	if (TYPE_ANIM == m_tAssimpDesc.iModelType)
		CHECK_FAILED(Ready_Animation(m_tAssimpDesc.iNumAnimations, &m_tAssimpDesc.Animations));

	if (FAILED(Link_ChannelToNode()))
	{
		MSGBOX("Link_ChannelToNode returned E_FAIL in CModel::NativeConstruct");
		return E_FAIL;
	}

	m_dLerpTimer = 0.0;
	m_dLerpDuration = 0.1;

	return S_OK;
}

HRESULT CModel::Set_TempIndex(_uint iAnimIndex)
{
	RETURN_FAIL(if (iAnimIndex >= m_iNumAnimations));

	m_iTempAnim = iAnimIndex;

	return S_OK;
}

HRESULT CModel::SetUp_AnimIndex(_uint iAnimIndex)	// 애니메이션을 바꿔주는 함수
{
	// 들어온 애니메이션 번호가 내가 가지고 있는 애니메이션의 수보다 많을 때
	RETURN_FAIL(if (iAnimIndex >= m_iNumAnimations));

	if (m_iCurrAnim != iAnimIndex)
	{
		m_iPrevAnim = m_iCurrAnim;
		m_dLerpTimer = 0.0;

		m_bAnimationChanged = true;
	}

	m_iCurrAnim = iAnimIndex;

	return S_OK;
}

HRESULT CModel::SetUp_AnimIndex_Auto(_uint iAnimIndex)
{
	// once animation list가 세팅되지 않았음.
	RETURN_FAIL(if (0 >= m_OnceAnimations.size()));

	RETURN_FAIL(if (iAnimIndex >= m_iNumAnimations));

	if (m_iCurrAnim != iAnimIndex)
	{
		m_iPrevAnim = m_iCurrAnim;
		m_Animations[m_iPrevAnim]->PrevPos_Zero();
		m_dLerpTimer = 0.0;

		if (true == Check_Lerp(iAnimIndex))
			m_bAnimationChanged = true;
		else
		{
			m_bAnimationChanged = false;
			m_bResetPrevious = true;
		}
	}

	m_iCurrAnim = iAnimIndex;

	if(true == Check_Once(m_iCurrAnim))
		m_Animations[m_iCurrAnim]->Set_AnimType(CAnimation::ANIM_ONCE);

	return S_OK;
}

_bool CModel::Check_ProgressRate(_float fRate)
{
	if (true == m_Animations[m_iCurrAnim]->Get_Stopped())
		return true;

	_double time_acc = m_Animations[m_iCurrAnim]->Get_TimeAcc();
	_double duration = m_Animations[m_iCurrAnim]->Get_Duration();

	if (time_acc >= duration * fRate)
		return true;

	return false;
}

_bool CModel::Check_ProgressRate_Input(_uint iAnimIndex, _float fRate)
{
	if (true == m_Animations[iAnimIndex]->Get_Stopped())
		return true;

	_double time_acc = m_Animations[iAnimIndex]->Get_TimeAcc();
	_double duration = m_Animations[iAnimIndex]->Get_Duration();

	if (time_acc >= duration * fRate)
		return true;

	return false;
}

_bool CModel::Check_TempAndCurr()
{
	if (m_iTempAnim == m_iCurrAnim)
		return true;
	else
		return false;
}

HRESULT CModel::Set_TempIndex_EachBody(_uint iBodyIndex, _uint iAnimIndex, _bool bTempWholeBody)
{
	if (iBodyIndex >= m_iNumberOfBodyPart)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	auto pair = m_AnimationGroup.find(iBodyIndex);
	pair->second.iTemp_Index = iAnimIndex;
	pair->second.bTempWholeBody = bTempWholeBody;

	return S_OK;
}

HRESULT CModel::PlayAnimation_EachBody(_uint iBodyIndex, _uint iAnimationIndex, CAnimation::ANIM_TYPE eAnimType, _bool bWholeBody)
{
	if (iBodyIndex >= m_iNumberOfBodyPart)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 부위 별로 애니메이션 변경
	if (false == bWholeBody)
	{
		auto pair = m_AnimationGroup.find(iBodyIndex);

		// 변경하고자하는 부위의 현재 애니메이션 인덱스가 입력받은 인덱스와 '다를 때'
		if (pair->second.iCurr_Index != iAnimationIndex)
		{
			pair->second.iPrev_Index = pair->second.iCurr_Index;
			pair->second.dLerpTimer = 0.0;

			pair->second.bAnimationChanged = true;
		}

		// 현재 애니메이션 인덱스 갱신, 애니메이션 타입 갱신
		pair->second.iCurr_Index = iAnimationIndex;
		m_Animations[pair->second.iCurr_Index]->Set_AnimType(eAnimType);
	}

	// 모든 신체부위의 애니메이션 변경
	else
	{
		for (auto& pair : m_AnimationGroup)
		{
			// 현재 애니메이션 인덱스가 입력받은 인덱스와 '다를 때'
			if (pair.second.iCurr_Index != iAnimationIndex)
			{
				pair.second.iPrev_Index = pair.second.iCurr_Index;
				pair.second.dLerpTimer = 0.0;

				pair.second.bAnimationChanged = true;
			}

			// 현재 애니메이션 인덱스 갱신, 애니메이션 타입 갱신
			pair.second.iCurr_Index = iAnimationIndex;
			m_Animations[pair.second.iCurr_Index]->Set_AnimType(eAnimType);
		}
	}

	return S_OK;
}

HRESULT CModel::PlayAnimation_EachBody_Auto(_uint iBodyIndex, _uint iAnimationIndex, _bool bWholeBody)
{
	// once animation list가 세팅되지 않았음.
	RETURN_FAIL(if (0 >= m_OnceAnimations.size()));

	// 부위 별로 애니메이션 변경
	if (false == bWholeBody)
	{
		auto pair = m_AnimationGroup.find(iBodyIndex);

		// 변경하고자하는 부위의 현재 애니메이션 인덱스가 입력받은 인덱스와 '다를 때'
		if (pair->second.iCurr_Index != iAnimationIndex)
		{
			pair->second.iPrev_Index = pair->second.iCurr_Index;
			m_Animations[pair->second.iPrev_Index]->PrevPos_Zero();
			pair->second.dLerpTimer = 0.0;

			if (true == Check_Lerp(iAnimationIndex))
				pair->second.bAnimationChanged = true;
			else
			{
				pair->second.bAnimationChanged = false;
				pair->second.bResetPrevious = true;
			}
		}

		// 현재 애니메이션 인덱스 갱신, 애니메이션 타입 갱신
		pair->second.iCurr_Index = iAnimationIndex;

		if(true == Check_Once(pair->second.iCurr_Index))
			m_Animations[pair->second.iCurr_Index]->Set_AnimType(CAnimation::ANIM_ONCE);
		else
			m_Animations[pair->second.iCurr_Index]->Set_AnimType(CAnimation::ANIM_LOOP);
	}

	// 모든 신체부위의 애니메이션 변경
	else
	{
		for (auto& pair : m_AnimationGroup)
		{
			// 현재 애니메이션 인덱스가 입력받은 인덱스와 '다를 때'
			if (pair.second.iCurr_Index != iAnimationIndex)
			{
				pair.second.iPrev_Index = pair.second.iCurr_Index;
				m_Animations[pair.second.iPrev_Index]->PrevPos_Zero();
				pair.second.dLerpTimer = 0.0;

				if (true == Check_Lerp(iAnimationIndex))
					pair.second.bAnimationChanged = true;
				else
					pair.second.bAnimationChanged = false;
			}

			// 현재 애니메이션 인덱스 갱신, 애니메이션 타입 갱신
			pair.second.iCurr_Index = iAnimationIndex;

			if (true == Check_Once(pair.second.iCurr_Index))
				m_Animations[pair.second.iCurr_Index]->Set_AnimType(CAnimation::ANIM_ONCE);
			else
				m_Animations[pair.second.iCurr_Index]->Set_AnimType(CAnimation::ANIM_LOOP);
		}
	}

	return S_OK;
}

_bool CModel::Check_ProgressRate_EachBody(_uint iBodyIndex, _float fRate)
{
	auto pair = m_AnimationGroup.find(iBodyIndex);

	if (true == m_Animations[pair->second.iCurr_Index]->Get_Stopped())
		return true;

	_double time_acc = m_Animations[pair->second.iCurr_Index]->Get_TimeAcc();
	_double duration = m_Animations[pair->second.iCurr_Index]->Get_Duration();

	_double target_rate = (duration * 0.01) * (_double)fRate;

	if (time_acc >= target_rate)
		return true;
	else
		return false;
}

_bool CModel::Check_TempAndCurr_EachBody(_uint iBodyIndex)
{
	auto pair = m_AnimationGroup.find(iBodyIndex);

	if (pair->second.iTemp_Index == pair->second.iCurr_Index)
		return true;
	else
		return false;
}

void CModel::ResetAnimation(_uint iAnimationIndex)
{
	m_Animations[iAnimationIndex]->Reset_Animation();
}

void CModel::Calculate_MovementAmount(_uint iAnimationIndex)
{
	m_Animations[iAnimationIndex]->Calculate_MovementAmount();
}

void CModel::Calculate_MovementAmount_All()
{
	for (auto& animation : m_Animations)
		animation->Calculate_MovementAmount();
}

HRESULT CModel::Update_CombinedTransformationMatrices(_double TimeDelta, class CTransform* pTransform, class CNavigation* pNavigation)
{
	if (TYPE_ANIM != m_eModelType)
		return E_FAIL;

	// 분리 애니메이션 사용
	if (true == m_bSeparatedAnim)
	{
		// 현재 재생 중인 애니메이션의 인덱스를 모아둘 vector
		vector<_uint> playing_animation_indices;

		for (auto& pair : m_AnimationGroup)
		{
			PlayAnimation_EachBody_Auto(pair.first, pair.second.iTemp_Index, pair.second.bTempWholeBody);

			if (true == pair.second.bAnimationChanged)
			{
				// 보간 진행
				Animation_Lerp_EachBody(pair.first, TimeDelta);
			}

			else
			{
				if (true == pair.second.bResetPrevious)
				{
					m_Animations[pair.second.iPrev_Index]->Reset_Animation_EachBody(pair.first);
					pair.second.bResetPrevious = false;
				}

				_uint body_index_of_this_loop = pair.first;						// 이번 루프 부위의 index
				_uint curr_anim_index_of_this_loop = pair.second.iCurr_Index;	// 이번 루프 부위의 curr_anim_index

				// 애니메이션이 가지고 있는 재생부위 목록에 현재 부위의 인덱스를 저장
				m_Animations[curr_anim_index_of_this_loop]->Push_BodyIndex(body_index_of_this_loop);

				/* 각 애니메이션은 어느 부위를 재생해야할지 리스트를 갖고있음. 분리 애니메이션을 사용할 때
				재생 부위 리스트에 신체부위를 추가하면 추가된 신체부위에 해당하는 채널들에 한하여 애니메이션이 재생된다. */

				// 재생 중인 애니메이션 인덱스 보관
				// 중복되는 애니메이션인지 확인하고 push한다. 이 과정이 없을 경우 한 애니메이션이 여러번 업데이트 된다.
				if (0 == (_int)playing_animation_indices.size())
					playing_animation_indices.push_back(curr_anim_index_of_this_loop);
				else
				{
					_bool same_index = false;
					for (_uint i = 0; i < playing_animation_indices.size(); ++i)
					{
						if (playing_animation_indices[i] == curr_anim_index_of_this_loop)
						{
							same_index = true;
							break;
						}
					}

					if (false == same_index)
						playing_animation_indices.push_back(curr_anim_index_of_this_loop);
				}
			}
		}

		for (auto& playing_index : playing_animation_indices)
			m_Animations[playing_index]->Update_TransformMatrices(TimeDelta, pTransform, pNavigation);
	}

	// 단일 애니메이션 사용
	else
	{
		SetUp_AnimIndex_Auto(m_iTempAnim);
		
		if (true == m_bAnimationChanged)
		{
			Animation_Lerp(TimeDelta);
		}

		else
		{
			if (true == m_bResetPrevious)
			{
				m_Animations[m_iPrevAnim]->Reset_Animation();
				m_bResetPrevious = false;
			}

			m_Animations[m_iCurrAnim]->Update_TransformMatrices(TimeDelta, pTransform, pNavigation);
		}
	}

	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Update_CombinedTransformationMatrix();

	return S_OK;
}

HRESULT CModel::Bind_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pValueName)
{
	if (iMaterialIndex >= m_iNumMaterials)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CTexture*	pTexture = m_Materials[iMaterialIndex]->pTexture[eTextureType];
	if (nullptr == pTexture)
	{
		return E_FAIL;
	}

	return pTexture->SetUp_OnShader(pShader, pValueName);
}

HRESULT CModel::Ready_BoneMatricesContainer()
{
	if (nullptr == m_pMeshContainersBoneMatrices)
		m_pMeshContainersBoneMatrices = new vector<_float4x4*>[m_iNumMaterials];
	else
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Ready_BoneMatrices(_uint iMaterialIndex, _uint iBoneSize)
{
	RETURN_FAIL(if (iMaterialIndex >= m_iNumMaterials || nullptr == m_pMeshContainers || nullptr == m_pMeshContainersBoneMatrices));

	for (auto& pMeshContainer : m_pMeshContainers[iMaterialIndex])	// 이 머테리얼을 사용하는 매쉬 컨테이너를 순회하며
	{
		_float4x4* pBoneMatrices = pMeshContainer->Ready_SetUp_Matrices(iBoneSize, &m_HierarchyNodes, XMLoadFloat4x4(&m_TransformMatrix));
		m_pMeshContainersBoneMatrices[iMaterialIndex].push_back(pBoneMatrices);
	}

	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName, vector<MESHDESC*>* pMeshDescVector)
{
	RETURN_FAIL(if (iMaterialIndex >= m_iNumMaterials || nullptr == m_pMeshContainers))

	_float4x4		BoneMatrices[280];	// 셰이더에 넘겨줄 배열 선언

	for (auto& pMeshContainer : m_pMeshContainers[iMaterialIndex])	// 이 머테리얼을 사용하는 매쉬 컨테이너를 순회하며
	{
		if (true == m_bSeparatedRender)
		{
			_uint mesh_index = pMeshContainer->Get_ContainerIndex();

			_bool skip = false;
			for (auto skip_index : m_RenderSkipList)
			{
				if (skip_index == mesh_index)
					skip = true;
			}

			if (true == skip)
				continue;
		}

		if (TYPE_ANIM == m_eModelType)	// 애니메이션 타입일 때
		{
			ZeroMemory(&BoneMatrices, sizeof(_float4x4) * 280);

			pMeshContainer->SetUp_Matrices(BoneMatrices, &m_HierarchyNodes, XMLoadFloat4x4(&m_TransformMatrix));	// 본 매트릭스를 채워줌
			pShader->Set_RawValue(pBoneValueName, BoneMatrices, sizeof(_float4x4) * 280);	// 셰이더에 본 매트릭스를 던져줌

			if (nullptr != pMeshDescVector)
			{
				MESHDESC* pMeshDesc = new MESHDESC;
				pMeshDesc->pMeshContainer = pMeshContainer;
				pMeshDesc->pBoneMatrices = new _float4x4[280];
				memcpy(pMeshDesc->pBoneMatrices, BoneMatrices, sizeof(_float4x4) * 280);
				pMeshDescVector->push_back(pMeshDesc);
			}
		}

		if (nullptr != pMeshContainer)
			pMeshContainer->Render(pShader, iPassIndex);
	}



	return S_OK;
}

HRESULT CModel::Render_Heavy(CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName, vector<MESHDESC*>* pMeshDescVector)
{
	RETURN_FAIL(if (iMaterialIndex >= m_iNumMaterials || nullptr == m_pMeshContainers))

	_float4x4		BoneMatrices[450];	// 셰이더에 넘겨줄 배열 선언

	for (auto& pMeshContainer : m_pMeshContainers[iMaterialIndex])	// 이 머테리얼을 사용하는 매쉬 컨테이너를 순회하며
	{
		if (true == m_bSeparatedRender)
		{
			_uint mesh_index = pMeshContainer->Get_ContainerIndex();

			_bool skip = false;
			for (auto skip_index : m_RenderSkipList)
			{
				if (skip_index == mesh_index)
					skip = true;
			}

			if (true == skip)
				continue;
		}

		if (TYPE_ANIM == m_eModelType)	// 애니메이션 타입일 때
		{
			ZeroMemory(&BoneMatrices, sizeof(_float4x4) * 450);

			pMeshContainer->SetUp_Matrices(BoneMatrices, &m_HierarchyNodes, XMLoadFloat4x4(&m_TransformMatrix));	// 본 매트릭스를 채워줌
			pShader->Set_RawValue(pBoneValueName, BoneMatrices, sizeof(_float4x4) * 450);	// 셰이더에 본 매트릭스를 던져줌

			if (nullptr != pMeshDescVector)
			{
				MESHDESC* pMeshDesc = new MESHDESC;
				pMeshDesc->pMeshContainer = pMeshContainer;
				pMeshDesc->pBoneMatrices = new _float4x4[450];
				memcpy(pMeshDesc->pBoneMatrices, BoneMatrices, sizeof(_float4x4) * 450);
				pMeshDescVector->push_back(pMeshDesc);
			}
		}

		if (nullptr != pMeshContainer)
			pMeshContainer->Render(pShader, iPassIndex);
	}
	return S_OK;
}

HRESULT CModel::Render_OtherModel(CShader * pShader, _uint iPassIndex, _uint iMaterialIndex, const char * pBoneValueName, _uint iSize)
{
	RETURN_FAIL(if (iMaterialIndex >= m_iNumMaterials || nullptr == m_pMeshContainers || nullptr == m_pMeshContainersBoneMatrices));

	_uint iIndex = 0;

	for (auto& pMeshContainer : m_pMeshContainers[iMaterialIndex])	// 이 머테리얼을 사용하는 매쉬 컨테이너를 순회하며
	{
		pShader->Set_RawValue(pBoneValueName, m_pMeshContainersBoneMatrices[iMaterialIndex][iIndex], sizeof(_float4x4) * iSize);

		if (nullptr != pMeshContainer)
			pMeshContainer->Render(pShader, iPassIndex);

		++iIndex;
	}

	return S_OK;
}

HRESULT CModel::Ready_MeshContainers(_uint iNumMeshContainers, vector<MESHCONTAINERDESC>* pMeshContainers)
{
	m_iNumMeshContainers = iNumMeshContainers;

	m_pMeshContainers = new MESHCONTAINERS[m_iNumMaterials];

	for (_uint i = 0; i < m_iNumMeshContainers; i++)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, &((*pMeshContainers)[i]), i);
		if (nullptr == pMeshContainer)
		{
			BREAKPOINT;
			return E_FAIL;
		}
		for (_uint j = 0; j < (*pMeshContainers)[i].iNumBones; j++)	// 해당 매쉬의 뼈의 갯수만큼 루프
		{
			pMeshContainer->Add_HierarChyNode((*pMeshContainers)[i].HierarchyNodeIndices[j]);	// 받아온 인덱스를 노드 컨테이너에 추가
		}

		m_pMeshContainers[(*pMeshContainers)[i].iMeshIndex].push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(_uint iNumMaterials, vector<MATERIAL>* Materials)
{
	m_iNumMaterials = iNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		MESHMATERIALDESC*	pMeshMaterialDesc = new MESHMATERIALDESC;
		ZeroMemory(pMeshMaterialDesc, sizeof(MESHMATERIALDESC));

		for (_uint j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			if (!lstrcmp(TEXT(""), (*Materials)[i].wszTexturePath[j]))
			{
				continue;
			}

			pMeshMaterialDesc->pTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, (*Materials)[i].wszTexturePath[j]);
			if (nullptr == pMeshMaterialDesc->pTexture[j])
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}

		m_Materials.push_back(pMeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(_uint iNumNodes, vector<HIERARCHYNODEDESC>* HierarchyNodes)
{
	for (_uint i = 0; i < iNumNodes; i++)
	{
		CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create((*HierarchyNodes)[i].m_szName, (*HierarchyNodes)[i].m_TransformationMatrix, (*HierarchyNodes)[i].iDepth, (*HierarchyNodes)[i].iParentIndex, (*HierarchyNodes)[i].iNodeIndex, (*HierarchyNodes)[i].m_OffsetMatrix, this);

		m_HierarchyNodes.push_back(pHierarchyNode);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation(_uint iNumAnimation, vector<ANIMATIONDESC>* pAnimationDesc)
{
	m_iNumAnimations = iNumAnimation;

	for (_uint i = 0; i < m_iNumAnimations; i++)	// 애니메이션 갯수만큼 루프
	{
		CAnimation*		pAnimation = CAnimation::Create((*pAnimationDesc)[i].szName, (*pAnimationDesc)[i].Duration, (*pAnimationDesc)[i].TicksPerSecond);
		if (nullptr == pAnimation)
		{
			BREAKPOINT;
			return E_FAIL;
		}

		pAnimation->Reserve((*pAnimationDesc)[i].iNumChannels);		// 현재 해당 애니메이션의 뼈의 갯수만큼 공간 할당

		for (_uint j = 0; j < (*pAnimationDesc)[i].iNumChannels; j++)	// 뼈의 갯수만큼 루프
		{
			CChannel*	pChannel = CChannel::Create((*pAnimationDesc)[i].Channels[j].szName);	// 뼈의 정보를 토대로 뼈 객체 생성
			if (nullptr == pChannel)
			{
				BREAKPOINT;
				return E_FAIL;
			}

			// 키 프레임의 총 갯수는 스케일, 로테이션, 포지션중에 가장 많은 프레임 수이다.
			// 보통 로테이션, 포지션은 거의 가지고 있고 스케일은 맨 앞에 하나정도 올 때도 많다.
			_uint		iNumKeyFrames = (*pAnimationDesc)[i].Channels[j].iNumKeyFrames;

			pChannel->Reserve(iNumKeyFrames);	// 키 프레임의 총 갯수만큼 저장공간 할당

			for (_uint k = 0; k < iNumKeyFrames; k++)	// 키 프레임 수만큼 루프
			{
				KEYFRAME*		pKeyFrame = new KEYFRAME;	// 키 프레임 정보 구조체 동적 할당
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				memcpy(pKeyFrame, &(*pAnimationDesc)[i].Channels[j].KeyFrames[k], sizeof(KEYFRAME));

				pChannel->Add_KeyFrame(pKeyFrame);		// 키 프레임 컨테이너에 추가
			}

			pAnimation->Add_Channel(pChannel);			// 뼈 컨테이너에 추가
		}
		m_Animations.push_back(pAnimation);				// 애니메이션 컨테이너에 추가
	}

	return S_OK;
}

_uint CModel::Find_ContainerIndex(const char * pName)
{
	_uint index = UINT_MAX;
	for (auto& container : *m_pMeshContainers)
	{
		if (!strcmp(pName, container->Get_ContainerName()))
		{
			index = container->Get_ContainerIndex();
			break;
		}
	}

	return index;
}

void CModel::Add_SkipList(_uint iIndex)
{
	for (auto& skip_index : m_RenderSkipList)
	{
		if (skip_index == iIndex)
			return;
	}

	m_RenderSkipList.push_back(iIndex);

	if (false == m_bSeparatedRender)
		m_bSeparatedRender = true;
}

void CModel::Rmv_SkipList(_uint iIndex)
{
	if (false == m_bSeparatedRender)
		return;

	auto iter = m_RenderSkipList.begin();
	for (auto& skip_index : m_RenderSkipList)
	{
		if ((*iter) == iIndex)
		{
			m_RenderSkipList.erase(iter);
			break;
		}
		else
			iter++;
	}
}

void CModel::Clear_SkipList()
{
	if (false == m_bSeparatedRender)
		return;

	m_RenderSkipList.clear();
	m_bSeparatedRender = false;
}

_bool CModel::Check_Once(_uint iAnimIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return false;

	for (auto& once_index : m_OnceAnimations)
	{
		if (once_index == iAnimIndex)
			return true;
	}

	return false;
}

void CModel::Add_OnceList(_uint iAnimIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return;

	for (auto& once_index : m_OnceAnimations)
	{
		if (once_index == iAnimIndex)
			return;
	}

	m_OnceAnimations.push_back(iAnimIndex);
	m_Animations[iAnimIndex]->Set_AnimType(CAnimation::ANIM_ONCE);
}

void CModel::Rmv_OnceList(_uint iAnimIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return;

	auto iter = m_OnceAnimations.begin();
	for (auto& once_index : m_OnceAnimations)
	{
		if ((*iter) == iAnimIndex)
		{
			m_Animations[(*iter)]->Set_AnimType(CAnimation::ANIM_LOOP);
			m_OnceAnimations.erase(iter);
			break;
		}
		else
			iter++;
	}
}

void CModel::Add_OnceListAll()
{
	if (TYPE_ANIM != m_eModelType)
		return;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		m_OnceAnimations.push_back(i);
		m_Animations[i]->Set_AnimType(CAnimation::ANIM_ONCE);
	}
}

_bool CModel::Check_Lerp(_uint iAnimIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return false;

	for (auto& lerp_index : m_LerpAnimations)
	{
		if (lerp_index == iAnimIndex)
			return true;
	}

	return false;
}

void CModel::Add_LerpList(_uint iAnimIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return;

	for (auto& lerp_index : m_LerpAnimations)
	{
		if (lerp_index == iAnimIndex)
			return;
	}

	m_LerpAnimations.push_back(iAnimIndex);
}

void CModel::Rmv_LerpList(_uint iAnimIndex)
{
	if (TYPE_ANIM != m_eModelType)
		return;

	auto iter = m_LerpAnimations.begin();
	for (auto& lerp_index : m_LerpAnimations)
	{
		if ((*iter) == iAnimIndex)
		{
			m_LerpAnimations.erase(iter);
			break;
		}
		else
			iter++;
	}
}

void CModel::Add_LerpListAll()
{
	if (TYPE_ANIM != m_eModelType)
		return;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		m_LerpAnimations.push_back(i);
	}
}

void CModel::Animation_Lerp(_double TimeDelta)
{
	if (m_dLerpTimer >= m_dLerpDuration)
	{
		m_dLerpTimer = 0.0;

		m_bAnimationChanged = false;
		m_bResetPrevious = true;

		return;
	}

	_double ratio = m_dLerpTimer / m_dLerpDuration;

	const vector<CChannel*>* prev_anim_channels = m_Animations[m_iPrevAnim]->Get_Channels();
	const vector<CChannel*>* curr_anim_channels = m_Animations[m_iCurrAnim]->Get_Channels();

	_bool played_at_least_once = m_Animations[m_iPrevAnim]->Get_Played();

	_vector from_scale, to_scale, result_scale;
	_vector from_rotation, to_rotation, result_rotation;
	_vector from_position, to_position, result_position;

	_uint number_of_channel_prev = (_uint)(*prev_anim_channels).size();
	_uint number_of_channel_curr = (_uint)(*curr_anim_channels).size();

	// case 1. 이전 애니메이션의 채널 개수가 현재 애니메이션의 채널 개수보다 작거나 같은 경우
	if (number_of_channel_prev <= number_of_channel_curr)
	{
		for (_uint i = 0; i < number_of_channel_prev; ++i)
		{
			if (true == played_at_least_once)
			{
				KEYFRAME from_keyframe = (*prev_anim_channels)[i]->Get_LastUsedKeyFrame();

				from_scale = XMLoadFloat3(&from_keyframe.vScale);
				from_rotation = XMLoadFloat4(&from_keyframe.vRotation);
				from_position = XMLoadFloat3(&from_keyframe.vPosition);
			}

			else
			{
				const vector<KEYFRAME*>* prev_keyframe_vector = (*prev_anim_channels)[i]->Get_KeyFrames();
				_uint prev_curr_keyframe = (*prev_anim_channels)[i]->Get_CurrentKeyFrame();

				from_scale = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vScale);
				from_rotation = XMLoadFloat4(&(*prev_keyframe_vector)[prev_curr_keyframe]->vRotation);
				from_position = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vPosition);
			}

			const vector<KEYFRAME*>* curr_keyframe_vector = (*curr_anim_channels)[i]->Get_KeyFrames();
			_uint curr_curr_keyframe = (*curr_anim_channels)[i]->Get_CurrentKeyFrame();

			to_scale = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vScale);
			to_rotation = XMLoadFloat4(&(*curr_keyframe_vector)[curr_curr_keyframe]->vRotation);
			to_position = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vPosition);

			result_scale = XMVectorLerp(from_scale, to_scale, (_float)ratio);
			result_rotation = XMQuaternionSlerp(from_rotation, to_rotation, (_float)ratio);
			result_position = XMVectorLerp(from_position, to_position, (_float)ratio);

			result_position = XMVectorSetW(result_position, 1.f);
			if (false == played_at_least_once)
			{
				// sync animation을 통해서 보정이 된 값을 쓰는게 아니라
				// 순수 키프레임 값에서 보간을 수행할 때 문제 해결.
				// 하지만 to_position이 원점이 아닐 경우 여전히 문제가 있다.
				result_position = to_position;
			}

			_matrix transformationMatrix = XMMatrixAffineTransformation(result_scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), result_rotation, result_position);

			(*prev_anim_channels)[i]->Set_TransformationMatrix(transformationMatrix);
		}
	}

	// case 2. 이전 애니메이션의 채널 개수가 현재 애니메이션의 채널 개수보다 큰 경우
	else
	{
		for (_uint i = 0; i < number_of_channel_curr; ++i)
		{
			if (true == played_at_least_once)
			{
				KEYFRAME from_keyframe = (*prev_anim_channels)[i]->Get_LastUsedKeyFrame();

				from_scale = XMLoadFloat3(&from_keyframe.vScale);
				from_rotation = XMLoadFloat4(&from_keyframe.vRotation);
				from_position = XMLoadFloat3(&from_keyframe.vPosition);
			}

			else
			{
				const vector<KEYFRAME*>* prev_keyframe_vector = (*prev_anim_channels)[i]->Get_KeyFrames();
				_uint prev_curr_keyframe = (*prev_anim_channels)[i]->Get_CurrentKeyFrame();

				from_scale = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vScale);
				from_rotation = XMLoadFloat4(&(*prev_keyframe_vector)[prev_curr_keyframe]->vRotation);
				from_position = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vPosition);
			}

			const vector<KEYFRAME*>* curr_keyframe_vector = (*curr_anim_channels)[i]->Get_KeyFrames();
			_uint curr_curr_keyframe = (*curr_anim_channels)[i]->Get_CurrentKeyFrame();

			to_scale = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vScale);
			to_rotation = XMLoadFloat4(&(*curr_keyframe_vector)[curr_curr_keyframe]->vRotation);
			to_position = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vPosition);

			result_scale = XMVectorLerp(from_scale, to_scale, (_float)ratio);
			result_rotation = XMQuaternionSlerp(from_rotation, to_rotation, (_float)ratio);
			result_position = XMVectorLerp(from_position, to_position, (_float)ratio);

			result_position = XMVectorSetW(result_position, 1.f);

			_matrix transformationMatrix = XMMatrixAffineTransformation(result_scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), result_rotation, result_position);

			(*prev_anim_channels)[i]->Set_TransformationMatrix(transformationMatrix);
		}

		for (_uint i = number_of_channel_curr; i < number_of_channel_prev; ++i)
		{
			if (true == played_at_least_once)
			{
				KEYFRAME from_keyframe = (*prev_anim_channels)[i]->Get_LastUsedKeyFrame();

				from_scale = XMLoadFloat3(&from_keyframe.vScale);
				from_rotation = XMLoadFloat4(&from_keyframe.vRotation);
				from_position = XMLoadFloat3(&from_keyframe.vPosition);
			}

			else
			{
				const vector<KEYFRAME*>* prev_keyframe_vector = (*prev_anim_channels)[i]->Get_KeyFrames();
				_uint prev_curr_keyframe = (*prev_anim_channels)[i]->Get_CurrentKeyFrame();

				from_scale = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vScale);
				from_rotation = XMLoadFloat4(&(*prev_keyframe_vector)[prev_curr_keyframe]->vRotation);
				from_position = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vPosition);
			}

			/*const vector<KEYFRAME*>* prev_keyframe_vector = (*prev_anim_channels)[i]->Get_KeyFrames();
			_uint prev_curr_keyframe = (*prev_anim_channels)[i]->Get_CurrentKeyFrame();

			from_scale = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vScale);
			from_rotation = XMLoadFloat4(&(*prev_keyframe_vector)[prev_curr_keyframe]->vRotation);
			from_position = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vPosition);*/

			to_scale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
			to_rotation = XMVectorZero();
			to_position = XMVectorZero();

			result_scale = XMVectorLerp(from_scale, to_scale, (_float)ratio);
			result_rotation = XMQuaternionSlerp(from_rotation, to_rotation, (_float)ratio);
			result_position = XMVectorLerp(from_position, to_position, (_float)ratio);

			result_position = XMVectorSetW(result_position, 1.f);

			_matrix transformationMatrix = XMMatrixAffineTransformation(result_scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), result_rotation, result_position);

			(*prev_anim_channels)[i]->Set_TransformationMatrix(transformationMatrix);
		}
	}

	m_dLerpTimer += TimeDelta;
}

void CModel::Animation_Lerp_EachBody(BODY_ENUM eBodyEnum, _double TimeDelta)
{
	auto pair = m_AnimationGroup.find(eBodyEnum);

	if (pair->second.dLerpTimer >= pair->second.dLerpDuration)
	{
		pair->second.dLerpTimer = 0.0;

		pair->second.bAnimationChanged = false;
		pair->second.bResetPrevious = true;

		return;
	}

	_double ratio = pair->second.dLerpTimer / pair->second.dLerpDuration;

	const vector<CChannel*>* prev_anim_channels = m_Animations[pair->second.iPrev_Index]->Get_Channels();
	const vector<CChannel*>* curr_anim_channels = m_Animations[pair->second.iCurr_Index]->Get_Channels();

	_bool played_at_least_once = m_Animations[pair->second.iPrev_Index]->Get_Played();

	_vector from_scale, to_scale, result_scale;
	_vector from_rotation, to_rotation, result_rotation;
	_vector from_position, to_position, result_position;

	_uint number_of_channel_prev = (_uint)(*prev_anim_channels).size();
	_uint number_of_channel_curr = (_uint)(*curr_anim_channels).size();

	// case 1. 이전 애니메이션의 채널 개수가 현재 애니메이션의 채널 개수보다 작거나 같은 경우
	if (number_of_channel_prev <= number_of_channel_curr)
	{
		for (_uint i = 0; i < number_of_channel_prev; ++i)
		{
			if (true == played_at_least_once)
			{
				KEYFRAME from_keyframe = (*prev_anim_channels)[i]->Get_LastUsedKeyFrame();

				from_scale = XMLoadFloat3(&from_keyframe.vScale);
				from_rotation = XMLoadFloat4(&from_keyframe.vRotation);
				from_position = XMLoadFloat3(&from_keyframe.vPosition);
			}

			else
			{
				const vector<KEYFRAME*>* prev_keyframe_vector = (*prev_anim_channels)[i]->Get_KeyFrames();
				_uint prev_curr_keyframe = (*prev_anim_channels)[i]->Get_CurrentKeyFrame();

				from_scale = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vScale);
				from_rotation = XMLoadFloat4(&(*prev_keyframe_vector)[prev_curr_keyframe]->vRotation);
				from_position = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vPosition);
			}
			
			const vector<KEYFRAME*>* curr_keyframe_vector = (*curr_anim_channels)[i]->Get_KeyFrames();
			_uint curr_curr_keyframe = (*curr_anim_channels)[i]->Get_CurrentKeyFrame();

			to_scale = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vScale);
			to_rotation = XMLoadFloat4(&(*curr_keyframe_vector)[curr_curr_keyframe]->vRotation);
			to_position = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vPosition);

			result_scale = XMVectorLerp(from_scale, to_scale, (_float)ratio);
			result_rotation = XMQuaternionSlerp(from_rotation, to_rotation, (_float)ratio);
			result_position = XMVectorLerp(from_position, to_position, (_float)ratio);

			if (false == played_at_least_once)
			{
				// sync animation을 통해서 보정이 된 값을 쓰는게 아니라
				// 순수 키프레임 값에서 보간을 수행할 때 문제 해결.
				// 하지만 to_position이 원점이 아닐 경우 여전히 문제가 있다.
				result_position = to_position;
			}

			result_position = XMVectorSetW(result_position, 1.f);

			_matrix transformationMatrix = XMMatrixAffineTransformation(result_scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), result_rotation, result_position);

			(*prev_anim_channels)[i]->Set_TransformationMatrix(transformationMatrix);
		}
	}

	// case 2. 이전 애니메이션의 채널 개수가 현재 애니메이션의 채널 개수보다 큰 경우
	else
	{
		/*
			탐색하여 처리한다면 채널 개수가 다르다는 것을 의미한다.
			prev에서 존재하는 채널이 curr에는 없을 수도 있고 그 반대의 경우도 있다는 이야기.

			prev -> curr의 보간을 진행할 때 경우에 따라서
			보간을 진행하지 않거나(case 1)
			0벡터와 보간을 진행한다(case 2)
		*/

		for (_uint i = 0; i < number_of_channel_curr; ++i)
		{
			if ((*prev_anim_channels)[i]->Get_Node()->Get_NodeIndex() !=
				(*curr_anim_channels)[i]->Get_Node()->Get_NodeIndex())
			{
				// 테스트용 코드. 
				// 채널의 개수가 다르거나 정렬 방식이 달라서 탐색을 해야할 때 참고해서 코드 작성하기 위함.
				_bool bDebug = true;
			}

			if (true == played_at_least_once)
			{
				KEYFRAME from_keyframe = (*prev_anim_channels)[i]->Get_LastUsedKeyFrame();

				from_scale = XMLoadFloat3(&from_keyframe.vScale);
				from_rotation = XMLoadFloat4(&from_keyframe.vRotation);
				from_position = XMLoadFloat3(&from_keyframe.vPosition);
			}

			else
			{
				const vector<KEYFRAME*>* prev_keyframe_vector = (*prev_anim_channels)[i]->Get_KeyFrames();
				_uint prev_curr_keyframe = (*prev_anim_channels)[i]->Get_CurrentKeyFrame();

				from_scale = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vScale);
				from_rotation = XMLoadFloat4(&(*prev_keyframe_vector)[prev_curr_keyframe]->vRotation);
				from_position = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vPosition);
			}

			const vector<KEYFRAME*>* curr_keyframe_vector = (*curr_anim_channels)[i]->Get_KeyFrames();
			_uint curr_curr_keyframe = (*curr_anim_channels)[i]->Get_CurrentKeyFrame();

			to_scale = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vScale);
			to_rotation = XMLoadFloat4(&(*curr_keyframe_vector)[curr_curr_keyframe]->vRotation);
			to_position = XMLoadFloat3(&(*curr_keyframe_vector)[curr_curr_keyframe]->vPosition);

			result_scale = XMVectorLerp(from_scale, to_scale, (_float)ratio);
			result_rotation = XMQuaternionSlerp(from_rotation, to_rotation, (_float)ratio);
			result_position = XMVectorLerp(from_position, to_position, (_float)ratio);

			result_position = XMVectorSetW(result_position, 1.f);

			_matrix transformationMatrix = XMMatrixAffineTransformation(result_scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), result_rotation, result_position);

			(*prev_anim_channels)[i]->Set_TransformationMatrix(transformationMatrix);
		}

		for (_uint i = number_of_channel_curr; i < number_of_channel_prev; ++i)
		{
			if (true == played_at_least_once)
			{
				KEYFRAME from_keyframe = (*prev_anim_channels)[i]->Get_LastUsedKeyFrame();

				from_scale = XMLoadFloat3(&from_keyframe.vScale);
				from_rotation = XMLoadFloat4(&from_keyframe.vRotation);
				from_position = XMLoadFloat3(&from_keyframe.vPosition);
			}

			else
			{
				const vector<KEYFRAME*>* prev_keyframe_vector = (*prev_anim_channels)[i]->Get_KeyFrames();
				_uint prev_curr_keyframe = (*prev_anim_channels)[i]->Get_CurrentKeyFrame();

				from_scale = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vScale);
				from_rotation = XMLoadFloat4(&(*prev_keyframe_vector)[prev_curr_keyframe]->vRotation);
				from_position = XMLoadFloat3(&(*prev_keyframe_vector)[prev_curr_keyframe]->vPosition);
			}
			
			to_scale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
			to_rotation = XMVectorZero();
			to_position = XMVectorZero();

			result_scale = XMVectorLerp(from_scale, to_scale, (_float)ratio);
			result_rotation = XMQuaternionSlerp(from_rotation, to_rotation, (_float)ratio);
			result_position = XMVectorLerp(from_position, to_position, (_float)ratio);

			result_position = XMVectorSetW(result_position, 1.f);

			_matrix transformationMatrix = XMMatrixAffineTransformation(result_scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), result_rotation, result_position);

			(*prev_anim_channels)[i]->Set_TransformationMatrix(transformationMatrix);
		}
	}

	pair->second.dLerpTimer += TimeDelta;
}

HRESULT CModel::Load_Data(const _tchar* pModelFilePath, _fmatrix TransformMatrix)
{
	HANDLE		hFile = CreateFile(pModelFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	CHECK_NULL(hFile)

		_ulong					dwByte = 0;

	_uint	iModelType = 0;


	ReadFile(hFile, &m_tAssimpDesc, sizeof(_uint) * 5, &dwByte, nullptr);

	m_tAssimpDesc.Materials.resize(m_tAssimpDesc.iNumMaterials);
	ReadFile(hFile, &m_tAssimpDesc.Materials[0], sizeof(MATERIAL) * m_tAssimpDesc.iNumMaterials, &dwByte, nullptr);

	m_tAssimpDesc.MeshContainers.resize(m_tAssimpDesc.iNumMeshContainers);

	if (TYPE_NONANIM == m_tAssimpDesc.iModelType)
	{
		for (_uint i = 0; i < m_tAssimpDesc.iNumMeshContainers; i++)
		{
			ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i], sizeof(_uint) * 6 + sizeof(char) * MAX_PATH, &dwByte, nullptr);

			m_tAssimpDesc.MeshContainers[i].NonAnimInfo.resize(m_tAssimpDesc.MeshContainers[i].iNumVertices);
			ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i].NonAnimInfo[0], sizeof(VTXMODEL) * m_tAssimpDesc.MeshContainers[i].iNumVertices, &dwByte, nullptr);

			m_tAssimpDesc.MeshContainers[i].HierarchyNodeIndices.resize(m_tAssimpDesc.MeshContainers[i].iNumBones);
			if (0 < m_tAssimpDesc.MeshContainers[i].iNumBones)
				ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i].HierarchyNodeIndices[0], sizeof(_uint) * m_tAssimpDesc.MeshContainers[i].iNumBones, &dwByte, nullptr);

			m_tAssimpDesc.MeshContainers[i].Indices.resize(m_tAssimpDesc.MeshContainers[i].iNumPrimitive);
			ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i].Indices[0], sizeof(FACEINDICES32) * m_tAssimpDesc.MeshContainers[i].iNumPrimitive, &dwByte, nullptr);
		}
	}
	else
	{
		for (_uint i = 0; i < m_tAssimpDesc.iNumMeshContainers; i++)
		{
			ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i], sizeof(_uint) * 6 + sizeof(char) * MAX_PATH, &dwByte, nullptr);

			m_tAssimpDesc.MeshContainers[i].AnimInfo.resize(m_tAssimpDesc.MeshContainers[i].iNumVertices);
			ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i].AnimInfo[0], sizeof(VTXANIMMODEL) * m_tAssimpDesc.MeshContainers[i].iNumVertices, &dwByte, nullptr);

			m_tAssimpDesc.MeshContainers[i].HierarchyNodeIndices.resize(m_tAssimpDesc.MeshContainers[i].iNumBones);
			if (0 < m_tAssimpDesc.MeshContainers[i].iNumBones)
				ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i].HierarchyNodeIndices[0], sizeof(_uint) * m_tAssimpDesc.MeshContainers[i].iNumBones, &dwByte, nullptr);

			m_tAssimpDesc.MeshContainers[i].Indices.resize(m_tAssimpDesc.MeshContainers[i].iNumPrimitive);
			ReadFile(hFile, &m_tAssimpDesc.MeshContainers[i].Indices[0], sizeof(FACEINDICES32) * m_tAssimpDesc.MeshContainers[i].iNumPrimitive, &dwByte, nullptr);
		}
	}

	m_eModelType = (MODELTYPE)m_tAssimpDesc.iModelType;

	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	m_iNumMaterials = m_tAssimpDesc.iNumMaterials;
	m_iNumMeshContainers = m_tAssimpDesc.iNumMeshContainers;
	m_iNumAnimations = m_tAssimpDesc.iNumAnimations;


	if (TYPE_ANIM == m_eModelType)
	{
		// 애니메이션을 읽어온다.
		m_tAssimpDesc.Animations.resize(m_iNumAnimations);
		for (_uint i = 0; i < m_iNumAnimations; i++)
		{
			ReadFile(hFile, &m_tAssimpDesc.Animations[i], sizeof(_uint) * 2 + sizeof(char) * MAX_PATH + sizeof(_double) * 2, &dwByte, nullptr);

			m_tAssimpDesc.Animations[i].Channels.resize(m_tAssimpDesc.Animations[i].iNumChannels);
			for (_uint j = 0; j < m_tAssimpDesc.Animations[i].iNumChannels; j++)
			{
				ReadFile(hFile, &m_tAssimpDesc.Animations[i].Channels[j], sizeof(_uint) + sizeof(char) * MAX_PATH, &dwByte, nullptr);

				m_tAssimpDesc.Animations[i].Channels[j].KeyFrames.resize(m_tAssimpDesc.Animations[i].Channels[j].iNumKeyFrames);

				ReadFile(hFile, &m_tAssimpDesc.Animations[i].Channels[j].KeyFrames[0], sizeof(KEYFRAME) * m_tAssimpDesc.Animations[i].Channels[j].iNumKeyFrames, &dwByte, nullptr);
			}
		}
	}

	m_tAssimpDesc.HierarchyNodes.resize(m_tAssimpDesc.iNumNodes);
	if (0 < m_tAssimpDesc.iNumNodes)
		ReadFile(hFile, &m_tAssimpDesc.HierarchyNodes[0], sizeof(HIERARCHYNODEDESC) * m_tAssimpDesc.iNumNodes, &dwByte, nullptr);

	if (0 == dwByte)		// 파일을 불러올 수 없는 상태
	{
		__debugbreak();
		CloseHandle(hFile);
		return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CModel::Link_ChannelToNode()
{
	for (auto& pAnimation : m_Animations)	// 모든 애니메이션을 루프
	{
		const vector<CChannel*>* pChannels = pAnimation->Get_Channels();	// 해당 애니메이션의 뼈 목록을 가져옴

		for (auto& pChannels : *pChannels)									// 그 뼈 목록을 루프
		{
			CHierarchyNode* pNode = Find_HierarchyNode(pChannels->Get_Name());	// 해당 뼈의 이름과 같은 노드가 있는지 검사해서 가져옴.
			if (nullptr == pNode)
			{
				MSGBOX("pNode is nullptr in CModel::Link_ChannelToNode");
				return E_FAIL;
			}

			pChannels->Set_HierarchyNodePtr(pNode);								// 가져온 노드를 해당 뼈가 가지고 있게 보내줌.
		}
	}
	return S_OK;
}

CHierarchyNode * CModel::Find_HierarchyNode(const char * pName, _uint* pOut)
{
	_uint	iIndex = 0;
	CHierarchyNode* pNode = nullptr;

	for (auto& pHierarchyNode : m_HierarchyNodes)		// 모든 하이어라키 노드를 순회
	{
		if (!strcmp(pHierarchyNode->Get_Name(), pName))	// 해당 노드의 이름과 찾는 이름이 같으면
		{
			pNode = pHierarchyNode;
			if (nullptr != pOut)
				*pOut = iIndex;

			break;
		}
		iIndex++;
	}

	return pNode;
}

CHierarchyNode * CModel::Find_HierarchyNode(_int iParentIndex)
{
	CHierarchyNode* pNode = nullptr;

	for (auto& pHierarchyNode : m_HierarchyNodes)		// 모든 하이어라키 노드를 순회
	{
		if (iParentIndex == pHierarchyNode->Get_NodeIndex())
		{
			pNode = pHierarchyNode;
			break;
		}
	}

	return pNode;
}

HRESULT CModel::Make_BodyPartInfo(_uint iNumberOfBodyPart)
{
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		if (FAILED(m_Animations[i]->Make_BodyPart(iNumberOfBodyPart)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	m_bSeparatedAnim = true;
	m_iNumberOfBodyPart = iNumberOfBodyPart;

	for (_uint i = 0; i < m_iNumberOfBodyPart; ++i)
	{
		ANIM_PLAY_INFO empty_desc;
		ZeroMemory(&empty_desc, sizeof(ANIM_PLAY_INFO));

		empty_desc.dLerpDuration = 0.1;

		m_AnimationGroup.emplace(i, empty_desc);
	}

	return S_OK;
}

HRESULT CModel::Fill_BodyPartInfo(const char * pPartName, const char * pNameWantToSearch[], _uint iNumberOfName)
{
	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		if (FAILED(m_Animations[i]->Fill_BodyPart(pPartName, pNameWantToSearch, iNumberOfName)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pModelFilePath, _fmatrix TransformMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pModelFilePath, TransformMatrix)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pMaterials : m_Materials)
	{
		for (_int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(pMaterials->pTexture[i]);
		
		if (false == m_isCloned)
			Safe_Delete(pMaterials);
	}
	m_Materials.clear();

	if (nullptr != m_pMeshContainers)
	{
		for (_uint i = 0; i < m_iNumMaterials; i++)
		{

			for (auto& pMeshContainer : m_pMeshContainers[i])
				Safe_Release(pMeshContainer);
			
			if (false == m_isCloned)
				m_pMeshContainers[i].clear();
		}

	}

	if (nullptr != m_pMeshContainersBoneMatrices)
	{
		for (_uint i = 0; i < m_iNumMaterials; i++)
		{
			for (size_t j = 0; j < m_pMeshContainersBoneMatrices[i].size(); j++)
			{
				if (nullptr != m_pMeshContainersBoneMatrices[i][j])
					Safe_Delete_Array(m_pMeshContainersBoneMatrices[i][j]);
			}
			m_pMeshContainersBoneMatrices[i].clear();
		}
		Safe_Delete_Array(m_pMeshContainersBoneMatrices);
	}

	for (auto& pNode : m_HierarchyNodes)
		Safe_Release(pNode);
	m_HierarchyNodes.clear();


	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pMeshContainers);
		m_Importer.FreeScene();
	}
}
