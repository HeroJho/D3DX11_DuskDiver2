#include "stdafx.h"
#include "..\Public\MiniGame_Yabawe.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "Yabawe.h"
#include "RealYabawe.h"
#include "Layer.h"
#include "YabaweBall.h"
#include "Camera_Manager.h"
#include "Bear_HP_Bar.h"
#include "UI_Manager.h"

CMiniGame_Yabawe::CMiniGame_Yabawe(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CMiniGame_Yabawe::CMiniGame_Yabawe(const CMiniGame_Yabawe & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniGame_Yabawe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame_Yabawe::Initialize(void * pArg)
{
	if(nullptr == pArg)
		return E_FAIL;

	MINIGAMEYABADESC* pDesc = (MINIGAMEYABADESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	Make_ImGui();
	m_sTag = "Tag_YabaMgr";

	m_pTransformCom->Set_WorldMatrix(pDesc->mMatrix);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 3.f, 1.f));
	//m_pTransformCom->RotationThree(_float3(1.f, 0.f, 0.f), 0.f, _float3(0.f, 1.f, 0.f), 0.f, _float3(0.f, 0.f, 1.f), 0.f);

	m_Matrix = pDesc->mMatrix;
	// XMStoreFloat4x4(&m_Matrix, m_pTransformCom->Get_WorldMatrix());

	Make_Yabawe();
	Make_Loot();


	return S_OK;
}




void CMiniGame_Yabawe::Level_Up()
{
	if (0 < m_iShakeCount)
		return;

	++m_iLevel;

	switch (m_iLevel)
	{
	case 1:
		m_iShakeCount = 2;
		m_fShakeSpeed = 7.f;
		m_fSens = 0.1f;
		m_fStopDis = 0.1f;
		break;
	case 2:
		m_iShakeCount = 5;
		m_fShakeSpeed = 8.f;
		m_fSens = 0.25f;
		m_fStopDis = 0.1f;

		//m_bEnd = true;
		//CRM->Get_Cam()->End_Hold();
		break;
	case 3:
		m_iShakeCount = 7;
		m_fShakeSpeed = 10.f;
		m_fSens = 0.25f;
		m_fStopDis = 0.2f;
		break;
	case 4:
		m_iShakeCount = 7;
		m_fShakeSpeed = 15.f;
		m_fSens = 0.5f;
		m_fStopDis = 0.3f;
		break;
	case 5:
		m_iShakeCount = 8;
		m_fShakeSpeed = 20.f;
		m_fSens = 0.9f;
		m_fStopDis = 0.3f;
		break;
	case 6:
		m_iShakeCount = 12;
		m_fShakeSpeed = 30.f;
		m_fSens = 0.9f;
		m_fStopDis = 0.3f;
		break;
	case 7:
		m_iShakeCount = 15;
		m_fShakeSpeed = 40.f;
		m_fSens = 0.9f;
		m_fStopDis = 0.3f;
		break;
	case 8:
		m_iShakeCount = 20;
		m_fShakeSpeed = 50.f;
		m_fSens = 0.9f;
		m_fStopDis = 0.7f;
		break;
	case 9:
		m_iShakeCount = 20;
		m_fShakeSpeed = 65.f;
		m_fSens = 1.f;
		m_fStopDis = 1.f;
		break;
	case 10:
		m_iShakeCount = 10;
		m_fShakeSpeed = 100.f;
		m_fSens = 1.f;
		m_fStopDis = 1.f;
		break;
	case 11:
	{
		m_bEnd = true;
		CRM->Get_Cam()->End_Hold();
		UM->Set_FrontUI(false);
		GI->PlaySoundW(L"END1.wav", SD_MAP, 1.f);
	}
		break;
	default:
		break;
	}

}

void CMiniGame_Yabawe::Make_Yabawe()
{
	CYabawe::YABAWEDESC Desc;
	CGameObject* pObj = nullptr;


	Desc.vPos = _float3(0.f, 0.7f, -10.f);
	Desc.mMatrix = m_Matrix;
	GI->Add_Get_GameObjectToLayer(TEXT("YavaweBall"), TM->Get_CurLevel(), L"Mini_Yabawe", &Desc, &pObj);
	m_pYabaweBall = (CYabaweBall*)pObj;


	Desc.vPos = _float3(-5.f, 0.f, 0.f);
	Desc.mMatrix = m_Matrix;
	GI->Add_Get_GameObjectToLayer(TEXT("Yabawe"), TM->Get_CurLevel(), TEXT("Mini_Yabawe"), &Desc, &pObj);
	m_pYabawes[0] =  (CYabawe*)pObj;

	Desc.vPos = _float3(0.f, 0.f, 0.f);
	Desc.mMatrix = m_Matrix;
	GI->Add_Get_GameObjectToLayer(TEXT("RealYabawe"), TM->Get_CurLevel(), TEXT("Mini_Yabawe"), &Desc, &pObj);
	m_pYabawes[1] = (CRealYabawe*)pObj;
	m_pRealYabawe = (CRealYabawe*)pObj;

	Desc.vPos = _float3(5.f, 0.f, 0.f);
	Desc.mMatrix = m_Matrix;
	GI->Add_Get_GameObjectToLayer(TEXT("Yabawe"), TM->Get_CurLevel(), TEXT("Mini_Yabawe"), &Desc, &pObj);
	m_pYabawes[2] = (CYabawe*)pObj;
}

void CMiniGame_Yabawe::Make_Loot()
{
	//           4
	//		 0       1
	//  [0]		[1]		[2]
	//       3       2
	//           5

	// 0
	XMStoreFloat3(&m_Loots[0][0], XMVector3TransformCoord(XMLoadFloat3(&_float3(-5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[0][1], XMVector3TransformCoord(XMLoadFloat3(&_float3(-2.5f, 0.f, 5.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[0][2], XMVector3TransformCoord(XMLoadFloat3(&_float3(-0.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	// 1
	XMStoreFloat3(&m_Loots[1][0], XMVector3TransformCoord(XMLoadFloat3(&_float3(0.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[1][1], XMVector3TransformCoord(XMLoadFloat3(&_float3(2.5f, 0.f, 5.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[1][2], XMVector3TransformCoord(XMLoadFloat3(&_float3(5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	// 2
	XMStoreFloat3(&m_Loots[2][0], XMVector3TransformCoord(XMLoadFloat3(&_float3(0.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[2][1], XMVector3TransformCoord(XMLoadFloat3(&_float3(2.5f, 0.f, -5.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[2][2], XMVector3TransformCoord(XMLoadFloat3(&_float3(5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	// 3
	XMStoreFloat3(&m_Loots[3][0], XMVector3TransformCoord(XMLoadFloat3(&_float3(-5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[3][1], XMVector3TransformCoord(XMLoadFloat3(&_float3(-2.5f, 0.f, -5.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[3][2], XMVector3TransformCoord(XMLoadFloat3(&_float3(0.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));

	// 4
	XMStoreFloat3(&m_Loots[4][0], XMVector3TransformCoord(XMLoadFloat3(&_float3(-5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[4][1], XMVector3TransformCoord(XMLoadFloat3(&_float3(0.f, 0.f, 6.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[4][2], XMVector3TransformCoord(XMLoadFloat3(&_float3(5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	// 5
	XMStoreFloat3(&m_Loots[5][0], XMVector3TransformCoord(XMLoadFloat3(&_float3(-5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[5][1], XMVector3TransformCoord(XMLoadFloat3(&_float3(0.f, 0.f, -6.f)), XMLoadFloat4x4(&m_Matrix)));
	XMStoreFloat3(&m_Loots[5][2], XMVector3TransformCoord(XMLoadFloat3(&_float3(5.f, 0.f, 0.f)), XMLoadFloat4x4(&m_Matrix)));
}

void CMiniGame_Yabawe::Shake_Random()
{
	// 두 숫자를 뽑는다.
	_int iIndex_1 = GI->Get_Random(0, 2);
	_int iIndex_2 = GI->Get_Random(0, 2);
	if (iIndex_1 == iIndex_2)
		iIndex_2 = (iIndex_2 + 1) % 3;


	_int iPass_1 = 0;
	_bool bInv_1 = false;
	_int iPass_2 = 0;
	_bool bInv_2 = false;
	// 둘의 차가 2가 난다면 4, 5번 루트
	if (1 < abs(iIndex_1 - iIndex_2))
	{
		//           4
		//		 0       1
		//  [0]		[1]		[2]
		//       3       2
		//           5

		// 인덱스 사이의 경로를 추출한다.
		_int iRan = GI->Get_Random(0, 1);
		if (0 == iIndex_1 && 2 == iIndex_2)
		{
			if (iRan)
			{
				iPass_1 = 4;
				iPass_2 = 5;

				bInv_1 = false;
				bInv_2 = true;
			}
			else
			{
				iPass_1 = 5;
				iPass_2 = 4;

				bInv_1 = false;
				bInv_2 = true;
			}
		}
		else if (2 == iIndex_1 && 0 == iIndex_2)
		{
			if (iRan)
			{
				iPass_1 = 4;
				iPass_2 = 5;

				bInv_1 = true;
				bInv_2 = false;
			}
			else
			{
				iPass_1 = 5;
				iPass_2 = 4;

				bInv_1 = true;
				bInv_2 = false;
			}
		}


	}
	// 아니라면 0, 1, 2, 3 루트
	else
	{
		//           4
		//		 0       1
		//  [0]		[1]		[2]
		//       3       2
		//           5


		// 인덱스 사이의 경로를 추출한다.
		_int iRan = GI->Get_Random(0, 1);
		if (0 == iIndex_1 && 1 == iIndex_2)
		{
			if (iRan)
			{
				iPass_1 = 0;
				iPass_2 = 3;

				bInv_1 = false;
				bInv_2 = true;
			}
			else
			{
				iPass_1 = 3;
				iPass_2 = 0;

				bInv_1 = false;
				bInv_2 = true;
			}
		}
		else if (1 == iIndex_1 && 0 == iIndex_2)
		{
			if (iRan)
			{
				iPass_1 = 0;
				iPass_2 = 3;

				bInv_1 = true;
				bInv_2 = false;
			}
			else
			{
				iPass_1 = 3;
				iPass_2 = 0;

				bInv_1 = true;
				bInv_2 = false;
			}
		}
		else if (1 == iIndex_1 && 2 == iIndex_2)
		{
			//           4
			//		 0       1
			//  [0]		[1]		[2]
			//       3       2
			//           5

			if (iRan)
			{
				iPass_1 = 1;
				iPass_2 = 2;

				bInv_1 = false;
				bInv_2 = true;
			}
			else
			{
				iPass_1 = 2;
				iPass_2 = 1;

				bInv_1 = false;
				bInv_2 = true;
			}
		}
		else if (2 == iIndex_1 && 1 == iIndex_2)
		{
			if (iRan)
			{
				iPass_1 = 1;
				iPass_2 = 2;

				bInv_1 = true;
				bInv_2 = false;
			}
			else
			{
				iPass_1 = 2;
				iPass_2 = 1;

				bInv_1 = true;
				bInv_2 = false;
			}
		}



	}

	Shake(iIndex_1, iPass_1, bInv_1, m_fShakeSpeed, m_fSens, iIndex_2, iPass_2, bInv_2, m_fShakeSpeed, m_fSens, m_fStopDis);

}

void CMiniGame_Yabawe::Shake(_uint iIndex_1, _uint iRootIndex_1, _bool bInv_1, _float fSpeed_1, _float fSens_1, _uint iIndex_2, _uint iRootIndex_2, _bool bInv_2, _float fSpeed_2, _float fSens_2, _float fStopDis)
{
	m_bIsShake = true;

	m_pYabawes[iIndex_1]->Set_Root(m_Loots[iRootIndex_1], bInv_1, fSpeed_1, fSens_1, fStopDis);
	m_pYabawes[iIndex_2]->Set_Root(m_Loots[iRootIndex_2], bInv_2, fSpeed_2, fSens_2, fStopDis);

	// 인덱스 바꾼다
	CYabawe* pTemp = m_pYabawes[iIndex_1];
	m_pYabawes[iIndex_1] = m_pYabawes[iIndex_2];
	m_pYabawes[iIndex_2] = pTemp;
}






void CMiniGame_Yabawe::Tick(_float fTimeDelta)
{
	Emp_Col();


	//           4
	//		 0       1
	//  [0]		[1]		[2]
	//       3       2
	//           5
	if (CK_DOWN(DIK_F8))
	{
		// Level_Up();
	}


	if (0 < m_iShakeCount && !m_bIsShake)
	{
		Shake_Random();
		--m_iShakeCount;
	}

}

void CMiniGame_Yabawe::LateTick(_float fTimeDelta)
{
	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Choose_Col(nullptr);
}








HRESULT CMiniGame_Yabawe::Render()
{

	return S_OK;
}

HRESULT CMiniGame_Yabawe::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.3f, 3.f, 3.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "MiniGame_Yabawe_Triger");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

CMiniGame_Yabawe * CMiniGame_Yabawe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniGame_Yabawe*		pInstance = new CMiniGame_Yabawe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniGame_Yabawe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniGame_Yabawe::Clone(void * pArg)
{
	CMiniGame_Yabawe*		pInstance = new CMiniGame_Yabawe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniGame_Yabawe"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniGame_Yabawe::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}






void CMiniGame_Yabawe::Set_Reset()
{
	CLayer* pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Mini_Yabawe")))->second;

	for (auto& pObj : *pLayer->Get_GameObjects())
		pObj->Set_Dead();

	Set_Dead();

	CRM->Get_Cam()->End_Target();
}



void CMiniGame_Yabawe::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Col(*m_Colliders.begin());

	ImGui::End();
}






