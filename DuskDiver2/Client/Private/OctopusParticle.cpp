#include "stdafx.h"
#include "..\Public\OctopusParticle.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Monster_Manager.h"
#include "Particle_Manager.h"

COctopusParticle::COctopusParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

COctopusParticle::COctopusParticle(const COctopusParticle & rhs)
	:CGameObject(rhs)
{
}

HRESULT COctopusParticle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COctopusParticle::Initialize(void * pArg)
{
	m_PartDesc = *(PARTDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_prop";

	Make_ImGui();

	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_PartDesc.fAngle));


	// m_pRigidCom->Set_Sleep(true);

	m_pRigidCom->Set_Sleep(false);
	m_bAddForce = true;

	return S_OK;
}

void COctopusParticle::Tick(_float fTimeDelta)
{
	//m_fFallAcc += 0.1f * fTimeDelta;

	//if (m_fFallAcc >= 0.7f) //죽는 타이밍
	//{
	//	Set_Dead();
	//}

	if (m_bAddForce)
	{
		//날아갈 방향
		_float3 vDir = { 0.f,1.f,0.f };
		XMStoreFloat3(&vDir, -1.f * m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (!m_bOneForce)
		{
			m_bOneForce = true;
			if (1 == m_PartDesc.iModelType)
			{
				vDir.y *= 200.f;
				Add_Force(vDir);
			}
			else
			{
				//m_PartDesc.iModelType   1 - 곰 2,3,4,5 - 문어다리 - 모델타입 구분해서 파워랑 날라가는 방향 다르게 잡기
				Add_Force(_float3(100.f * vDir.x, 100.f * vDir.y, 100.f * vDir.z));
				//회전
				Add_Torque();
			}

		}

		

	}
	
	if (1 == m_PartDesc.iModelType)
		Tick_Cuma(fTimeDelta);


	//디버깅용
	//if (CK_DOWN(DIK_Z))
	//{
	//	m_pRigidCom->Set_Sleep(false);
	//	m_bAddForce = true;
	//}
}

void COctopusParticle::LateTick(_float fTimeDelta)
{
	
	if (nullptr == m_pRendererCom)
		return;

	m_pRigidCom->Update_Transform(m_pTransformCom);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
	
}

HRESULT COctopusParticle::Render()
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

	_float fGama = 1.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
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

void COctopusParticle::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::Checkbox("RenderBody", &m_bRender);


	ImGui::End();
}

void COctopusParticle::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}

void COctopusParticle::Add_Force(_float3 vForce)
{
	m_pRigidCom->Add_Force(vForce);
}

void COctopusParticle::Add_Torque()
{
	// 회전을 준다. Right기준
	_float4 vTorqueAxis = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&vTorqueAxis, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	vTorqueAxis.x *= 10.f;
	vTorqueAxis.z *= 10.f;
	m_pRigidCom->Add_Torque(*(_float3*)(&vTorqueAxis));
}

void COctopusParticle::Tick_Cuma(_float fTimeDelta)
{
	_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (4.f > fY)
	{
		// 길 열기
		MM->Arcade_Event2();
		GI->PlaySoundW(L"song043_ojama_okuri_shou.wav", SD_NPC, 0.7f);
		Set_Dead();
	}


	m_fTimeAcc += fTimeDelta;
	if (0.2f < m_fTimeAcc)
	{
		_matrix mM = XMMatrixIdentity();
		mM.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 vM; XMStoreFloat4x4(&vM, mM);
		PTM->Create_InstanceParticle(L"BearSmoke", vM);

		m_fTimeAcc = 0.f;
	}


}

HRESULT COctopusParticle::Ready_Components()
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


	_float RandomZ = GI->Get_FloatRandom(0.f, 300.f);


	//기계로봇 포지션 또는 특정 포지션 넣어줘야한다
	XMStoreFloat3(&RigidDesc.vStartPos, XMLoadFloat3(&m_PartDesc.vStartPos) + XMLoadFloat4(&m_PartDesc.vLocalPosition));
	RigidDesc.vPos = m_PartDesc.vPos; //콜라이더랑 모델의 위치가 달라서 맞춰주는것 툴로
	RigidDesc.vExtense = m_PartDesc.vExtense; //콜라이더 크기

	if(m_PartDesc.iModelType != 1) //곰돌이 아닌것만 랜덤으로 회전
		RigidDesc.vRotation = { 0.f, 0.f, RandomZ };

	//나머지는 피직스박스보면서 정하기
	RigidDesc.fWeight = 1.f;
	RigidDesc.fStaticFriction = 1.f;
	RigidDesc.fDynamicFriction = 1.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;

	m_pRigidCom->Get_PxBody()->setMaxLinearVelocity(20.f); // 피직스 객체의 최대 속도를 제한해준다

	return S_OK;
}

HRESULT COctopusParticle::Ready_ModelCom()
{

	if (m_PartDesc.iModelType == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SadBear"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("OctopusLeg"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	return S_OK;
}

COctopusParticle * COctopusParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	COctopusParticle*		pInstance = new COctopusParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : COctopusParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COctopusParticle::Clone(void * pArg)
{
	COctopusParticle*		pInstance = new COctopusParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COctopusParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctopusParticle::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);

}
