#include "stdafx.h"
#include "..\Public\Stage4Face.h"
#include "GameInstance.h"

CStage4Face::CStage4Face(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage4Face::CStage4Face(const CStage4Face & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage4Face::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage4Face::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_prop";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CStage4Face::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_NUMPADENTER))
	//{
	//	++m_iCurFace;
	//	if (FACE_END <= m_iCurFace)
	//		m_iCurFace = 0;
	//}
	Change_RandomFace(fTimeDelta);
}

void CStage4Face::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

}

HRESULT CStage4Face::Render()
{
	if (nullptr == m_pModelCom[m_iCurFace] ||
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


	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);

	//if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpa, sizeof(_float))))
	//	return E_FAIL;


	_uint		iNumMeshes = m_pModelCom[m_iCurFace]->Get_NumMeshes();
	_uint iPassIndex = 20;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom[m_iCurFace]->SetUp_OnShader(m_pShaderCom, m_pModelCom[m_iCurFace]->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom[m_iCurFace]->SetUp_OnShader(m_pShaderCom, m_pModelCom[m_iCurFace]->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			iPassIndex = 0;

		if (FAILED(m_pModelCom[m_iCurFace]->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage4Face::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Face_Happy"), TEXT("Com_Model0"), (CComponent**)&m_pModelCom[FACE_HAPPY])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Face_Angry"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom[FACE_ANGRY])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Face_Confused"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom[FACE_CONFUSED])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Face_Tearful"), TEXT("Com_Model3"), (CComponent**)&m_pModelCom[FACE_TEARFUL])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Face_Faint"), TEXT("Com_Model4"), (CComponent**)&m_pModelCom[FACE_FAINT])))
		return E_FAIL;


	return S_OK;
}

void CStage4Face::Change_RandomFace(_float fTimeDelta)
{
	m_fChangeTime += 1.f * fTimeDelta;


	if (3.f < m_fChangeTime)
	{
		m_fChangeTime = 0.f;
		m_iCurFace = GI->Get_Random(0, 4);
	}
}

CStage4Face * CStage4Face::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage4Face*		pInstance = new CStage4Face(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage4Face"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage4Face::Clone(void * pArg)
{
	CStage4Face*		pInstance = new CStage4Face(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage4Face"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage4Face::Free()
{
	__super::Free();

	for (_uint i = 0; FACE_END > i; ++i)
	{
		Safe_Release(m_pModelCom[i]);
	}
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CStage4Face::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::InputFloat("BlurPow", &m_fBlurPower);


	ImGui::End();
}