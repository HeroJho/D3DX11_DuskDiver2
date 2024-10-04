#include "stdafx.h"
#include "..\Public\Player_Status03.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "PS03_Number.h"
#include "HP.h"
#include "Status_Manager.h"

CPlayer_Status03::CPlayer_Status03(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer_Status03::CPlayer_Status03(const CPlayer_Status03 & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer_Status03::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Status03::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(-14.336615f, 16.680101f, 126.7f, 15.4f, TEXT("Prototype_GameObject_Player_Status03_HP"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.140245f, 28.645380f, 164.5f, 16.1f, TEXT("Prototype_GameObject_Player_Status03_SP"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(26.930470f, 44.687756f, 212.1f, 18.2f, TEXT("Prototype_GameObject_Player_Status03_BP"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-131.f, 47.6f, 13.f, 13.f, TEXT("Prototype_GameObject_PS03_Number"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	iter += 3;
	m_pUINum = (CPS03_Number*)(*iter);


	return S_OK;
}

void CPlayer_Status03::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number(UM->Get_Player2Level());


	if (UM->Get_ShakeOn2())
	{
		if (!m_bRemeber)
		{
			m_fOriginfX = 193.f;
			m_fOriginfY = 68.1f;
			m_bRemeber = true;
		}

		m_fTimeAcc += 1.f * fTimeDelta;

		if (m_fShakeTime < m_fTimeAcc)
		{
			UM->Set_ShakeOn2(false);
			m_fTimeAcc = 0.f;
			m_UiInfo.fX = m_fOriginfX;
			m_UiInfo.fY = m_fOriginfY;
			m_bRemeber = false;
		}

		_float fTempX = GI->Get_FloatRandom(-4.f, 4.f);
		_float fTempZ = GI->Get_FloatRandom(-4.f, 4.f);

		m_UiInfo.fX += fTempX;
		m_UiInfo.fY += fTempZ;
	}

	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && CPlayer::PLAYER_03 == PM->Get_SelectPlayer() && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CPlayer_Status03::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	if (!m_bRender && CPlayer::PLAYER_03 == PM->Get_SelectPlayer() && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (!m_bRender && CPlayer::PLAYER_03 == PM->Get_SelectPlayer() && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer_Status03::Render()
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


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	/*if (m_bDebug)
		UI_RenderDebug();*/



	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}






HRESULT CPlayer_Status03::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status03"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Status03::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer_Status03 * CPlayer_Status03::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Status03*		pInstance = new CPlayer_Status03(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_Status03"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Status03::Clone(void * pArg)
{
	CPlayer_Status03*		pInstance = new CPlayer_Status03(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer_Status03"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer_Status03::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
