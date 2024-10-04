#include "stdafx.h"
#include "..\Public\ShopUseMoney.h"
#include "GameInstance.h"
#include "UI_Manager.h"


CShopUseMoney::CShopUseMoney(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CShopUseMoney::CShopUseMoney(const CShopUseMoney & rhs)
	: CUI(rhs)
{

}

HRESULT CShopUseMoney::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopUseMoney::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CShopUseMoney::Tick(_float fTimeDelta)
{
	if (UM->Get_UsedMoney())
		m_fVanish += fTimeDelta * 0.5f;
	else
		m_fVanish = 0.f;

	if (m_fVanish > 0.8f && UM->Get_UsedMoney())
	{
		UM->Set_UsedMoney(false);
		m_fVanish = 0.f;
	}
	if (UM->Get_ShopSelect() == 0 || UM->Get_ShopSelect() == 2)
		m_iType = 0;
	else
		m_iType = 1;



	//UI_InputDebug(fTimeDelta);
	
	__super::Tick(fTimeDelta);
}

void CShopUseMoney::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);

	if (UM->Get_UsedMoney())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CShopUseMoney::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fVanish, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_Type", &m_iType, sizeof(_float));
	

	_float4x4 mStoreMax;
	
	_float fX = m_TotalInfo.fX;

	for (_uint i = 0; i < m_sNumber.size(); ++i)
	{
		_uint iIndex = m_sNumber[i] - 48;

		_matrix mMax = m_pTransformCom->Get_WorldMatrix();
		mMax.r[3] = XMVectorSet(fX - g_iWinSizeX * 0.5f, -m_TotalInfo.fY + g_iWinSizeY * 0.5f, 0.0f, 1.f);


		mMax = XMMatrixTranspose(mMax);
		XMStoreFloat4x4(&mStoreMax, mMax);
		m_pShaderCom->Set_RawValue("g_WorldMatrix", &mStoreMax, sizeof(_float4x4));

		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", iIndex)))
			return E_FAIL;


		if (UM->Get_UsedMoney())
		{
			if (FAILED(m_pShaderCom->Begin(37)))
				return E_FAIL;


			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}


 		fX += m_UiInfo.fSizeX * 0.67f;
	}

	//UI_RenderDebug();

	// 자식들의 Render 호출
	//__super::Render();


	return S_OK;
}



// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CShopUseMoney::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Test Int"))
	{
		++m_iTestNum;
		Set_Number(m_iTestNum);
	}


	ImGui::End();
}


void CShopUseMoney::Set_Number(_uint iNum)
{
	m_iNumber = iNum;
	m_sNumber = to_string(m_iNumber);
}



HRESULT CShopUseMoney::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CShopUseMoney * CShopUseMoney::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopUseMoney*		pInstance = new CShopUseMoney(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNumBuy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopUseMoney::Clone(void * pArg)
{
	CShopUseMoney*		pInstance = new CShopUseMoney(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNumBuy"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShopUseMoney::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
