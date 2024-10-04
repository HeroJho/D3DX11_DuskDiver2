#include "stdafx.h"
#include "..\Public\Stage2.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"

CStage2::CStage2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage2::CStage2(const CStage2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	// m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));

	UM->MoveBlind(false);

	return S_OK;
}

void CStage2::Tick(_float fTimeDelta)
{
	if (UM->Get_MoveOver())
	{
		TM->Change_Level(LEVEL_STAGE3);
		UM->Set_FrontUI(false);
		return;
	}

	if (TM->Get_Stage2_2Bool())
	{
		m_fAlpa += fTimeDelta;
		if (1.f < m_fAlpa)
			m_fAlpa = 1.f;

	}
	else
	{
		m_fAlpa = 0.f;
	}

	if (!TM->Get_Stage2_2Bool())
		Crowd_Sound(fTimeDelta);
	else
	{
		GI->StopSound(SD_NPC);
		GI->StopSound(SD_MAP);
	}
}

void CStage2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CStage2::Render()
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


	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpa, sizeof(_float))))
		return E_FAIL;

	_float fGama = 0.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	// if (CK_DOWN(DIK_N))
	// 	++test;
	// else if (CK_DOWN(DIK_B))
	// 	test = 0;

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

		// 15 전등  4 간판 16
		if (15 == i)
		{
			fBlurPower = 0.5f;
			iPassIndex = 15;
			bDiffuse = false;
		}
		else if (4 == i)
		{
			fBlurPower = 0.5f;
			iPassIndex = 15;
			bDiffuse = true;
		}
		else if (16 == i)
		{
			fBlurPower = 0.5f;
			iPassIndex = 15;
			bDiffuse = true;
		}
		else
		{
			fBlurPower = 0.f;
			bDiffuse = false;
		}


		if (FAILED(m_pShaderCom->Set_RawValue("g_Stage22Diffuse", &bDiffuse, sizeof(_bool))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CStage2::Crowd_Sound(_float fTimeDelta)
{
	m_fSoundAcc += 1.f * fTimeDelta;

	if (!m_bSound)
	{
		GI->PlaySound_Linear(L"se_Env51_Ambient_people.wav", SD_NPC, 0.7f, true, 0.2f);
		m_bSound = true;
	}

	else
	{
		if (10.f <= m_fSoundAcc)
		{
			m_bSound = false;
			m_fSoundAcc = 0.f;
		}
	}
}

HRESULT CStage2::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CStage2 * CStage2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage2*		pInstance = new CStage2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage2::Clone(void * pArg)
{
	CStage2*		pInstance = new CStage2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage2::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CStage2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::InputFloat("Gama", &m_fGama);


	ImGui::End();
}