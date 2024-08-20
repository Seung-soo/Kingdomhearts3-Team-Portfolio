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
	// ������ �ȵ� �Ϳ� ���� ����ó��.
	// �ڼ��ϰ� ���� �ʿ� ����.
	if (true == m_isSeparated &&
		nullptr == m_pBodyPart)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// Once �ִϸ��̼��� ������ ���.
	if (true == m_isStopped)
		return S_OK;

	// �ִϸ��̼� ���� �ð��� ����
	m_PlayTimeAcc += m_TickPerSecond * TimeDelta;

	if (m_PlayTimeAcc >= m_Duration)	// �ݺ��Ǵ� �ִϸ��̼��� ��� �����ð��� �� �ִϸ��̼��� �� �ð� ���̺��� Ŀ����
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

	// case 1. �и����� ���� �ִϸ��̼�
	if (false == m_isSeparated)
	{
		_vector		vScale, vRotation, vPosition;

		/* ���� �ð�(��� �ð�)�� ����Ͽ� ��� ä��(��)�� ���¸� �����Ѵ�. */
		for (_uint i = 0; i < m_iNumChannels; ++i)
		{
			// �̹� �������� ������ ä��(��)
			CChannel* channel_of_this_loop = m_Channels[i];

			// �ִϸ��̼��� ��� ����Ǿ��ٸ� Ű�������� 0���� �ǵ�����.
			if (true == m_isFinished)
				channel_of_this_loop->Set_CurrentKeyFrame(0);

			// ���� �ð��� ���� ���� ���°��� ǥ���� Ű������ vector�� �����´�.
			const vector<KEYFRAME*>*	pKeyFrames = m_Channels[i]->Get_KeyFrames();
			CHECK_NULL(pKeyFrames);

			_uint iNumKeyFrameSize = (_uint)pKeyFrames->size();	// Ű �������� ����� �޾ƿ�
			_uint iCurrentKeyFrameIndex = channel_of_this_loop->Get_CurrentKeyFrame();	// ���� Ű�������� �޾ƿ�

			// ���� �ð��� ���� ������ Ű �������� �ð��� �Ѿ��
			if (m_PlayTimeAcc >= (*pKeyFrames)[iNumKeyFrameSize - 1]->Time)	
			{
				vScale = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vScale);		// ������ �����Ϸ� ����
				vRotation = XMLoadFloat4(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vRotation);	// ������ ȸ�������� ����
				vPosition = XMLoadFloat3(&(*pKeyFrames)[iNumKeyFrameSize - 1]->vPosition);	// ������ ���������� ����
			}
			else
			{
				while (m_PlayTimeAcc >= (*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time)
				{
					// Ű �������� �������� �Ѱ��ش�
					channel_of_this_loop->Set_CurrentKeyFrame(++iCurrentKeyFrameIndex);
				}

				// ���� Ű �����ӿ��� �� �ۼ�Ʈ ����ƴ��� �ۼ�Ʈ��ġ.
				_double	Ratio = (m_PlayTimeAcc - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time)
					/ ((*pKeyFrames)[iCurrentKeyFrameIndex + 1]->Time - (*pKeyFrames)[iCurrentKeyFrameIndex]->Time);

				// ����� ���� Ű�����ӵ��� Ʈ������ ������
				_vector	vSourScale, vDestScale;
				_vector	vSourRotation, vDestRotation;
				_vector	vSourPosition, vDestPosition;

				vSourScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vScale);
				vSourRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vRotation);
				vSourPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex]->vPosition);

				vDestScale = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vScale);
				vDestRotation = XMLoadFloat4(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vRotation);
				vDestPosition = XMLoadFloat3(&(*pKeyFrames)[iCurrentKeyFrameIndex + 1]->vPosition);

				// ���� �Լ�. �� ���� ���Ͱ��� �� ��° ���ڸ�ŭ�� �ۼ�Ʈ ��ġ�� �����ؼ� ��������.
				vScale = XMVectorLerp(vSourScale, vDestScale, _float(Ratio));
				vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, _float(Ratio));
				vPosition = XMVectorLerp(vSourPosition, vDestPosition, _float(Ratio));
			}

			vPosition = XMVectorSetW(vPosition, 1.f);		// �������� W���� 1�� ä����

			// ��Ʈ����� Ű������ �̵����� 0 ���� ���� �ִϸ��̼��� ���ڸ����� �������� �ʰ� ��
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
				// i��° ä��(��)
				CChannel* channel_of_this_loop = m_pBodyPart[current_part_index][i];

				// �ִϸ��̼��� ��� ����Ǿ��ٸ� ���� ���� curr_key_frame�� 0���� �����.
				if (true == m_isFinished)
					channel_of_this_loop->Set_CurrentKeyFrame(0);

				/* ������ ������ �ð����� ���� ���°��� ǥ���� Ű�����ӵ��� �����´�. */
				const vector<KEYFRAME*>*	keyframe_vector = channel_of_this_loop->Get_KeyFrames();
				if (nullptr == keyframe_vector)
				{
					BREAKPOINT;
					return E_FAIL;
				}

				_uint number_of_keyframe = (_uint)(keyframe_vector->size());
				_uint current_key_frame = channel_of_this_loop->Get_CurrentKeyFrame();

				/* ������ Ű�������� �Ѿ��. == ������ ��° �ִϸ��̼ǿ� ���� ó����. */
				if (m_PlayTimeAcc >= (*keyframe_vector)[number_of_keyframe - 1]->Time)
				{
					vScale = XMLoadFloat3(&(*keyframe_vector)[number_of_keyframe - 1]->vScale);
					vRotation = XMLoadFloat4(&(*keyframe_vector)[number_of_keyframe - 1]->vRotation);
					vPosition = XMLoadFloat3(&(*keyframe_vector)[number_of_keyframe - 1]->vPosition);
				}
				/* Ư�� Ű������ �ΰ� ���̿� �����Ҷ�?! */
				else
				{
					// m_PlayTimeAcc�� ���� Ű�������� time�� ��������
					// ���� Ű������ + 1�� time���� �����ϱ� Ű �������� �� �Ѿ�� ��� ���� ���°� ��.
					while (m_PlayTimeAcc >= (*keyframe_vector)[current_key_frame + 1]->Time)
					{
						// Ű �������� �������� �Ѱ��ش�
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
					// �߰����� Y_Root(center)�� ����� ���
					if (true == m_bUseRootY)
					{
						// Y_Root�� ���� ������ ��ġ�� �޾Ƶд�
						if (m_iRootNodeY == channel_of_this_loop->Get_Node()->Get_NodeIndex())
						{
							vecy(vPosition) *= m_vMovementRatioY;
							XMStoreFloat3(&m_vPosition_RootY, vPosition);
						}
					}

					// channel_of_this_loop�� Root�� ��
					if (m_iRootNodeIndex == channel_of_this_loop->Get_Node()->Get_NodeIndex())
					{
						// ������ ���������� Ű������ ������ ��ġ���� �����´�
						_float3 position;
						XMStoreFloat3(&position, vPosition);

						// Export�ɼǿ� �°� Axis�� �ٲ���
						position = Calculate_Axes(position);

						// �߰����� Y_Root(center)�� ����� ���
						if (true == m_bUseRootY)
						{
							// Root�� Y_Root�� ���̰��� ���Ѵ�
							_float gap_between_root = fabsf(m_vPosition_RootY.y - position.y);

							// ù �����ӿ� �ش� ���̰��� ����صд�
							// �� ����� ���̰��� �������� ������ �����ӿ� �̵����� ������ ��
							if (false == m_bRecorded)
							{
								m_bRecorded_Gap = gap_between_root;
								m_bRecorded = true;
							}

							// ���������� ������ Y���� ����
							_float apply_y = gap_between_root - m_bRecorded_Gap;

							// Y���� -�� �������� �� ����
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

	// �� ���͸� ã�´�
	_uint empty_vector_index = UINT_MAX;
	for (_uint i = m_iNumberOfBodyPart - 1; i >= 0; --i)
	{
		if (true == m_isEmpty[i])
		{
			empty_vector_index = i;
			break;
		}
	}

	// �� ���͸� �� ã���� ��� return
	if (UINT_MAX == empty_vector_index)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���� �̸��� ����
	m_Vector_PartName[empty_vector_index] = pPartName;

	// ��ü ������ ���͸� reserve�ϱ� ���� ����� �ӽú���.
	// ���ǿ� �´� ä���� ��Ƶд�.
	vector<CChannel*> temp_channel_vector;
	for (auto& channel : m_Channels)
	{
		CHierarchyNode* channelNode = channel->Get_Node();

		for (_uint i = 0; i < iNumberOfSearchName; ++i)
		{
			if (false == channelNode->Search_Ancestry(pSearchName[i]))
				continue;

			// ���� ã�� �� ������ �ε����� ��ü ���� ���� - 1���� �۴ٸ�(== ó�� ���� ���Ͱ� �ƴ϶��)
			if (m_iNumberOfBodyPart - 1 > empty_vector_index)
			{
				_uint num_loop = (m_iNumberOfBodyPart - 1) - empty_vector_index;
				_bool duplicated = false;	// �ߺ� ����

				for (_uint i = 1; i <= num_loop; ++i)
				{
					for (auto& pushed_channel : m_pBodyPart[empty_vector_index + i])
					{
						// �ߺ��Ǵ� ���� �ִ�
						if (channel == pushed_channel)
						{
							duplicated = true;
							break;
						}
					}
				}

				// �ߺ����� �ʾ��� ���� push
				if (false == duplicated)
					temp_channel_vector.push_back(channel);
			}
			else
				temp_channel_vector.push_back(channel);
		}
	}

	// ��Ƶ� ä���� ������ŭ reserve�� �� ��Ƶ� ä���� push.back�Ѵ�.
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

	// ��� �� �ʱ�ȭ
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
	// ä�� ������ ���� ���� �ڿ� �����ؾ��� �Լ�
	if (0 == (_int)m_Channels.size())
	{
		BREAKPOINT;
	}

	// ��Ʈ ��� �ε����� ���� ���õ� �Ŀ� �����ؾ��� �Լ�
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

		// Y Root�� ����� ��� ���� ������ ä���� Y Root ä�η� �������ش�
		if (true == m_bUseRootY)
			root_channel = m_pRootChannel_Y;

		const vector<KEYFRAME*>* keyframes = root_channel->Get_KeyFrames();
		for (auto& keyframe : *keyframes)
		{
			_float calculated_position_z = Calculate_Axes(keyframe->vPosition).z;

			// z�� y����
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
	// ������ ���� ����
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