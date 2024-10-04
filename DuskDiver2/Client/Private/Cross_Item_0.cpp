#include "stdafx.h"
#include "..\Public\Cross_Item_0.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Status_Manager.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "NumItemEffect.h"



CCross_Item_0::CCross_Item_0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCross_Item_0::CCross_Item_0(const CCross_Item_0 & rhs)
	: CUI(rhs)
{
}

HRESULT CCross_Item_0::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCross_Item_0::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(13.f, 26.f, 15.f, 15.f, TEXT("Prototype_GameObject_NumItemEffect"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CNumItemEffect*)(*iter);

	return S_OK;
}

void CCross_Item_0::Tick(_float fTimeDelta)
{
	switch (UM->Get_BattleItemNum(0))
	{
	case 0:
		m_pUINum->Set_Number(UM->Get_BattleItem(0));
		break;
	case 1:
		m_pUINum->Set_Number(UM->Get_BattleItem(1));
		break;
	case 2:
		m_pUINum->Set_Number(UM->Get_BattleItem(2));
		break;
	case 3:
		m_pUINum->Set_Number(UM->Get_BattleItem(3));
		break;
	case 4:
		m_pUINum->Set_Number(UM->Get_BattleItem(4));
		break;
	case 5:
		m_pUINum->Set_Number(UM->Get_BattleItem(5));
		break;
	case 6:
		m_pUINum->Set_Number(UM->Get_BattleItem(6));
		break;
	default:
		break;
	}

	if (CK_DOWN(DIK_LEFT) && !UM->Get_ShopState())
	{
		switch (UM->Get_BattleItemNum(0))
		{
		case 0:
			if (PM->Get_SelectPlayer() == CPlayer::PLAYER_01 && UM->Get_BattleItem(0) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_HP);
				_float fHp = (STM->Get_FinalHp(CStatus_Manager::PLAYER01) * 0.3f) + PM->Get_PlayerHP(CPlayer::PLAYER_01);
				if (fHp >= STM->Get_FinalHp(CStatus_Manager::PLAYER01))
					fHp = STM->Get_FinalHp(CStatus_Manager::PLAYER01);
				PM->Set_PlayerHP(fHp, CPlayer::PLAYER_01);

				UM->Use_BattleItem(0);

			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_03 && UM->Get_BattleItem(0) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_HP);
				_float fHp = (STM->Get_FinalHp(CStatus_Manager::PLAYER03) * 0.3f) + PM->Get_PlayerHP(CPlayer::PLAYER_03);
				if (fHp >= STM->Get_FinalHp(CStatus_Manager::PLAYER03))
					fHp = STM->Get_FinalHp(CStatus_Manager::PLAYER03);
				PM->Set_PlayerHP(fHp, CPlayer::PLAYER_03);
				UM->Use_BattleItem(0);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_04 && UM->Get_BattleItem(0) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_HP);
				_float fHp = (STM->Get_FinalHp(CStatus_Manager::PLAYER04) * 0.3f) + PM->Get_PlayerHP(CPlayer::PLAYER_04);
				if (fHp >= STM->Get_FinalHp(CStatus_Manager::PLAYER04))
					fHp = STM->Get_FinalHp(CStatus_Manager::PLAYER04);
				PM->Set_PlayerHP(fHp, CPlayer::PLAYER_04);
				UM->Use_BattleItem(0);
			}
			break;
		case 1:
			if (PM->Get_SelectPlayer() == CPlayer::PLAYER_01 && UM->Get_BattleItem(1) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_HP);
				PM->Set_PlayerHP(STM->Get_FinalHp(CStatus_Manager::PLAYER01), CPlayer::PLAYER_01);
				UM->Use_BattleItem(1);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_03 && UM->Get_BattleItem(1) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_HP);
				PM->Set_PlayerHP(STM->Get_FinalHp(CStatus_Manager::PLAYER03), CPlayer::PLAYER_03);
				UM->Use_BattleItem(1);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_04 && UM->Get_BattleItem(1) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_HP);
				PM->Set_PlayerHP(STM->Get_FinalHp(CStatus_Manager::PLAYER04), CPlayer::PLAYER_04);
				UM->Use_BattleItem(0);
			}
			break;
		case 2:

			if (PM->Get_SelectPlayer() == CPlayer::PLAYER_01 && UM->Get_BattleItem(2) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_SP);
				_float fSp = 30.f + PM->Get_PlayerSP(CPlayer::PLAYER_01);
				if (fSp >= 100.f)
					fSp = 100.f;
				PM->Set_PlayerSP(fSp, CPlayer::PLAYER_01);
				UM->Use_BattleItem(2);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_03 && UM->Get_BattleItem(2) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_SP);
				_float fSp = 30.f + PM->Get_PlayerSP(CPlayer::PLAYER_03);
				if (fSp >= 100.f)
					fSp = 100.f;
				PM->Set_PlayerSP(fSp, CPlayer::PLAYER_03);
				UM->Use_BattleItem(2);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_04 && UM->Get_BattleItem(2) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_SP);
				_float fSp = 30.f + PM->Get_PlayerSP(CPlayer::PLAYER_04);
				if (fSp >= 100.f)
					fSp = 100.f;
				PM->Set_PlayerSP(fSp, CPlayer::PLAYER_04);
				UM->Use_BattleItem(2);
			}
			break;
		case 3:
			if (PM->Get_SelectPlayer() == CPlayer::PLAYER_01 && UM->Get_BattleItem(3) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_SP);
				PM->Set_PlayerSP(100, CPlayer::PLAYER_01);
				UM->Use_BattleItem(3);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_03 && UM->Get_BattleItem(3) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_SP);
				PM->Set_PlayerSP(100, CPlayer::PLAYER_03);
				UM->Use_BattleItem(3);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_04 && UM->Get_BattleItem(3) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_SP);
				PM->Set_PlayerSP(100, CPlayer::PLAYER_04);
				UM->Use_BattleItem(3);
			}
			break;
		case 4:

			if (PM->Get_SelectPlayer() == CPlayer::PLAYER_01 && UM->Get_BattleItem(4) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_EP);
				_float fBp = 30.f + PM->Get_PlayerEP(CPlayer::PLAYER_01);
				if (fBp >= 100.f)
					fBp = 100.f;
				PM->Set_PlayerEP(fBp, CPlayer::PLAYER_01);
				UM->Use_BattleItem(4);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_03 && UM->Get_BattleItem(4) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_EP);
				_float fBp = 30.f + PM->Get_PlayerEP(CPlayer::PLAYER_03);
				if (fBp >= 100.f)
					fBp = 100.f;
				PM->Set_PlayerEP(fBp, CPlayer::PLAYER_03);
				UM->Use_BattleItem(4);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_04 && UM->Get_BattleItem(4) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_EP);
				_float fBp = 30.f + PM->Get_PlayerEP(CPlayer::PLAYER_04);
				if (fBp >= 100.f)
					fBp = 100.f;
				PM->Set_PlayerEP(fBp, CPlayer::PLAYER_04);
				UM->Use_BattleItem(4);
			}
			break;
		case 5:
			if (PM->Get_SelectPlayer() == CPlayer::PLAYER_01 && UM->Get_BattleItem(5) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_EP);
				PM->Set_PlayerEP(100, CPlayer::PLAYER_01);
				UM->Use_BattleItem(5);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_03 && UM->Get_BattleItem(5) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_EP);
				PM->Set_PlayerEP(100, CPlayer::PLAYER_03);
				UM->Use_BattleItem(5);
			}
			else if (PM->Get_SelectPlayer() == CPlayer::PLAYER_04 && UM->Get_BattleItem(5) > 0)
			{
				PM->Create_PortionEffect(CPlayer_Manager::PORTION_EP);
				PM->Set_PlayerEP(100, CPlayer::PLAYER_04);
				UM->Use_BattleItem(5);
			}
			break;
		case 6:
			GI->PlaySoundW(L"UI_TPFull.wav", SD_DROPITEM, 0.3f);
			if (PM->Get_PlayerHP(CPlayer::PLAYER_01) == 0)
			{
				PM->Set_PlayerRevive(CPlayer::PLAYER_01);
				UM->Use_BattleItem(6);
			}
			else if (PM->Get_PlayerHP(CPlayer::PLAYER_03) == 0)
			{
				PM->Set_PlayerRevive(CPlayer::PLAYER_03);
				UM->Use_BattleItem(6);
			}
			else if (PM->Get_PlayerHP(CPlayer::PLAYER_04) == 0)
			{
				PM->Set_PlayerRevive(CPlayer::PLAYER_04);
				UM->Use_BattleItem(6);
			}
			break;
		}
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CCross_Item_0::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCross_Item_0::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	_uint i = UM->Get_BattleItemNum(0);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CCross_Item_0::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Item"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CCross_Item_0::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CCross_Item_0 * CCross_Item_0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCross_Item_0*		pInstance = new CCross_Item_0(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCross_Item_0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCross_Item_0::Clone(void * pArg)
{
	CCross_Item_0*		pInstance = new CCross_Item_0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCross_Item_0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCross_Item_0::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
