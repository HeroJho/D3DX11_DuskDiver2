#include "stdafx.h"
#include "..\Public\Coin_Effect.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CCoin_Effect::CCoin_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCoin_Effect::CCoin_Effect(const CCoin_Effect & rhs)
	: CUI(rhs)
{
}

HRESULT CCoin_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin_Effect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	return S_OK;
}

void CCoin_Effect::Tick(_float fTimeDelta)
{
	if (UM->Get_CoinUp())
	{
		m_bRender = true;
		m_iFrame = 0;
	}
	
	if (m_bRender)
	{
		m_fFrameAcc += 60.f * fTimeDelta;

		if (1 <= m_fFrameAcc)
		{
			m_iFrame++;
			m_fFrameAcc = 0.f;

			if (15 == m_iFrame)
			{
				m_iFrame = 0;
				m_bRender = false;
			}
		}
	}

	//if (m_bRender)
	//{
	//	if (m_fXFrame == m_fXYIndex.x - 1.f && m_fYFrame == m_fXYIndex.y - 1.f) // 맨끝번호가 되면 render꺼버리고 x, y 둘다 0으로 만들어주기
	//	{
	//		m_bRender = false;
	//		m_fXFrame = 0.f;
	//		m_fYFrame = 0.f;
	//	}

	//	m_fXFrame++;

	//	if (m_fXFrame >= m_fXYIndex.x)
	//	{
	//		m_fXFrame = 0.f;
	//		m_fYFrame++;

	//	}

	//	//테스트 위해서 넣어둔 코드
	//	if (m_fYFrame > m_fXYIndex.y)
	//	{
	//		m_fYFrame = 0;
	//	}

	//}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CCoin_Effect::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCoin_Effect::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	//m_pShaderCom->Set_RawValue("XYIndex", &m_fXYIndex, sizeof(_float2));
	//m_pShaderCom->Set_RawValue("XFrame", &m_fXFrame, sizeof(_float));
	//m_pShaderCom->Set_RawValue("YFrame", &m_fYFrame, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iFrame)))
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






HRESULT CCoin_Effect::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CCoin_Effect::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CCoin_Effect * CCoin_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCoin_Effect*		pInstance = new CCoin_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCoin_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCoin_Effect::Clone(void * pArg)
{
	CCoin_Effect*		pInstance = new CCoin_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCoin_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCoin_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
