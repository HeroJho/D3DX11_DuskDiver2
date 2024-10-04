#include "stdafx.h"
#include "..\Public\NPC_T.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"

#include "AnimMap_Tool.h"

CNPC_T::CNPC_T(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CNPC_T::CNPC_T(const CNPC_T & rhs)
	: CGameObject(rhs)
{
}



void CNPC_T::Set_DelayAnim(_bool bAnimDelay, _float fTime, _float fDelayTime)
{
	m_bAnimDelay = bAnimDelay;
	m_fDelayTime = fTime;
	m_fAnimDelayTime = fDelayTime;
}

void CNPC_T::Tick_DelayAnim(_float fTimeDelta)
{
	if (m_bAnimDelay)
	{
		m_fAnimDelayTimeAcc += fTimeDelta;
		if (m_fDelayTime < m_fAnimDelayTimeAcc)
		{
			m_pModelCom->Play_Animation(m_fAnimDelayTime);
			m_fAnimDelayTimeAcc = 0.f;
		}
	}
	else
		m_pModelCom->Play_Animation(fTimeDelta);

}

HRESULT CNPC_T::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_T::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_Desc = *((NPCIDLEDESC*)pArg);


	if (FAILED(Ready_Components()))
		return E_FAIL;



	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_Desc.vPos), 1.f));
	m_pTransformCom->RotationThree(_float3(1.f, 0.f, 0.f), m_Desc.vRot.x, _float3(0.f, 1.f, 0.f), m_Desc.vRot.y, _float3(0.f, 0.f, 1.f), m_Desc.vRot.z);
	m_pTransformCom->Set_Scale(XMVectorSet(m_Desc.vScale.x, m_Desc.vScale.y, m_Desc.vScale.z, 1.f));

	m_pTransformCom->Set_Rotation_Debug(m_Desc.vRot);

	// m_pModelCom->Make_NonModelVtx(m_Desc.iAnimIndex);
	m_pModelCom->Set_AnimIndex(m_Desc.iAnimIndex);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);

	return S_OK;
}

HRESULT CNPC_T::Render_ShadowDepth()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_ShadowLightViewMatrix(GI->Get_Cur_LevelIndex()), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(0), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CNPC_T::Tick(_float fTimeDelta)
{
	//_vector vPlayerPos = XMLoadFloat4(&PM->Get_PlayerPos());
	//_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//_float fDis = XMVectorGetX(XMVector3Length(vPlayerPos - vPos));

	//if (5.f < fDis)
	//	m_bStopAnim = true;
	//else
	//	m_bStopAnim = false; 

	m_Desc.vRot = m_pTransformCom->Get_Rotation_Debug();
	XMStoreFloat3(&m_Desc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CNPC_T::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	if (!m_bStopAnim)
	{
		// 애니메이션 딜레이
		Tick_DelayAnim(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

}

HRESULT CNPC_T::Render()
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


	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_NoneAnimColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4))))
		return E_FAIL;





	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_uint		iPassIndex = 0;
	if (m_bStopAnim)
		iPassIndex = 9;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render_CanStop(m_pShaderCom, i, iPassIndex, m_bStopAnim)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC_T::Ready_Components()
{

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Desc.sModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	//m_pModelCom->Set_RootHierarchy(1);


	//CCollider::COLLIDERDESC ColDesc;
	//ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	//ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	//ColDesc.pOwner = this;
	//strcpy(ColDesc.sTag, "NPC_Idle"); // 충돌체tag
	//if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
	//	return E_FAIL;


	return S_OK;
}










CNPC_T * CNPC_T::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPC_T*		pInstance = new CNPC_T(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNPCIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPC_T::Clone(void * pArg)
{
	CNPC_T*		pInstance = new CNPC_T(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNPCIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_T::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}






void CNPC_T::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	IG->Debug_Transform(m_pTransformCom);

	if (ImGui::InputInt("AnimIndex", (_int*)&m_Desc.iAnimIndex))
	{
		m_pModelCom->Set_AnimIndex(m_Desc.iAnimIndex);
	}
	
	if (ImGui::Button("Save"))
		TM->Get_AnimMap_Tool()->Save_Data(TM->Get_AnimMap_Tool()->Get_SaveID());

	if (ImGui::Button("Delete"))
		TM->Get_AnimMap_Tool()->Delete_Obj(m_sImGuiTag);
	
	ImGui::Checkbox("StopAnim", &m_bStopAnim);


	ImGui::End();
}

void CNPC_T::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{



	}
}

