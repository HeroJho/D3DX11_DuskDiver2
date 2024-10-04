#include "stdafx.h"
#include "..\Public\Rock.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"

CRock::CRock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRock::CRock(const CRock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRock::Initialize(void * pArg)
{

	CRock::ROCKINFO _tInfo = *(CRock::ROCKINFO*)pArg;

	switch (_tInfo.iIndex)
	{
	case 0:
		lstrcpy(m_cModelkey, TEXT("ParticleRock1"));
		break;
	case 1:
		lstrcpy(m_cModelkey, TEXT("ParticleRock2"));
		break;
	case 2:
		lstrcpy(m_cModelkey, TEXT("ParticleRock3"));
		break;
	default:
		break;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if(_tInfo.iIndex != 2)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	else
		m_pTransformCom->Set_WorldMatrix(_tInfo.vWorldMatrix);

	_vector vPosition = XMLoadFloat4(&_tInfo.vPosition);

	_matrix Identity = XMMatrixIdentity();
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);

	m_pTransformCom->Set_WorldMatrix(Matrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Set_Scale(XMVectorSet(_tInfo.fScale, _tInfo.fScale, _tInfo.fScale, 1.f));

	m_eCurState = START;
	m_pModelCom->Set_AnimIndex(START);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

void CRock::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	Check_Death(fTimeDelta);

}

void CRock::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, nullptr);

	End_Animation();

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CRock::Render()
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

	_float fGama = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float));


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 6)))
			return E_FAIL;
	}

	return S_OK;
}

void CRock::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	IG->Debug_Transform(m_pTransformCom);

	ImGui::End();
}

void CRock::End_Animation()
{
	if(m_pModelCom->GetAniEnd())
	{
		m_pModelCom->Set_Duration(0.f);
		m_bDissolve = true;
	}
}

void CRock::Update(_float fTimeDelta)
{
}

void CRock::Check_Rim(_float fTimeDelta)
{
}

void CRock::Check_Death(_float fTimeDelta)
{
	if (m_bDissolve)
	{
		m_fDissolveAcc += 0.5f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;
			Set_Dead();

		}
	}
}

HRESULT CRock::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_cModelkey, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	return S_OK;
}

CRock * CRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRock*		pInstance = new CRock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRock::Clone(void * pArg)
{
	CRock*		pInstance = new CRock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRock::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	
}
