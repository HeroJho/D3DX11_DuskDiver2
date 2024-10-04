#include "stdafx.h"
#include "..\Public\DGCuma_2.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"


CDGCuma_2::CDGCuma_2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CDGCuma_2::CDGCuma_2(const CDGCuma_2 & rhs)
	: CCreture(rhs)
{
}

HRESULT CDGCuma_2::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CDGCuma_2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();



	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-195.499f, 38.7f, 127.638f, 1.f));

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f));

	

	return S_OK;
}
void CDGCuma_2::Set_Down()
{
	m_bDown = true;
}
void CDGCuma_2::Tick(_float fTimeDelta)
{


	if (m_bDown)
	{
		if (m_bRealDown)
		{
			if (m_bTickUp)
			{
				if (!m_bUp)
				{
					GI->PlaySoundW(L"se_Env12_Mole_Opening_04.wav", SD_MAP, 1.f);
					m_bUp = true;
				}
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float fMyY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				if (38.7f + 0.3f < fMyY)
				{
					//vPos = XMVectorSetY(vPos, 38.7f + 0.3f);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
					m_bTickUp = false;
					m_bTickDown = true;
					return;
				}

				m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 4.f, fTimeDelta);

			}
			else if (m_bTickDown)
			{
				if (m_bUp)
				{
					GI->PlaySoundW(L"se_Env12_Mole_Opening_05.wav", SD_MAP, 1.f);
					m_bUp = false;
				}
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float fMyY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				if (38.7f > fMyY)
				{
					//vPos = XMVectorSetY(vPos, 38.7f);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
					//m_bDown = false;

					m_fDownTimeAcc += fTimeDelta;
					if (1.f < m_fDownTimeAcc)
					{
						m_fDownTimeAcc = 0.f;
						m_bRealDown = false;
					}

					return;
				}

				m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 4.f, fTimeDelta);
			}
		}
		else
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fMyY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			if (34.7f > fMyY)
			{
				vPos = XMVectorSetY(vPos, 34.7f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
				m_bDown = false;
				Set_Dead();
				return;
			}

			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 1.f, fTimeDelta);
		}

	}

}

void CDGCuma_2::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CDGCuma_2::Render()
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
	_float fAlpha = 0.f;


	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 1.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	if (FACE_SMILE == m_eFace)
	{
		iNumMeshes = m_pModelCom1->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;


		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom1->SetUp_OnShader(m_pShaderCom, m_pModelCom1->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			// 노말 텍스쳐가 있다면
			if (!FAILED(m_pModelCom1->SetUp_OnShader(m_pShaderCom, m_pModelCom1->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				iPassIndex = 2;

			if (FAILED(m_pModelCom1->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}
	else if (FACE_IDLE == m_eFace)
	{
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

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}
	else if (FACE_HURT == m_eFace)
	{
		iNumMeshes = m_pModelCom2->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;

		//// For.Rim
		//if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		//	return E_FAIL;
		//_float m_fRim = 10.f;
		//if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &m_fRim, sizeof(_float))))
		//	return E_FAIL;



		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			//// 노말 텍스쳐가 있다면
			//if (!FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			//	iPassIndex = 2;

			if (FAILED(m_pModelCom2->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}



	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CDGCuma_2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CDGCuma_2::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}


HRESULT CDGCuma_2::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAa"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom1)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAc"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom2)))
		return E_FAIL;


	return S_OK;
}



CDGCuma_2 * CDGCuma_2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDGCuma_2*		pInstance = new CDGCuma_2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDGCuma_2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDGCuma_2::Clone(void * pArg)
{
	CDGCuma_2*		pInstance = new CDGCuma_2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDGCuma_2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDGCuma_2::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);


}
