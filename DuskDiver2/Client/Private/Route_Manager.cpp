#include "stdafx.h"
#include "..\Public\Route_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CRoute_Manager)

CRoute_Manager::CRoute_Manager()
{
	Initialize_Route();
}

void CRoute_Manager::Move_Route(_uint iRouteIndex, CTransform * pTransformCom, _float fSpeed, _float fTimeDelta, _uint* pCurNodeIndex)
{
	// true�� �����ϸ� ù��° ��ǥ ��忡 ���������Ƿ� ��带 �ٲپ���
	if (true == pTransformCom->Move(XMLoadFloat4(&vRouteNodePosition[vRoutes[iRouteIndex][*pCurNodeIndex]]), fSpeed, fTimeDelta, 0.4f))
	{
		// �ε����� �ٲٴ� �Լ� �� input�� iCurNodeIndex
		Change_NodeIndex(iRouteIndex, pCurNodeIndex);
	}
	// ȸ���� �ϴ� �Լ�
	pTransformCom->Turn(pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat4(&Search_NewLook(pTransformCom, iRouteIndex, pCurNodeIndex)), 0.05f);
}

void CRoute_Manager::Initialize_Route()
{
	// 0�� ��Ʈ
	vRoute.clear();
	for (_uint i = 0; 4 > i; ++i)
	{
		vRoute.push_back(i);
	}
	/*vRoute.push_back(0);
	vRoute.push_back(1);
	vRoute.push_back(2);
	vRoute.push_back(3);*/
	vRoutes.push_back(vRoute);

	// 1�� ��Ʈ
	vRoute.clear();
	for (_uint i = 0; 12 > i; ++i)
	{
		vRoute.push_back(i);
	}
	vRoutes.push_back(vRoute);

	// 2�� ��Ʈ
	vRoute.clear();
	for (_uint i = 12; 30 > i; ++i)
	{
		vRoute.push_back(i);
	}
	vRoutes.push_back(vRoute);

	// 3�� ��Ʈ
	vRoute.clear();
	for (_uint i = 30; 35 > i; ++i)
	{
		vRoute.push_back(i);
	}
	vRoutes.push_back(vRoute);

	// 4�� ��Ʈ
	vRoute.clear();
	for (_uint i = 35; 41 > i; ++i)
	{
		vRoute.push_back(i);
	}
	vRoutes.push_back(vRoute);

	// 5�� ��Ʈ
	vRoute.clear();
	for (_uint i = 41; 49 > i; ++i)
	{
		vRoute.push_back(i);
	}
	vRoutes.push_back(vRoute);

	// 6�� ��Ʈ
	vRoute.clear();
	for (_uint i = 49; 68 > i; ++i)
	{
		vRoute.push_back(i);
	}
	vRoutes.push_back(vRoute);
}

void CRoute_Manager::Change_NodeIndex(_uint iRouteIndex, _uint* iCurNodeIndex)
{
	*iCurNodeIndex += 1;
	if (*iCurNodeIndex >= vRoutes[iRouteIndex].size())
	{
		*iCurNodeIndex = 0;
	}
}

_float4 CRoute_Manager::Search_NewLook(CTransform * pTransformCom, _uint iRouteIndex, _uint* pCurNodeIndex)
{
	_float4 NewLook = { 0.f,0.f,0.f,1.f };

	XMStoreFloat4(&NewLook, XMVector4Normalize(XMLoadFloat4(&vRouteNodePosition[vRoutes[iRouteIndex][*pCurNodeIndex]]) - pTransformCom->Get_State(CTransform::STATE_POSITION)));

	return NewLook;
}

void CRoute_Manager::Free()
{
}
