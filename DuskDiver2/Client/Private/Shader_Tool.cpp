#include "stdafx.h"
#include "..\Public\Shader_Tool.h"
#include "GameInstance.h"

#include "ToolManager.h"

CShader_Tool::CShader_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CShader_Tool::CShader_Tool(const CShader_Tool & rhs)
	: CGameObject(rhs)
{
}

HRESULT CShader_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShader_Tool::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Shader_Tool";

	Make_ImGui();

	return S_OK;
}

void CShader_Tool::Tick(_float fTimeDelta)
{

}

void CShader_Tool::LateTick(_float fTimeDelta)
{

}

HRESULT CShader_Tool::Render()
{
	

	return S_OK;
}



HRESULT CShader_Tool::Ready_Components()
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





CShader_Tool * CShader_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShader_Tool*		pInstance = new CShader_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShader_Tool::Clone(void * pArg)
{
	CShader_Tool*		pInstance = new CShader_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShader_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

}



void CShader_Tool::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::DragFloat("LinePower ", &GI->Get_ValueDesc()->fLinePower, 0.01f);
	ImGui::DragFloat("SpecPower ", &GI->Get_ValueDesc()->fSpacPower, 0.01f);
	ImGui::NewLine();
	LIGHTDESC* pLightDesc = GI->Get_DirLightDesc();
	if (pLightDesc)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Dir Light");
		ImGui::DragFloat3("vDirection", (_float*)&pLightDesc->vDirection, 0.01f);
		ImGui::DragFloat3("Diffuse", (_float*)&pLightDesc->vDiffuse, 0.01f);
		ImGui::DragFloat3("Ambient", (_float*)&pLightDesc->vAmbient, 0.01f);
	}
	
	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "LUT");
	ImGui::InputInt("Index", &GI->Get_ValueDesc()->iLUT_Index);



	ImGui::NewLine();
	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "ShadowView");

	if (ImGui::DragFloat4("ViewPos", (_float*)&m_vViewPos, 0.1f) ||
		ImGui::DragFloat4("ViewLookPos", (_float*)&m_vViewLookPos, 0.1f))
	{
		_float4x4 ShadowMatrix;
		_vector		vLightEye = XMLoadFloat4(&m_vViewPos);
		_vector		vLightAt = XMLoadFloat4(&m_vViewLookPos);
		_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

		GI->Add_ShadowLightViewMatrix(TM->Get_CurLevel(), ShadowMatrix);
	}

	if (ImGui::Button("Set_ViewPos_Cam"))
		m_vViewPos = GI->Get_CamPosition();
	ImGui::SameLine();
	if (ImGui::Button("Set_ViewLookPos_Cam"))
		m_vViewLookPos = GI->Get_CamPosition();
	ImGui::SameLine();
	if (ImGui::Button("Make_View"))
	{
		_float4x4 ShadowMatrix;
		_vector		vLightEye = XMLoadFloat4(&m_vViewPos);
		_vector		vLightAt = XMLoadFloat4(&m_vViewLookPos);
		_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

		GI->Add_ShadowLightViewMatrix(TM->Get_CurLevel(), ShadowMatrix);
	}



	ImGui::End();
}