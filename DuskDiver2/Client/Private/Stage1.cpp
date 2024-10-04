#include "stdafx.h"
#include "..\Public\Stage1.h"
#include "GameInstance.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Light.h"
#include "ToolManager.h"
#include "UI_Manager.h"

CStage1::CStage1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage1::CStage1(const CStage1 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage1::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Stage";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	CCamera_Main::CUTDESC CamDesc;
	CamDesc.vPos = _float3(-0.079f, 2.165f, 20.575f);
	CamDesc.vLook = _float3(-0.083f, 1.891f, 22.22f);
	CamDesc.bLerp = false;
	CamDesc.bLookLerp = false;
	CamDesc.fLerpSpeed = 0.f;
	CamDesc.fLookLerpSpeed = 0.f;
	CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
	CamDesc.pTran = nullptr;
	CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
	CamDesc.bPixLocal = false;
	CRM->Get_Cam()->Show_Target(CamDesc);

	m_fRatio = 0.f;


	XMStoreFloat4(&(GI->Get_DirLightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLightDesc()->vDiffuse) * m_fRatio, 1.f));
	XMStoreFloat4(&(GI->Get_DirLightDesc()->vSpecular), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLight()->Get_OriLightDesc()->vSpecular) * m_fRatio, 1.f));

	list<CLight*>* pLights = GI->Get_Lights();
	for (auto& pLight : *pLights)
		XMStoreFloat4(&(pLight->Get_LightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&pLight->Get_OriLightDesc()->vDiffuse) * m_fRatio, 1.f));


	return S_OK;
}

void CStage1::Tick(_float fTimeDelta)
{
	if (UM->Get_MoveOver())
	{
		TM->Change_Level(LEVEL_STAGE2);
		UM->Set_FrontUI(false);
		return;
	}
	//43
	if (LEVEL_TOOL != TM->Get_CurLevel())
	{
		if (!m_bEndLight)
		{
			m_fLightTimeAcc += fTimeDelta;

			if (10.f < m_fLightTimeAcc)
			{
				m_fRatio += fTimeDelta * 2.f;
				if (1.f < m_fRatio)
				{
					m_fRatio = 1.f;
					
				}


				XMStoreFloat4(&(GI->Get_DirLightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLightDesc()->vDiffuse) * m_fRatio, 1.f));
				XMStoreFloat4(&(GI->Get_DirLightDesc()->vSpecular), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLight()->Get_OriLightDesc()->vSpecular) * m_fRatio, 1.f));

				list<CLight*>* pLights = GI->Get_Lights();
				for (auto& pLight : *pLights)
					XMStoreFloat4(&(pLight->Get_LightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&pLight->Get_OriLightDesc()->vDiffuse) * m_fRatio, 1.f));
			}

			if (11.f < m_fLightTimeAcc)
			{
				UM->Set_FrontUI(true);
				m_bEndLight = true;
				//CRM->Get_Cam()->Set_CamPos(0.f, 20.f);
				CRM->Get_Cam()->End_Target();
			}
		}
	}
	else
	{
		
		if (0.1f > m_fRatio)
		{
			m_fRatio = 1.f;

			XMStoreFloat4(&(GI->Get_DirLightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLightDesc()->vDiffuse) * m_fRatio, 1.f));
			XMStoreFloat4(&(GI->Get_DirLightDesc()->vSpecular), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLight()->Get_OriLightDesc()->vSpecular) * m_fRatio, 1.f));

			list<CLight*>* pLights = GI->Get_Lights();
			for (auto& pLight : *pLights)
				XMStoreFloat4(&(pLight->Get_LightDesc()->vDiffuse), XMVectorSetW(XMLoadFloat4(&pLight->Get_OriLightDesc()->vDiffuse) * m_fRatio, 1.f));
		}

	}



	
}

void CStage1::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CStage1::Render()
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

	//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 0.7f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}



HRESULT CStage1::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CStage1 * CStage1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage1*		pInstance = new CStage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage1::Clone(void * pArg)
{
	CStage1*		pInstance = new CStage1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage1::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CStage1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	if (ImGui::Button("TextNorTex"))
	{
		if (m_DIndex)
			m_DIndex = 0;
		else
			m_DIndex = 2;
	}


	ImGui::End();

}