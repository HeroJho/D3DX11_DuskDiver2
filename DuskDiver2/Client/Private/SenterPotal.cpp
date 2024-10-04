#include "stdafx.h"
#include "..\Public\SenterPotal.h"
#include "GameInstance.h"


#include "Player_Manager.h"
#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"



CSenterPotal::CSenterPotal(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CSenterPotal::CSenterPotal(const CSenterPotal & rhs)
	: CCreture(rhs)
{
}

HRESULT CSenterPotal::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CSenterPotal::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.5f, 7.9f, 78.3f, 1.f));



	return S_OK;
}

void CSenterPotal::Tick(_float fTimeDelta)
{


}


void CSenterPotal::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;





	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSenterPotal::Render()
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

	_uint		iNumMeshes = 0;
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;


	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 2.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_float fAlpha = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
		return E_FAIL;

	iNumMeshes = m_pModelCom->Get_NumMeshes();
	iPassIndex = 0;
	fBlurPower = 0.f;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 25)))
			return E_FAIL;
	}



	return S_OK;
}



// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CSenterPotal::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

	}

}

void CSenterPotal::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt)
	{
		m_bHurt = true;


	}
}





HRESULT CSenterPotal::Ready_Components()
{

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Portal"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	




	return S_OK;
}



CSenterPotal * CSenterPotal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSenterPotal*		pInstance = new CSenterPotal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSenterPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSenterPotal::Clone(void * pArg)
{
	CSenterPotal*		pInstance = new CSenterPotal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSenterPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSenterPotal::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);


}
