#include "..\public\Collision_Manager.h"
#include "GameInstance.h"
READY_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

_bool CCollision_Manager::Collision_AABB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		BREAKPOINT;
		return false;
	}

	CCollider::STATE eCollidingState;

	eCollidingState = pSrcCollider->Erase_Colliding(pDestCollider);
	pDestCollider->Erase_Colliding(pSrcCollider);

	if (true == pSrcCollider->Get_AABB()->Intersects(*pDestCollider->Get_AABB()))
	{
		switch (eCollidingState)
		{
		case CCollider::STATE_ENTER:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_STAY:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_EXIT:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		case CCollider::STATE_END:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		}

		pSrcCollider->Set_IsCollision(true);
		pDestCollider->Set_IsCollision(true);

		return true;
	}

	switch (eCollidingState)
	{
	case CCollider::STATE_ENTER:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	case CCollider::STATE_STAY:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	}

	pSrcCollider->Set_IsCollision(false);
	pDestCollider->Set_IsCollision(false);

	return false;
}

_bool CCollision_Manager::Collision_OBB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		BREAKPOINT;
		return false;
	}

	CCollider::STATE eCollidingState;

	eCollidingState = pSrcCollider->Erase_Colliding(pDestCollider);
	pDestCollider->Erase_Colliding(pSrcCollider);

	if (nullptr != pDestCollider->Get_AABB())
	{
		if (true == pSrcCollider->Get_OBB()->Intersects(*pDestCollider->Get_AABB()))
		{
			switch (eCollidingState)
			{
			case CCollider::STATE_ENTER:
				pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
				break;
			case CCollider::STATE_STAY:
				pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
				break;
			case CCollider::STATE_EXIT:
				pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
				break;
			case CCollider::STATE_END:
				pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
				break;
			}

			pSrcCollider->Set_IsCollision(true);
			pDestCollider->Set_IsCollision(true);

			return true;
		}
	}
	else
	{
		if (true == pSrcCollider->Get_OBB()->Intersects(*pDestCollider->Get_OBB()))
		{
			switch (eCollidingState)
			{
			case CCollider::STATE_ENTER:
				pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
				break;
			case CCollider::STATE_STAY:
				pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
				break;
			case CCollider::STATE_EXIT:
				pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
				break;
			case CCollider::STATE_END:
				pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
				pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
				break;
			}

			pSrcCollider->Set_IsCollision(true);
			pDestCollider->Set_IsCollision(true);

			return true;
		}
	}

	switch (eCollidingState)
	{
	case CCollider::STATE_ENTER:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	case CCollider::STATE_STAY:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	}

	pSrcCollider->Set_IsCollision(false);
	pDestCollider->Set_IsCollision(false);

	return false;
}

_bool CCollision_Manager::Collision_Sphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		BREAKPOINT;
		return false;
	}
	// 각각의 충돌 상태 별로 Enter, Stay, Exit 구분해서 넣어줄 변수
	CCollider::STATE eCollidingState;

	eCollidingState = pSrcCollider->Erase_Colliding(pDestCollider);
	pDestCollider->Erase_Colliding(pSrcCollider);

	if (true == pSrcCollider->Get_Sphere()->Intersects(*pDestCollider->Get_Sphere()))
	{
		switch (eCollidingState)
		{
		case CCollider::STATE_ENTER:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_STAY:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_EXIT:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		case CCollider::STATE_END:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		}

		pSrcCollider->Set_IsCollision(true);
		pDestCollider->Set_IsCollision(true);

		return true;
	}

	switch (eCollidingState)
	{
	case CCollider::STATE_ENTER:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	case CCollider::STATE_STAY:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	}

	pSrcCollider->Set_IsCollision(false);
	pDestCollider->Set_IsCollision(false);

	return false;
}

_bool CCollision_Manager::Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider * pDestCollider, _float* pDist)
{
	if (nullptr == pDestCollider)
	{
		BREAKPOINT;
		return false;
	}

	_bool bCollision = pDestCollider->Get_Sphere()->Intersects(vStartPosition, vDir, *pDist);

	pDestCollider->Set_IsCollision(bCollision);

	return bCollision;
}

_bool CCollision_Manager::Collision_RayToAABB(_fvector vStartPosition, _fvector vDir, CCollider * pDestCollider, _float * pDist)
{
	if (nullptr == pDestCollider)
	{
		BREAKPOINT;
		return false;
	}

	_bool bCollision = pDestCollider->Get_AABB()->Intersects(vStartPosition, vDir, *pDist);

	pDestCollider->Set_IsCollision(bCollision);

	return bCollision;
}

_bool CCollision_Manager::Collision_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		BREAKPOINT;
		return false;
	}

	// 각각의 충돌 상태 별로 Enter, Stay, Exit 구분해서 넣어줄 변수
	CCollider::STATE eCollidingState;

	eCollidingState = pSrcCollider->Erase_Colliding(pDestCollider);
	pDestCollider->Erase_Colliding(pSrcCollider);

	if (true == pSrcCollider->Get_AABB()->Intersects(*pDestCollider->Get_Sphere()))
	{
		switch (eCollidingState)
		{
		case CCollider::STATE_ENTER:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_STAY:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_EXIT:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		case CCollider::STATE_END:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		}

		pSrcCollider->Set_IsCollision(true);
		pDestCollider->Set_IsCollision(true);

		return true;
	}

	switch (eCollidingState)
	{
	case CCollider::STATE_ENTER:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	case CCollider::STATE_STAY:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	}

	pSrcCollider->Set_IsCollision(false);
	pDestCollider->Set_IsCollision(false);

	return false;
}

_bool CCollision_Manager::Collision_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	if (nullptr == pSrcCollider || nullptr == pDestCollider)
	{
		BREAKPOINT;
		return false;
	}

	// 각각의 충돌 상태 별로 Enter, Stay, Exit 구분해서 넣어줄 변수
	CCollider::STATE eCollidingState;

	eCollidingState = pSrcCollider->Erase_Colliding(pDestCollider);
	pDestCollider->Erase_Colliding(pSrcCollider);

	if (true == pSrcCollider->Get_OBB()->Intersects(*pDestCollider->Get_Sphere()))
	{
		switch (eCollidingState)
		{
		case CCollider::STATE_ENTER:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_STAY:
			pSrcCollider->Add_Colliding(CCollider::STATE_STAY, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_STAY, pSrcCollider);
			break;
		case CCollider::STATE_EXIT:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		case CCollider::STATE_END:
			pSrcCollider->Add_Colliding(CCollider::STATE_ENTER, pDestCollider);
			pDestCollider->Add_Colliding(CCollider::STATE_ENTER, pSrcCollider);
			break;
		}

		pSrcCollider->Set_IsCollision(true);
		pDestCollider->Set_IsCollision(true);

		return true;
	}

	switch (eCollidingState)
	{
	case CCollider::STATE_ENTER:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	case CCollider::STATE_STAY:
		pSrcCollider->Add_Colliding(CCollider::STATE_EXIT, pDestCollider);
		pDestCollider->Add_Colliding(CCollider::STATE_EXIT, pSrcCollider);
		break;
	}

	pSrcCollider->Set_IsCollision(false);
	pDestCollider->Set_IsCollision(false);

	return false;
}

_bool CCollision_Manager::Collision_Enter_AABB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_AABB(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_ENTER == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Enter_OBB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_OBB(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_ENTER == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Enter_Sphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_Sphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_ENTER == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Enter_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_AABBToSphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_ENTER == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Enter_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_OBBToSphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_ENTER == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Stay_AABB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_AABB(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_STAY == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Stay_OBB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_OBB(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_STAY == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Stay_Sphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_Sphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_STAY == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Stay_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_AABBToSphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_STAY == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Stay_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_OBBToSphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_STAY == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Exit_AABB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_AABB(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_EXIT == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Exit_OBB(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_OBB(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_EXIT == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Exit_Sphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_Sphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_EXIT == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Exit_AABBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_AABBToSphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_EXIT == eCollidingState)
		return true;

	return false;
}

_bool CCollision_Manager::Collision_Exit_OBBToSphere(CCollider * pSrcCollider, CCollider * pDestCollider)
{
	Collision_OBBToSphere(pSrcCollider, pDestCollider);

	CCollider::STATE eCollidingState = pSrcCollider->Find_Colliding(pDestCollider);
	if (CCollider::STATE_EXIT == eCollidingState)
		return true;

	return false;
}

list<CCollider*>* CCollision_Manager::Get_ColliderList(string pColliderTag)
{
	list<CCollider*>* pColliderList = nullptr;

	for (auto& Collider : m_ColliderMap)
	{
		if (Collider.first == pColliderTag)
		{
			pColliderList = &Collider.second;
		}
	}

	if (nullptr == pColliderList)
	{
		m_ColliderMap.emplace(pColliderTag, list<CCollider*>());
		pColliderList = &(m_ColliderMap.at(pColliderTag));
	}

	return pColliderList;
}

HRESULT CCollision_Manager::Add_Collider(string pColliderTag, CCollider* pCollider)
{
	if (nullptr == pCollider)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (true == pCollider->Get_Parent()->Get_Dead())
		return S_OK;

	COLLIDERLIST* pColliderList = Get_ColliderList(pColliderTag);
	if (nullptr == pColliderList)
	{
		COLLIDERLIST ColliderList;
		ColliderList.push_back(pCollider);

		m_ColliderMap.emplace(pColliderTag, ColliderList);
	}
	else
		pColliderList->push_back(pCollider);

	return S_OK;
}

void CCollision_Manager::Clear_Collider()
{
	for (auto& ColliderList : m_ColliderMap)
		ColliderList.second.clear();

	m_ColliderMap.clear();
}

HRESULT CCollision_Manager::Pop_DeadObjectCollider(string pColliderTag, CCollider * pCollider)
{
	if (nullptr == pCollider)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	auto pair = m_ColliderMap.find(pColliderTag);

	// 입력받은 태그로 등록된 모든 콜라이더를 순회
	for (auto& collider : pair->second)
		collider->Erase_Colliding_All(pCollider);

	return S_OK;
}

void CCollision_Manager::Free()
{
}
