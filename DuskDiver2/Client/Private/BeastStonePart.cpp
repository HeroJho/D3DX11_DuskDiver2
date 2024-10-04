#include "stdafx.h"
#include "..\Public\BeastStonePart.h"
#include "GameInstance.h"

CBeastStonePart::CBeastStonePart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CBeastStonePart::CBeastStonePart(const CBeastStonePart & rhs)
	:CGameObject(rhs)
{
}

HRESULT CBeastStonePart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastStonePart::Initialize(void * pArg)
{
	m_PartDesc = *(PARTDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_prop";

	Make_ImGui();

	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_PartDesc.fAngle));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	m_pRigidCom->Set_Sleep(false);


	return S_OK;
}

void CBeastStonePart::Tick(_float fTimeDelta)
{
	m_fFallAcc += 0.1f * fTimeDelta;

	if (m_fFallAcc >= 0.5f)
	{
		m_bDissolve = true;
	}

	if (m_bDissolve)
	{
		m_iPathIndex = 16;
		m_fDissolve += 0.5f * fTimeDelta;
		if (m_fDissolve >= 1.f)
		{
			m_fDissolve = 1.f;
			Set_Dead();
		}

	}

}

void CBeastStonePart::LateTick(_float fTimeDelta)
{
	
	if (nullptr == m_pRendererCom)
		return;

	m_pRigidCom->Update_Transform(m_pTransformCom);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
	
}

HRESULT CBeastStonePart::Render()
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

	_float fGama = 1.4f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;


		if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iPathIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastStonePart::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::Checkbox("RenderBody", &m_bRender);


	ImGui::End();
}

void CBeastStonePart::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}

void CBeastStonePart::Phx_Awake()
{
	m_pRigidCom->Set_Sleep(false);

}


HRESULT CBeastStonePart::Ready_Components()
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
	if (FAILED(Ready_ModelCom()))
		return E_FAIL;

	//피직스 추가 //인자로 다 따르게 받아와야함
	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	/*XMStoreFloat3(&RigidDesc.vStartPos, XMLoadFloat3(&m_PartDesc.vStartPos) + XMLoadFloat4(&m_PartDesc.vLocalPosition));
	RigidDesc.vRotation = m_PartDesc.vRotation;
	RigidDesc.vPos = m_PartDesc.vPos;
	RigidDesc.vExtense = m_PartDesc.vExtense;*/

	XMStoreFloat3(&RigidDesc.vStartPos, XMLoadFloat3(&m_PartDesc.vStartPos) + XMLoadFloat4(&m_PartDesc.vLocalPosition)); //시작 돌기둥안에서 얘네의 위치, 돌기둥 위치 더해줘야함
	RigidDesc.vPos = m_PartDesc.vPos; //콜라이더랑 모델의 위치가 달라서 맞춰주는것 툴로
	RigidDesc.vExtense = m_PartDesc.vExtense; //콜라이더 크기

	//나머지는 피직스박스보면서 정하기
	RigidDesc.fWeight = 140.f;
	RigidDesc.fStaticFriction = 2.f;
	RigidDesc.fDynamicFriction = 2.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;

	m_pRigidCom->Get_PxBody()->setMaxLinearVelocity(6.f); // 피직스 객체의 최대 속도를 제한해준다

	return S_OK;
}

HRESULT CBeastStonePart::Ready_ModelCom()
{

	switch (m_PartDesc.iStoneType) //모델컴이랑 제한 y축 같이 정해주기
	{
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart5"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 6:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart6"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 7:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart7"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 8:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStonePart8"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}

	return S_OK;
}

CBeastStonePart * CBeastStonePart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastStonePart*		pInstance = new CBeastStonePart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastStonePart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBeastStonePart::Clone(void * pArg)
{
	CBeastStonePart*		pInstance = new CBeastStonePart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastStonePart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastStonePart::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
	Safe_Release(m_pDissolveTexture);


}
