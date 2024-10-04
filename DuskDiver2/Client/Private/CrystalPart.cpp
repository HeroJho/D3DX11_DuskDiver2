#include "stdafx.h"
#include "..\Public\CrystalPart.h"
#include "GameInstance.h"

CCrystalPart::CCrystalPart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CCrystalPart::CCrystalPart(const CCrystalPart & rhs)
	:CGameObject(rhs)
{
}

HRESULT CCrystalPart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrystalPart::Initialize(void * pArg)
{
	m_PartDesc = *(PARTDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_prop";

	Make_ImGui();

	Set_InitState();

	m_pRigidCom->Set_Sleep(true);

	return S_OK;
}

void CCrystalPart::Tick(_float fTimeDelta)
{
	
}

void CCrystalPart::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRigidCom->Update_Transform(m_pTransformCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this); //RENDER_NONLIGHT 

	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
}

HRESULT CCrystalPart::Render()
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

	_float fBlurPower = 0.4f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
		return E_FAIL;
	m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float));

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint		iPassIndex = 19;
	if (0.f < m_fDissolveAcc)
		iPassIndex = 23;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CCrystalPart::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}

void CCrystalPart::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}

void CCrystalPart::Phx_Awake()
{
	m_pRigidCom->Set_Sleep(false);
}

void CCrystalPart::Add_Force(_float3 vForce)
{
	m_pRigidCom->Add_Force(vForce);
}

#pragma region Debug & ImGui
void CCrystalPart::Change_Position(_float4 vPos)
{
	m_PartDesc.vLocalPosition = vPos;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
}

void CCrystalPart::Change_Rotation(_float fAngle)
{
	m_PartDesc.fAngle = fAngle;
	m_pTransformCom->Rotation(XMVectorSet(0.f,1.f, 0.f, 0.f), fAngle);
}

void CCrystalPart::Change_Size(_float fSize)
{
	m_PartDesc.fSize = fSize;
	m_pTransformCom->Set_Scale(XMVectorSet(fSize, fSize, fSize, 1.f));
}

_float4 CCrystalPart::Get_Position()
{
	return m_PartDesc.vLocalPosition;
}

_float CCrystalPart::Get_RotationAngle()
{
	return m_PartDesc.fAngle;
}

_float CCrystalPart::Get_Size()
{
	return m_PartDesc.fSize;
}
#pragma endregion Debug & ImGui

HRESULT CCrystalPart::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	_float3 vStartPos3 = *(_float3*)(&m_PartDesc.vLocalPosition);
	_float RandomY = GI->Get_FloatRandom(0.f, 20.f);
	switch (m_PartDesc.iCrystalType)
	{
	case TYPE_A:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_CrystalA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		
		//RigidDesc.vStartPos = _float3(0.f, 5.f, 0.f);
		vStartPos3.y += 0.32f;
		RigidDesc.vStartPos = vStartPos3;
		RigidDesc.vPos = _float3(0.f, -0.32f, 0.f);
		RigidDesc.vExtense = _float3(0.65f, 0.65f, 0.65f);
		RigidDesc.vRotation = { 0.f, RandomY, 0.f };
		RigidDesc.fWeight = 100.f;
		RigidDesc.fStaticFriction = 1.f;
		RigidDesc.fDynamicFriction = 1.f;
		RigidDesc.fRestitution = 0.1f;
		RigidDesc.fAngleDump = 1.f;

		// Prototype_Component_Rigid_Sphere
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
			return E_FAIL;
		break;
	case TYPE_B:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_CrystalB"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		//RigidDesc.vStartPos = _float3(0.f, 5.f, 0.f);
		vStartPos3.y += 0.2f;
		RigidDesc.vStartPos = vStartPos3;
		RigidDesc.vPos = _float3(0.f, -0.18f, 0.f);
		RigidDesc.vExtense = _float3(0.37f, 0.37f, 0.37f);
		RigidDesc.vRotation = { 0.f, RandomY, 0.f };
		RigidDesc.fWeight = 100.f;
		RigidDesc.fStaticFriction = 1.f;
		RigidDesc.fDynamicFriction = 1.f;
		RigidDesc.fRestitution = 0.1f;
		RigidDesc.fAngleDump = 1.f;

		// Prototype_Component_Rigid_Sphere
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
			return E_FAIL;
		break;
	default:
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	m_pRigidCom->Get_PxBody()->setMaxLinearVelocity(5.f); // 피직스 객체의 최대 속도를 제한해준다

	return S_OK;
}

void CCrystalPart::Set_InitState()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_PartDesc.vLocalPosition));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_PartDesc.fAngle);
	m_pTransformCom->Set_Scale(XMVectorSet(m_PartDesc.fSize, m_PartDesc.fSize, m_PartDesc.fSize, 1.f));
}

CCrystalPart * CCrystalPart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrystalPart*		pInstance = new CCrystalPart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCrystalPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrystalPart::Clone(void * pArg)
{
	CCrystalPart*		pInstance = new CCrystalPart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCrystalPart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrystalPart::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
}
