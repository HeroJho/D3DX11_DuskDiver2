#include "stdafx.h"
#include "..\Public\Blind.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CBlind::CBlind(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBlind::CBlind(const CBlind & rhs)
	: CUI(rhs)
{
}

HRESULT CBlind::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlind::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	UM->BackUp_Blind(this);

	return S_OK;
}

void CBlind::Tick(_float fTimeDelta)
{
		if (true == m_bMoving)
		{
			Update_Blind();
		}

	//UI_InputDebug(fTimeDelta);


	__super::Tick(fTimeDelta);
}

void CBlind::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);


	if (true == m_bClose)
		m_iTextureIndex = 0;
	else
		m_iTextureIndex = 1;
	

	//if (true == m_bMoveOver)
	//{
	//	if (Check_HideTime(fTimeDelta)) // 닫김이 끝나면 렌더하지마라 (로딩화면을 가린다)
	//	{
	//		return;
	//	}
	//}
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBlind::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	//if (true == m_bClose)
	//{
	//	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex))) //0
	//		return E_FAIL;
	//}

	//if (false == m_bClose)
	//{
	//	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1))) //1
	//		return E_FAIL;
	//}

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex))) //0
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


HRESULT CBlind::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blind"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBlind::Move_Blind(_bool b)
{
	m_bMoving = true;
	m_bClose = b;
	m_bMoveOver = false;// 움직임을 넣었으므로 렌더를 방해하는 요소 제거
}

void CBlind::Update_Blind()
{
	if (true == m_bClose)
	{
		if (false == m_bChanged)
		{
			m_UiInfo.fY = 1440.f; //->0.f면 닫힘
			m_bChanged = true;
			GI->PlaySoundW(L"SE_SceneSwitch.wav", SD_UI, 0.7f);
		}
		Close_Blind();
	}
	else if (false == m_bClose)
	{
		if (false == m_bChanged)
		{
			m_UiInfo.fY = 720.f; //-> -720.f면 열림
			m_bChanged = true;
			GI->PlaySoundW(L"SE_SceneSwitch.wav", SD_UI, 0.7f);
		}
		Open_Blinde();
	}
}

void CBlind::Close_Blind()
{
	UM->Set_FrontUI(false);
	if (m_UiInfo.fY <= 0.f)
	{
		m_UiInfo.fY = 0.f; // 닫
		m_bChanged = false;
		m_bMoving = false;
		m_bMoveOver = true;
	}
	else
		m_UiInfo.fY -= 16.f;
}

void CBlind::Open_Blinde()
{
	if (m_UiInfo.fY <= -720.f)
	{
		m_UiInfo.fY = -720.f;   // 열
		m_bChanged = false;
		m_bMoving = false;
	}
	else
		m_UiInfo.fY -= 16.f;
}

_bool CBlind::Check_HideTime(_float fTimeDelta)
{
	m_fHideTime += 1.f * fTimeDelta;

	if (1.f < m_fHideTime)
	{
		m_fHideTime = 0.f;
		m_bMoveOver = false;
		return true;
	}
		
	return false;
}

void CBlind::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBlind * CBlind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBlind*		pInstance = new CBlind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlind"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBlind::Clone(void * pArg)
{
	CBlind*		pInstance = new CBlind(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBlind"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBlind::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
