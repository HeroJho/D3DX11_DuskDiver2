#include "stdafx.h"
#include "..\Public\AnimMesh.h"
#include "Model.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "DataManager.h"
#include "ToolManager.h"

CAnimMesh::CAnimMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAnimMesh::CAnimMesh(const CAnimMesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnimMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimMesh::Initialize(void * pArg)
{
	m_MeshInfo = ((MESHINFO*)pArg);
	sTag = m_MeshInfo->sTag;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, sTag.data(), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bDead = false;
	m_MeshInfo->fPos.w = 1.f;
	Set_Pos(m_MeshInfo->fPos);
	Set_Scale(m_MeshInfo->fScale);
	m_fAngles = m_MeshInfo->fAngle;
	if(m_pModelCom->Get_NumAnimations() > 0)
	m_pModelCom->Set_AnimIndex(0);
	m_pModelCom->Play_Animation(1.f);
	IG->AddAnimMesh(this);
	m_pTransformCom->Set_Scale(_vector{ 1.f,1.f,1.f });
	m_pModelCom->Set_RootHierarchy(0);
	return S_OK;
}

void CAnimMesh::Tick(_float fTimeDelta)
{
	Emp_Col();
}

void CAnimMesh::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_bSelected)
	{
		if (m_bPlay)
		{
			if (TK_PRESS(DIK_RIGHT))
			{
				m_pModelCom->Play_Animation(fTimeDelta * m_fPlaySpeed);
			}

			if (TK_PRESS(DIK_LEFT))
			{
				m_pModelCom->Play_AniReverse(fTimeDelta * m_fPlaySpeed);
			}
		}
		else
		{
			if (TK_PRESS(DIK_RIGHT))
			{
				m_pModelCom->Play_Animation_NoMove(fTimeDelta * m_fPlaySpeed);
			}

			if (TK_PRESS(DIK_LEFT))
			{
				m_pModelCom->Play_AniReverse_NoMove(fTimeDelta * m_fPlaySpeed);
			}
		}
	}


	if (m_iPreAniIndex != m_iAniIndex)
	{
		m_pModelCom->SetNextIndex(m_iAniIndex);
		m_pModelCom->SetChangeBool(true);
	}

	m_iPreAniIndex = m_iAniIndex;
	if (m_bPlay)
	{
		if(!m_bLocalPos)
			m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
		else
			m_pModelCom->Play_Animation_NoMove(fTimeDelta);
	}

	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

	m_bSelected = false;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CAnimMesh::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;
	//월드 뷰 역행렬 역전치
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;
	_float fGama = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;*/
	}

	return S_OK;
}

void CAnimMesh::Rotation(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3)
{
	m_fAngles.x = fRadian;
	m_fAngles.y = fRadian2;
	m_fAngles.z = fRadian3;
	m_pTransformCom->RotationThree(vAxis, fRadian, vAxis2, fRadian2, vAxis3, fRadian3);
}

_float3 CAnimMesh::Get_Pos()
{
	_float3 Pos;
	XMStoreFloat3(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return Pos;
}

void CAnimMesh::Set_Pos(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
}

int CAnimMesh::Get_NumAnimations()
{
	return m_pModelCom->Get_NumAnimations(); 
}

const char * CAnimMesh::Get_AnimName(int AniIndex)
{
	return m_pModelCom->Get_Name(AniIndex); 
}

void CAnimMesh::Set_AnimName(const char * Name, int AniIndex)
{
	m_pModelCom->Set_Name(Name, AniIndex); 
}

HRESULT CAnimMesh::SaveBinary()
{	
	char* Tag = TM->Get_ManagedChar();
	TM->TCtoC(sTag.data(), Tag);
	if(FAILED(DM->Conv_Bin_Model(m_pModelCom, Tag, CDataManager::DATA_ANIM)))
		return E_FAIL;
	MSG_BOX(TEXT("Success Binary"));
	return S_OK;
}

void CAnimMesh::ChangeAni(int iAniIndex)
{
	m_pModelCom->SetNextIndex(iAniIndex);
	m_pModelCom->SetChangeBool(true);
}

_float CAnimMesh::Get_Duration()
{
	return m_pModelCom->Get_Duration();
}

void CAnimMesh::Set_Duration(_float fDuration)
{
	m_pModelCom->Set_Duration(fDuration);
}

_float CAnimMesh::Get_TickPerSecond()
{
	return m_pModelCom->Get_TickPerSecond();
}

void CAnimMesh::Set_TickPerSecond(_float fTickPerSecond)
{
	m_pModelCom->Set_TickPerSecond(fTickPerSecond);
}

_float CAnimMesh::Get_PlayTime()
{
	return m_pModelCom->Get_PlayTime();
}

void CAnimMesh::Set_PlayTime(_float fPlayTime)
{
	m_pModelCom->SetPlayTime(fPlayTime);
}

_uint CAnimMesh::Get_KeyFrame()
{
	return m_pModelCom->Get_KeyFrame();
}

void CAnimMesh::Set_KeyFrame(_float fKeyFrame)
{
	m_pModelCom->Set_KeyFrame(fKeyFrame);
}

vector<_float>& CAnimMesh::Get_TimeLimit()
{
	return m_pModelCom->GetTimeLimit();
}

_float CAnimMesh::Get_TimeLimit(int iIndex)
{
	return m_pModelCom->Get_TimeLimit(iIndex);
}

void CAnimMesh::Set_TimeLimit(_float fTimeLimit, int iIndex)
{
	m_pModelCom->SetTimeLimit(fTimeLimit, iIndex);
}

void CAnimMesh::Add_TimeLimit()
{
	m_pModelCom->Add_TimeLimit();
}


void CAnimMesh::Delete_TimeLimit()
{
	m_pModelCom->Delete_TimeLimit();
}

void CAnimMesh::Set_RootHierarchy(_int iIndex)
{
	m_pModelCom->Set_RootHierarchy(iIndex);
}

void CAnimMesh::Change_AniIndex(int Index1, int Index2)
{
	m_pModelCom->ChangeAnimIndex(Index1, Index2);
}

void CAnimMesh::Reset_KeyFrame()
{
	m_pModelCom->ResetKeyFrame();
	m_pModelCom->Play_Animation(0.00001f);
}

void CAnimMesh::DeleteAnimation(int Index)
{
	m_pModelCom->DeleteAnimation(Index);
}

HRESULT CAnimMesh::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC Transform;
	Transform.fRotationPerSec = 30.f;
	Transform.fSpeedPerSec = 5.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &Transform)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.8f, 0.f);
	ColDesc.vSize = _float3(0.7f, 1.6f, 0.7f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Test");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CAnimMesh * CAnimMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAnimMesh*		pInstance = new CAnimMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAnimMesh::Clone(void * pArg)
{
	CAnimMesh*		pInstance = new CAnimMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAnimMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimMesh::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
