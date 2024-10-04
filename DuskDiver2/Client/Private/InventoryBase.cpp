#include "stdafx.h"
#include "..\Public\InventoryBase.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "NumExp.h"


CInventoryBase::CInventoryBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CInventoryBase::CInventoryBase(const CInventoryBase & rhs)
	: CUI(rhs)
{
}

HRESULT CInventoryBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventoryBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(0.f, 0.f, 1280.f, 750.f, TEXT("Prototype_GameObject_DuskDiverBackGround"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(0.f, -295.f, 1280.f, 150.f, TEXT("Prototype_GameObject_InventoryUpside"))))
		return E_FAIL;


	//if (FAILED(Make_ChildUI(118.3f, -339.3f, 20.f, 20.f, TEXT("Prototype_GameObject_NumExp"))))
	//	return E_FAIL;

	//vector<CUI*>::iterator iter = m_pChildUIs.begin();
	//iter += 2;
	//m_pUINum = (CNumExp*)(*iter);

	if (FAILED(Make_ChildUI(580.83f, -339.182f, 66.f, 28.f, TEXT("Prototype_GameObject_Hunger"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-27.5f, 23.7f, 860.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_InventoryWindow"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-305.f, -243.5f, 64.f * 0.5f, 64.f * 0.5f, TEXT("Prototype_GameObject_InvenQ"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-212.5f, -242.5f, 40.f * 0.6f, 40.f * 0.6f, TEXT("Prototype_GameObject_InvenBattleItem"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-120.f, -242.5f, 40.f * 0.7f, 40.f * 0.7f, TEXT("Prototype_GameObject_InvenFood"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(32.5f, -242.5f, 40.f * 0.7f, 40.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipment"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(65.f, -242.5f, 40.f * 0.7f, 40.f * 0.7f, TEXT("Prototype_GameObject_InvenDropItem"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(157.6f, -242.5f, 40.f * 0.7f, 40.f * 0.7f, TEXT("Prototype_GameObject_InvenQuestItem"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(252.f, -243.5f, 64.f * 0.5f, 64.f * 0.5f, TEXT("Prototype_GameObject_InvenE"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(460.f, -170.5f, 530.f * 0.65f, 210.f * 0.65f, TEXT("Prototype_GameObject_InvenEquipmentBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(460.f, 18.f, 530.f * 0.65f, 820.f * 0.65f, TEXT("Prototype_GameObject_InvenBattleItemBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(460.f, 18.f, 530.f * 0.65f, 820.f * 0.65f, TEXT("Prototype_GameObject_InvenFoodBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(460.f, -170.5f, 530.f * 0.65f, 210.f * 0.65f, TEXT("Prototype_GameObject_InvenQuestItemBackground"))))
		return E_FAIL;




	if (FAILED(Make_ChildUI(-27.5f, -206.f, 860.f * 0.7f, 50.f * 0.7f, TEXT("Prototype_GameObject_InventorySelectBar"))))
		return E_FAIL;



	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_DropItemSlot0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -171.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_DropItemSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -136.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_DropItemSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -101.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_DropItemSlot3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -66.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_DropItemSlot4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -31.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_DropItemSlot5"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_BattleItemSlot0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -171.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_BattleItemSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -136.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_BattleItemSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -101.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_BattleItemSlot3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -66.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_BattleItemSlot4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -31.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_BattleItemSlot5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, 4.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_BattleItemSlot6"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_FoodSlot0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -171.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_FoodSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -136.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_FoodSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -101.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_FoodSlot3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -66.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_FoodSlot4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -31.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_FoodSlot5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, 4.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_FoodSlot6"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot6"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot7"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot8"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot9"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot10"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot11"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot12"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot13"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-90.f, -206.f, 588.f * 0.7f, 30.f * 0.7f, TEXT("Prototype_GameObject_InvenEquipSlot14"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(449.67f, -180.f, 420.f * 0.7f, 48.f * 0.7f, TEXT("Prototype_GameObject_EquipmentText"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(398.36f, -75.72f, 224.f * 0.55f, 210.f * 0.55f, TEXT("Prototype_GameObject_SelectSlot0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(459.1f, -133.02f, 219.f * 0.55f, 206.f * 0.55f, TEXT("Prototype_GameObject_SelectSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(522.02f, -78.83f, 222.f * 0.55f, 214.f * 0.55f, TEXT("Prototype_GameObject_SelectSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(457.217f, -16.79f, 218.f * 0.55f, 215.f * 0.55f, TEXT("Prototype_GameObject_SelectSlot3"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(397.69f, -72.f, 128.f * 0.55f, 128.f * 0.55f, TEXT("Prototype_GameObject_SelectImage0"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(461.f, -135.68f, 128.f * 0.55f, 128.f * 0.55f, TEXT("Prototype_GameObject_SelectImage1"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(520.f, -75.f, 128.f * 0.55f, 128.f * 0.55f, TEXT("Prototype_GameObject_SelectImage2"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(461.f, -16.79f, 128.f * 0.55f, 128.f * 0.55f, TEXT("Prototype_GameObject_SelectImage3"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(0.f, 0.f, 1049.f * 0.7f, 406.f * 0.7f, TEXT("Prototype_GameObject_EatText"))))
		return E_FAIL;



	return S_OK;
}

void CInventoryBase::Tick(_float fTimeDelta)
{
	if (UM->Get_BeFull() > 0)
	{
		m_fHungryTime += fTimeDelta;
	}

	if (m_fHungryTime > 30.f)
	{
		UM->Flow_BeFull();
		m_fHungryTime = 0.f;
	}

	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
	{
		m_bRender = true;
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
	}
	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
	{
		m_bRender = false;
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
	}


	if (!m_bRender)
	{
		m_bSelect = false;
		m_UiInfo.fX = 1920.f;
	}

	if (m_bRender)
	{

		if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && m_bSelect && m_iNum == 1 && !m_bEatPage && m_iInvenSelNum > 0 && UM->Get_BeFull() < 4)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bEatPage = true;
		}

		if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && m_bSelect && m_iNum == 0 && m_iBattleNum == 0)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bBattleSelect = false;
			m_bBattle = true;
			m_iBattleNum = 5;
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && m_bSelect && m_iBattleNum == 5)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_iInveType = 1;
			UM->Set_InvenType(m_iInveType);
			m_iBattleNum = 0;
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && m_bSelect && m_iNum == 1)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_iInveType = 2;
			UM->Set_InvenType(m_iInveType);
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && m_bSelect && m_iNum == 2)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_iInveType = 3;
			UM->Set_InvenType(m_iInveType);
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && m_bSelect && m_iNum == 3)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_iInveType = 4;
			UM->Set_InvenType(m_iInveType);
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && m_bSelect && m_iNum == 4)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_iInveType = 5;
			UM->Set_InvenType(m_iInveType);
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 5 && !m_bSelect)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
		}


		if (m_iInveType > 0)
		{
			if (CK_DOWN(DIK_DOWN) && m_iBattleNum == 0)
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iInvenSelNum == 15)
					m_iInvenSelNum = 15;
				else
					m_iInvenSelNum++;

				m_bBattleSelect = true;
			}

			if (CK_DOWN(DIK_UP) && m_iBattleNum == 0)
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iInvenSelNum == 1)
					m_iInvenSelNum = 1;
				else
					m_iInvenSelNum--;

				m_bBattleSelect = true;
			}
		}

		if (m_bBattle)
			m_fBattlTime += fTimeDelta;

		if (m_bEatPage)
			m_fEatTime += fTimeDelta;

		if (m_fEatTime > 1.f && CK_DOWN(DIK_RETURN))
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			UM->Eat_Food(UM->Get_FoodType());
			m_fEatTime = 0;
			m_bEatPage = false;

			if(UM->Get_FoodType() == 0 || UM->Get_FoodType() == 2 || UM->Get_FoodType() == 6)
				UM->Eat_Str();
			if (UM->Get_FoodType() == 1 || UM->Get_FoodType() == 4 || UM->Get_FoodType() == 5)
				UM->Eat_Hp();
			if (UM->Get_FoodType() == 3)
				UM->Add_Exp(500);

			GI->PlaySoundW(L"Eat2.wav", SD_UI, 0.8f);
		}





		if (CK_DOWN(DIK_BACK) && m_iBattleNum > 0)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_iBattleNum = 0;
			UM->Set_BattleNum(m_iBattleNum);
		}

		else if (CK_DOWN(DIK_BACK) && m_iBattleNum == 0)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_iBattleNum = 0;
			UM->Set_BattleNum(m_iBattleNum);
		}

		if (CK_DOWN(DIK_BACK) && m_bSelect && m_iInveType == 0)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelect = false;
			m_iInvenSelNum = 1;
		}

		else if (CK_DOWN(DIK_BACK) && m_iInveType > 0)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_iInveType = 0;
			UM->Set_InvenType(m_iInveType);
			m_iInvenSelNum = 1;
		}



		if (!m_bSelect)
		{
			if (m_UiInfo.fX >= 1920.f)
				m_UiInfo.fX = 1920.f;
			else
				m_UiInfo.fX += 128.f;
		}
		else if (m_bSelect)
		{
			if (m_UiInfo.fX <= 640.f)
				m_UiInfo.fX = 640.f;
			else
				m_UiInfo.fX -= 128.f;
		}


		//UI_InputDebug(fTimeDelta);

	//	m_pUINum->Set_Number(UM->Get_Exp());



		if (m_bRender)
		{
			__super::Tick(fTimeDelta);
		}


		if (m_iInveType == 1 && CK_DOWN(DIK_RIGHT) && m_iBattleNum > 0)
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			m_iBattleNum = 3;
			UM->Set_BattleNum(m_iBattleNum);
		}
		else if (m_iInveType == 1 && CK_DOWN(DIK_UP) && m_iBattleNum > 0)
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			m_iBattleNum = 2;
			UM->Set_BattleNum(m_iBattleNum);
		}
		else if (m_iInveType == 1 && CK_DOWN(DIK_LEFT) && m_iBattleNum > 0)
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			m_iBattleNum = 1;
			UM->Set_BattleNum(m_iBattleNum);
		}
		else if (m_iInveType == 1 && CK_DOWN(DIK_DOWN) && m_iBattleNum > 0)
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			m_iBattleNum = 4;
			UM->Set_BattleNum(m_iBattleNum);
		}

		if (m_fBattlTime > 0.5f && CK_DOWN(DIK_RETURN))
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			if (m_iBattleNum == 1)
				UM->Set_BattleItemNum(0, m_iInvenSelNum - 1);
			if (m_iBattleNum == 2)
				UM->Set_BattleItemNum(1, m_iInvenSelNum - 1);
			if (m_iBattleNum == 3)
				UM->Set_BattleItemNum(2, m_iInvenSelNum - 1);
			if (m_iBattleNum == 4)
				UM->Set_BattleItemNum(3, m_iInvenSelNum - 1);


			m_fBattlTime = 0;
		}






		if (CK_DOWN(DIK_Q) && UM->Get_BackNum() == 5)
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			if (m_iNum != 0)
				m_iNum--;
			else
				m_iNum = 0;
		}
		else if (CK_DOWN(DIK_E) && UM->Get_BackNum() == 5)
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			if (m_iNum != 4)
				m_iNum++;
			else
				m_iNum = 4;
		}
	}
	UM->Set_InvenNum(m_iNum);
	UM->Set_Inven(m_bSelect);
	UM->Set_InvenSelNum(m_iInvenSelNum);
	UM->Set_UnSelect(m_bBattleSelect);
	UM->Set_EatText(m_bEatPage);
}

void CInventoryBase::LateTick(_float fTimeDelta)
{

	if (m_bRender)
	{
		__super::LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInventoryBase::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (m_bRender)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출


	if (m_bRender)
	{
		__super::Render();
	}

	return S_OK;
}






HRESULT CInventoryBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CInventoryBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CInventoryBase * CInventoryBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInventoryBase*		pInstance = new CInventoryBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInventoryBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventoryBase::Clone(void * pArg)
{
	CInventoryBase*		pInstance = new CInventoryBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CInventoryBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CInventoryBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
