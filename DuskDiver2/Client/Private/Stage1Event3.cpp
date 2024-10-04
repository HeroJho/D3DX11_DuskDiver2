#include "stdafx.h"
#include "..\Public\Stage1Event3.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Holo_1.h"
#include "LightObj.h"

CStage1Event3::CStage1Event3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage1Event3::CStage1Event3(const CStage1Event3 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage1Event3::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage1Event3::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.029f, 1.f, 9.354f, 1.f));

	return S_OK;
}

void CStage1Event3::Tick(_float fTimeDelta)
{

}

void CStage1Event3::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CStage1Event3::Render()
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


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

void CStage1Event3::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Potal")->Set_Size(vSize);
	Get_Colliders("Potal")->Set_Center(vCenter);
	ImGui::End();
}

void CStage1Event3::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			Start_Holo();
			Set_Dead();
		}

	}
}

HRESULT CStage1Event3::Start_Holo()
{
	GI->PlaySoundW(L"Holo_On.wav", SD_NPC, 0.5f);

	CHolo_1::HOLODESC HoloDesc;
	HoloDesc.vPos = _float3(0.f, 1.f, 10.f);
	HoloDesc.vRot = _float3(90.f, 0.f, 0.f);
	HoloDesc.vScale = _float3(100.f, 1.f, 100.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Holo_1"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;
	HoloDesc.vPos = _float3(0.f, 1.f, -22.6f);
	HoloDesc.vRot = _float3(90.f, 0.f, 0.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Holo_1"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;
	HoloDesc.vPos = _float3(-14.4f, 1.f, -19.f);
	HoloDesc.vRot = _float3(90.f, 90.f, 0.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Holo_1"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;
	HoloDesc.vPos = _float3(14.7f, 1.f, -19.f);
	HoloDesc.vRot = _float3(90.f, 90.f, 0.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Holo_1"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;

	HoloDesc.vPos = _float3(12.5f, 4.f, -12.3f);
	HoloDesc.vRot = _float3(90.f, 115, 0.f);
	HoloDesc.vScale = _float3(10.f, 1.f, 5.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("RealHolo_1"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;
	HoloDesc.vPos = _float3(13.4f, 3.4f, 0.1f);
	HoloDesc.vRot = _float3(90.f, 90.f, 0.f);
	HoloDesc.vScale = _float3(11.f, 1.f, 4.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("RealHolo_2"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;
	HoloDesc.vPos = _float3(-13.5f, 5.8f, -4.8f);
	HoloDesc.vRot = _float3(90.f, -90, -22.f);
	HoloDesc.vScale = _float3(12.f, 3.f, 5.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("RealHolo_3"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;

	HoloDesc.vPos = _float3(0.f, 4.5f, -22.2f);
	HoloDesc.vRot = _float3(90.f, -180.f, 0.f);
	HoloDesc.vScale = _float3(15.f, 1.f, 8.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("RealHolo_5"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;

	HoloDesc.vPos = _float3(-13.7f, 2.7f, -12.8f);
	HoloDesc.vRot = _float3(90.f, 90.f, 0.f);
	HoloDesc.vScale = _float3(5.f, 3.f, 2.5f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Holo_8"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;


	HoloDesc.vPos = _float3(13.7f, 2.7f, -5.1f);
	HoloDesc.vRot = _float3(90.f, 90, 0.f);
	HoloDesc.vScale = _float3(2.f, 3.f, 2.5f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Holo_9"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;
	HoloDesc.vPos = _float3(-13.7f, 2.7f, -15.4f);
	HoloDesc.vRot = _float3(90.f, 90, 0.f);
	HoloDesc.vScale = _float3(2.f, 3.f, 2.5f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Holo_9"), LEVEL_TEST, TEXT("BackGround"), &HoloDesc)))
		return E_FAIL;


	CLightObj::LIGHTOBJDESC LightObjDesc;
	LightObjDesc.vPos = _float4(-9.08f, 4.8f, -18.6f, 1.f);
	LightObjDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightObjDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightObjDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 0.3f);
	LightObjDesc.fRange = 10.f;
	LightObjDesc.fEndTime = 8.f;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(9.92f, 4.8f, -18.6f, 1.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(9.92f, 4.8f, 1.67f, 1.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(-9.25f, 4.8f, 1.67f, 1.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(0.083f, 4.8f, -19.8f, 1.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(0.083f, 4.8f, 2.5f, 1.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(10.75f, 4.8f, -9.83f, 1.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(-7.92f, 4.8f, -9.83f, 1.f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	LightObjDesc.vPos = _float4(0.f, 4.8f, -9.83f, 1.f);
	LightObjDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightObjDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 1.f);
	LightObjDesc.fRange = 20.f;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LightObj"), LEVEL_TEST, TEXT("LightObj"), &LightObjDesc)))
		return E_FAIL;

	return S_OK;

}



HRESULT CStage1Event3::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player01Face"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CStage1Event3 * CStage1Event3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage1Event3*		pInstance = new CStage1Event3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage1Event3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage1Event3::Clone(void * pArg)
{
	CStage1Event3*		pInstance = new CStage1Event3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage1Event3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage1Event3::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
