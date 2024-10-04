#include "stdafx.h"
#include "..\Public\Mesh.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "ToolManager.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
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
	Rotation(_float3{ 1.f,0.f,0.f }, m_fAngles.x, _float3{ 0.f,1.f,0.f }, m_fAngles.y, _float3{ 0.f,0.f,1.f }, m_fAngles.z);
	m_iShaderPass = 0;
	char* SearchName = TM->Get_ManagedChar();
	TM->TCtoC(m_MeshInfo->sTag, SearchName);
	IG->Add_Mesh(this);

	return S_OK;
}

void CMesh::Tick(_float fTimeDelta)
{
	if (m_bTurn && !m_bTurnStop)
	{
		m_pTransformCom->Set_TurnSpeed(m_fTurnSpeed);
		if (m_bTurnDir)
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);
		else
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP) * -1, fTimeDelta);
	}

	if (m_vRatio.x > 0.f)
	{
		m_vRatio.x -= m_vRatioSpeed.x * fTimeDelta;
		if (m_vRatio.x <= 0.f)
			m_vRatio.x = 0.f;
	}

	if (m_vRatio.y > 0.f)
	{
		m_vRatio.y -= m_vRatioSpeed.y * fTimeDelta;
		if (m_vRatio.y <= 0.f)
			m_vRatio.y = 0.f;
	}

	if (m_bUVFix)
	{
		m_vUVFixAcc.x += m_vUVFixSpeed.x * fTimeDelta;
		m_vUVFixAcc.y += m_vUVFixSpeed.y * fTimeDelta;
	}

	if (m_bFixPosScale)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMLoadFloat3(&m_vPosFixSpeed) * fTimeDelta));
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_vScaleFixSpeed) * fTimeDelta));
	}

}

void CMesh::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	m_fShaderUVAcc += 1.f * fTimeDelta;
	if (m_fShaderUVAcc >= m_fUVSpeed)
	{
		m_fShaderUVAcc = 0.f;
		m_fShaderUVIndexX += 1.f;
		if (m_fShaderUVIndexX >= m_fMaxUVIndexX)
		{
			m_fShaderUVIndexX = 0.f;
			m_fShaderUVIndexY += 1.f;
			if (m_fShaderUVIndexY >= m_fMaxUVIndexY)
			{
				m_fShaderUVIndexY = 0.f;
			}				
		}
	}
	

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if(m_iShaderPass == 4)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);	
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMesh::Render()
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



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_fMaxUVIndexY, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexX", &m_fShaderUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexY", &m_fShaderUVIndexY, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlphaAcc, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_bFixColor", &m_bFixColor, sizeof(_bool));
		m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower, sizeof(_float));
		_float4 RC = { 1.f,0.f,0.f,1.f };
		m_pShaderCom->Set_RawValue("g_RimColor", &RC, sizeof(_float4));
		_float RP = 0.1f;
		m_pShaderCom->Set_RawValue("g_RimPower", &RP, sizeof(_float));

		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, m_iShaderPass)))
			return E_FAIL;
	}

	return S_OK;
}

void CMesh::Rotation(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3)
{
	m_fAngles.x = fRadian;
	m_fAngles.y = fRadian2;
	m_fAngles.z = fRadian3;
	m_pTransformCom->RotationThree(vAxis, fRadian, vAxis2, fRadian2, vAxis3, fRadian3);	
}

_float3 CMesh::Get_Pos()
{
	_float3 Pos;
	XMStoreFloat3(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return Pos;
}

void CMesh::Set_Pos(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
}


_float3 CMesh::Get_Scale()
{
	
	_float3 Scale = m_pTransformCom->Get_Scale();
	/*if (Scale.x < 0.f || Scale.y < 0.f || Scale.z < 0.f)
	{
		_vector Right = { 1.f,0.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Right);

		_vector Up = { 0.f,1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_UP, Up);

		_vector Look = { 0.f,0.f,1.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, Look);

		_float3 Scale = m_pTransformCom->Get_Scale();
	}*/
	return Scale;
	
}

_float4x4 CMesh::Get_Matrix()
{
	_float4x4 Matrix;
	Matrix = m_pTransformCom->Get_WorldFloat4x4();
	return Matrix;
}

void CMesh::PreViewPos()
{
	
}

_float3 CMesh::Get_Right()
{
	_float3 Right;
	XMStoreFloat3(&Right, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	return Right;
}

_float3 CMesh::Get_Up()
{
	_float3 Up;
	XMStoreFloat3(&Up, m_pTransformCom->Get_State(CTransform::STATE_UP));
	return Up;
}

_float3 CMesh::Get_Look()
{
	_float3 Look;
	XMStoreFloat3(&Look, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	return Look;
}

_float3 CMesh::Get_Position()
{
	_float3 Pos;
	XMStoreFloat3(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return Pos;
}

HRESULT CMesh::Ready_Components()
{

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TransformDesc));

	TransformDesc.fSpeedPerSec = 50.f;
	TransformDesc.fRotationPerSec = 15.f;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ModelTool"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
