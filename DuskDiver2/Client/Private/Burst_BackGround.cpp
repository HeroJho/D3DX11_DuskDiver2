#include "stdafx.h"
#include "..\Public\Burst_BackGround.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"

CBurst_BackGround::CBurst_BackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBurst_BackGround::CBurst_BackGround(const CBurst_BackGround & rhs)
	: CUI(rhs)
{
}

HRESULT CBurst_BackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBurst_BackGround::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, 0.f, 281.6f, 281.6f, TEXT("Prototype_GameObject_Burst_BarBG"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 281.6f, 281.6f, TEXT("Prototype_GameObject_Burst_Bar"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 281.6f, 281.6f, TEXT("Prototype_GameObject_Burst_Num"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 281.6f, 281.6f, TEXT("Prototype_GameObject_Burst_Text"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 281.6f, 281.6f, TEXT("Prototype_GameObject_Burst_Effect"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(5.f, 10.5f, 332.8f, 332.8f, TEXT("Prototype_GameObject_Burst_Effect2"))))
		return E_FAIL;


	return S_OK;
}

void CBurst_BackGround::Tick(_float fTimeDelta)
{
	if (CPlayer::PLAYER_01 == PM->Get_SelectPlayer())
	{
		if (PM->Get_PlayerBurstSkillOn(CPlayer::PLAYER_01) && !m_bOut)
		{
			m_UiInfo.fX -= 500.f * fTimeDelta;

			if (m_UiInfo.fX <= -150.f)
			{
				m_UiInfo.fX = -150.f;
				m_bOut = true;
			}
		}

		if (!PM->Get_PlayerBurstSkillOn(CPlayer::PLAYER_01) && m_bOut)
		{
			m_UiInfo.fX += 500.f * fTimeDelta;

			if (m_UiInfo.fX >= 150.f)
			{
				m_UiInfo.fX = 150.f;
				m_bOut = false;
			}
		}
	}

	else if (CPlayer::PLAYER_03 == PM->Get_SelectPlayer())
	{
		if (PM->Get_PlayerBurstSkillOn(CPlayer::PLAYER_03) && !m_bOut)
		{
			m_UiInfo.fX -= 500.f * fTimeDelta;

			if (m_UiInfo.fX <= -150.f)
			{
				m_UiInfo.fX = -150.f;
				m_bOut = true;
			}
		}

		if (!PM->Get_PlayerBurstSkillOn(CPlayer::PLAYER_03) && m_bOut)
		{
			m_UiInfo.fX += 500.f * fTimeDelta;

			if (m_UiInfo.fX >= 150.f)
			{
				m_UiInfo.fX = 150.f;
				m_bOut = false;
			}
		}
	}

	else if (CPlayer::PLAYER_04 == PM->Get_SelectPlayer())
	{
		if (PM->Get_PlayerBurstSkillOn(CPlayer::PLAYER_04) && !m_bOut)
		{
			m_UiInfo.fX -= 500.f * fTimeDelta;

			if (m_UiInfo.fX <= -150.f)
			{
				m_UiInfo.fX = -150.f;
				m_bOut = true;
			}
		}

		if (!PM->Get_PlayerBurstSkillOn(CPlayer::PLAYER_04) && m_bOut)
		{
			m_UiInfo.fX += 500.f * fTimeDelta;

			if (m_UiInfo.fX >= 150.f)
			{
				m_UiInfo.fX = 150.f;
				m_bOut = false;
			}
		}
	}
	

	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CBurst_BackGround::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;
	
	if (!m_bRender && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (!m_bRender && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBurst_BackGround::Render()
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


	if (CPlayer::PLAYER_01 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_01))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	else if (CPlayer::PLAYER_03 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_03))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	else if (CPlayer::PLAYER_04 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_04))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}



	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}






HRESULT CBurst_BackGround::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_BackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBurst_BackGround::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBurst_BackGround * CBurst_BackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBurst_BackGround*		pInstance = new CBurst_BackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBurst_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBurst_BackGround::Clone(void * pArg)
{
	CBurst_BackGround*		pInstance = new CBurst_BackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBurst_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBurst_BackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
