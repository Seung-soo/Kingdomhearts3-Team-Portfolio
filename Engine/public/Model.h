#pragma once

#include "Component.h"
#include "Animation.h"

BEGIN(Engine)
class CHierarchyNode;

typedef _uint BODY_ENUM;


class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };

	typedef struct tagBoneMatrixPtr
	{
		_float4x4*			pOffsetMatrix;
		_float4x4*			pCombinedMatrix;

	}BONEMATRIX_PTR;

	typedef struct tagAnimationPlayingStruct
	{
		_uint iCurr_Index;
		_uint iTemp_Index;
		_uint iPrev_Index;

		_double dLerpTimer;
		_double dLerpDuration;

		_bool bAnimationChanged;
		_bool bResetPrevious;

		_bool bTempWholeBody;

	} ANIM_PLAY_INFO;

	typedef struct tagMeshDesc
	{
		_float4x4*				pBoneMatrices = nullptr;
		class CMeshContainer*	pMeshContainer = nullptr;
	}MESHDESC;
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_float4x4	Get_TransformMatrix() { return m_TransformMatrix; }

	_uint		Get_NumberOfMeshContainers() { return m_iNumMeshContainers; }
	_uint		Get_NumberOfMaterials() const { return m_iNumMaterials; }
	_uint		Get_NumberOfAnimations() { return m_iNumAnimations; }

#pragma region Not Separated

	// Temp //
	_uint Get_TempIndex() { return m_iTempAnim; }

	// Current //
	_uint Get_CurrAnimIndex() { return m_iCurrAnim; }

	char* Get_CurrName() { return m_Animations[m_iCurrAnim]->Get_Name(); }
	_double Get_CurrDuration() { return m_Animations[m_iCurrAnim]->Get_Duration(); }

	_double Get_CurrPlaySpeed() { return m_Animations[m_iCurrAnim]->Get_PlaySpeed(); }
	void Set_CurrPlaySpeed(_double dPlaySpeed) { m_Animations[m_iCurrAnim]->Set_PlaySpeed(dPlaySpeed); }
	void Set_CurrPlaySpeed_Rate(_double dPlaySpeedRate) { m_Animations[m_iCurrAnim]->Set_PlaySpeed_Rate(dPlaySpeedRate); }

	_double Get_CurrTimeAcc() { return m_Animations[m_iCurrAnim]->Get_TimeAcc(); }
	void Set_CurrTimeAcc(_double dTimeAcc) { m_Animations[m_iCurrAnim]->Set_TimeAcc(dTimeAcc); }

	CAnimation::ANIM_TYPE Get_CurrAnimType() { return m_Animations[m_iCurrAnim]->Get_AnimType(); }
	void Set_CurrAnimType(CAnimation::ANIM_TYPE eAnimType) { m_Animations[m_iCurrAnim]->Set_AnimType(eAnimType); }

	_bool Get_CurrFinished() { return m_Animations[m_iCurrAnim]->Get_Finished(); }
	void Set_CurrFinished(_bool bFinished) { m_Animations[m_iCurrAnim]->Set_Finished(bFinished); }

	_bool Get_CurrStopped() { return m_Animations[m_iCurrAnim]->Get_Stopped(); }
	void Set_CurrStopped(_bool bStopped) { m_Animations[m_iCurrAnim]->Set_Stopped(bStopped); }

	_bool Get_CurrSeparated() { return m_Animations[m_iCurrAnim]->Get_Separated(); }
	void Set_CurrSeparated(_bool bSeparated) { m_Animations[m_iCurrAnim]->Set_Separated(bSeparated); }

	_float Get_CurrMovementAmountXZ() { return m_Animations[m_iCurrAnim]->Get_MovementAmountXZ(); }
	_float Get_CurrMovementAmountY() { return m_Animations[m_iCurrAnim]->Get_MovementAmountY(); }

	_float Get_CurrMovementRatioXZ() { return m_Animations[m_iCurrAnim]->Get_MovementRatioXZ(); }
	void Set_CurrMovementRatioXZ(_float fRatioXZ) { m_Animations[m_iCurrAnim]->Set_MovementRatioXZ(fRatioXZ); }

	_float Get_CurrMovementRatioY() { return m_Animations[m_iCurrAnim]->Get_MovementRatioY(); }
	void Set_CurrMovementRatioY(_float fRatioY) { m_Animations[m_iCurrAnim]->Set_MovementRatioY(fRatioY); }

	// Previous //
	_uint Get_PrevAnimIndex() { return m_iPrevAnim; }

	char* Get_PrevName() { return m_Animations[m_iPrevAnim]->Get_Name(); }
	_double Get_PrevDuration() { return m_Animations[m_iPrevAnim]->Get_Duration(); }

	_double Get_PrevPlaySpeed() { return m_Animations[m_iPrevAnim]->Get_PlaySpeed(); }
	void Set_PrevPlaySpeed(_double dPlaySpeed) { m_Animations[m_iPrevAnim]->Set_PlaySpeed(dPlaySpeed); }

	_double Get_PrevTimeAcc() { return m_Animations[m_iPrevAnim]->Get_TimeAcc(); }
	void Set_PrevTimeAcc(_double dTimeAcc) { m_Animations[m_iPrevAnim]->Set_TimeAcc(dTimeAcc); }

	CAnimation::ANIM_TYPE Get_PrevAnimType() { return m_Animations[m_iPrevAnim]->Get_AnimType(); }
	void Set_PrevAnimType(CAnimation::ANIM_TYPE eAnimType) { m_Animations[m_iPrevAnim]->Set_AnimType(eAnimType); }

	_bool Get_PrevFinished() { return m_Animations[m_iPrevAnim]->Get_Finished(); }
	void Set_PrevFinished(_bool bFinished) { m_Animations[m_iPrevAnim]->Set_Finished(bFinished); }

	_bool Get_PrevStopped() { return m_Animations[m_iPrevAnim]->Get_Stopped(); }
	void Set_PrevStopped(_bool bStopped) { m_Animations[m_iPrevAnim]->Set_Stopped(bStopped); }

	_bool Get_PrevSeparated() { return m_Animations[m_iPrevAnim]->Get_Separated(); }
	void Set_PrevSeparated(_bool bSeparated) { m_Animations[m_iPrevAnim]->Set_Separated(bSeparated); }

	_float Get_PrevMovementAmountXZ() { return m_Animations[m_iPrevAnim]->Get_MovementAmountXZ(); }
	_float Get_PrevMovementAmountY() { return m_Animations[m_iPrevAnim]->Get_MovementAmountY(); }

	_float Get_PrevMovementRatioXZ() { return m_Animations[m_iPrevAnim]->Get_MovementRatioXZ(); }
	void Set_PrevMovementRatioXZ(_float fRatioXZ) { m_Animations[m_iPrevAnim]->Set_MovementRatioXZ(fRatioXZ); }

	_bool CheckStop();
	_bool CheckStop_Input(_uint iAnimationIndex);

	_bool CheckFinished();
	_bool CheckFinished_Input(_uint iAnimationIndex);

	_bool Check_CurAnim_And_Finished(_uint iAnimIndex); /* 훈식 추가 */

	// 보간에 사용되는 변수들에 대한 메서드
	_bool Get_AnimationChanged() { return m_bAnimationChanged; }
	_double Get_LerpTimer() { return m_dLerpTimer; }
	_double Get_LerpDuration() { return m_dLerpDuration; }
	void Set_LerpDuration(_double dDuration) { m_dLerpDuration = dDuration; }

	vector<class CHierarchyNode*>*	Get_Nodes() { return &m_HierarchyNodes; }

#pragma endregion

#pragma region Separated

	// Temp //
	_uint Get_TempIndex_EachBody(BODY_ENUM eBodyEnum);

	// Current //
	_uint Get_CurrAnimIndex_EachBody(BODY_ENUM eBodyEnum);

	char* Get_CurrName_EachBody(BODY_ENUM eBodyEnum);
	_double Get_CurrDuration_EachBody(BODY_ENUM eBodyEnum);

	_double Get_CurrPlaySpeed_EachBody(BODY_ENUM eBodyEnum);
	void Set_CurrPlaySpeed_EachBody(BODY_ENUM eBodyEnum, _double dPlaySpeed);
	void Set_CurrPlaySpeed_Rate_EachBody(BODY_ENUM eBodyEnum, _double dPlaySpeedRate);

	_double Get_CurrTimeAcc_EachBody(BODY_ENUM eBodyEnum);
	void Set_CurrTimeAcc_EachBody(BODY_ENUM eBodyEnum, _double dTimeAcc);

	CAnimation::ANIM_TYPE Get_CurrAnimType_EachBody(BODY_ENUM eBodyEnum);
	void Set_CurrAnimType_EachBody(BODY_ENUM eBodyEnum, CAnimation::ANIM_TYPE eAnimType);

	_bool Get_CurrFinished_EachBody(BODY_ENUM eBodyEnum);
	void Set_CurrFinished_EachBody(BODY_ENUM eBodyEnum, _bool bFinished);

	_bool Get_CurrStopped_EachBody(BODY_ENUM eBodyEnum);
	void Set_CurrStopped_EachBody(BODY_ENUM eBodyEnum, _bool bStopped);

	_bool Get_CurrSeparated_EachBody(BODY_ENUM eBodyEnum);
	void Set_CurrSeparated_EachBody(BODY_ENUM eBodyEnum, _bool bSeparated);

	_float Get_CurrMovementAmountXZ_EachBody(BODY_ENUM eBodyEnum);
	_float Get_CurrMovementAmountY_EachBody(BODY_ENUM eBodyEnum);

	_float Get_CurrMovementRatioXZ(BODY_ENUM eBodyEnum);
	void Set_CurrMovementRatioXZ(BODY_ENUM eBodyEnum, _float fRatioXZ);

	_float Get_CurrMovementRatioY(BODY_ENUM eBodyEnum);
	void Set_CurrMovementRatioY(BODY_ENUM eBodyEnum, _float fRatioY);

	// Previous //
	_uint Get_PrevAnimIndex_EachBody(BODY_ENUM eBodyEnum);

	char* Get_PrevName_EachBody(BODY_ENUM eBodyEnum);
	_double Get_PrevDuration_EachBody(BODY_ENUM eBodyEnum);

	_double Get_PrevPlaySpeed_EachBody(BODY_ENUM eBodyEnum);
	void Set_PrevPlaySpeed_EachBody(BODY_ENUM eBodyEnum, _double dPlaySpeed);

	_double Get_PrevTimeAcc_EachBody(BODY_ENUM eBodyEnum);
	void Set_PrevTimeAcc_EachBody(BODY_ENUM eBodyEnum, _double dTimeAcc);

	CAnimation::ANIM_TYPE Get_PrevAnimType_EachBody(BODY_ENUM eBodyEnum);
	void Set_PrevAnimType_EachBody(BODY_ENUM eBodyEnum, CAnimation::ANIM_TYPE eAnimType);

	_bool Get_PrevFinished_EachBody(BODY_ENUM eBodyEnum);
	void Set_PrevFinished_EachBody(BODY_ENUM eBodyEnum, _bool bFinished);

	_bool Get_PrevStopped_EachBody(BODY_ENUM eBodyEnum);
	void Set_PrevStopped_EachBody(BODY_ENUM eBodyEnum, _bool bStopped);

	_bool Get_PrevSeparated_EachBody(BODY_ENUM eBodyEnum);
	void Set_PrevSeparated_EachBody(BODY_ENUM eBodyEnum, _bool bSeparated);

	_float Get_PrevMovementAmountXZ_EachBody(BODY_ENUM eBodyEnum);
	_float Get_PrevMovementAmountY_EachBody(BODY_ENUM eBodyEnum);

	_float Get_PrevMovementRatioXZ(BODY_ENUM eBodyEnum);
	void Set_PrevMovementRatioXZ(BODY_ENUM eBodyEnum, _float fRatioXZ);

	_float Get_PrevMovementRatioY(BODY_ENUM eBodyEnum);
	void Set_PrevMovementRatioY(BODY_ENUM eBodyEnum, _float fRatioY);

	// 해당 부위의 CurrIndex의 정지상태를 반환
	_bool CheckStop_EachBody(BODY_ENUM eBodyEnum);
	_bool CheckFinished_EachBody(BODY_ENUM eBodyEnum);

	_bool Get_AnimationChanged_EachBody(BODY_ENUM eBodyEnum);
	_double Get_LerpTimer_EachBody(BODY_ENUM eBodyEnum);
	_double Get_LerpDuration_EachBody(BODY_ENUM eBodyEnum);
	void Set_LerpDuration_EachBody(BODY_ENUM eBodyEnum, _double dDuration);

#pragma endregion

#pragma region TargetIndex

	char* Get_TargetName(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_Name(); }
	_double Get_TargetDuration(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_Duration(); }

	_double Get_TargetPlaySpeed(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_PlaySpeed(); }
	void Set_TargetPlaySpeed(_uint iTargetIndex, _double dPlaySpeed) { m_Animations[iTargetIndex]->Set_PlaySpeed(dPlaySpeed); }
	void Set_TargetPlaySpeed_Rate(_uint iTargetIndex, _double dPlaySpeedRate) { m_Animations[iTargetIndex]->Set_PlaySpeed_Rate(dPlaySpeedRate);	}

	_double Get_TargetTimeAcc(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_TimeAcc(); }
	void Set_TargetTimeAcc(_uint iTargetIndex, _double dTimeAcc) { m_Animations[iTargetIndex]->Set_TimeAcc(dTimeAcc); }

	CAnimation::ANIM_TYPE Get_TargetAnimType(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_AnimType(); }
	void Set_TargetAnimType(_uint iTargetIndex, CAnimation::ANIM_TYPE eAnimType) { m_Animations[iTargetIndex]->Set_AnimType(eAnimType); }

	_bool Get_TargetFinished(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_Finished(); }
	void Set_TargetFinished(_uint iTargetIndex, _bool bFinished) { m_Animations[iTargetIndex]->Set_Finished(bFinished); }

	_bool Get_TargetStopped(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_Stopped(); }
	void Set_TargetStopped(_uint iTargetIndex, _bool bStopped) { m_Animations[iTargetIndex]->Set_Stopped(bStopped); }

	_bool Get_TargetSeparated(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_Separated(); }
	void Set_TargetSeparated(_uint iTargetIndex, _bool bSeparated) { m_Animations[iTargetIndex]->Set_Separated(bSeparated); }

	_float Get_TargetMovementAmountXZ(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_MovementAmountXZ(); }
	_float Get_TargetMovementAmountY(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_MovementAmountY(); }

	_float Get_TargetMovementRatioXZ(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_MovementRatioXZ(); }
	void Set_TargetMovementRatioXZ(_uint iTargetIndex, _float fRatioXZ) { m_Animations[iTargetIndex]->Set_MovementRatioXZ(fRatioXZ); }

	_float Get_TargetMovementRatioY(_uint iTargetIndex) { return m_Animations[iTargetIndex]->Get_MovementRatioY(); }
	void Set_TargetMovementRatioY(_uint iTargetIndex, _float fRatioY) { m_Animations[iTargetIndex]->Set_MovementRatioY(fRatioY); }

#pragma endregion

	_bool Get_ModelSeparated() { return m_bSeparatedAnim; }
	void Set_ModelSeparated(_bool bSeparated) { m_bSeparatedAnim = bSeparated; }

	const char* Get_RootName();
	_int Get_NodeIndexByName(const char* pName);

	BONEMATRIX_PTR Get_BoneMatrixPtr(const char* pBoneName);

	vector<CAnimation*>* Get_Animations() { return &m_Animations; }
	map<BODY_ENUM, ANIM_PLAY_INFO>* Get_AnimationGroup() { return &m_AnimationGroup; }

	void Set_RootNodeIndex(_uint iRootNodeIndex);
	void Set_RootNodeY(_uint iRootNodeY);

public:
	virtual HRESULT		NativeConstruct_Prototype(const _tchar* pModelFilePath,_fmatrix TransformMatrix);
	virtual HRESULT		NativeConstruct(void* pArg);

public:
	/* Single Animation */
	HRESULT				Set_TempIndex(_uint iAnimIndex);
	HRESULT				SetUp_AnimIndex(_uint iAnimIndex);	// 현재 애니메이션 갱신 해주는 함수
	HRESULT				SetUp_AnimIndex_Auto(_uint iAnimIndex);

	_bool				Check_ProgressRate(_float fRate);
	_bool				Check_ProgressRate_Input(_uint iAnimIndex, _float fRate);
	_bool				Check_TempAndCurr();

	/* Separated Animation */
	HRESULT				Set_TempIndex_EachBody(_uint iBodyIndex, _uint iAnimIndex, _bool bTempWholeBody = false);
	HRESULT				PlayAnimation_EachBody(_uint iBodyIndex, _uint iAnimationIndex, CAnimation::ANIM_TYPE eAnimType, _bool bWholeBody = false);
	HRESULT				PlayAnimation_EachBody_Auto(_uint iBodyIndex, _uint iAnimationIndex, _bool bWholeBody = false);

	_bool				Check_ProgressRate_EachBody(_uint iBodyIndex, _float fRate);
	_bool				Check_TempAndCurr_EachBody(_uint iBodyIndex);

	void				ResetAnimation(_uint iAnimationIndex);
	void				Calculate_MovementAmount(_uint iAnimationIndex);
	void				Calculate_MovementAmount_All();

	HRESULT				Update_CombinedTransformationMatrices(_double TimeDelta, class CTransform* pTransform = nullptr, class CNavigation* pNavigation = nullptr);	// 누적행렬을 갱신해주는 함수
	HRESULT				Bind_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pValueName);
	HRESULT				Ready_BoneMatricesContainer();
	HRESULT				Ready_BoneMatrices(_uint iMaterialIndex, _uint iBoneSize);
	HRESULT				Render(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName = nullptr, vector<MESHDESC*>* pMeshDescVector = nullptr);
	HRESULT				Render_Heavy(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName = nullptr, vector<MESHDESC*>* pMeshDescVector = nullptr);
	HRESULT				Render_OtherModel(class CShader* pShader, _uint iPassIndex, _uint iMaterialIndex, const char* pBoneValueName, _uint iSize);
public:
	CHierarchyNode*		Find_HierarchyNode(const char* pName, _uint* pOut = nullptr);
	CHierarchyNode*		Find_HierarchyNode(_int iParentIndex);

	HRESULT Make_BodyPartInfo(_uint iNumberOfBodyPart);
	HRESULT Fill_BodyPartInfo(const char* pPartName, const char* pNameWantToSearch[], _uint iNumberOfName);

	_uint Find_ContainerIndex(const char* pName);
	void Add_SkipList(_uint iIndex);
	void Rmv_SkipList(_uint iIndex);
	void Clear_SkipList();
	_uint Size_SkipList() { return (_uint)m_RenderSkipList.size(); }

	_bool Check_Once(_uint iAnimIndex);
	void Add_OnceList(_uint iAnimIndex);
	void Rmv_OnceList(_uint iAnimIndex);
	void Add_OnceListAll();

	_bool Check_Lerp(_uint iAnimIndex);
	void Add_LerpList(_uint iAnimIndex);
	void Rmv_LerpList(_uint iAnimIndex);
	void Add_LerpListAll();

	void Animation_Lerp(_double TimeDelta);
	void Animation_Lerp_EachBody(BODY_ENUM eBodyEnum, _double TimeDelta);

private:
	const aiScene*		m_pScene = nullptr;
	Importer			m_Importer;
private:
	_uint									m_iNumMeshContainers = 0;
	vector<class CMeshContainer*>*			m_pMeshContainers = nullptr;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;
	MODELTYPE								m_eModelType = TYPE_END;
	_float4x4								m_TransformMatrix;

	vector<_float4x4*>*						m_pMeshContainersBoneMatrices = nullptr;

private:
	_uint									m_iNumMaterials = 0;
	vector<MESHMATERIALDESC*>				m_Materials;
	typedef vector<MESHMATERIALDESC*>		MATERIALS;

private:
	_int									m_iNodeIndex = 0;
	vector<CHierarchyNode*>					m_HierarchyNodes;
	typedef vector<CHierarchyNode*>			HIERARCHYNODES;

private:
	_uint									m_iNumAnimations;		// 애니메이션의 갯수
	vector<class CAnimation*>				m_Animations;			// 애니메이션들을 담은 벡터 컨테이너
	typedef vector<class CAnimation*>		ANIMATIONS;

	_bool									m_bAnimationChanged = false;
	_bool									m_bResetPrevious = false;

	_uint									m_iCurrAnim = 0;		// 현재 애니메이션 인덱스
	_uint									m_iTempAnim = 0;		// 인덱스 임시 보관 변수
	_uint									m_iPrevAnim = 0;		// 이전 애니메이션 인덱스

	_double									m_dLerpTimer = 0.0;
	_double									m_dLerpDuration = 0.0;

	_uint									m_iNumberOfBodyPart = 0;
	map<BODY_ENUM, ANIM_PLAY_INFO>			m_AnimationGroup;
	typedef map<BODY_ENUM, ANIM_PLAY_INFO>	ANIMATION_GROUP;

	list<_uint>								m_OnceAnimations;
	list<_uint>								m_LerpAnimations;

	_bool									m_bSeparatedRender = false;
	_bool									m_bSeparatedAnim = false;

	list<_uint>								m_RenderSkipList;

	/**************어심프로드 정보 저장된 변수***************/
	ASSIMPDESC								m_tAssimpDesc;

private:
	HRESULT				Ready_MeshContainers(_uint iNumMeshContainers, vector<MESHCONTAINERDESC>* pMeshContainers);
	HRESULT				Ready_Materials(_uint iNumMaterials, vector<MATERIAL>* Materials);
	HRESULT				Ready_HierarchyNodes(_uint iNumNodes, vector<HIERARCHYNODEDESC>* HierarchyNodes);
	HRESULT				Ready_Animation(_uint iNumAnimation, vector<ANIMATIONDESC>* pAnimationDesc);
	HRESULT				Link_ChannelToNode();
	HRESULT				Load_Data(const _tchar* pModelFilePath, _fmatrix TransformMatrix);

public:
	static CModel*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent*	Clone(void* pArg) override;
	virtual void		Free() override;
};

END