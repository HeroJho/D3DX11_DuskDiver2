#include "stdafx.h"
#include "..\Public\Tower.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"

#include "ToolManager.h"

CTower::CTower(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CTower::CTower(const CTower & rhs)
	: CCreture(rhs)
{
}

HRESULT CTower::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTower::Initialize(void * pArg)
{
	m_CretureInfo = *(CCreture::CREATEINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CretureInfo.vPosition));

	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	if (m_CretureInfo.iIndex == 0)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.1f, 0.f, 0.f), XMConvertToRadians(4.f));
	else if (m_CretureInfo.iIndex == 1)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.1f, 0.f, 0.f), XMConvertToRadians(-34.f));
	else if (m_CretureInfo.iIndex == 2)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.1f, 0.f, 0.f), XMConvertToRadians(-59.f));
	else if (m_CretureInfo.iIndex == 3)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.1f, 0.f, 0.f), XMConvertToRadians(60.f));
	return S_OK;
}

void CTower::Tick(_float fTimeDelta)
{

}

void CTower::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

}

HRESULT CTower::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;
	_bool	bDiffuse = false;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
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

void CTower::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}

HRESULT CTower::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_TowerA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.6f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}


CTower * CTower::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTower*		pInstance = new CTower(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CTower::Clone(void * pArg)
{
	CTower*		pInstance = new CTower(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTower::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CTower::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	_float3 vSize = Get_Colliders("Monster_Push_Range")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Push_Range")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Push_Range")->Set_Size(vSize);
	Get_Colliders("Monster_Push_Range")->Set_Center(vCenter);



	ImGui::End();
}
