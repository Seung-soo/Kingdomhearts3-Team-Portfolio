#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	explicit CChannel() = default;
	virtual ~CChannel() = default;

public:
	const char*					Get_Name() const { return m_szName; }

	void						Add_KeyFrame(KEYFRAME* pKeyFrame) { m_KeyFrames.push_back(pKeyFrame); }
	const vector<KEYFRAME*>*	Get_KeyFrames() const { return &m_KeyFrames; }

	_uint						Get_CurrentKeyFrame() const { return m_iCurrentKeyFrame; }
	void						Set_CurrentKeyFrame(_uint iKeyFrameIndex) { m_iCurrentKeyFrame = iKeyFrameIndex; }

	KEYFRAME*					Get_StartKeyFrame() { return m_KeyFrames[0]; }
	KEYFRAME*					Get_EndKeyFrame() { return m_KeyFrames[m_iNumKeyFrames - 1]; }

	KEYFRAME					Get_LastUsedKeyFrame() { return m_LastUsedKeyFrame; }
	void						Set_LastUsedKeyFrame(_fvector vScale, _fvector vRotation, _fvector vPosition);

	KEYFRAME*					Get_CurrentKeyFrameInfo() const { return m_KeyFrames[m_iCurrentKeyFrame]; }
	KEYFRAME*					Get_IndexKeyFrameInfo(_uint iIndex) const { return m_KeyFrames[iIndex]; }

	class CHierarchyNode*		Get_Node() { return m_pNode; }

	void						Set_TransformationMatrix(_fmatrix TransformationMatrix);
	void						Set_HierarchyNodePtr(class CHierarchyNode* pNode);

public:
	void						Reserve(_uint iNumKeyFrames) {
		m_KeyFrames.reserve(iNumKeyFrames);
		m_iNumKeyFrames = iNumKeyFrames;
	}

	HRESULT						NativeConstruct(const char* pName);

private:
	char						m_szName[MAX_PATH] = "";		// 뼈의 이름

	_uint						m_iCurrentKeyFrame = 0;			// 현재 키 프레임
	_uint						m_iNumKeyFrames = 0;			// 키 프레임 수
	vector<KEYFRAME*>			m_KeyFrames;					// 하나의 뼈가 가지고 있는 키 프레임을 저장한 벡터 컨테이너
	typedef vector<KEYFRAME*>	KEYFRAMES;

	KEYFRAME					m_LastUsedKeyFrame;

	class CHierarchyNode*		m_pNode = nullptr;				// 이 뼈의 노드를 저장

public:
	static CChannel*	Create(const char* pName);
	virtual void		Free() override;
};

END