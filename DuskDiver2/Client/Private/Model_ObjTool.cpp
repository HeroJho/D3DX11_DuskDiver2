#include "stdafx.h"
#include "..\Public\Model_ObjTool.h"
#include "GameInstance.h"

#include "ToolManager.h"

#include "HierarchyNode.h"

CModel_ObjTool::CModel_ObjTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CModel_ObjTool::CModel_ObjTool(const CModel_ObjTool & rhs)
	: CGameObject(rhs)
{
}

HRESULT CModel_ObjTool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModel_ObjTool::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Shader_Tool";

	Make_ImGui();

	return S_OK;
}

void CModel_ObjTool::Tick(_float fTimeDelta)
{

}

void CModel_ObjTool::LateTick(_float fTimeDelta)
{

}

HRESULT CModel_ObjTool::Render()
{


	return S_OK;
}






void CModel_ObjTool::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	CModel* pModel = TM->Get_ModelTool();
	if (!pModel)
	{
		ImGui::End();
		return;
	}

	const vector<CHierarchyNode*>* pBones = pModel->Get_HierarchyNodes();

	if (ImGui::BeginListBox("Bones", ImVec2(300.f, 500.f)))
	{
		for (auto& pBone : *pBones)
		{

			_float4x4 mOri = pBone->Get_OriTransformation();
			_float4x4 mCom; XMStoreFloat4x4(&mCom, pBone->Get_CombinedTransformation());

			ImGui::Text("==========================");

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Name: "); ImGui::SameLine(); ImGui::Text(pBone->Get_Name());
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "ParName: "); ImGui::SameLine(); ImGui::Text(pBone->Get_ParentName());
			
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f) ,"===OriTran===");
			ImGui::Text("R: %.1f, %.1f, %.1f, %.1f", mOri._11, mOri._12, mOri._13, mOri._14);
			ImGui::Text("U: %.1f, %.1f, %.1f, %.1f", mOri._21, mOri._22, mOri._23, mOri._24);
			ImGui::Text("L: %.1f, %.1f, %.1f, %.1f", mOri._31, mOri._32, mOri._33, mOri._34);
			ImGui::Text("P: %f, %f, %f, %f", mOri._41, mOri._42, mOri._43, mOri._44);
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "===CombinedTran===");
			ImGui::Text("R: %.1f, %.1f, %.1f, %.1f", mCom._11, mCom._12, mCom._13, mCom._14);
			ImGui::Text("U: %.1f, %.1f, %.1f, %.1f", mCom._21, mCom._22, mCom._23, mCom._24);
			ImGui::Text("L: %.1f, %.1f, %.1f, %.1f", mCom._31, mCom._32, mCom._33, mCom._34);
			ImGui::Text("P: %.1f, %.1f, %.1f, %.1f", mCom._41, mCom._42, mCom._43, mCom._44);

			ImGui::Text("==========================");
		}

		ImGui::EndListBox();
	}







	ImGui::End();
}





HRESULT CModel_ObjTool::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}





CModel_ObjTool * CModel_ObjTool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CModel_ObjTool*		pInstance = new CModel_ObjTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : Model_ObjTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CModel_ObjTool::Clone(void * pArg)
{
	CModel_ObjTool*		pInstance = new CModel_ObjTool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : Model_ObjTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CModel_ObjTool::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

}
