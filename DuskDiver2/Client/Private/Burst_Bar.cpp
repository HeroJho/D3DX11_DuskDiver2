#include "stdafx.h"
#include "..\Public\Burst_Bar.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"

CBurst_Bar::CBurst_Bar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBurst_Bar::CBurst_Bar(const CBurst_Bar & rhs)
	: CUI(rhs)
{
}

HRESULT CBurst_Bar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBurst_Bar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CBurst_Bar::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);

	if (0 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_01))
		{
			//오르는 값
			m_fBurstBp = (PM->Get_PlayerBP(CPlayer::PLAYER_01) / 100.f) * 170.f;
		
			//각도
			m_fBurstBp = 170.f - m_fBurstBp;

			if (m_fBurstBp > 120.f)
				UM->Set_BurstBarNum(0); //120

			else if (m_fBurstBp <= 120.f && m_fBurstBp > 60.f)
				UM->Set_BurstBarNum(1); //140

			else if (m_fBurstBp <= 60.f && m_fBurstBp > 0.f)
				UM->Set_BurstBarNum(2); //160
			
			else
				UM->Set_BurstBarNum(3); //180

			// 0보다 작을때는 다 보이도록
			if (m_fBurstBp <= 0.f)
			{
				m_fLimitAngle = 400.f;
			}
			else
				m_fLimitAngle = 300.f;
		}
	}

	else if (1 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_03))
		{
			//오르는 값
			m_fBurstBp = (PM->Get_PlayerBP(CPlayer::PLAYER_03) / 100.f) * 170.f;

			//각도
			m_fBurstBp = 170.f - m_fBurstBp;

			if (m_fBurstBp > 120.f)
				UM->Set_BurstBarNum(0); //120

			else if (m_fBurstBp <= 120.f && m_fBurstBp > 60.f)
				UM->Set_BurstBarNum(1); //140

			else if (m_fBurstBp <= 60.f && m_fBurstBp > 0.f)
				UM->Set_BurstBarNum(2); //160

			else
				UM->Set_BurstBarNum(3); //180

			// 0보다 작을때는 다 보이도록
			if (m_fBurstBp <= 0.f)
			{
				m_fLimitAngle = 400.f;
			}
			else
				m_fLimitAngle = 300.f;
		}
	}

	else if (2 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_04))
		{
			//오르는 값
			m_fBurstBp = (PM->Get_PlayerBP(CPlayer::PLAYER_04) / 100.f) * 170.f;

			//각도
			m_fBurstBp = 170.f - m_fBurstBp;

			if (m_fBurstBp > 120.f)
				UM->Set_BurstBarNum(0); //120

			else if (m_fBurstBp <= 120.f && m_fBurstBp > 60.f)
				UM->Set_BurstBarNum(1); //140

			else if (m_fBurstBp <= 60.f && m_fBurstBp > 0.f)
				UM->Set_BurstBarNum(2); //160

			else
				UM->Set_BurstBarNum(3); //180

			// 0보다 작을때는 다 보이도록
			if (m_fBurstBp <= 0.f)
			{
				m_fLimitAngle = 400.f;
			}
			else
				m_fLimitAngle = 300.f;
		}
	}

	__super::Tick(fTimeDelta);
}

void CBurst_Bar::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBurst_Bar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBP", &m_fBurstBp, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fLimitAngle", &m_fLimitAngle, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(25)))
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






HRESULT CBurst_Bar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Bar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBurst_Bar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::DragFloat("BP ", &m_fBurstBp, 0.f);

	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;


	ImGui::End();
}



CBurst_Bar * CBurst_Bar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBurst_Bar*		pInstance = new CBurst_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBurst_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBurst_Bar::Clone(void * pArg)
{
	CBurst_Bar*		pInstance = new CBurst_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBurst_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBurst_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
