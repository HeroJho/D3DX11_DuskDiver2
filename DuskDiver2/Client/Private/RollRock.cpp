#include "stdafx.h"
#include "..\Public\RollRock.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"

CRollRock::CRollRock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CRollRock::CRollRock(const CRollRock & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRollRock::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRollRock::Initialize(void * pArg)
{
	m_RockInfo = *(ROLLROCKINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_Navigation(m_RockInfo.iNaviNum)))
		return E_FAIL;


	m_bDead = false;

	m_pTransformCom->Set_WorldMatrix(m_RockInfo.vWorldMatrix);

	m_pTransformCom->Set_Scale(XMVectorSet(m_RockInfo.fScale, m_RockInfo.fScale, m_RockInfo.fScale, 1.f));

	if (m_RockInfo.bJump)
	{
		m_pTransformCom->ResetAndJump(m_RockInfo.fJumpPower);
	}

	_float fRandomX = GI->Get_FloatRandom(50.f, 200.f);
	_float fRandomZ = GI->Get_FloatRandom(100.f, 360.f);

	m_pTransformCom->RotationThree(_float3(1.f, 0.f, 0.f), fRandomX,
		_float3(0.f, 1.f, 0.f), 0.f,
		_float3(0.f, 0.f, 1.f), fRandomZ);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

void CRollRock::Tick(_float fTimeDelta)
{
	fTimeDelta *= m_fTMRatio;

	m_fDeathAcc += 0.1f * fTimeDelta;
	
	if (m_fDeathAcc > 0.5f)
		m_bDissolve = true;

	Check_Death(fTimeDelta);

}

void CRollRock::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	if (m_RockInfo.bJump)
	{
		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 20.f);

		if(m_pTransformCom->Get_Air())
			m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f, 1.2f, fTimeDelta, m_pNavigationCom);

	}

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_COMPUTEALPHABLEND, this);
}

HRESULT CRollRock::Render()
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

	
	_float fGama = 2.2f;
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


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 16)))
			return E_FAIL;
	}

	return S_OK;
}

void CRollRock::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	IG->Debug_Transform(m_pTransformCom);

	ImGui::End();
}


void CRollRock::Check_Death(_float fTimeDelta)
{
	if (m_bDissolve)
	{
		m_fDissolveAcc += 0.15f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;
			Set_Dead();

		}
	}
}

HRESULT CRollRock::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */

	switch (m_RockInfo.iIndex)
	{
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("PParticleRock1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("PParticleRock2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("PParticleRock3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("PParticleRock4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CRollRock::Create_Navigation(_uint iNaviNum)
{
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage3Navigation"), TEXT("Stage3"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	m_pNavigationCom->Set_NaviIndex(iNaviNum);

	return S_OK;
}

CRollRock * CRollRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRollRock*		pInstance = new CRollRock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRollRock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRollRock::Clone(void * pArg)
{
	CRollRock*		pInstance = new CRollRock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRollRock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRollRock::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	
}
