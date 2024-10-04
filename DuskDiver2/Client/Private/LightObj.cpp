#include "stdafx.h"
#include "..\Public\LightObj.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"

#include "Light.h"

CLightObj::CLightObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CLightObj::CLightObj(const CLightObj & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLightObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightObj::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	LIGHTOBJDESC* pDesc = (LIGHTOBJDESC*)pArg;


	m_sTag = "Tag_Light";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->vPos.x, pDesc->vPos.y, pDesc->vPos.z, 1.f));

	
	m_vInitDiffuse = pDesc->vDiffuse;
	m_fEndTime = pDesc->fEndTime;


	// º¸Á¶±¤¿ø1
	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.fRange = pDesc->fRange;
	XMStoreFloat4(&LightDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = pDesc->vAmbient;
	LightDesc.vSpecular = pDesc->vSpecular;
	if (FAILED(GI->Add_Light(LightDesc, &m_pLight)))
		return E_FAIL;



	return S_OK;
}

void CLightObj::Tick(_float fTimeDelta)
{
	// XMStoreFloat4(&m_pLight->Get_LightDesc()->vPosition,  m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (1.f < m_fRatio)
	{
		 m_pLight->Get_LightDesc()->vDiffuse = m_vInitDiffuse;
		return;
	}
	m_fTimeAcc += fTimeDelta;
	m_fRatio = m_fTimeAcc / m_fEndTime;
	
	_vector vDiff = XMVectorSetW(XMLoadFloat4(&m_vInitDiffuse) * m_fRatio, 1.f);
	XMStoreFloat4(&m_pLight->Get_LightDesc()->vDiffuse, vDiff);

}

void CLightObj::LateTick(_float fTimeDelta)
{


	//Tick_Col(m_pTransformCom->Get_WorldMatrix());
	//Choose_Col(nullptr);
}

HRESULT CLightObj::Render()
{


	return S_OK;
}

void CLightObj::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::End();
}

void CLightObj::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{



	}
}



HRESULT CLightObj::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;



	//CCollider::COLLIDERDESC ColDesc;
	//ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	//ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	//ColDesc.pOwner = this;
	//strcpy(ColDesc.sTag, "Potal");
	//if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
	//	return E_FAIL;


	return S_OK;
}





CLightObj * CLightObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLightObj*		pInstance = new CLightObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLightObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLightObj::Clone(void * pArg)
{
	CLightObj*		pInstance = new CLightObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLightObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLightObj::Free()
{
	__super::Free();

	if (m_pLight)
		m_pLight->Set_Dead();

	if(m_pTransformCom)	
		Safe_Release(m_pTransformCom);
}
