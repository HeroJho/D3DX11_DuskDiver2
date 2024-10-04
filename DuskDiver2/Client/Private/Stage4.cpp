#include "stdafx.h"
#include "..\Public\Stage4.h"
#include "GameInstance.h"

#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Crack_Manager.h"

CStage4::CStage4(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage4::CStage4(const CStage4 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage4::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage4::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	UM->MoveBlind(false);

	return S_OK;
}

void CStage4::Tick(_float fTimeDelta)
{
	if (true == CRKM->Get_GotoStage5() && (true == UM->Get_MoveOver()))
	{
		TM->Change_Level(LEVEL_STAGE5);
		UM->Set_FrontUI(false);
		return;
	}

	if (CK_DOWN(DIK_F7))
	{
		//if (FAILED(GI->Add_GameObjectToLayer(TEXT("MiniGame_Block_Triger"), LEVEL_STAGE4, TEXT("fff"))))
		//	return;

		// TM->Start_MiniGame_Yabawe();

		//TM->Start_MiniGame_Funch();


		//TM->Open_Stage4Trigger(Test);
		//++Test;		
		// TM->Open_Stage4Trigger(3);

		//TM->Start_CutScene_DG();
		// TM->End_CutScene_DG();

		//if (FAILED(GI->Add_GameObjectToLayer(TEXT("BallingCuma"), LEVEL_STATIC, TEXT("Layer_Balling"))))
		////	return;

		//UM->Set_TalkIndex(69);
		//UM->Set_FrontUI(false);
		//UM->Set_QuestPointOff();

		// TM->Make_Potal();


		// if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Event5"), LEVEL_STAGE4, TEXT("Layer_Test"))))
		// 	return;
		// MM->Spawn_MechanicalOctopus();


	}
	if (CK_DOWN(DIK_F8))
	{
		// MM->Spawn_Stage4_2();

		// TM->Get_MiniGameBlock()->Set_Reset();

		// TM->Get_MiniGameYabawe()->Set_Reset();

		//TM->End_MiniGame_Funch();

		//TM->End_CutScene_DG();
	}



}

void CStage4::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CStage4::Render()
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

	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 0.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);


	//if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpa, sizeof(_float))))
	//	return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;

		//if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
		//	iPassIndex = 20;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage4::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}


CStage4 * CStage4::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage4*		pInstance = new CStage4(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage4"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage4::Clone(void * pArg)
{
	CStage4*		pInstance = new CStage4(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage4"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage4::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
