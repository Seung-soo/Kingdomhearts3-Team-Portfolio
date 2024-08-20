#pragma once

#include "Collider.h"

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
	MAKE_SINGLETON(CCollision_Manager)
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	list<CCollider*>*	Get_ColliderList(string pColliderTag);

public:
	HRESULT				Add_Collider(string pColliderTag, CCollider* pCollider);
	void				Clear_Collider();

	HRESULT				Pop_DeadObjectCollider(string pColliderTag, CCollider* pCollider);

public:
	// �浹 ���θ� �˷��ִ� �Լ�
	_bool				Collision_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);

	// ó�� �浹 ���� ���� �������ִ� �Լ�
	_bool				Collision_Enter_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Enter_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);

	// �浹���� ���� �������ִ� �Լ�
	_bool				Collision_Stay_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Stay_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);

	// �浹�� �� ������ ���� �������ִ� �Լ�
	_bool				Collision_Exit_AABB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_OBB(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_Sphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_AABBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);
	_bool				Collision_Exit_OBBToSphere(CCollider* pSrcCollider, CCollider* pDestCollider);

	// ���̿� �� �浹
	_bool				Collision_RayToSphere(_fvector vStartPosition, _fvector vDir, CCollider* pDestCollider, _float* pDist);
	_bool				Collision_RayToAABB(_fvector vStartPosition, _fvector vDir, CCollider* pDestCollider, _float* pDist);


private:
	typedef list<CCollider*>			COLLIDERLIST;
	map <string, COLLIDERLIST>			m_ColliderMap;

public:
	virtual void	Free() override;
};

END