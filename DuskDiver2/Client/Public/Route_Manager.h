#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRoute_Manager final : public CBase
{
	DECLARE_SINGLETON(CRoute_Manager)

private:
	_float4 vRouteNodePosition[68] = {
		{ 47.5f,0.f,59.5f,1.f },//0 -> 1
		{ 48.f,0.f,135.f,1.f },
		{ 56.f,0.f,142.f,1.f },
		{ 97.f,0.f,150.f,1.f },
		{ 97.f,0.f,178.5f,1.f },
		{ 30.5f,0.f,191.5f,1.f },
		{ 22.5f,0.f,202.5f,1.f },
		{ -15.f,0.f,204.f,1.f },
		{ -15.f,0.f,152.5f,1.f },
		{ 8.f,0.f,142.5f,1.f },
		{ 10.5f,0.f,116.5f,1.f },
		{ 28.f,0.f,88.5f,1.f }, // 11 -> 1
		{ 10.0f,0.f,48.5f,1.f },//12 -> 2
		{ -1.5f,0.f,61.f,1.f },
		{ -10.5f,0.f,82.f,1.f },
		{ -24.f,0.f,104.5f,1.f },
		{ -48.5f,0.f,111.f,1.f },
		{ -48.5f,0.f,141.5f,1.f },
		{ -24.f,0.f,156.5f,1.f },
		{ -22.5f,0.f,193.f,1.f },
		{ -15.f,0.f,230.5f,1.f },
		{ -3.f,0.f,230.5f,1.f },
		{ 3.f,0.f,207.5f,1.f },
		{ 21.5f,0.f,202.f,1.f },
		{ 35.5f,0.f,179.f,1.f },
		{ 35.5f,0.f,148.5f,1.f },
		{ 39.f,0.f,140.5f,1.f },
		{ 39.f,0.f,73.f,1.f },
		{ 36.f,0.f,63.5f,1.f },
		{ 31.f,0.f,58.5f,1.f }, // 29 -> 2
		{ -16.5,0.f,122.f,1.f },//30 -> 3
		{ 9.f,0.f,113.f,1.f },
		{ 42.5f,0.f,113.f,1.f },
		{ 42.5f,0.f,144.f,1.f },
		{ -16.f,0.f,144.f,1.f }, //34 -> 3
		{ 46.f,0.f,180.5f,1.f }, //35 -> 4
		{ 99.5f,0.f,183.f,1.f },
		{ 103.5f,0.f,208.5f,1.f },
		{ 97.f,0.f,206.5f,1.f },
		{ 90.f,0.f,187.f,1.f },
		{ 54.f,0.f,184.5f,1.f }, //40 -> 4
		{ -0.5f,0.f,-8.f,1.f }, //41 -> 5
		{ 32.f,0.f,-8.5f,1.f },
		{ 53.5f,0.f,-15.f,1.f },
		{ 56.f,0.f,7.f,1.f },
		{ 25.5f,0.f,6.5f,1.f },
		{ 11.f,0.f,6.5f,1.f },
		{ -20.5f,0.f,8.5f,1.f },
		{ -20.5f,0.f,-7.5f,1.f }, //48 -> 5
		{ 2.f,4.f,26.f,1.f },//49 -> 6번루트 시작
		{ 21.f,14.f,40.f,1.f },
		{ 51.f,6.f,65.f,1.f },
		{ 65.f,15.f,111.f,1.f },
		{ 120.f,-4.f,79.f,1.f },
		{ 155.f,-11.f,75.f,1.f },
		{ 178.f,20.f,121.f,1.f },
		{ 181.f,1.f,206.f,1.f },
		{ 141.f,-20.f,199.f,1.f },
		{ 101.f,-29.f,220.f,1.f },
		{ 93.f,-27.f,218.f,1.f },
		{ 25.f,10.f,230.f,1.f },
		{ 17.f,30.f,230.f,1.f },
		{ -47.f,30.f,208.f,1.f },
		{ -65.f,18.f,156.f,1.f },
		{ -102.f,1.f,145.f,1.f },
		{ -119.f,3.f,129.f,1.f },
		{ -90.f,11.f,86.f,1.f },
		{ -33.f,25.f,41.f,1.f }//67 -> 6번루트 끝
	}; // Route중의 각 노드의 월드좌표

private:
	CRoute_Manager();
	virtual ~CRoute_Manager() = default;

public:
	void Move_Route(_uint iRouteIndex, CTransform* pTransformCom, _float fSpeed, _float fTimeDelta, _uint* pCurNodeIndex);

private:
	void Initialize_Route(); // Manager생성시 루트를 만들어주는 함수
	void Change_NodeIndex(_uint iRouteIndex, _uint* iCurNodeIndex);  // 노드에 도착하고 다음 노드의 인덱스를 바꾸는 함수
	_float4 Search_NewLook(CTransform* pTransformCom, _uint iRouteIndex, _uint* pCurNodeIndex);

private:
	vector<_uint> vRoute; // 하나의 루트 : 노드의 인덱스를 가지고 있다
	vector<vector<_uint>> vRoutes; // 루트들을 총괄하는 벡터

public:
	virtual void Free() override;
};

END