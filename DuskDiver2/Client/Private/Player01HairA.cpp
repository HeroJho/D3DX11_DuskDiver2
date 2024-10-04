#include "stdafx.h"
#include "..\Public\Player01HairA.h"
#include "GameInstance.h"
#include "Player_Manager.h"

#include "ToolManager.h"
#include "HierarchyNode.h"

CPlayer01HairA::CPlayer01HairA(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CParts(pDevice, pContext)
{
}

CPlayer01HairA::CPlayer01HairA(const CPlayer01HairA & rhs)
	: CParts(rhs)
{
}

HRESULT CPlayer01HairA::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer01HairA::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Player01_HairA";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-1.371f, 0.031f, 0.f, 1.f));
	m_pTransformCom->RotationThree(_float3{ 1.f,0.f,0.f }, 270.f, _float3{ 0.f,1.f,0.f }, 270.f, _float3{ 0.f,0.f,1.f }, 0.f);

	// 초기 뼈위치로 Col붙이기
	if (FAILED(Ready_JointHair()))
		return E_FAIL;



#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCodes = nullptr;
	size_t			iLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
		return E_FAIL;
#endif



	return S_OK;
}

void CPlayer01HairA::Tick(_float fTimeDelta)
{

}

void CPlayer01HairA::LateTick(_float fTimeDelta)
{
	_matrix mBoneWorld = m_pModelCom->Get_PivotMatrix() *
		m_pTransformCom->Get_WorldMatrix() *
		m_pParentTransformCom->Get_WorldMatrix();

	// 조인트에 내 월드포스를 적용해서 연산을 한다.
	_matrix mPos = XMLoadFloat4x4(&m_HeadInitComM) * mBoneWorld;
	_float4 vQuater; XMStoreFloat4(&vQuater, XMQuaternionRotationMatrix(mPos));
	m_pHeadRigid->setKinematicTarget(PxTransform(XMVectorGetX(mPos.r[3]), XMVectorGetY(mPos.r[3]), XMVectorGetZ(mPos.r[3]),
		PxQuat(vQuater.x, vQuater.y, vQuater.z, vQuater.w)));

	XMStoreFloat4x4(&m_BoneWorld, mBoneWorld);

}

HRESULT CPlayer01HairA::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	// 피직스 결과를(Pos 값만) 하라키 m_CombinedTran에 넣는다
	_uint iSize = (_uint)m_pBones.size();
	for (_uint i = 0; i < iSize; ++i)
	{
		PxVec3 PxPos = m_pRigid[i]->getGlobalPose().p;
		_vector vPos = XMVectorSet(PxPos.x, PxPos.y, PxPos.z, 1.f);
		_matrix mCombTran = m_pBones[i]->Get_CombinedTransformation();

		vPos = XMVector3TransformCoord(vPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_BoneWorld)));
		mCombTran.r[3] = vPos;
		m_pBones[i]->Set_CombinedTransformation(mCombTran);
	}


	// 모델 매쉬 그린다.
	Render_Vtx();


	//// 리지드에서 꺼내서 콜을 출력한다.
	//Render_HairCol();

	return S_OK;
}


// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CPlayer01HairA::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::DragFloat3("Te", (float*)&Test, 0.1f);


	ImGui::End();
}

void CPlayer01HairA::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CPlayer01HairA::Render_Vtx()
{
	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return;

	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return;

	_float fGama = 1.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return;

	if (!m_bPlayerLight)
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return;
		}
	}
	else
	{
		_float Alpha = 1.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &Alpha, sizeof(_float))))
			return;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vPlayerLightColor", &m_vPlayerLightColor, sizeof(_float3))))
			return;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 11)))
				return;
		}
	}
}

PxRigidDynamic * CPlayer01HairA::Find_Rigid(const char * pName, CHierarchyNode** Out_pHeadBone)
{

	_uint iSize = (_uint)m_pRigid.size();
	for (_uint i = 0; i < iSize; ++i)
		if (!strcmp(m_pBones[i]->Get_Name(), pName))
		{
			if (Out_pHeadBone)
				*Out_pHeadBone = m_pBones[i];
			return m_pRigid[i];
		}


	return nullptr;
}

void CPlayer01HairA::SetInitPos()
{
	Set_Wake();

	_uint iCount = 0;
	for (auto& pBone : m_pBones)
	{

		// 초기 포지션
		_matrix mInitPos = pBone->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
		_float4 vInitQuater; XMStoreFloat4(&vInitQuater, XMQuaternionRotationMatrix(mInitPos));
		_float3 vInitPPos(XMVectorGetX(mInitPos.r[3]), XMVectorGetY(mInitPos.r[3]), XMVectorGetZ(mInitPos.r[3]));
		PxTransform vInitPxPos(PxVec3(vInitPPos.x, vInitPPos.y, vInitPPos.z), PxQuat(vInitQuater.x, vInitQuater.y, vInitQuater.z, vInitQuater.w));

		m_pRigid[iCount]->setGlobalPose(vInitPxPos);
		m_pRigid[iCount]->clearForce();
		m_pRigid[iCount]->clearTorque();

		++iCount;
	}

	GI->Tick(1.f / 60.f);

	//Set_Sleep();
}

void CPlayer01HairA::StagMass()
{
	PxRigidBodyExt::updateMassAndInertia(*m_pRigid[0], 10.f);
	PxRigidBodyExt::updateMassAndInertia(*m_pRigid[1], 5.f);
	PxRigidBodyExt::updateMassAndInertia(*m_pRigid[2], 4.f);
	PxRigidBodyExt::updateMassAndInertia(*m_pRigid[3], 3.f);
	PxRigidBodyExt::updateMassAndInertia(*m_pRigid[4], 2.f);
	PxRigidBodyExt::updateMassAndInertia(*m_pRigid[5], 1.f);
	// PxRigidBodyExt::updateMassAndInertia(*m_pRigid[6], 0.1f);
}

void CPlayer01HairA::SkillMass()
{
	//PxRigidBodyExt::updateMassAndInertia(*m_pRigid[0], 10.f);
	//PxRigidBodyExt::updateMassAndInertia(*m_pRigid[1], 3.5f);
	//PxRigidBodyExt::updateMassAndInertia(*m_pRigid[2], 2.f);
	//PxRigidBodyExt::updateMassAndInertia(*m_pRigid[3], 0.5f);
	//PxRigidBodyExt::updateMassAndInertia(*m_pRigid[4], 0.1f);
	//PxRigidBodyExt::updateMassAndInertia(*m_pRigid[5], 0.005f);
}

void CPlayer01HairA::Set_Sleep()
{
	_uint iSize = (_uint)m_pRigid.size();
	for (_uint i = 0; i < iSize; ++i)
		m_pRigid[i]->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
}

void CPlayer01HairA::Set_Wake()
{
	_uint iSize = (_uint)m_pRigid.size();
	for (_uint i = 0; i < iSize; ++i)
		m_pRigid[i]->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
}

_float4x4 CPlayer01HairA::Get_ConbineMatrix()
{
	_float4x4 Worldmatrix;
	XMStoreFloat4x4(&Worldmatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
	return Worldmatrix;
}

HRESULT CPlayer01HairA::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Player01HairA", TEXT("Player01HairA"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	TM->Set_ModelTool(m_pModelCom);

	return S_OK;
}

HRESULT CPlayer01HairA::Ready_JointHair()
{
	// 초기 뼈 위치를 잡는다. (Combined)
	m_pModelCom->Play_Bone();
	const vector<CHierarchyNode*>* pBones = m_pModelCom->Get_HierarchyNodes();

	_uint iCount = 0;
	for (auto& pBone : *pBones)
	{
		if (//strcmp("HeadEnd_M", pBone->Get_Name())  &&
			//strcmp("HairB_02_M", pBone->Get_Name()) &&
			strcmp("HairB_03_M", pBone->Get_Name()) &&
			strcmp("HairB_04_M", pBone->Get_Name()) &&
			strcmp("HairB_05_M", pBone->Get_Name()) &&
			strcmp("HairB_06_M", pBone->Get_Name()) &&
			strcmp("HairB_07_M", pBone->Get_Name()) &&
			strcmp("HairB_08_M", pBone->Get_Name()))
			continue;


		_matrix mCom = pBone->Get_CombinedTransformation();

		_float4 vQuater; XMStoreFloat4(&vQuater, XMQuaternionRotationMatrix(mCom));
		_float3 vPPos(XMVectorGetX(mCom.r[3]), XMVectorGetY(mCom.r[3]), XMVectorGetZ(mCom.r[3]));
		PxTransform vPxPos(PxVec3(vPPos.x, vPPos.y, vPPos.z), PxQuat(vQuater.x, vQuater.y, vQuater.z, vQuater.w));

		++iCount;

		PxMaterial* pMater = GI->Create_Material(0.5f, 0.5f, 0.6f);
		PxRigidDynamic* pRigid;
		if (!strcmp("HairB_03_M", pBone->Get_Name()))
			pRigid = GI->Create_JointBox(vPxPos, _float3(0.03f, 0.03f, 0.03f), pMater, 9, true);
		else
			pRigid = GI->Create_JointBox(vPxPos, _float3(0.03f, 0.03f, 0.03f), pMater, 9);

		//pRigid->setMaxLinearVelocity(5.f);

		GI->Add_Actor(pRigid);
		m_pRigid.push_back(pRigid);
		m_pBones.push_back(pBone);

		pMater->release();

		BoundingOrientedBox* pOriginal_OBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.03f, 0.03f, 0.03f), _float4(0.f, 0.f, 0.f, 1.f));
		BoundingOrientedBox* pOBB = new BoundingOrientedBox(*pOriginal_OBB);

		m_pOriginal_OBBs.push_back(pOriginal_OBB);
		m_pOBBs.push_back(pOBB);

	}




	//PxRigidDynamic* pRigid2 = Find_Rigid("HairB_02_M", &m_pHeadBone);
	//m_pHeadRigid = pRigid2;
	//XMStoreFloat4x4(&m_HeadInitComM, m_pHeadBone->Get_CombinedTransformation());
	PxRigidDynamic* pRigid3 = Find_Rigid("HairB_03_M", &m_pHeadBone);
	m_pHeadRigid = pRigid3;
	XMStoreFloat4x4(&m_HeadInitComM, m_pHeadBone->Get_CombinedTransformation());
	PxRigidDynamic* pRigid4 = Find_Rigid("HairB_04_M");
	PxRigidDynamic* pRigid5 = Find_Rigid("HairB_05_M");
	PxRigidDynamic* pRigid6 = Find_Rigid("HairB_06_M");
	PxRigidDynamic* pRigid7 = Find_Rigid("HairB_07_M");
	PxRigidDynamic* pRigid8 = Find_Rigid("HairB_08_M");

	StagMass();
	// SkillMass();

	PxVec3 PxPivot(0.f, 0.f, 0.03f);
	// GI->Create_DampedD6(pRigid2, PxTransform(PxPivot), pRigid3, PxTransform(-PxPivot));
	GI->Create_DampedD6(pRigid3, PxTransform(PxPivot), pRigid4, PxTransform(-PxPivot));
	GI->Create_DampedD6(pRigid4, PxTransform(PxPivot), pRigid5, PxTransform(-PxPivot));
	GI->Create_DampedD6(pRigid5, PxTransform(PxPivot), pRigid6, PxTransform(-PxPivot));
	GI->Create_DampedD6(pRigid6, PxTransform(PxPivot), pRigid7, PxTransform(-PxPivot));
	GI->Create_DampedD6(pRigid7, PxTransform(PxPivot), pRigid8, PxTransform(-PxPivot));


	return S_OK;
}


CPlayer01HairA * CPlayer01HairA::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer01HairA*		pInstance = new CPlayer01HairA(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer01HairA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer01HairA::Clone(void * pArg)
{
	CPlayer01HairA*		pInstance = new CPlayer01HairA(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer01HairA"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer01HairA::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);


	for (auto& pObb : m_pOriginal_OBBs)
		Safe_Delete(pObb);
	for (auto& pObb : m_pOBBs)
		Safe_Delete(pObb);


#ifdef _DEBUG

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);

#endif // _DEBUG

}

void CPlayer01HairA::Render_HairCol()
{

	for (_uint i = 0; i < m_pRigid.size(); ++i)
	{
		// 행렬을 만든다.
		PxTransform PxTran = m_pRigid[i]->getGlobalPose();
		_float4x4 mWorldMtx;

		_vector vRight = (XMVectorSet(PxTran.q.getBasisVector0().x, PxTran.q.getBasisVector0().y, PxTran.q.getBasisVector0().z, 0.f));
		_vector vUp = (XMVectorSet(PxTran.q.getBasisVector1().x, PxTran.q.getBasisVector1().y, PxTran.q.getBasisVector1().z, 0.f));
		_vector vLook = (XMVectorSet(PxTran.q.getBasisVector2().x, PxTran.q.getBasisVector2().y, PxTran.q.getBasisVector2().z, 0.f));

		XMStoreFloat4((_float4*)&mWorldMtx._11, vRight);
		XMStoreFloat4((_float4*)&mWorldMtx._21, vUp);
		XMStoreFloat4((_float4*)&mWorldMtx._31, vLook);

		mWorldMtx._41 = PxTran.p.x;
		mWorldMtx._42 = PxTran.p.y;
		mWorldMtx._43 = PxTran.p.z;
		mWorldMtx._44 = 1.f;

		m_pOriginal_OBBs[i]->Transform(*m_pOBBs[i], XMLoadFloat4x4(&mWorldMtx));
	}




	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(GI->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(GI->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	m_pEffect->Apply(m_pContext);
	m_pContext->IASetInputLayout(m_pInputLayout);


	m_pBatch->Begin();
	for (_uint i = 0; i < m_pOBBs.size(); ++i)
		ClientDX::Draw(m_pBatch, *m_pOBBs[i], XMLoadFloat4(&_float4(1.f, 1.f, 0.f, 1.f)));
	m_pBatch->End();


}