#include "stdafx.h"
#include "..\Public\Bear.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "UI.h"
#include "Bear1.h"
#include "Player_Manager.h"
#include "Monster_Manager.h"
#include "UI_Number.h"
#include "Layer.h"

CBear::CBear(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBear::CBear(const CBear & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBear::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBear::Initialize(void * pArg)
{
	//두더지 네마리 생성하고 list에다가 두기
	if (FAILED(Ready_Bear(TEXT("Layer_Arcade"))))
		return E_FAIL;

	m_fPlayerPos = { 0.f, 0.f, 0.f, 1.f };


	m_fMaxHp = 1000.f;
	m_fNowHp = m_fMaxHp;

	m_fPreHp = m_fNowHp;

	MM->Add_Bear(this);
	UM->BearHunt_On();

	//if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	//	return E_FAIL;

	return S_OK;
}

void CBear::Tick(_float fTimeDelta)
{
	//자식 전부 다  false

	/* if (m_fNowHp <= 0.f)
	 {
	 	m_fNowHp = 0.f;
	 	if (!m_bStartEndCut)
	 	{	
	 		TM->End_CutScene_DG();
	 		m_bStartEndCut = true;
	 	}

	 }*/

	if (!m_pBear.empty())
	{
		for (auto pBear : m_pBear)
		{
			if (!static_cast<CBear1*>(pBear)->Get_DeadOn())
				static_cast<CBear1*>(pBear)->Hit_OnOff(false);
		}
	}

	if (TM->Get_EndDG())
	{
		if (!m_bStartEndCut)
		{
			TM->End_CutScene_DG();
			m_bStartEndCut = true;
			Dead_AllBear();
			GI->PlaySoundW(L"END1.wav", SD_MAP, 1.f);

			for (auto pBear : m_pBear)
			{
				if (static_cast<CBear1*>(pBear)->Get_MonsterSpawn())
				{
					CLayer* pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Layer_BearMonster")))->second;


					for (auto& pObj : *pLayer->Get_GameObjects())
						pObj->Set_Dead();
				}
			}

	
		}
	}
}

void CBear::LateTick(_float fTimeDelta)
{
	if (!m_pBear.empty())
		Find_NearstBear(fTimeDelta);
}

void CBear::Dead_AllBear()
{
	for (auto& iter : m_pBear)
	{
		static_cast<CBear1*>(iter)->Render_OnOff(false); 

	}
}

HRESULT CBear::Ready_Bear(const _tchar * pLayerTag)
{
	//0
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));
	
	_tInfo.iIndex = 0;
	_tInfo.vPosition = { -180.86f, 38.7f, 136.778f, 1.f };

	CGameObject*		pBear1 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear1));
	if (nullptr == pBear1)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear1);


	CGameObject*		pBear2 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear2));
	if (nullptr == pBear2)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear2);

	//1
	_tInfo.iIndex = 1;
	_tInfo.vPosition = { -184.46f, 38.7f, 129.894f, 1.f };


	CGameObject*		pBear3 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear3));
	if (nullptr == pBear3)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear3);

	CGameObject*		pBear4 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear4));
	if (nullptr == pBear4)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear4);

	//2
	_tInfo.iIndex = 2;
	_tInfo.vPosition = { -188.6f, 38.7f, 123.665f, 1.f };


	CGameObject*		pBear5 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear5));
	if (nullptr == pBear5)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear5);

	CGameObject*		pBear6 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear6));
	if (nullptr == pBear6)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear6);

	//3
	_tInfo.iIndex = 3;
	_tInfo.vPosition = { -192.29f, 38.7f, 117.568f, 1.f };

	CGameObject*		pBear7 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear7));
	if (nullptr == pBear7)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear7);

	CGameObject*		pBear8 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("Bear1"), LEVEL_STAGE4, pLayerTag, &_tInfo, &pBear8));
	if (nullptr == pBear8)
		return E_FAIL;

	m_pBear.push_back((CCreture*)pBear8);

	return S_OK;
}

HRESULT CBear::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 524.f;
	UiInfoDesc.fSizeY = 42.f;
	UiInfoDesc.fX = 650.5f;
	UiInfoDesc.fY = 76.9f;
	UiInfoDesc.pDesc = this;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Bear_HP"), TM->Get_CurLevel(), pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 467.f;
	UiInfoDesc.fY = 81.38f;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Bear_HP_Bar"), TM->Get_CurLevel(), pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	return S_OK;
}

void CBear::Find_NearstBear(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	if (!static_cast<CBear1*>(m_pBear.front())->Get_DeadOn())
	{
		CCreture* pNearstBear = nullptr;
		_float fNeastDis = FLT_MAX;

		for (auto& pBear : m_pBear)
		{
			_vector vBearPos = XMLoadFloat4(&((CCreture*)pBear)->Get_WorldPos());
			_float fDis = XMVectorGetX(XMVector3Length(vBearPos - XMLoadFloat4(&m_fPlayerPos)));
			if (fDis < fNeastDis)
			{
				pNearstBear = (CCreture*)pBear;
				fNeastDis = fDis;
			}
		}

		if (pNearstBear != nullptr)
		{
			static_cast<CBear1*>(pNearstBear)->Hit_OnOff(true);
		}
	}
}

void CBear::Set_PreBar(_float fTimeDelta)
{

	if (m_fPreHp > m_fNowHp)
	{
		m_fMinusHp = (m_fPreHp - m_fNowHp) / m_fMaxHp;
		m_fPreHp -= m_fMinusHp * 5000.f * fTimeDelta;

		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}
}

void CBear::Check_TotalHp()
{
	m_fNowHp = 1000.f - static_cast<CBear1*>(m_pBear[BEAR1])->Get_BearDamage() - static_cast<CBear1*>(m_pBear[BEAR2])->Get_BearDamage() - static_cast<CBear1*>(m_pBear[BEAR3])->Get_BearDamage()
		- static_cast<CBear1*>(m_pBear[BEAR4])->Get_BearDamage() - static_cast<CBear1*>(m_pBear[BEAR5])->Get_BearDamage() - static_cast<CBear1*>(m_pBear[BEAR6])->Get_BearDamage()
		- static_cast<CBear1*>(m_pBear[BEAR7])->Get_BearDamage() - static_cast<CBear1*>(m_pBear[BEAR8])->Get_BearDamage();

	if (m_fNowHp >= 1000.f)
		m_fNowHp = 1000.f;
}


CBear * CBear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBear*		pInstance = new CBear(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBear::Clone(void * pArg)
{
	CBear*		pInstance = new CBear(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBear::Free()
{
	__super::Free();


	for (auto iter : m_pBear)
		Safe_Release(iter);

	m_pBear.clear();

}
