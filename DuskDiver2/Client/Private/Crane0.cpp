#include "stdafx.h"
#include "..\Public\Crane0.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "Monster_Manager.h"

#include "ToolManager.h"

CCrane0::CCrane0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CCrane0::CCrane0(const CCrane0 & rhs)
	: CCreture(rhs)
{
}

HRESULT CCrane0::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrane0::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	Make_ImGui();

	XMVECTOR Pos = *(XMVECTOR*)pArg;
	

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	 m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));


	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_fHurtTime = 0.02f;

	return S_OK;
}

void CCrane0::Tick(_float fTimeDelta)
{
	Emp_Col();

	if (XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) == 1.344f)
	{
		if (MM->Get_Arcade_Event1())
		{
			if (!m_bOpen)
			{
				GI->PlaySoundW(L"Open.wav", SD_UI, 1.f);
				m_bOpen = true;
			}
			m_fAracadeTime += fTimeDelta;
			if (m_fAracadeTime <= 5.2f && m_fAracadeTime > 3.6f)
			{
				if (!m_bDown)
				{
					GI->PlaySoundW(L"Down.wav", SD_UI, 1.f);
					m_bDown = true;
				}
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y -= 0.13f;
					m_fTickTime = 0.f;
				}
			}
			else if (m_fAracadeTime > 11.f && m_fAracadeTime < 12.6f)
			{
				if (!m_bUp)
				{
					GI->PlaySoundW(L"Up.wav", SD_UI, 1.f);
					m_bUp = true;
				}
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y += 0.13f;
					m_fTickTime = 0.f;
				}
			}
			if (m_fAracadeTime > 5.2f)
			{
				if (!m_bDownFinish)
				{
					GI->PlaySoundW(L"DownFinish.wav", SD_UI, 1.f);
					m_bDownFinish = true;
				}
			}
			if (m_fAracadeTime > 6.2f)
			{
				if (!m_bDownOpen)
				{
					GI->PlaySoundW(L"DownOpen.wav", SD_UI, 1.f);
					m_bDownOpen = true;
				}
			}
		}
	}

	else if (XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) == 1.6f)
	{
		if (!m_Event2)
		{
			m_bOpen = false;
			m_bDown = false;
			m_bDownFinish = false;
			m_bFinish = false;
			m_bDownOpen = false;
			m_bUp = false;
			m_Event2 = true;
		}
		if (MM->Get_Arcade_Event2())
		{
			if (!m_bOpen)
			{
				GI->PlaySoundW(L"Open.wav", SD_UI, 1.f);
				m_bOpen = true;
			}
			m_fAracadeTime += fTimeDelta;
			if (m_fAracadeTime <= 5.2f && m_fAracadeTime > 3.6f)
			{
				if (!m_bDown)
				{
					GI->PlaySoundW(L"Down.wav", SD_UI, 1.f);
					m_bDown = true;
				}
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y -= 0.13f;
					m_fTickTime = 0.f;
				}
			}
			else if (m_fAracadeTime > 11.f && m_fAracadeTime < 12.6f)
			{
				if (!m_bUp)
				{
					GI->PlaySoundW(L"Up.wav", SD_UI, 1.f);
					m_bUp = true;
				}
				m_fTickTime += fTimeDelta;
				if (m_fTickTime > 0.01f)
				{
					y += 0.13f;
					m_fTickTime = 0.f;
				}
			}
			if (m_fAracadeTime > 5.2f)
			{
				if (!m_bDownFinish)
				{
					GI->PlaySoundW(L"DownFinish.wav", SD_UI, 1.f);
					m_bDownFinish = true;
				}
			}
			if (m_fAracadeTime > 6.2f)
			{
				if (!m_bDownOpen)
				{
					GI->PlaySoundW(L"DownOpen.wav", SD_UI, 1.f);
					m_bDownOpen = true;
				}
			}
		}
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(m_OriginPos) + x, XMVectorGetY(m_OriginPos) + y, XMVectorGetZ(m_OriginPos) + z, 1));

}

void CCrane0::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);


}

HRESULT CCrane0::Render()
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


	_float fGama = 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_float fBlurPower = 0.f;
	_bool	bDiffuse = false;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;


		if (MM->Get_Stage4Scene())
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CCrane0::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	//if ("Tag_Player" == Desc.pOther->Get_Tag())
	//{
	//	// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
	//	if (!strcmp("YMTest", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag) && m_fHitTime > 1.f)
	//	{
	//			((CCreture*)Desc.pOther)->On_Hit(this);
	//			m_fHitTime = 0.f;
	//	}
	//}
}

HRESULT CCrane0::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_Claw0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CCrane0 * CCrane0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCrane0*		pInstance = new CCrane0(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCrane0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CCrane0::Clone(void * pArg)
{
	CCrane0*		pInstance = new CCrane0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCrane0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrane0::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CCrane0::Render_Debug()
{
}
