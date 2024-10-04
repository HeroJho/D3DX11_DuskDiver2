#include "stdafx.h"
#include "..\Public\Dogbox.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "DropItem.h"

CDogbox::CDogbox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCreture(pDevice, pContext)
{
}

CDogbox::CDogbox(const CDogbox & rhs)
	: CCreture(rhs)
{
}

HRESULT CDogbox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDogbox::Initialize(void * pArg)
{
	m_DogboxDesc = *(DOGBOXDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	Set_InitState();

	return S_OK;
}

void CDogbox::Tick(_float fTimeDelta)
{
	if (true == m_bHited)
	{
		if (false == GI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		{
			m_bDelete = true;
		}

		if (false == m_bAddForce)
		{
			m_bAddForce = true;
			_float4 vDir = { 0.f,0.f,0.f,0.f };
			XMStoreFloat4(&vDir, XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&PM->Get_PlayerPos()) + XMVectorSet(0.f,1.f,0.f,0.f)));
			Add_Force(_float3(4850.f * vDir.x, 5450.f * vDir.y, 4350.f * vDir.z));
			Create_Item();
		}
		
	}
}

void CDogbox::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRigidCom->Update_Transform(m_pTransformCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this); 

	m_pRendererCom->Add_DebugGroup(m_pRigidCom);


	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CDogbox::Render()
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

void CDogbox::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}

void CDogbox::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}

void CDogbox::On_Hit(CCreture * pOther)
{
	m_bHited = true;
}

void CDogbox::Add_Force(_float3 vForce)
{
	m_pRigidCom->Add_Force(vForce);

	// 회전을 준다. Right기준
	_float4 vTorqueAxis = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&vTorqueAxis, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	vTorqueAxis.x *= 10.f;
	vTorqueAxis.z *= 10.f;
	m_pRigidCom->Add_Torque(*(_float3*)(&vTorqueAxis));
}

HRESULT CDogbox::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BoxAa_DM"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	RigidDesc.vStartPos = *(_float3*)(&m_DogboxDesc.vPosition);
	RigidDesc.vPos = _float3(0.f, -0.15f, 0.f);
	RigidDesc.vExtense = _float3(0.75f, 0.6f, 0.75f);
	RigidDesc.vRotation = { 0.f, 0.f, 0.f };
	RigidDesc.fWeight = 1.f;
	RigidDesc.fStaticFriction = 1.f;
	RigidDesc.fDynamicFriction = 1.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;

	m_pRigidCom->Get_PxBody()->setMaxLinearVelocity(5.f); // 피직스 객체의 최대 속도를 제한해준다

	// 충돌체
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.4f, 0.4f, 0.4f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.1f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

void CDogbox::Set_InitState()
{
	m_pTransformCom->Set_Scale(XMVectorSet(2.2f, 2.2f, 2.2f, 1.f));
	m_DogboxDesc.vPosition.z += 0.3f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_DogboxDesc.vPosition));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
}

void CDogbox::Create_Item()
{
	// 단서를 제공한다
	CDropItem::DROPITEMDESC DropItemDesc;
	DropItemDesc.iItemType = CDropItem::TYPE_CLUE;
	DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
	DropItemDesc.vStartPosition = m_DogboxDesc.vPosition;
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
}

CDogbox * CDogbox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDogbox*		pInstance = new CDogbox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDogbox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDogbox::Clone(void * pArg)
{
	CDogbox*		pInstance = new CDogbox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDogbox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDogbox::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
}
