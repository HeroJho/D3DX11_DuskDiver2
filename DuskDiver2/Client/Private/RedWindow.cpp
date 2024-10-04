#include "stdafx.h"
#include "..\Public\RedWindow.h"
#include "GameInstance.h"

#include "ToolManager.h"

CRedWindow::CRedWindow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CRedWindow::CRedWindow(const CRedWindow & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRedWindow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRedWindow::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_RedWindow";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CRedWindow::Tick(_float fTimeDelta)
{

	// 현실세계
	if (!TM->Get_Stage3_Bool())
	{
		m_fAlpha = 0.f;
		m_bBring = false;
		m_fSpeed = 0.2f;
	}
	else
	{
		m_fAlpha += m_fSpeed * fTimeDelta;
		if (1.f < m_fAlpha)
		{
			m_fAlpha = 1.f;
			m_bBring = true;
			m_fSpeed = 10.f;
		}

		if (m_bBring)
		{
			m_fTimeAcc += fTimeDelta;
			if (m_fTime < m_fTimeAcc)
			{
				m_fTime = GI->Get_FloatRandom(4.f, 8.f);
				m_iCount = GI->Get_Random(1, 4);
				m_fTimeAcc = 0.f;
			}
			else
			{
				if (0.95f < m_fAlpha && 0 < m_iCount)
				{
					m_fAlpha = 0.f;
					--m_iCount;
				}
			}
		}

	}

}

void CRedWindow::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (0.f < m_fAlpha)
	{
		Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

}

HRESULT CRedWindow::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_float fGama = 1.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	_float fBlurPow = m_fAlpha * 0.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPow, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRedWindow::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_RedWindow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CRedWindow * CRedWindow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRedWindow*		pInstance = new CRedWindow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRedWindow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRedWindow::Clone(void * pArg)
{
	CRedWindow*		pInstance = new CRedWindow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRedWindow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRedWindow::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
