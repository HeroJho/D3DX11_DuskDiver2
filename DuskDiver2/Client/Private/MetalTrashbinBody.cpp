#include "stdafx.h"
#include "..\Public\MetalTrashbinBody.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "MetalTrashbinTop.h"
#include "Trash.h"
#include "DropItem.h"

CMetalTrashbinBody::CMetalTrashbinBody(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CMetalTrashbinBody::CMetalTrashbinBody(const CMetalTrashbinBody & rhs)
	: CCreture(rhs)
{
}

HRESULT CMetalTrashbinBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMetalTrashbinBody::Initialize(void * pArg)
{
	m_MetalTrashbinDesc = *(METALTRASHBINDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	return S_OK;
}

void CMetalTrashbinBody::Tick(_float fTimeDelta)
{
	Check_RenderState();

	if (true == m_bHit)
	{
		// 맞았을때 한번만 실행할 함수
		if (false == m_bCreateItem)
		{
			m_bCreateItem = true;
			Create_Item();
			AddForceToTop();
		}
		// 맞았을때 수시로 체크할 함수
		Check_DeadTIme();
	}

#pragma region 디버그
	//if (0 == m_MetalTrashbinDesc.iTrashbinIndex)
	//{
	//	if (CK_DOWN(DIK_Z))
	//	{
	//		Speed *= 10.f;
	//		if (10.f < Speed)
	//			Speed = 0.1f;
	//	}
	//	if (CK_PRESS(DIK_RSHIFT))
	//	{
	//		if (CK_PRESS(DIK_UP))
	//		{
	//			NewY += Speed;
	//		}
	//		if (CK_PRESS(DIK_DOWN))
	//		{
	//			NewY -= Speed;
	//		}
	//	}
	//	else
	//	{
	//		if (CK_PRESS(DIK_UP))
	//		{
	//			NewZ += Speed;
	//		}
	//		if (CK_PRESS(DIK_DOWN))
	//		{
	//			NewZ -= Speed;
	//		}
	//	}

	//	if (CK_PRESS(DIK_LEFT))
	//	{
	//		NewX -= Speed;
	//	}
	//	if (CK_PRESS(DIK_RIGHT))
	//	{
	//		NewX += Speed;
	//	}

	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(NewX, NewY, NewZ, 1.f));
	//}
#pragma endregion 디버그

}

void CMetalTrashbinBody::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	if (true == m_bRenderState)
	{
		Tick_Col(m_pTransformCom->Get_WorldMatrix());
		Choose_Col(nullptr);
		Render_Col(m_pRendererCom);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		m_pRigidCom->Update_Transform(m_pTransformCom);
		m_pRendererCom->Add_DebugGroup(m_pRigidCom);

		// 자식의 LateTIck을 돌린다
		m_pTrashbinTop->LateTick(fTimeDelta);
		if (nullptr != m_pTrash)
		{
			m_pTrash->LateTick(fTimeDelta);
		}
	}
		

}

HRESULT CMetalTrashbinBody::Render()
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
		
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}

	// 자식들을 렌더한다
	m_pTrashbinTop->Render();
	if (nullptr != m_pTrash)
	{
		m_pTrash->Render();
	}

	return S_OK;
}

void CMetalTrashbinBody::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}

void CMetalTrashbinBody::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}

void CMetalTrashbinBody::On_Hit(CCreture * pOther)
{
	m_bHit = true;
}

HRESULT CMetalTrashbinBody::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	InitState(); // 위치를 지정해준다. Rigid설정에 필요하기에 여기서 한다

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_MetalTrashbinA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	_float4 vPosition = { 0.f,0.f,0.f,1.f };
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vPosition.y += 0.f;
	RigidDesc.vStartPos = *(_float3*)(&vPosition);
	RigidDesc.vPos = _float3(0.f, -0.6f, 0.f);
	RigidDesc.vExtense = _float3(0.8f, 1.f, 0.8f);
	RigidDesc.vRotation = { 0.f, 0.f, 0.f };
	RigidDesc.fWeight = 100.f;
	RigidDesc.fStaticFriction = 1.f;
	RigidDesc.fDynamicFriction = 1.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;

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
	ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColDesc.vCenter = _float3(0.f, 0.6f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

void CMetalTrashbinBody::InitState()
{
	_float4 vInitPosition[10] = {
		{ 29.6f,0.f,115.4f,1.f }, //0
		{ 6.f,0.f,122.9f,1.f },
		{ -27.7f,0.f,112.5f,1.f },
		{ 5.5f,0.f,162.5f,1.f },
		{ -27.7f,0.f,196.2f,1.f },
		{ -37.84f,0.12f,208.57f,1.f }, //5 -> 식당내부
		{ 52.65f,0.f,192.02f,1.f }, // -> 아지트 구역 나무근처
		{ 67.1f,0.f,151.9f,1.f },
		{ -7.1f,0.f,203.f,1.f }, //->장터 뒷길
		{ -8.03f,0.f,89.11f,1.f } //9 -> 컨테이너안쪽
	};

	// 본인의 위치를 잡는다.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vInitPosition[m_MetalTrashbinDesc.iTrashbinIndex]));

	// 자식을 생성한다 
	CMetalTrashbinTop::METALTRASHBINTOPDESC TrashbinTopDesc;
	TrashbinTopDesc.vPosition = vInitPosition[m_MetalTrashbinDesc.iTrashbinIndex];
	TrashbinTopDesc.vPosition.y += 1.1f;
	m_pTrashbinTop = (CMetalTrashbinTop*)GI->Clone_GameObject(TEXT("MetalTrashbinTop"), &TrashbinTopDesc);

	if (5 != m_MetalTrashbinDesc.iTrashbinIndex)
	{
		CTrash::TRASHDESC TrashDesc;
		TrashDesc.vPosition = vInitPosition[m_MetalTrashbinDesc.iTrashbinIndex];
		m_pTrash = (CTrash*)GI->Clone_GameObject(TEXT("Trash"), &TrashDesc);
	}
}

void CMetalTrashbinBody::Check_RenderState()
{
	// 절두체 컬링
	m_bRenderState = GI->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.f);
}

void CMetalTrashbinBody::Create_Item()
{
	
	// 쓰레기가 있으면 아이템 생성을 안한다.
	if (nullptr != m_pTrash)
		return;
	
	if (5 == m_MetalTrashbinDesc.iTrashbinIndex)
	{
		_float4 vPosition = { 0.f,0.f,0.f,1.f };
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	
		CDropItem::DROPITEMDESC DropItemDesc;
		DropItemDesc.iItemType = CDropItem::TYPE_CLUE;
		DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
		DropItemDesc.vStartPosition = vPosition;

		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
	}
	
}

void CMetalTrashbinBody::AddForceToTop()
{
	// 플레이어에서 쓰레기통의 방향벡터를 구해서 힘을 준다
	_float4 vDir = { 0.f,0.f,0.f,0.f };
	XMStoreFloat4(&vDir, XMVector4Normalize(XMVector4Normalize(XMLoadFloat4(&PM->Get_PlayerPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)) + XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	// 힘을 10배로
	vDir.x *= 120.f;
	vDir.y *= 60.f;
	vDir.z *= 60.f;

	// 힘의 크기는 나중에 고려....
	m_pTrashbinTop->Add_Force(*(_float3*)(&vDir));
}

void CMetalTrashbinBody::Check_DeadTIme()
{
	if (false == m_bRenderState)
		m_bDead = true;
}

CMetalTrashbinBody * CMetalTrashbinBody::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMetalTrashbinBody*		pInstance = new CMetalTrashbinBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMetalTrashbinBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMetalTrashbinBody::Clone(void * pArg)
{
	CMetalTrashbinBody*		pInstance = new CMetalTrashbinBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMetalTrashbinBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMetalTrashbinBody::Free()
{
	__super::Free();

	Safe_Release(m_pTrashbinTop);
	Safe_Release(m_pTrash);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
}
