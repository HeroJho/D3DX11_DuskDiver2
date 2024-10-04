#include "stdafx.h"
#include "..\Public\Player_Status01_HP.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "Status_Manager.h"
#include "PS01_STNumber.h"
#include "UI_Manager.h"

CPlayer_Status01_HP::CPlayer_Status01_HP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer_Status01_HP::CPlayer_Status01_HP(const CPlayer_Status01_HP & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer_Status01_HP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Status01_HP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	//HP 숫자
	if (FAILED(Make_ChildUI(65.6f, -14.8f, 15.f, 15.f, TEXT("Prototype_GameObject_PS01_STNumber"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CPS01_STNumber*)(*iter);

	return S_OK;
}

void CPlayer_Status01_HP::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number((_uint)PM->Get_PlayerHP(CPlayer::PLAYER_01));
	
	if (!UM->Get_StatUp1())
	{
		m_fOriginalSize = m_UiInfo.fSizeX;
		m_iOriginalHP = UM->Get_Player1Hp();
	}

	else
	{
		m_UiInfo.fSizeX = 126.7f + 3.f * UM->Get_Player1Hp();

		if (m_fOriginalSize != m_UiInfo.fSizeX)
		{
			if (!m_bOnce)
			{
				m_UiInfo.fX += 1.5f;
				m_bOnce = true;
			}
			_float fNumX = m_pUINum->Get_fX();
			fNumX += 1.5f * (UM->Get_Player1Hp() - m_iOriginalHP);
			m_pUINum->Set_fX(fNumX);
			m_UiInfo.fX += 1.5f * (UM->Get_Player1Hp() - m_iOriginalHP);
			UM->Set_StatUp1(false);
		}
	}

	//if (UM->Get_ShakeOn1())
	//{
	//	if (!m_bRemeber)
	//	{
	//		m_fOriginfX = m_UiInfo.fX;
	//		m_fOriginfY = m_UiInfo.fY;
	//		m_bRemeber = true;
	//	}
	//	
	//	m_fTimeAcc += 1.f * fTimeDelta;

	//	if (m_fTimeAcc < 0.03f)
	//	{
	//		m_UiInfo.fX += 0.7f;
	//		m_UiInfo.fY += 0.7f;
	//	}
	//	else if (m_fTimeAcc < 0.08f)
	//	{
	//		m_UiInfo.fX -= 0.7f;
	//		m_UiInfo.fY -= 0.7f;
	//	}
	//	else if (m_fTimeAcc < 0.11f)
	//	{
	//		m_UiInfo.fX += 0.7f;
	//		m_UiInfo.fY += 0.7f;
	//	}
	//	else if (m_fTimeAcc < 0.12f)
	//	{
	//		m_UiInfo.fX -= 0.7f;
	//		m_UiInfo.fY -= 0.7f;
	//	}
	//	else
	//	{
	//		UM->Set_ShakeOn1(false);
	//		m_fTimeAcc = 0.f;
	//		m_UiInfo.fX = m_fOriginfX;
	//		m_UiInfo.fY = m_fOriginfY;
	//		m_bRemeber = false;
	//	}
	//}
	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CPlayer_Status01_HP::LateTick(_float fTimeDelta)
{
	m_fNowHp = PM->Get_PlayerHP(CPlayer::PLAYER_01);
	m_fMaxHp = STM->Get_FinalHp(CStatus_Manager::PLAYER01);
	m_fHpGauge = m_fNowHp / m_fMaxHp;

	m_fPreHp = PM->Get_PlayerPreHP(CPlayer::PLAYER_01);
	m_fPreHpGauge = m_fPreHp / m_fMaxHp;

	m_bPreUp = PM->Get_PlayerPreUp();
	m_bPreDown = PM->Get_PlayerPreDown();

	m_fMinusGauge = 1.f - m_fHpGauge;
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer_Status01_HP::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_HP", &m_fHpGauge, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_HPColor", &m_fMinusGauge, sizeof(_bool));
	m_pShaderCom->Set_RawValue("g_PreHP", &m_fPreHpGauge, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_Up", &m_bPreUp, sizeof(_bool));
	m_pShaderCom->Set_RawValue("g_Down", &m_bPreDown, sizeof(_bool));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (PM->Get_ReHp(CPlayer::PLAYER_01))
	{
		//부활했을때는
		if (FAILED(m_pShaderCom->Begin(40)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pShaderCom->Begin(19)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CPlayer_Status01_HP::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status_HP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Status01_HP::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer_Status01_HP * CPlayer_Status01_HP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Status01_HP*		pInstance = new CPlayer_Status01_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_Status01_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Status01_HP::Clone(void * pArg)
{
	CPlayer_Status01_HP*		pInstance = new CPlayer_Status01_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer_Status01_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer_Status01_HP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
