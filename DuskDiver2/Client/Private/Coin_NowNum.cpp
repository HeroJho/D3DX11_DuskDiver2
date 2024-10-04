#include "stdafx.h"
#include "..\Public\Coin_NowNum.h"
#include "GameInstance.h"
#include "UI_Manager.h"


CCoin_NowNum::CCoin_NowNum(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{

}

CCoin_NowNum::CCoin_NowNum(const CCoin_NowNum & rhs)
	: CUI(rhs)
{

}

HRESULT CCoin_NowNum::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin_NowNum::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CCoin_NowNum::Tick(_float fTimeDelta)
{
	if (UM->Get_CoinUp())
	{
		if (m_bSizeUp)
		{
			m_UiInfo.fSizeX = 28.6f;
			m_UiInfo.fSizeY = 37.7f;
			m_bSizeUp = false;
			//UM->Set_CoinUp(false);
		}
	}

	if(!m_bSizeUp)

	{
		m_fSizeUpAcc -= 50.f * fTimeDelta;

		if (-1.f >= m_fSizeUpAcc)
		{
			m_bSizeUp = true;
			m_fSizeUpAcc = 3.f;
			m_UiInfo.fSizeX = 19.8f;
			m_UiInfo.fSizeY = 26.1f;
		}

		m_UiInfo.fSizeX -= m_fSizeUpAcc;
		m_UiInfo.fSizeY -= m_fSizeUpAcc;
	}
	


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CCoin_NowNum::LateTick(_float fTimeDelta)
{


	__super::LateTick(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CCoin_NowNum::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	_float4x4 mStoreMax;
	
	_float fX = m_TotalInfo.fX;

	//이거 넣으면 뒷숫자로 정렬
	fX -= m_UiInfo.fSizeX * 0.67f *  m_sNumber.size();

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

		if (FAILED(m_pShaderCom->Begin(10)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;


 		fX += m_UiInfo.fSizeX * 0.77f;
	}

	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}



// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CCoin_NowNum::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Test Int"))
	{
		++m_iTestNum;
		Set_Number(m_iTestNum);
	}


	ImGui::End();
}


void CCoin_NowNum::Set_Number(_uint iNum)
{
	m_iNumber = iNum;
	m_sNumber = to_string(m_iNumber);
}



HRESULT CCoin_NowNum::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CCoin_NowNum * CCoin_NowNum::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCoin_NowNum*		pInstance = new CCoin_NowNum(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCoin_NowNum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCoin_NowNum::Clone(void * pArg)
{
	CCoin_NowNum*		pInstance = new CCoin_NowNum(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCoin_NowNum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCoin_NowNum::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
