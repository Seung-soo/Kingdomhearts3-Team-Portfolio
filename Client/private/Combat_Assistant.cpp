#include "stdafx.h"
#include "Combat_Assistant.h"

#include "GameInstance.h"
#include "Player.h"
#include "Model.h"

HRESULT CCombat_Assistant::Initialize(CGameObject* _chief, ASSIST_INIT_DESC _initDesc)
{
	m_pPlayer = _chief;
	m_pPlayerTransform = _chief->Get_Transform();

	m_pGI = CGameInstance::GetInstance();

	m_fRange = _initDesc.range;
	m_iMaxNumber = _initDesc.max_search;
	m_iSkipLimit = _initDesc.skip_limit;

	return S_OK;
}

void CCombat_Assistant::Run(_double Timedelta)
{
	if (true == static_cast<CPlayer*>(m_pPlayer)->Get_Input(CPlayer::SI_R))
	{
		// ���� Ÿ���� ���� ��
		if (nullptr == m_pFasten)
		{
			// Ž���� Ÿ���� ������ return
			if (nullptr == m_pLockOn)
				return;

			// Ž���� Ÿ���� ���� ��
			else
			{
				m_pFasten = m_pLockOn;
				m_pFastenObject = m_pLockOnObject;
			}
		}

		// ���� Ÿ���� ���� ��
		else
		{
			// ���� Ÿ���� �����Ѵ�
			m_pFasten = nullptr;
			m_pFastenObject = nullptr;
		}
	}

	if (true == static_cast<CPlayer*>(m_pPlayer)->Get_Input(CPlayer::SI_E))
	{
		if (nullptr != m_pFasten)
		{
			Change_FastenTarget();
		}
	}

	// ���� �ȿ� ���� �ݶ��̴� �� ����� �͵��� �־���Ѵ�.
	// ���� ���� �ݶ��̴��鿡 ���� ��üŽ���� �ʿ��ϱ� ������ �ݶ��̴��� ��Ƶ� �����̳ʸ� ����
	vector<CCollider*> colliders_in_range;

	// ���⿡ �� �±� ����� �̸� �̴ϼȶ������ �������ְų�
	// ���� �±� ��� �ʱ�ȭ�Լ��� ���� ȣ�����ִ� ���� �ʿ���.
	auto pColliderList = m_pGI->Get_ColliderList(string("Monster_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		_vector player_position = m_pPlayerTransform->Get_Position();
		_vector collider_position = pDestCollider->Get_Center();

		_float distance_to_collider = DISTANCE(player_position - collider_position);

		// �Ÿ���, Ž�� ���� �ۿ� �ִٸ� continue
		if (distance_to_collider > m_fRange)
			continue;

		// ��ü Ž���� �����̳ʿ� �߰�
		colliders_in_range.push_back(pDestCollider);
	}

	// ��ó�� �ƹ��� �浹ü�� ���ٸ� return
	if (0 == colliders_in_range.size())
	{
		Clear();
		return;
	}

	// �̹� ���� ���� �����ϴ� �ݶ��̴��鿡 ���� ���ܿ���
	vector<ASSIST_DESC> delete_vector;
	for (auto& desc : m_Colliders)
	{
		_bool erase = false;

		_float distance = DISTANCE(desc.pCollider->Get_Center() - m_pPlayerTransform->Get_Position());
		_bool dead = desc.pCollider->Get_Parent()->Get_Dead();

		// ���� ��ü�� ���
		if (false == erase && true == dead)
			erase = true;

		// ������ �����.
		if (false == erase && distance > m_fRange)
		{
			// ������ ���¶�� �������� ����
			if (desc.pCollider == m_pFasten)
				continue;

			erase = true;
		}

		// ����
		if (true == erase)
		{
			delete_vector.push_back(desc);
		}
	}

	for (auto& delete_desc : delete_vector)
	{
		if (delete_desc.pCollider == m_pLockOn)
		{
			m_pLockOn = nullptr;
			m_pLockOnObject = nullptr;
		}

		Rmv_ObjectList(delete_desc);
		Rmv_ColliderList(delete_desc);
	}

	// ���� ���� ���
	_uint number_of_empty_slot = m_iMaxNumber - (_uint)m_Colliders.size();

	// �� ������ �ִ� : Ž�� ��Ͽ� �߰�
	if (0 != number_of_empty_slot)
	{
		// ���� ����� ���� push�Ǿ����� Ȯ���ϱ����� ����
		_bool nearest_one_is_pushed = false;

		while (0 != number_of_empty_slot)
		{
			// ���� �ȿ� �����ִ¸�ŭ �����ߴٸ� ���� ���԰� ������� �״�� �ݺ��� Ż��
			if (m_Colliders.size() >= colliders_in_range.size())
				break;

			_float closest = FLT_MAX;
			CCollider* closest_collider = nullptr;

			_vector player_position = m_pPlayerTransform->Get_Position();

			for (auto& collider : colliders_in_range)
			{
				// �ߺ� Ȯ��
				if (true == Check_Duplicated_Col(collider))
					continue;

				_vector center = collider->Get_Center();

				// �Ÿ� ��
				_float distance = DISTANCE(player_position - center);
				if (closest > distance)
				{
					closest = distance;
					closest_collider = collider;
				}
			}

			// Ž���� Ÿ�� ��Ͽ� �߰�
			if (true == Add_ColliderList(closest_collider))
			{
				number_of_empty_slot--;

				if (true == nearest_one_is_pushed)
					continue;

				Add_ObjectList(closest_collider->Get_Parent());
				nearest_one_is_pushed = true;
			}
		}
	}

	// ����
	m_Colliders.sort([](ASSIST_DESC sour_desc, ASSIST_DESC dest_desc)->_bool
	{
		return sour_desc.fDistance < dest_desc.fDistance;
	});

	// ���� ����
	for (auto& desc : m_Colliders)
		Calculate_Values(desc);

	// ���� Ÿ���� ���� ���
	if (nullptr == m_pFasten)
	{
		m_pLockOn = m_Colliders.front().pCollider;
		m_pLockOnObject = m_pLockOn->Get_Parent();
	}
}

void CCombat_Assistant::Clear()
{
	m_pLockOn = nullptr;
	m_pLockOnObject = nullptr;

	m_pAttack = nullptr;
	m_pAttackObject = nullptr;

	m_Colliders.clear();
	m_Objects.clear();

	if (nullptr != m_pFasten)
	{
		Add_ColliderList(m_pFasten);
	}
}

CCollider * CCombat_Assistant::Get_TargetCollider(ASSIST_TYPE* out)
{
	// ���� Ÿ���� ���� ��� ���� Ÿ���� ��ȯ
	if (nullptr != m_pFasten)
	{
		if (nullptr != out)
			*out = ASSIST_FASTEN;

		return m_pFasten;
	}

	// ���� Ÿ���� ���� ���
	else
	{
		// ���� �ȿ� �浹ü�� ���� ��� return nullptr
		if (0 >= (_int)m_Colliders.size())
			return nullptr;

		// ���� �ȿ� �浹ü�� �ִٸ� �ּ��� LockOn�� �����Ѵ�
		// Attack�� ���� ��� Attack�� ��ȯ, ���� ��� LockOn ��ȯ
		if (nullptr != m_pAttack)
		{
			if (nullptr != out)
				*out = ASSIST_ATTACK;

			return m_pAttack;
		}
		else
		{
			if (nullptr != out)
				*out = ASSIST_LOCKON;

			return m_pLockOn;
		}
	}
}

CCombat_Assistant::ASSIST_TYPE CCombat_Assistant::Get_TargetType()
{
	// ���� Ÿ���� ���� ��� ���� Ÿ���� ��ȯ
	if (nullptr != m_pFasten)
		return ASSIST_FASTEN;

	// ���� Ÿ���� ���� ���
	else
	{
		// ���� �ȿ� �浹ü�� ���� ��� return nullptr
		if (0 >= (_int)m_Colliders.size())
			return ASSIST_END;

		// ���� �ȿ� �浹ü�� �ִٸ� �ּ��� LockOn�� �����Ѵ�
		// Attack�� ���� ��� Attack�� ��ȯ, ���� ��� LockOn ��ȯ
		if (nullptr != m_pAttack)
			return ASSIST_ATTACK;
		else
			return ASSIST_LOCKON;
	}
}

CCollider * CCombat_Assistant::Get_Collider(ASSIST_TYPE _assistType)
{
	CCollider* out = nullptr;
	switch (_assistType)
	{
	case Client::CCombat_Assistant::ASSIST_LOCKON:
		out = m_pLockOn;
		break;

	case Client::CCombat_Assistant::ASSIST_FASTEN:
		out = m_pFasten;
		break;

	case Client::CCombat_Assistant::ASSIST_ATTACK:
		out = m_pAttack;
		break;
	}

	return out;
}

void CCombat_Assistant::Set_Collider(ASSIST_TYPE _assistType, CCollider * _collider)
{
	if (nullptr == _collider)
	{
		BREAKPOINT;
		return;
	}

	switch (_assistType)
	{
	case Client::CCombat_Assistant::ASSIST_LOCKON:
		m_pLockOn = _collider;
		break;

	case Client::CCombat_Assistant::ASSIST_FASTEN:
		m_pFasten = _collider;
		break;

	case Client::CCombat_Assistant::ASSIST_ATTACK:
		m_pAttack = _collider;
		break;
	}
}

CGameObject * CCombat_Assistant::Get_Object(ASSIST_TYPE _assistType)
{
	CGameObject* out = nullptr;
	switch (_assistType)
	{
	case Client::CCombat_Assistant::ASSIST_LOCKON:
		out = m_pLockOnObject;
		break;

	case Client::CCombat_Assistant::ASSIST_FASTEN:
		out = m_pFastenObject;
		break;

	case Client::CCombat_Assistant::ASSIST_ATTACK:
		out = m_pAttackObject;
		break;
	}

	return out;
}

void CCombat_Assistant::Set_Object(ASSIST_TYPE _assistType, CGameObject * _object)
{
	if (nullptr == _object)
	{
		BREAKPOINT;
		return;
	}

	switch (_assistType)
	{
	case Client::CCombat_Assistant::ASSIST_LOCKON:
		m_pLockOnObject = _object;
		break;

	case Client::CCombat_Assistant::ASSIST_FASTEN:
		m_pFastenObject = _object;
		break;

	case Client::CCombat_Assistant::ASSIST_ATTACK:
		m_pAttackObject = _object;
		break;
	}
}

void CCombat_Assistant::Setup_Target(ASSIST_TYPE _assistType, CCollider * _collider)
{
	switch (_assistType)
	{
	case Client::CCombat_Assistant::ASSIST_LOCKON:

		m_pLockOn = _collider;
		m_pLockOnObject = m_pLockOn->Get_Parent();

		break;
	case Client::CCombat_Assistant::ASSIST_FASTEN:

		m_pFasten = _collider;
		m_pFastenObject = m_pLockOn->Get_Parent();

		break;
	case Client::CCombat_Assistant::ASSIST_ATTACK:

		if (nullptr == m_pAttack)
		{
			m_pAttack = _collider;
			m_pAttackObject = _collider->Get_Parent();
		}

		break;
	}
}

void CCombat_Assistant::Clear_Target(ASSIST_TYPE _assistType)
{
	switch (_assistType)
	{
	case Client::CCombat_Assistant::ASSIST_LOCKON:

		m_pLockOn = nullptr;
		m_pLockOnObject = nullptr;

		break;
	case Client::CCombat_Assistant::ASSIST_FASTEN:

		m_pFasten = nullptr;
		m_pFastenObject = nullptr;

		break;
	case Client::CCombat_Assistant::ASSIST_ATTACK:

		m_pAttack = nullptr;
		m_pAttackObject = nullptr;

		break;
	}
}

CCombat_Assistant::ASSIST_DESC CCombat_Assistant::Get_Desc(ASSIST_TYPE _assistType)
{
	ASSIST_DESC empty_desc;
	ZeroMemory(&empty_desc, sizeof(ASSIST_DESC));

	switch (_assistType)
	{
	case Client::CCombat_Assistant::ASSIST_LOCKON:

		for (auto& desc : m_Colliders)
		{
			if (desc.pCollider == m_pLockOn)
			{
				empty_desc = desc;
				break;
			}
		}

		break;

	case Client::CCombat_Assistant::ASSIST_FASTEN:

		for (auto& desc : m_Colliders)
		{
			if (desc.pCollider == m_pFasten)
			{
				empty_desc = desc;
				break;
			}
		}

		break;

	case Client::CCombat_Assistant::ASSIST_ATTACK:

		for (auto& desc : m_Colliders)
		{
			if (desc.pCollider == m_pAttack)
			{
				empty_desc = desc;
				break;
			}
		}

		break;
	}

	return empty_desc;
}

void CCombat_Assistant::Change_FastenTarget()
{
	m_iSkipNumber++;

	if (m_iSkipNumber >= m_iSkipLimit ||
		m_iSkipNumber >= (_uint)m_Colliders.size())
		m_iSkipNumber = 0;

	if (nullptr == m_pFasten)
		return;

	auto iter = m_Colliders.begin();
	if (1 < (_int)m_Colliders.size())
	{
		for (_uint i = 0; i < m_iSkipNumber; ++i)
			iter++;
	}

	m_pFasten = (*iter).pCollider;
	m_pFastenObject = m_pFasten->Get_Parent();
}

void CCombat_Assistant::Auto_Rotation()
{
	// ��ý���Ʈ �۵�����, �ڵ�ȸ�� ��� ��뿩�� Ȯ��
	if (false == m_bPower || false == m_bAutoDir)
		return;

	ASSIST_TYPE target_type = ASSIST_END;

	// Ÿ�� ������ �Ǿ��ִ� ���¶��
	if (nullptr != m_pFasten)
	{
		target_type = ASSIST_FASTEN;
	}

	// Ÿ�� ������ �Ǿ����� ���� ��
	else
	{
		// ���� ���, ���� ��� ��
		if (nullptr != m_pAttack)
		{
			target_type = ASSIST_ATTACK;
		}

		else
		{
			target_type = ASSIST_LOCKON;
		}
	}

	CCollider* target_collider = Get_Collider(target_type);
	if (nullptr == target_collider)
		return;

	ASSIST_DESC desc;
	ZeroMemory(&desc, sizeof(ASSIST_DESC));

	desc = Get_Desc(target_type);
	m_pPlayerTransform->MakeSameDirection(XMLoadFloat3(&desc.vDirectionXZ));
}

void CCombat_Assistant::Auto_Position(_float _movementXZ, _float _movementY)
{
	CModel* player_model = static_cast<CModel*>(m_pPlayer->Get_Component(TEXT("Com_Model")));
	if (nullptr == player_model)
	{
		BREAKPOINT;
		return;
	}

	_uint temp_index = player_model->Get_TempIndex_EachBody(CPlayer::SB_BODY);

	// ��ý���Ʈ �۵�����, ��ġ���� ��� ��뿩�� Ȯ��
	if (false == m_bPower || false == m_bAutoPos)
	{
		player_model->Set_TargetMovementRatioXZ(temp_index, 1.f);
		player_model->Set_TargetMovementRatioY(temp_index, 1.f);

		return;
	}

	ASSIST_TYPE target_type = ASSIST_END;

	// Ÿ�� ������ �Ǿ��ִ� ���¶��
	if (nullptr != m_pFasten)
	{
		target_type = ASSIST_FASTEN;
	}

	// Ÿ�� ������ �Ǿ����� ���� ��
	else
	{
		// ���� ���, ���� ��� ��
		if (nullptr != m_pAttack)
		{
			target_type = ASSIST_ATTACK;
		}

		else
		{
			target_type = ASSIST_LOCKON;
		}
	}

	CCollider* target_collider = Get_Collider(target_type);
	if (nullptr == target_collider)
	{
		player_model->Set_TargetMovementRatioXZ(temp_index, 1.f);
		player_model->Set_TargetMovementRatioY(temp_index, 1.f);

		return;
	}

	ASSIST_DESC desc;
	ZeroMemory(&desc, sizeof(ASSIST_DESC));

	desc = Get_Desc(target_type);

	/*
		:: �ִϸ��̼� �̵��� ����

		float ratio = (��ü�Ÿ� - �÷��̾� ���ݰŸ�) / �ִϸ��̼� ����Ÿ�;
		if(2.0 < ratio)
		ratio = 2.f;

		2.0�� ���Ƿ� ������ ���Ѱ��̴�. ���� ���ӿ��� �ִ� 2�������� �þ�� ������ �ñ� ������ 2.0���� �����Ͽ���.
		��, '�÷��̾� ���ݰŸ�'�� �ִϸ��̼Ǹ��� �ٸ� �� �ִ�.

		�ִϸ��̼� ���� ���ݰŸ��� �����ؼ� ��Ƶΰ� �� ���� ���� �� ����.
	*/

	_float ratioXZ = 0.f;
	_float ratioY = 0.f;

	// 0 ������ �� inf�� �߻��ϴ� ���� �����ϱ����� ���ǹ�
	if (0.f < _movementXZ)
	{
		_vector collider_position = target_collider->Get_Center();
		_vector player_position = m_pPlayer->Get_Transform()->Get_Position();

		vecy(collider_position) = 0.f;
		vecy(player_position) = 0.f;

		_float distance_xz = DISTANCE(collider_position - player_position);

		ratioXZ = (distance_xz - 1.5f /* 1.5�� ���ݰŸ� */) / _movementXZ;

		// 0���� ���� ���
		if (m_fClippingMin > ratioXZ)
		{
			ratioXZ = m_fClippingMin;
		}

		// �ִ�ġ���� Ŭ ��� 
		else if (m_fClippingMax < ratioXZ)
		{
			ratioXZ = m_fClippingMax;
		}
	}

	// 0 ������ �� inf�� �߻��ϴ� ���� �����ϱ����� ���ǹ�
	if (0.f < _movementY)
	{
		ratioY = (desc.fDistanceY - 0.f) / _movementY;

		// 0���� ���� ���
		if (m_fClippingMin_Y > ratioY)
		{
			ratioY = m_fClippingMin_Y;
		}

		// �ִ�ġ���� Ŭ ��� 
		else if (m_fClippingMax_Y < ratioY)
		{
			ratioY = m_fClippingMax_Y;
		}
	}

	ratioY = 1.f;

	player_model->Set_TargetMovementRatioXZ(temp_index, ratioXZ);
	player_model->Set_TargetMovementRatioY(temp_index, ratioY);
}

_uint CCombat_Assistant::EnemyNumber_In_Range(_float _range)
{
	_uint enemy_number = 0;
	if (0 == m_Colliders.size())
		return enemy_number;

	for (auto& desc : m_Colliders)
	{
		if (desc.fDistanceXZ < _range)
			enemy_number++;
	}

	return enemy_number;
}

_bool CCombat_Assistant::Eliminate_DeadTarget(CGameObject * _deadTarget)
{
	if (nullptr == _deadTarget)
		return false;

	/*_bool erase = false;*/

	auto iter = m_Colliders.begin();
	for (auto& desc : m_Colliders)
	{
		// Ÿ�� ����� ��ȸ�ϸ鼭 �θ� _deadTarget�� �浹ü�� ���� ����
		if (desc.pCollider->Get_Parent() == _deadTarget)
		{
			m_Colliders.erase(iter);
			/*erase = true;*/

			break;
		}
		else
			iter++;
	}

	/*if (false == erase)
		return false;*/

	// �߰����� ����� �ϴ� Ÿ�ٵ�� ���ؼ� ���� ���
	// �߰� Ÿ�ٵ��� nullptr�� ����ش�
	if (m_pFastenObject == _deadTarget)
		Clear_Target(ASSIST_TYPE::ASSIST_FASTEN);

	if (m_pAttackObject == _deadTarget)
		Clear_Target(ASSIST_TYPE::ASSIST_ATTACK);

	if (m_pLockOnObject == _deadTarget)
		Clear_Target(ASSIST_TYPE::ASSIST_LOCKON);

	// Ÿ�� ��Ͽ� �� �̻� ���� ���� ���ٸ� return
	if (0 >= m_Colliders.size())
		return true;

	// ����� Ÿ���� ���� ����� Ÿ������ �ٽ� ä���
	m_pLockOn = m_Colliders.front().pCollider;
	m_pLockOnObject = m_pLockOn->Get_Parent();
	
	return true;
}

void CCombat_Assistant::Calculate_Values(ASSIST_DESC& _desc)
{
	// ������ �غ�
	_vector player_position = m_pPlayerTransform->Get_Position();
	_vector collider_center = _desc.pCollider->Get_Center();
	
	// ��, �Ʒ�
	if (vecy(player_position) <= vecy(collider_center))
		_desc.bUpDown = true;
	else
		_desc.bUpDown = false;

	// �Ÿ�, ����
	_desc.fDistance = DISTANCE(collider_center - player_position);
	_desc.fDistanceY = vecy(collider_center) - vecy(player_position);
	XMStoreFloat3(&_desc.vDirection, collider_center - player_position);

	// Y�� ���� ������ �غ�
	vecy(player_position) = 0.f;
	vecy(collider_center) = 0.f;

	// XZ�Ÿ�, XZ����, XZ����
	_desc.fDistanceXZ = DISTANCE(collider_center - player_position);
	XMStoreFloat3(&_desc.vDirectionXZ, collider_center - player_position);

	// �÷��̾ ��Ÿ���� ���� ���� �� �غ�
	_vector direction_to_collider_norm = VecNLZ3(_desc.vDirectionXZ);
	_vector player_look_norm = XMVector3Normalize(m_pPlayerTransform->Get_Look());
	_vector player_right_norm = XMVector3Normalize(m_pPlayerTransform->Get_Right());

	vecy(player_look_norm) = 0.f;
	vecy(player_right_norm) = 0.f;

	_desc.fAngleXZ = vecx(XMVector3AngleBetweenNormals(direction_to_collider_norm, player_look_norm));
	_desc.fAngleXZ = XMConvertToDegrees(_desc.fAngleXZ);

	_desc.bLeftRight = true;

	if (0 < DOT3(direction_to_collider_norm, player_right_norm))
	{
		_desc.fAngleXZ *= -1.f;
		_desc.fAngleXZ += 360.f;

		_desc.bLeftRight = false;
	}
}

_bool CCombat_Assistant::Check_Duplicated_Col(CCollider * _collider)
{
	for (auto& desc : m_Colliders)
	{
		if (desc.pCollider == _collider)
			return true;
	}

	return false;
}

_bool CCombat_Assistant::Check_Duplicated_Col(ASSIST_DESC _desc)
{
	for (auto& desc : m_Colliders)
	{
		if (desc.pCollider == _desc.pCollider)
			return true;
	}

	return false;
}

_bool CCombat_Assistant::Check_Duplicated_Obj(CGameObject* _object)
{
	for (auto& object : m_Objects)
	{
		if (object == _object)
			return true;
	}

	return false;
}

_bool CCombat_Assistant::Check_Duplicated_Obj(ASSIST_DESC _desc)
{
	for (auto& object : m_Objects)
	{
		if (object == _desc.pCollider->Get_Parent())
			return true;
	}

	return false;
}

_bool CCombat_Assistant::Add_ColliderList(CCollider * _collider)
{
	if (nullptr == _collider)
	{
		BREAKPOINT;
		return false;
	}

	ASSIST_DESC desc;
	ZeroMemory(&desc, sizeof(ASSIST_DESC));

	desc.pCollider = _collider;
	Calculate_Values(desc);
	m_Colliders.push_back(desc);
	
	return true;
}

_bool CCombat_Assistant::Rmv_ColliderList(ASSIST_DESC _desc)
{
	auto iter = m_Colliders.begin();
	for (auto& desc : m_Colliders)
	{
		if (desc.pCollider == _desc.pCollider)
		{
			m_Colliders.erase(iter);
			return true;
		}
		else
			iter++;
	}

	return false;
}

_bool CCombat_Assistant::Add_ObjectList(CGameObject* _object)
{
	if (nullptr == _object)
		return false;

	if (true == Check_Duplicated_Obj(_object))
		return false;

	return true;
}

_bool CCombat_Assistant::Rmv_ObjectList(ASSIST_DESC _desc)
{
	// �θ� ���� ��ü�� �ݶ��̴��� ã�� �� ���� ����
	_uint same_object = 0;
	for (auto& desc : m_Colliders)
	{
		if (desc.pCollider->Get_Parent() == _desc.pCollider->Get_Parent())
			same_object++;
	}

	// ������ ���� 1�� ��� �ش� �ݶ��̴� �ܿ� �ٸ� �ݶ��̴��� �θ𿡰� �پ����� ����
	if (1 != same_object)
		return false;

	// ����
	auto iter = m_Objects.begin();
	for (auto& object : m_Objects)
	{
		if (object == _desc.pCollider->Get_Parent())
		{
			m_Objects.erase(iter);
			return true;
		}
		else
			iter++;
	}

	return false;
}

CCombat_Assistant * CCombat_Assistant::Create(CGameObject* _chief, ASSIST_INIT_DESC _initDesc)
{
	CCombat_Assistant*	pInstance = new CCombat_Assistant();

	if (FAILED(pInstance->Initialize(_chief, _initDesc)))
	{
		MSGBOX("Failed to Created CCombat_Assistant");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCombat_Assistant::Free()
{
}
