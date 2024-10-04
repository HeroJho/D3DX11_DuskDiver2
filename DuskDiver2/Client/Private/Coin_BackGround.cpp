#include "stdafx.h"
#include "..\Public\Coin_BackGround.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "ToolManager.h"



CCoin_BackGround::CCoin_BackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCoin_BackGround::CCoin_BackGround(const CCoin_BackGround & rhs)
	: CUI(rhs)
{
}

HRESULT CCoin_BackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin_BackGround::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, 0.f, 297.5f, 164.5f, TEXT("Prototype_GameObject_Coin_BackGround2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 297.5f, 164.5f, TEXT("Prototype_GameObject_Coin_BackGround3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-64.08f, 2.09f, 102.4f, 102.4f, TEXT("Prototype_GameObject_Coin_Effect"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 297.5f, 164.5f, TEXT("Prototype_GameObject_Coin_Icon"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 297.5f, 164.5f, TEXT("Prototype_GameObject_Coin_Num"))))
		return E_FAIL;

	return S_OK;
}

void CCoin_BackGround::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && UM->Get_FrontUI() && UM->Get_CoinOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);

	if (UM->Get_CoinUp())
	{
		if (m_bSizeUp)
		{
			m_UiInfo.fSizeX = 340.f;
			m_UiInfo.fSizeY = 188.f;
			m_bSizeUp = false;
			UM->Set_CoinUp(false);
		}
	}

	if (!m_bSizeUp)

	{
		m_fSizeUpAcc += 60.f * fTimeDelta;

		if (5.f <= m_fSizeUpAcc)
		{
			m_bSizeUp = true;
			m_fSizeUpAcc = 0.f;
			m_UiInfo.fSizeX = 297.5f;
			m_UiInfo.fSizeY = 164.5f;
		}

		m_UiInfo.fSizeX -= m_fSizeUpAcc;
		m_UiInfo.fSizeY -= m_fSizeUpAcc;
	}
}

void CCoin_BackGround::LateTick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	if (!m_bRender && UM->Get_FrontUI() && UM->Get_CoinOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (!m_bRender && UM->Get_FrontUI() && UM->Get_CoinOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCoin_BackGround::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}






HRESULT CCoin_BackGround::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_BackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CCoin_BackGround::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CCoin_BackGround * CCoin_BackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCoin_BackGround*		pInstance = new CCoin_BackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCoin_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCoin_BackGround::Clone(void * pArg)
{
	CCoin_BackGround*		pInstance = new CCoin_BackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCoin_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCoin_BackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
