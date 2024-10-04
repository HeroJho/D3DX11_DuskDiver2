#include "stdafx.h"
#include "..\Public\DumiBlock.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "MiniGame_Block.h"

CDumiBlock::CDumiBlock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CDumiBlock::CDumiBlock(const CDumiBlock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDumiBlock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDumiBlock::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_Desc = *(DUMIBLOCK*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();
	m_sTag = "Tag_Cube";


	//_vector vPos = XMVectorSetW(XMLoadFloat3(&m_Desc.vPos), 1.f);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scale(XMVectorSetW(XMLoadFloat3(&m_Desc.vScale), 1.f));


	return S_OK;
}




void CDumiBlock::Tick(_float fTimeDelta)
{
	Emp_Col();


}

void CDumiBlock::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRigidCom->Update_Transform(m_pTransformCom);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
}

HRESULT CDumiBlock::Render()
{
	if (nullptr == m_pVIBuffer_Cube ||
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_Desc.vColor, sizeof(_float4))))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBuffer_Cube->Render();

	return S_OK;
}

HRESULT CDumiBlock::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_Model"), (CComponent**)&m_pVIBuffer_Cube)))
		return E_FAIL;

	
	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	RigidDesc.vStartPos = m_Desc.vPos;
	RigidDesc.vPos = _float3(0.f, 0.f, 0.f);
	RigidDesc.vExtense = m_Desc.vScale;
	RigidDesc.fWeight = 100.f;
	RigidDesc.fStaticFriction = 1.f;
	RigidDesc.fDynamicFriction = 1.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;


	return S_OK;
}

CDumiBlock * CDumiBlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDumiBlock*		pInstance = new CDumiBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDumiBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDumiBlock::Clone(void * pArg)
{
	CDumiBlock*		pInstance = new CDumiBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDumiBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDumiBlock::Free()
{
	__super::Free();

	Safe_Release(m_pVIBuffer_Cube);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRigidCom);
}





void CDumiBlock::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}