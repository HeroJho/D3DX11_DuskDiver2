#include "stdafx.h"
#include "..\Public\BlockWin.h"
#include "GameInstance.h"


#include "ToolManager.h"



CBlockWin::CBlockWin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBlockWin::CBlockWin(const CBlockWin & rhs)
	: CUI(rhs)
{
}

HRESULT CBlockWin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBlockWin::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CBlockWin::Tick(_float fTimeDelta)
{
	_bool b = TM->Get_Stage3_Bool();
	_bool b2 = TM->Get_Stage2_2Bool();
	// 현실 세계
	if (!TM->Get_Stage2_2Bool() && !TM->Get_Stage3_Bool())
	{
		if (m_bTriger)
		{
			m_bTriger = false;
			m_fAlpha = 1.f;
		}

		if (0.f < m_fAlpha)
			m_fAlpha -= 0.5f * fTimeDelta;
		else
			m_fAlpha = 0.f;
	}
	else
	{
		m_bTriger = true;
		m_fAlpha = 0.f;
	}



	__super::Tick(fTimeDelta);
}

void CBlockWin::LateTick(_float fTimeDelta)
{
	if ((TM->Get_Stage2_2Bool() && 0.f == m_fAlpha) || (TM->Get_Stage3_Bool() && 0.f == m_fAlpha))
		return;

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBlockWin::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	//_float fGama = 0.8f;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
	//	return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CBlockWin::Ready_Components()
{


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CBlockWin::Handle_Hover()
{
	int a = 0;
}

void CBlockWin::Handle_Down()
{
	int a = 0;
}

void CBlockWin::Handle_Press()
{
	int a = 0;
}

void CBlockWin::Handle_Click()
{

}




void CBlockWin::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::End();
}



CBlockWin * CBlockWin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBlockWin*		pInstance = new CBlockWin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlockWin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBlockWin::Clone(void * pArg)
{
	CBlockWin*		pInstance = new CBlockWin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBlockWin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBlockWin::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
}
