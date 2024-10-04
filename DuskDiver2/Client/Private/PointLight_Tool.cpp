#include "stdafx.h"
#include "..\Public\PointLight_Tool.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "ImGui_Manager.h"
#include "Light.h"


CPointLight_Tool::CPointLight_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPointLight_Tool::CPointLight_Tool(const CPointLight_Tool & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPointLight_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPointLight_Tool::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Px_Tool";

	Make_ImGui();



	return S_OK;
}

void CPointLight_Tool::Tick(_float fTimeDelta)
{
	if (TK_PRESS(DIK_LSHIFT))
		m_fSpeed = 10.f;

	if (TK_PRESS(DIK_LEFT))
		m_pTransformCom->Go_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
	else if (TK_PRESS(DIK_RIGHT))
		m_pTransformCom->Go_Dir(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
	else if (TK_PRESS(DIK_UP))
	{
		if (TK_PRESS(DIK_RSHIFT))
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
		else
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fSpeed, fTimeDelta);
	}
	else if (TK_PRESS(DIK_DOWN))
	{
		if (TK_PRESS(DIK_RSHIFT))
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
		else
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, -1.f, 0.f), m_fSpeed, fTimeDelta);
	}


}

void CPointLight_Tool::LateTick(_float fTimeDelta)
{

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Render_Col(m_pRendererCom);

}

HRESULT CPointLight_Tool::Render()
{


	return S_OK;
}




void CPointLight_Tool::Create_Point()
{

	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	XMStoreFloat4(&LightDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	GI->Add_Light(LightDesc);

}

void CPointLight_Tool::Save_Data(_int iLevelIndex)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "Light_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/LightData/");
	strcat_s(cPullName, cName);


	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return;

	list<CLight*>* pLights = GI->Get_Lights();

	_int iNumObj = (_int)pLights->size();

	ofs.write((char*)&iLevelIndex, sizeof(int));		// ID
	ofs.write((char*)&iNumObj, sizeof(int));			// NumObj


	for (auto& pLight : *pLights)
	{
		LIGHTDESC* pDesc = pLight->Get_LightDesc();
		ofs.write((char*)&pDesc->vPosition, sizeof(_float4));
		ofs.write((char*)&pDesc->fRange, sizeof(_float));

		ofs.write((char*)&pDesc->vDiffuse, sizeof(_float4));
		ofs.write((char*)&pDesc->vAmbient, sizeof(_float4));
		ofs.write((char*)&pDesc->vSpecular, sizeof(_float4));
	}


	ofs.close();

	return;
}

void CPointLight_Tool::Load_Data(_int iLevelIndex)
{
	GI->Reset_Light();

	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "Light_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/LightData/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return;



	_int iID = 0;
	_int iSize = 0;
	ifs.read((char*)&iID, sizeof(int));
	ifs.read((char*)&iSize, sizeof(int));

	for (_int i = 0; i < iSize; ++i)
	{
		LIGHTDESC Desc;

		Desc.eType = LIGHTDESC::TYPE_POINT;
		ifs.read((char*)&Desc.vPosition, sizeof(_float4));
		ifs.read((char*)&Desc.fRange, sizeof(_float));

		ifs.read((char*)&Desc.vDiffuse, sizeof(_float4));
		ifs.read((char*)&Desc.vAmbient, sizeof(_float4));
		ifs.read((char*)&Desc.vSpecular, sizeof(_float4));
		// Desc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);

		GI->Add_Light(Desc);
	}


	return;
}

void CPointLight_Tool::SameLight()
{
	list<CLight*>* pLights = GI->Get_Lights();

	if (!m_pSelectedLight)
		return;

	for (auto& pLight : *pLights)
	{
		pLight->Get_LightDesc()->fRange = m_pSelectedLight->Get_LightDesc()->fRange;
		pLight->Get_LightDesc()->vDiffuse = m_pSelectedLight->Get_LightDesc()->vDiffuse;
		pLight->Get_LightDesc()->vAmbient = m_pSelectedLight->Get_LightDesc()->vAmbient;
	}


}





HRESULT CPointLight_Tool::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Tool");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CPointLight_Tool * CPointLight_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPointLight_Tool*		pInstance = new CPointLight_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPointLight_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPointLight_Tool::Clone(void * pArg)
{
	CPointLight_Tool*		pInstance = new CPointLight_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPointLight_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPointLight_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);


}




void CPointLight_Tool::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());



	LIGHTDESC* pLightDesc = GI->Get_DirLightDesc();
	if (pLightDesc)
	{
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Dir Light");
		ImGui::DragFloat3("Dir_vDirection", (_float*)&pLightDesc->vDirection, 0.01f);
		ImGui::DragFloat3("Dir_Diffuse", (_float*)&pLightDesc->vDiffuse, 0.01f);
		ImGui::DragFloat3("Dir_Ambient", (_float*)&pLightDesc->vAmbient, 0.01f);
	}
	ImGui::NewLine();
	ImGui::NewLine();

	list<CLight*>* pLights = GI->Get_Lights();
	ImVec2 vSize{ 150.f, 200.f };
	_int iCount = 0;
	if (ImGui::BeginListBox("Layers", vSize))
	{
		for (auto& pLight : *pLights)
		{
			ImVec2 vSize{ 100.f, 10.f };

			bool isSelected = !strcmp(to_string(m_iSelectedIndex).data(), to_string(iCount).data());
			if (ImGui::Selectable(to_string(iCount).data(), isSelected, 0, vSize))
			{
				m_iSelectedIndex = iCount;
				m_pSelectedLight = pLight;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();

			++iCount;
		}

		ImGui::EndListBox();
	}


	if (ImGui::Button("Create"))
		Create_Point();
	if (ImGui::Button("Show"))
		m_bShow = !m_bShow;


	if (!m_pSelectedLight)
	{
		ImGui::End();
		return;
	}

	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Light Info");
	_float4 vPos = m_pSelectedLight->Get_LightDesc()->vPosition;
	if (ImGui::DragFloat4("Pos", (_float*)&vPos))
		m_pSelectedLight->Get_LightDesc()->vPosition = vPos;

	_float fRange = m_pSelectedLight->Get_LightDesc()->fRange;
	if (ImGui::DragFloat("Range", &fRange))
		m_pSelectedLight->Get_LightDesc()->fRange = fRange;

	_float4 vDiffuse = m_pSelectedLight->Get_LightDesc()->vDiffuse;
	if (ImGui::DragFloat4("Diffuse", (_float*)&vDiffuse))
		m_pSelectedLight->Get_LightDesc()->vDiffuse = vDiffuse;

	_float4 vAmbient = m_pSelectedLight->Get_LightDesc()->vAmbient;
	if (ImGui::DragFloat4("Ambient", (_float*)&vAmbient))
		m_pSelectedLight->Get_LightDesc()->vAmbient = vAmbient;

	_float4 vSpecular = m_pSelectedLight->Get_LightDesc()->vSpecular;
	if (ImGui::DragFloat4("Specular", (_float*)&vSpecular))
		m_pSelectedLight->Get_LightDesc()->vSpecular = vSpecular;

	if (ImGui::Button("SameLight"))
		SameLight();
	if (ImGui::Button("Delete"))
		GI->Delete_Light(m_iSelectedIndex);

	if (m_bShow)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pSelectedLight->Get_LightDesc()->vPosition));



	ImGui::InputInt("LevelId", &m_iLevelIndex);
	if (ImGui::Button("Save"))
		Save_Data(m_iLevelIndex);
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		Load_Data(m_iLevelIndex);



	ImGui::End();
}
