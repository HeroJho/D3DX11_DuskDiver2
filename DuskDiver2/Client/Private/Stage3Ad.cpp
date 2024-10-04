#include "stdafx.h"
#include "..\Public\Stage3Ad.h"
#include "GameInstance.h"

CStage3Ad::CStage3Ad(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CStage3Ad::CStage3Ad(const CStage3Ad & rhs)
	:CGameObject(rhs)
{
}

HRESULT CStage3Ad::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage3Ad::Initialize(void * pArg)
{
	m_ADDesc = *(ADDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_Prop";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CStage3Ad::Tick(_float fTimeDelta)
{
	Update_AD(fTimeDelta);
}

void CStage3Ad::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CStage3Ad::Render()
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

	_float fGama = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	
	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveU", &m_fMoveUVu, sizeof(_float))))
		return E_FAIL;


	if (m_ADDesc.iADType == TYPE_ACC)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_LineBallTexture", m_ADDesc.iADIndex)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		/*if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		iPassIndex = 2;*/

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_ADDesc.iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage3Ad::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectSpin"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	switch (m_ADDesc.iADType)
	{
	case TYPE_UV:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_Prop_ADuv"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case TYPE_ACC:
		if (0 == m_ADDesc.iADIndex)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_Prop_ADc0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (1 == m_ADDesc.iADIndex)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_Prop_ADc1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_BD_acad"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	

	return S_OK;
}

void CStage3Ad::Update_AD(_float fTimeDelta)
{
	switch (m_ADDesc.iADType)
	{
	case TYPE_UV:
		Update_ADUV(fTimeDelta);
		break;
	case TYPE_ACC:
		Update_ADACC(fTimeDelta);
		break;
	default:
		break;
	}

}

void CStage3Ad::Update_ADUV(_float fTimeDelta)
{
	m_fMoveTime += 1.f * fTimeDelta;

	if (8.f < m_fMoveTime)
	{
		m_fMoveUVu += 0.1f * fTimeDelta;

		if (m_fLastUVu + 0.5f < m_fMoveUVu) 
		{
			m_fMoveUVu = m_fLastUVu + 0.5f;
			m_fMoveTime = 0.f;
			m_fLastUVu = m_fMoveUVu;
		}
	}
}

void CStage3Ad::Update_ADACC(_float fTimeDelta)
{
	m_fMoveTime += 1.f * fTimeDelta;

	if (5.f < m_fMoveTime)
	{
		if (false == m_bReverse)
		{
			m_fAlpha += 0.2f * fTimeDelta;

			if (1.f < m_fAlpha)
			{
				m_bReverse = true;
				m_fAlpha = 1.f;
				m_fMoveTime = 0.f;
			}
		}
		else if (true == m_bReverse)
		{
			m_fAlpha -= 0.1f * fTimeDelta;

			if (0.f > m_fAlpha)
			{
				m_bReverse = false;
				m_fAlpha = 0.f;
				m_fMoveTime = 0.f;
			}
		}

	}

}

CStage3Ad * CStage3Ad::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage3Ad*		pInstance = new CStage3Ad(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3Ad"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage3Ad::Clone(void * pArg)
{
	CStage3Ad*		pInstance = new CStage3Ad(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3Ad"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage3Ad::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
