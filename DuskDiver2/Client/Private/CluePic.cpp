#include "stdafx.h"
#include "..\Public\CluePic.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Time_Manager.h"
#include "UI_Manager.h"

CCluePic::CCluePic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CCluePic::CCluePic(const CCluePic & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCluePic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCluePic::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Stage_Prop";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.2f, 0.95f, 158.5f, 1.f));

	return S_OK;
}

void CCluePic::Tick(_float fTimeDelta)
{
	Clue_Move(fTimeDelta);
}

void CCluePic::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (true == m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCluePic::Render()
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


	_float fGama = 2.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CCluePic::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	list<CCollider*>::iterator iter = m_Colliders.begin();
	IG->Debug_Col(*iter);


	ImGui::End();

}


HRESULT CCluePic::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_CluePic"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

void CCluePic::Clue_Move(_float fTimeDelta)
{
	if (true == m_bFound)
	{
		_float4 vPicPos = { 0.f,0.f,0.f,1.f };
		XMStoreFloat4(&vPicPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (20.35f >= vPicPos.x)
		{
			vPicPos.x += 0.0015f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPicPos));
		}
		else
		{
			if (false == m_bShowUI)
			{
				m_bShowUI = true;
				m_bFind = true;
			}
		}


		if (m_bFind && m_bRender)
		{
			m_fTimeAcc += fTimeDelta;
			if(2.f < m_fTimeAcc)
			{
				m_bRender = false;

				UM->Add_ClueCnt();
				UM->Render_Clue2();
				UM->Plus_Coin();
			}

		}

	}
}

CCluePic * CCluePic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCluePic*		pInstance = new CCluePic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCluePic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCluePic::Clone(void * pArg)
{
	CCluePic*		pInstance = new CCluePic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCluePic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCluePic::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}