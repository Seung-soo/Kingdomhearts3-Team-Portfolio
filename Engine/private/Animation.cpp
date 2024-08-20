#include "..\public\Animation.h"
#include "Model.h"
#include "Channel.h"
#include "HierarchyNode.h"
#include "Transform.h"

HRESULT CAnimation::NativeConstruct(const char* pName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_szName, pName);

	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;
	m_TickPerSecond_Origin = m_TickPerSecond;

	return S_OK;
}

HRESULT CAnimation::Update_TransformMatrices(_double TimeDelta, class CTransform* pTransform, class CNavigation* pNavigation)
{
	// 세팅이 안된 것에 대한 예외처리.
	// 자세하게 읽을 필요 없음.
	if (true == m_isSeparated &&
		nullptr == m_pBodyPart)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Once 애니메이션이 멈췄을 경우.
	if (true == m_isStopped)
		return S_OK;

	// 애니메이션 누적 시간을 갱신
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta;

	if (m_PlayTimeAcc >= m_Duration)	// 반복되는 애니메이션일 경우 누적시간이 한 애니메이션의 총 시간 길이보다 커지면
	{
		m_isFinished = true;
		m_isPlayed = true;

		m_PlayTimeAcc = 0.0;
		m_vPrevRootPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		m_vPosition_RootY = _float3(0.f);

		if (ANIM_TYPE::ANIM_ONCE == m_eAnimType)
		{
			m_isStopped = true;
			return S_OK;
		}
	}
	else
		m_isFinished = false;

	// case 1. 분리되지 않은 애니메이션
	if (false == m_isSeparated)
	{
		_vector		vScale, vRotation, vPosition;

		/* 누적 시간(재생 시간)에 기반하여 모든 채널(뼈)의 상태를 갱신한다. */
		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			// 이번 루프에서 갱신할 채널(뼈)
			CChannel* channel_of_this_loop = m_Channels[i];

			// 애니메이션이 모두 재생되었다면 키프레임을 0으로 되돌린다.
			if (true == m_isFinished)
				channel_of_this_loop->Set_CurrentKeyFrame(0);

			// 누적 시간에 따른 뼈의 상태값을 표현한 키프레임 vector를 가져온다.
			const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
			CHECK_NULL(pKeyFrames);

			_uint iNumKeyFrameSize = (_uint)pKeyFrames->size();	// 키 프레임의 사이즈를 받아옴
			_uint iCurrentKeyFrameIndex = channel_of_this_loop->Get_CurrentKeyFrame();	// 현재 키프레임을 받아옴

			// 누적 시간이 가장 마지막 키 프레임의 시간을 넘어가면
			if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrameSize - 1]->Time)	
			{
				vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vScale);		// 마지막 스케일로 고정
				vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vRotation);	// 마지막 회전값으로 고정
				vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vPosition);	// 마지막 포지션으로 고정
			}
			else
			{
				while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
				{
					// 키 프레임을 다음으로 넘겨준다
					channel_of_this_loop->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);
				}

				// 현재 키 프레임에서 몇 퍼센트 진행됐는지 퍼센트수치.
				_double	Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time)
					/ ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

				// 현재와 다음 키프레임들의 트랜스폼 정보들
				_vector	vSourScale, vDestScale;
				_vector	vSourRotation, vDestRotation;
				_vector	vSourPosition, vDestPosition;

				vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
				vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
				vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

				vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
				vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
				vDestPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

				// 보간 함수. 두 개의 벡터값을 세 번째 인자만큼의 퍼센트 수치로 보간해서 리턴해줌.
				vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
				vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
				vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
			}

			vPosition = XMVectorSetW(vPosition, 1.f);		// 포지션의 W값을 1로 채워줌

			// 루트노드의 키프레임 이동값을 0 으로 만들어서 애니메이션이 제자리에서 움직이지 않게 함
			if (nullptr != pTransform)
			{
				if (m_iRootNodeIndex == channel_of_this_loop->Get_Node()->Get_NodeIndex())
				{
					_float3 position;
					XMStoreFloat3(&position, vPosition);

					position = Calculate_Axes(position);

					position.x *= m_vMovementRatioXZ;
					position.y *= m_vMovementRatioY;
					position.z *= m_vMovementRatioXZ;

					position = Calculate_Axes(position);
					vPosition = XMVectorSetW(XMLoadFloat3(&position), 1.f);

					_vector move_value = vPosition - m_vPrevRootPos;
					pTransform->Sync_Animation(move_value, TimeDelta, pNavigation);

					m_vPrevRootPos = vPosition;
					vPosition = { 0.f, 0.f, 0.f, 1.f };
				}
			}

			channel_of_this_loop->Set_LastUsedKeyFrame(vScale, vRotation, vPosition);

			_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
			channel_of_this_loop->Set_TransformationMatrix(TransformationMatrix);
		}
	}

	else
	{
		for (auto& body_number : m_Vector_PlayingBody)
		{
			_uint current_part_index = body_number;

			const char* part_name = m_Vector_PartName[current_part_index];
			_uint part_channel_nummber = (_uint)m_pBodyPart[current_part_index].size();

			_vector	vScale, vRotation, vPosition;
			for (_uint i = 0; i < part_channel_nummber; ++i)
			{
				// i번째 채널(뼈)
				CChannel* channel_of_this_loop = m_pBodyPart[current_part_index][i];

				// 애니메이션이 모두 재생되었다면 현재 뼈의 curr_key_frame을 0으로 만든다.
				if (true == m_isFinished)
					channel_of_this_loop->Set_CurrentKeyFrame(0);

				/* 각각의 뼈들이 시간값에 따른 상태값을 표현한 키프레임들을 가져온다. */
				const vector<KEYFRAME*>*	keyframe_vector = channel_of_this_loop->Get_KeyFrames();
				if (nullptr == keyframe_vector)
				{
					BREAKPOINT;
					return E_FAIL;
				}

				_uint number_of_keyframe = (_uint)(keyframe_vector->size());
				_uint current_key_frame = channel_of_this_loop->Get_CurrentKeyFrame();

				/* 마지막 키프레임을 넘어가면. == 마지막 번째 애니메이션에 대한 처리임. */
				if (m_PlayTimeAcc >= (*keyframe_vector)[number_of_keyframe - 1]->Time)
				{
					vScale = XMLoadFloat3(&(*keyframe_vector)[number_of_keyframe - 1]->vScale);
					vRotation = XMLoadFloat4(&(*keyframe_vector)[number_of_keyframe - 1]->vRotation);
					vPosition = XMLoadFloat3(&(*keyframe_vector)[number_of_keyframe - 1]->vPosition);
				}
				/* 특정 키프레임 두개 사이에 존재할때?! */
				else
				{
					// m_PlayTimeAcc가 현재 키프레임의 time은 지났지만
					// 현재 키프레임 + 1의 time보다 작으니까 키 프레임이 안 넘어가고 재생 중인 상태가 됨.
					while (m_PlayTimeAcc >= (*keyframe_vector)[current_key_frame + 1]->Time)
					{
						// 키 프레임을 다음으로 넘겨준다
						channel_of_this_loop->Set_CurrentKeyFrame(++current_key_frame);
					}

					_double		Ratio = (m_PlayTimeAcc - (*keyframe_vector)[current_key_frame]->Time)
						/ ((*keyframe_vector)[current_key_frame + 1]->Time - (*keyframe_vector)[current_key_frame]->Time);

					_vector		vSourScale, vDestScale;
					_vector		vSourRotation, vDestRotation;
					_vector		vSourPosition, vDestPosition;

					vSourScale = XMLoadFloat3(&(*keyframe_vector)[current_key_frame]->vScale);
					vSourRotation = XMLoadFloat4(&(*keyframe_vector)[current_key_frame]->vRotation);
					vSourPosition = XMLoadFloat3(&(*keyframe_vector)[current_key_frame]->vPosition);

					vDestScale = XMLoadFloat3(&(*keyframe_vector)[current_key_frame + 1]->vScale);
					vDestRotation = XMLoadFloat4(&(*keyframe_vector)[current_key_frame + 1]->vRotation);
					vDestPosition = XMLoadFloat3(&(*keyframe_vector)[current_key_frame + 1]->vPosition);

					vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
					vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
					vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
				}

				vPosition = XMVectorSetW(vPosition, 1.f);

				if (nullptr != pTransform)
				{
					// 추가적인 Y_Root(center)를 사용할 경우
					if (true == m_bUseRootY)
					{
						// Y_Root의 현재 프레임 위치를 받아둔다
						if (m_iRootNodeY == channel_of_this_loop->Get_Node()->Get_NodeIndex())
						{
							vecy(vPosition) *= m_vMovementRatioY;
							XMStoreFloat3(&m_vPosition_RootY, vPosition);
						}
					}

					// channel_of_this_loop가 Root일 때
					if (m_iRootNodeIndex == channel_of_this_loop->Get_Node()->Get_NodeIndex())
					{
						// 위에서 선형보간된 키프레임 사이의 위치값을 가져온다
						_float3 position;
						XMStoreFloat3(&position, vPosition);

						// Export옵션에 맞게 Axis를 바꿔줌
						position = Calculate_Axes(position);

						// 추가적인 Y_Root(center)를 사용할 경우
						if (true == m_bUseRootY)
						{
							// Root와 Y_Root의 차이값을 구한다
							_float gap_between_root = fabsf(m_vPosition_RootY.y - position.y);

							// 첫 프레임에 해당 차이값을 기록해둔다
							// 이 기록한 차이값을 기준으로 이후의 프레임에 이동량을 측정할 것
							if (false == m_bRecorded)
							{
								m_bRecorded_Gap = gap_between_root;
								m_bRecorded = true;
							}

							// 실질적으로 적용할 Y값을 구함
							_float apply_y = gap_between_root - m_bRecorded_Gap;

							// Y값이 -로 내려가는 것 방지
							if (0.f < apply_y)
								position.y = apply_y;
						}

						position.x *= m_vMovementRatioXZ;
						position.y *= m_vMovementRatioY;
						position.z *= m_vMovementRatioXZ;

						position = Calculate_Axes(position);
						vPosition = XMVectorSetW(XMLoadFloat3(&position), 1.f);

						_vector move_value = vPosition - m_vPrevRootPos;
						pTransform->Sync_Animation(move_value, TimeDelta, pNavigation);

						m_vPrevRootPos = vPosition;
						vPosition = { 0.f, 0.f, 0.f, 1.f };
					}
				}

				channel_of_this_loop->Set_LastUsedKeyFrame(vScale, vRotation, vPosition);

				_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
				channel_of_this_loop->Set_TransformationMatrix(TransformationMatrix);
			}
		}

		m_Vector_PlayingBody.clear();
	}

	return S_OK;
}

HRESULT CAnimation::Make_BodyPart(_uint iNumberOfBodyPart)
{
	if (nullptr != m_pBodyPart)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_iNumberOfBodyPart = iNumberOfBodyPart;
	m_Vector_PartName.reserve(m_iNumberOfBodyPart);

	for (_uint i = 0; i < m_iNumberOfBodyPart; ++i)
	{
		const char* empty_name = "";
		m_Vector_PartName.push_back(empty_name);
	}

	m_isEmpty = new _bool[m_iNumberOfBodyPart];

	for (_uint i = 0; i < m_iNumberOfBodyPart; ++i)
		m_isEmpty[i] = true;

	m_pBodyPart = new BODYPART[m_iNumberOfBodyPart];
	m_isSeparated = true;

	return S_OK;
}

HRESULT CAnimation::Fill_BodyPart(const char * pPartName, const char * pSearchName[], _uint iNumberOfSearchName)
{
	if (nullptr == m_pBodyPart)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 빈 벡터를 찾는다
	_uint empty_vector_index = UINT_MAX;
	for (_uint i = m_iNumberOfBodyPart - 1; i >= 0; --i)
	{
		if (true == m_isEmpty[i])
		{
			empty_vector_index = i;
			break;
		}
	}

	// 빈 벡터를 못 찾았을 경우 return
	if (UINT_MAX == empty_vector_index)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 부위 이름을 저장
	m_Vector_PartName[empty_vector_index] = pPartName;

	// 신체 부위별 벡터를 reserve하기 위해 사용할 임시벡터.
	// 조건에 맞는 채널을 모아둔다.
	vector<CChannel*> temp_channel_vector;
	for (auto& channel : m_Channels)
	{
		CHierarchyNode* channelNode = channel->Get_Node();

		for (_uint i = 0; i < iNumberOfSearchName; ++i)
		{
			if (false == channelNode->Search_Ancestry(pSearchName[i]))
				continue;

			// 현재 찾은 빈 벡터의 인덱스가 전체 벡터 개수 - 1보다 작다면(== 처음 들어온 벡터가 아니라면)
			if (m_iNumberOfBodyPart - 1 > empty_vector_index)
			{
				_uint num_loop = (m_iNumberOfBodyPart - 1) - empty_vector_index;
				_bool duplicated = false;	// 중복 여부

				for (_uint i = 1; i <= num_loop; ++i)
				{
					for (auto& pushed_channel : m_pBodyPart[empty_vector_index + i])
					{
						// 중복되는 뼈가 있다
						if (channel == pushed_channel)
						{
							duplicated = true;
							break;
						}
					}
				}

				// 중복되지 않았을 때만 push
				if (false == duplicated)
					temp_channel_vector.push_back(channel);
			}
			else
				temp_channel_vector.push_back(channel);
		}
	}

	// 모아둔 채널의 개수만큼 reserve한 뒤 모아둔 채널을 push.back한다.
	_uint node_number_of_body = (_uint)temp_channel_vector.size();
	m_pBodyPart[empty_vector_index].reserve(node_number_of_body);

	for (auto& channel : temp_channel_vector)
		m_pBodyPart[empty_vector_index].push_back(channel);

	m_isEmpty[empty_vector_index] = false;

	return S_OK;
}

void CAnimation::Push_BodyIndex(_uint iBodyIndex)
{
	for (auto& iPlayingBodyIndex : m_Vector_PlayingBody)
	{
		if (iPlayingBodyIndex == iBodyIndex)
			return;
	}

	m_Vector_PlayingBody.push_back(iBodyIndex);
}

void CAnimation::Reset_Animation()
{
	m_PlayTimeAcc = 0.0;
	m_isFinished = false;
	m_isStopped = false;

	m_eAnimType = ANIM_TYPE::ANIM_LOOP;

	// 모든 뼈 초기화
	_vector			vScale, vRotation, vPosition;
	for (auto& pChannel : m_Channels)
	{
		pChannel->Set_CurrentKeyFrame(0);

		const vector<KEYFRAME*>* keyframe_vector = pChannel->Get_KeyFrames();

		vScale = XMLoadFloat3(&(*keyframe_vector)[0]->vScale);
		vRotation = XMLoadFloat4(&(*keyframe_vector)[0]->vRotation);
		vPosition = XMLoadFloat3(&(*keyframe_vector)[0]->vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		pChannel->Set_TransformationMatrix(TransformationMatrix);
	}
}

void CAnimation::Reset_Animation_EachBody(_uint iBodyIndex)
{
	m_PlayTimeAcc = 0.0;
	m_isFinished = false;
	m_isStopped = false;

	m_eAnimType = ANIM_TYPE::ANIM_LOOP;

	_vector			vScale, vRotation, vPosition;
	for (auto& channel : m_pBodyPart[iBodyIndex])
	{
		channel->Set_CurrentKeyFrame(0);

		const vector<KEYFRAME*>* keyframe_vector = channel->Get_KeyFrames();

		vScale = XMLoadFloat3(&(*keyframe_vector)[0]->vScale);
		vRotation = XMLoadFloat4(&(*keyframe_vector)[0]->vRotation);
		vPosition = XMLoadFloat3(&(*keyframe_vector)[0]->vPosition);

		vPosition = XMVectorSetW(vPosition, 1.f);

		_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
		channel->Set_TransformationMatrix(TransformationMatrix);
	}
}

void CAnimation::PrevPos_Zero()
{
	m_vPrevRootPos = XMVectorZero();
}

void CAnimation::Calculate_MovementAmount()
{
	// 채널 정보가 먼저 생긴 뒤에 수행해야할 함수
	if (0 == (_int)m_Channels.size())
	{
		BREAKPOINT;
	}

	// 루트 노드 인덱스가 먼저 세팅된 후에 수행해야할 함수
	if (UINT_MAX == m_iRootNodeIndex)
	{
		BREAKPOINT;
	}

	CChannel* root_channel = nullptr;
	for (auto& channel : m_Channels)
	{
		if (nullptr == root_channel && channel->Get_Node()->Get_NodeIndex() == m_iRootNodeIndex)
		{
			root_channel = channel;
			break;
		}
	}

	if (nullptr == root_channel)
	{
		BREAKPOINT;
	}

	// movement_xz
	{
		_float3 str_position = root_channel->Get_StartKeyFrame()->vPosition;
		_float3 end_position = root_channel->Get_EndKeyFrame()->vPosition;

		str_position = Calculate_Axes(str_position);
		end_position = Calculate_Axes(end_position);

		m_vMovementAmountXZ = vecx(XMVector3Length(XMLoadFloat3(&str_position) - XMLoadFloat3(&end_position)));
	}
	
	// movement_y
	{
		_float low_y = FLT_MAX;
		_float high_y = -1.f;

		// Y Root를 사용할 경우 값을 가져올 채널을 Y Root 채널로 설정해준다
		if (true == m_bUseRootY)
			root_channel = m_pRootChannel_Y;

		const vector<KEYFRAME*>* keyframes = root_channel->Get_KeyFrames();
		for (auto& keyframe : *keyframes)
		{
			_float calculated_position_z = Calculate_Axes(keyframe->vPosition).z;

			// z가 y값임
			if (low_y > calculated_position_z)
				low_y = calculated_position_z;

			if(high_y < calculated_position_z)
				high_y = calculated_position_z;
		}

		m_vMovementAmountY = high_y - low_y;
	}
}

_float3 CAnimation::Calculate_Axes(_float3 _position)
{
	_vector position = XMLoadFloat3(&_position);
	_vector result;

	vecx(result) = vecx((_axisX * -XMVectorGetX(position)));
	vecy(result) = vecy((_axisY * -XMVectorGetZ(position)));
	vecz(result) = vecz((_axisZ * -XMVectorGetY(position)));

	_float3 result_out;
	XMStoreFloat3(&result_out, result);

	return result_out;
}

void CAnimation::Set_RootChannelY()
{
	if (UINT_MAX == m_iRootNodeY)
		_breakpoint;

	for (auto& channel : m_Channels)
	{
		if (m_iRootNodeY == channel->Get_Node()->Get_NodeIndex())
		{
			m_pRootChannel_Y = channel;
			break;
		}
	}
}

CAnimation * CAnimation::Create(const char* pName, _double Duration, _double TickPerSecond)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pName, Duration, TickPerSecond)))
	{
		BREAKPOINT;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	// 부위별 벡터 제거
	if (nullptr != m_pBodyPart &&
		UINT_MAX != m_iNumberOfBodyPart)
	{
		m_Vector_PartName.clear();

		for (_uint i = 0; i < m_iNumberOfBodyPart; ++i)
			m_pBodyPart[i].clear();

		Safe_Delete_Array(m_pBodyPart);
		Safe_Delete_Array(m_isEmpty);
	}

	for (auto* pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}