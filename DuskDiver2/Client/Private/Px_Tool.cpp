#include "stdafx.h"
#include "..\Public\Px_Tool.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "ImGui_Manager.h"


CPx_Tool::CPx_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPx_Tool::CPx_Tool(const CPx_Tool & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPx_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPx_Tool::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Px_Tool";

	Make_ImGui();

	IG->Set_Px_Tool(this);

	return S_OK;
}

void CPx_Tool::Tick(_float fTimeDelta)
{
	if (TK_PRESS(DIK_LSHIFT))
		m_fSpeed = 10.f;

	if (TK_PRESS(DIK_LEFT))
		m_pTransformCom->Go_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
	else if (TK_PRESS(DIK_RIGHT))
		m_pTransformCom->Go_Dir(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
	else if (TK_PRESS(DIK_UP))
	{
		if(TK_PRESS(DIK_RSHIFT))
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

void CPx_Tool::LateTick(_float fTimeDelta)
{

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Render_Col(m_pRendererCom);

}

HRESULT CPx_Tool::Render()
{


	return S_OK;
}






void CPx_Tool::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	const list<string>* pNames = DM->Get_NonAnimFileNames();
	ImVec2 vSize{ 150.f, 200.f };
	if (ImGui::BeginListBox("Layers", vSize))
	{
		for (auto& Name : *pNames)
		{
			ImVec2 vSize{ 100.f, 10.f };
			bool isSelected = !strcmp(m_sSelectedModel, Name.data());
			if (ImGui::Selectable(Name.data(), isSelected, 0, vSize))
				strcpy(m_sSelectedModel, Name.data());

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}

	if (ImGui::Button("Create"))
		Create_PxObj();
	if (ImGui::Button("Same_All_Edit"))
		Edit_SameName();
	if (ImGui::Button("Reset_PosQuat"))
		Reset_PxObj();
	if (ImGui::Button("On_PhysX"))
		OnPhys_PxObj();

	ImGui::InputInt("Save_ID", &m_iSaveID);
	if (ImGui::Button("Save"))
		Save_Data(m_iSaveID);
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		Load_Data(m_iSaveID);


	ImGui::End();
}





void CPx_Tool::Save_Data(_int iLevelIndex)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "Phys_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/PhysObjData/");
	strcat_s(cPullName, cName);


	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return;


 
	_int iNumObj = (_int)m_pPxObjs.size();



	ofs.write((char*)&iLevelIndex, sizeof(int));		// ID
	ofs.write((char*)&iNumObj, sizeof(int));			// NumObj


	for (auto& PxObj : m_pPxObjs)
	{
		CRigid::RIGIDEDESC* pPxDesc = ((CRigid*)PxObj->Get_ComponentPtr(TEXT("Com_Rigid")))->Get_Desc();

		// Name
		char cTempName[MAX_PATH];
		CToolManager::Get_Instance()->TCtoC(PxObj->Get_Desc()->sModelName, cTempName);
		ofs.write((char*)&cTempName, sizeof(char)*MAX_PATH);


		ofs.write((char*)&pPxDesc->vStartPos, sizeof(_float3));
		ofs.write((char*)&pPxDesc->vRotation, sizeof(_float3));

	
		ofs.write((char*)&pPxDesc->vPos, sizeof(_float3));
		ofs.write((char*)&pPxDesc->vExtense, sizeof(_float3));

		ofs.write((char*)&pPxDesc->fWeight, sizeof(_float));
		ofs.write((char*)&pPxDesc->fAngleDump, sizeof(_float));
		ofs.write((char*)&pPxDesc->fStaticFriction, sizeof(_float));
		ofs.write((char*)&pPxDesc->fDynamicFriction, sizeof(_float));
		ofs.write((char*)&pPxDesc->fRestitution, sizeof(_float));
		ofs.write((char*)&pPxDesc->fMaxVel, sizeof(_float));
	}


	ofs.close();

	return;
}

void CPx_Tool::Load_Data(_int iLevelIndex)
{
	m_pPxObjs.clear();

	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "Phys_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/PhysObjData/");
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
		char sName[MAX_PATH];
		_float3 vPos, vRot;


		CPxObj::PXOBJDESC PxDesc;
		

		ifs.read((char*)sName, sizeof(char)*MAX_PATH);
		TM->CtoTC(sName, PxDesc.sModelName);
		ifs.read((char*)&PxDesc.RigidDesc.vStartPos, sizeof(_float3));
		ifs.read((char*)&PxDesc.RigidDesc.vRotation, sizeof(_float3));

		ifs.read((char*)&PxDesc.RigidDesc.vPos, sizeof(_float3));
		ifs.read((char*)&PxDesc.RigidDesc.vExtense, sizeof(_float3));

		ifs.read((char*)&PxDesc.RigidDesc.fWeight, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fAngleDump, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fStaticFriction, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fDynamicFriction, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fRestitution, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fMaxVel, sizeof(_float));


		Create_PxObj_Load(PxDesc);
	}


	return;
}





void CPx_Tool::Create_PxObj()
{
	CGameObject* pObj = nullptr;

	LEVEL eCulLevel = (LEVEL)GI->Get_Cur_LevelIndex();

	CPxObj::PXOBJDESC PxDesc;
	TM->CtoTC(m_sSelectedModel, PxDesc.sModelName);
	PxDesc.vScale = _float3(1.f, 1.f, 1.f);
	CRigid::RIGIDEDESC RigidDesc;

	XMStoreFloat3(&RigidDesc.vStartPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	RigidDesc.vRotation = _float3(0.f, 0.f, 0.f);
	RigidDesc.vPos = _float3(0.f, -.5f, 0.f);
	RigidDesc.vExtense = _float3(0.5f, 1.f, 0.5f);
	RigidDesc.fWeight = 10.f;
	RigidDesc.fStaticFriction = 2.f;
	RigidDesc.fDynamicFriction = 2.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;
	RigidDesc.fMaxVel = 0.f;
	PxDesc.RigidDesc = RigidDesc;
	GI->Add_Get_GameObjectToLayer(TEXT("PxObj"), eCulLevel, TEXT("Layer_PxTool"), &PxDesc, &pObj);

	CPxObj* pPxObj = (CPxObj*)pObj;
	pPxObj->Set_Static(true);
	m_pPxObjs.push_back(pPxObj);
}

void CPx_Tool::Create_PxObj_Load(CPxObj::PXOBJDESC PxDesc)
{
	CGameObject* pObj = nullptr;
	LEVEL eCulLevel = (LEVEL)GI->Get_Cur_LevelIndex();
	GI->Add_Get_GameObjectToLayer(TEXT("PxObj"), eCulLevel, TEXT("Layer_PxTool"), &PxDesc, &pObj);


	CPxObj* pPxObj = (CPxObj*)pObj;
	pPxObj->Set_Static(true);
	m_pPxObjs.push_back(pPxObj);
}

void CPx_Tool::Reset_PxObj()
{
	for (auto& pPxObj : m_pPxObjs)
	{
		((CRigid*)pPxObj->Get_ComponentPtr(TEXT("Com_Rigid")))->Edit_PosRot();
	}
}

void CPx_Tool::OnPhys_PxObj()
{
	for (auto& pPxObj : m_pPxObjs)
	{
		((CRigid*)pPxObj->Get_ComponentPtr(TEXT("Com_Rigid")))->Get_PxBody()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	}
}

void CPx_Tool::Edit_SameName()
{
	CPxObj* pSelectedPxObj = (CPxObj*)IG->Get_SelectedObj();
	if (!pSelectedPxObj)
		return;

	for (auto& pPxObj : m_pPxObjs)
	{
		if (!lstrcmp(pSelectedPxObj->Get_Desc()->sModelName, pPxObj->Get_Desc()->sModelName))
		{
			CRigid* pRigid = (CRigid*)pPxObj->Get_ComponentPtr(TEXT("Com_Rigid"));
			CRigid* pSelectedRigid = (CRigid*)pSelectedPxObj->Get_ComponentPtr(TEXT("Com_Rigid"));
			pRigid->Get_Desc()->vPos = pSelectedRigid->Get_Desc()->vPos;
			pRigid->Get_Desc()->vExtense = pSelectedRigid->Get_Desc()->vExtense;

			pRigid->Get_Desc()->fWeight = pSelectedRigid->Get_Desc()->fWeight;
			pRigid->Get_Desc()->fAngleDump = pSelectedRigid->Get_Desc()->fAngleDump;
			pRigid->Get_Desc()->fStaticFriction = pSelectedRigid->Get_Desc()->fStaticFriction;
			pRigid->Get_Desc()->fDynamicFriction = pSelectedRigid->Get_Desc()->fDynamicFriction;
			pRigid->Get_Desc()->fRestitution = pSelectedRigid->Get_Desc()->fRestitution;


			pRigid->Edit_Px();
		}
	}
}

void CPx_Tool::Delete_Obj(string sImGuiID)
{
	for (list<class CPxObj*>::iterator iter = m_pPxObjs.begin(); iter != m_pPxObjs.end();)
	{
		if ((*iter)->Get_ImGuiTag() == sImGuiID)
		{
			(*iter)->Set_Dead();
			m_pPxObjs.erase(iter);
			return;
		}
		else
			++iter;

	}
}





HRESULT CPx_Tool::Ready_Components()
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





CPx_Tool * CPx_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPx_Tool*		pInstance = new CPx_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPx_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPx_Tool::Clone(void * pArg)
{
	CPx_Tool*		pInstance = new CPx_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPx_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPx_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);


}
