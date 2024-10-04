#include "stdafx.h"
#include "..\Public\Stage3Door.h"
#include "GameInstance.h"
#include "Player_Manager.h"

CStage3Door::CStage3Door(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CStage3Door::CStage3Door(const CStage3Door & rhs)
	:CGameObject(rhs)
{
}

HRESULT CStage3Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage3Door::Initialize(void * pArg)
{
	m_DoorDesc = *(DOORDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_Prop";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CStage3Door::Tick(_float fTimeDelta)
{
	Update_Door(fTimeDelta);

}

void CStage3Door::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CStage3Door::Render()
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


	_float fGama = 1.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_float fBlurPow = 0.1f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPow, sizeof(_float))))
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

	return S_OK;
}

HRESULT CStage3Door::Ready_Components()
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
	if (0 == m_DoorDesc.iDoorIndex)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_TumazA_DoorL"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}
	else if (1 == m_DoorDesc.iDoorIndex)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_TumazA_DoorR"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}


	return S_OK;
}

_bool CStage3Door::CheckPlayerOnPos()
{
	_float4 vPlayerPos = PM->Get_PlayerPos();

	if ((165.f > vPlayerPos.z) && (160.f < vPlayerPos.z) && ((28.f < vPlayerPos.x) && (32.f > vPlayerPos.x)))
		return true;

	return false;
}

void CStage3Door::Update_Door(_float fTimeDelta)
{
	if (true == CheckPlayerOnPos()) // 문이 열린다
	{
		if (!m_bOpen)
		{
			GI->PlaySoundW(L"DoorOpen1.wav", SD_MAP, 0.6f);
			m_bOpen = true;
		}
		
		if (0 == m_DoorDesc.iDoorIndex)
		{
			MoveLeft(fTimeDelta);
		}
		else if (1 == m_DoorDesc.iDoorIndex)
		{
			MoveRight(fTimeDelta);
		}
	}
	else // 문이 닫긴다
	{
		m_bOpen = false;

		if (1 == m_DoorDesc.iDoorIndex)
		{
			MoveLeft(fTimeDelta);
		}
		else if (0 == m_DoorDesc.iDoorIndex)
		{
			MoveRight(fTimeDelta);
		}
	}

}

void CStage3Door::MoveLeft(_float fTimeDelta)
{
	_float4 vCurPos = { 0.f,0.f,0.f,1.f };
	XMStoreFloat4(&vCurPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_DoorDesc.fMinZ < vCurPos.z)
	{
		vCurPos.z -= 2.f * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCurPos));
		if (m_DoorDesc.fMinZ >= vCurPos.z)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCurPos.x, 0.f, m_DoorDesc.fMinZ, 1.f));
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCurPos.x, 0.f, m_DoorDesc.fMinZ, 1.f));
	}

}

void CStage3Door::MoveRight(_float fTimeDelta)
{
	_float4 vCurPos = { 0.f,0.f,0.f,1.f };
	XMStoreFloat4(&vCurPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_DoorDesc.fMaxZ > vCurPos.z)
	{
		vCurPos.z += 2.f * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vCurPos));
		if (m_DoorDesc.fMaxZ <= vCurPos.z)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCurPos.x, 0.f, m_DoorDesc.fMaxZ, 1.f));
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCurPos.x, 0.f, m_DoorDesc.fMaxZ, 1.f));
	}
}

CStage3Door * CStage3Door::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage3Door*		pInstance = new CStage3Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3Door"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage3Door::Clone(void * pArg)
{
	CStage3Door*		pInstance = new CStage3Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3Door"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage3Door::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
