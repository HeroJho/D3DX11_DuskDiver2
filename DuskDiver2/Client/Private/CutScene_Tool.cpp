#include "stdafx.h"
#include "..\Public\CutScene_Tool.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "ImGui_Manager.h"
#include "ModelObj_Instance.h"

#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube_HR.h"
#include "RenderCube.h"
#include "CamSelectingCube.h"
#include "Camera_Manager.h"



CCutScene_Tool::CCutScene_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCutScene_Tool::CCutScene_Tool(const CCutScene_Tool & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCutScene_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCutScene_Tool::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_AnimMap_Tool";

	Make_ImGui();

	CRM->Set_CutSceneTool(this);

	CRM->Get_CutSceneTool()->Create_SelectingCube();

	m_fMarkMoveSens = 0.1f;
	m_fLookMoveSens = 0.1f;

	return S_OK;
}

void CCutScene_Tool::Tick(_float fTimeDelta)
{
	if (TK_DOWN(DIK_C))
	{
		Create_MarkCube();
	}
	else if (TK_DOWN(DIK_V))
	{
		Create_LookCube();
	}

	PlayCutScene(fTimeDelta);
}

void CCutScene_Tool::LateTick(_float fTimeDelta)
{

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Render_Col(m_pRendererCom);

}

HRESULT CCutScene_Tool::Render()
{


	return S_OK;
}





HRESULT CCutScene_Tool::Ready_Components()
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





CCutScene_Tool * CCutScene_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCutScene_Tool*		pInstance = new CCutScene_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCutScene_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCutScene_Tool::Clone(void * pArg)
{
	CCutScene_Tool*		pInstance = new CCutScene_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCutScene_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCutScene_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pSelectingCube);

	for (auto& pCube : m_MarkCubes)
		Safe_Release(pCube);
	m_MarkCubes.clear();

	for (auto& pCube : m_LookCubes)
		Safe_Release(pCube);
	m_LookCubes.clear();

	m_PlayPosTemp.clear();


}







_float3 CCutScene_Tool::Get_SelectingCubePos()
{
	return m_pSelectingCube->Get_Pos();
}

void CCutScene_Tool::Set_SelectingCube(_float3 vPos)
{
	_vector vVec = XMLoadFloat3(&vPos);
	vVec = XMVectorSetW(vVec, 1.f);
	m_pSelectingCube->Set_Pos(vVec);
}



CGameObject * CCutScene_Tool::Get_SelectedCube()
{
	for (auto& pCube : m_MarkCubes)
	{
		if (m_sSelectedMarkCubeTag == pCube->Get_Tag())
		{
			return pCube;
		}

	}

	for (auto& pCube : m_LookCubes)
	{
		if (m_sSelectedMarkCubeTag == pCube->Get_Tag())
		{
			return pCube;
		}
	}

	return nullptr;
}

void CCutScene_Tool::Set_SelectingCubePosToSelectedCube()
{
	CGameObject* pObj = Get_SelectedCube();
	if (nullptr == pObj)
		return;

	CTransform* pTran = (CTransform*)pObj->Get_ComponentPtr(TEXT("Com_Transform"));

	_vector vVec = pTran->Get_State(CTransform::STATE_POSITION);
	_float3 vPos;
	XMStoreFloat3(&vPos, vVec);
	Set_SelectingCube(vPos);
}

void CCutScene_Tool::Set_SelectingCubePosToCamPos()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vPos = *(_float3*)&pGameInstance->Get_CamPosition();
	Set_SelectingCube(vPos);

	RELEASE_INSTANCE(CGameInstance);
}



void CCutScene_Tool::Create_SelectingCube()
{
	CCamSelectingCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CCamSelectingCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);

	CGameObject* pTempObj = nullptr;

	
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_CamSelectingCube"),	LEVEL_STATIC, TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_pSelectingCube = (CCamSelectingCube*)pTempObj;
	Safe_AddRef(m_pSelectingCube);

}

void CCutScene_Tool::Create_MarkCube(CDataManager::CAMDATA* pData)
{
	CMarkCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CMarkCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 0.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = Get_SelectingCubePos();
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_MarkCube"), TM->Get_CurLevel(), TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_MarkCubes.push_back((CMarkCube*)pTempObj);

	string MarkTag = "MarkCube_" + std::to_string(m_iTagIndex);
	pTempObj->Set_Tag(MarkTag);
	m_iTagIndex++;

	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CMarkCube*)pTempObj)->Set_SaveDATA(pData);
	}


	// 포스 계산
	MakeRenderPos();
}

void CCutScene_Tool::Create_LookCube(CDataManager::CAMDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLookCube_HR::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CLookCube_HR::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 1.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = Get_SelectingCubePos();
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;
	
	if (FAILED(pGameInstance->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_LookCube"), TM->Get_CurLevel(), TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_LookCubes.push_back((CLookCube_HR*)pTempObj);

	string LookTag = "LookCube_" + std::to_string(m_iTagIndex);
	pTempObj->Set_Tag(LookTag);
	m_iTagIndex++;

	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CLookCube_HR*)pTempObj)->Set_SaveDATA(pData);
	}

	Safe_Release(pGameInstance);

	MakeRenderPos();
}

void CCutScene_Tool::Create_RenderPosCube(_float3 vPos, _bool vMark)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMarkCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CMarkCube::COLORCUBEDESC));

	if (vMark)
		ColorCubeDesc.vColor = _float4(1.f, 1.f, 0.f, 1.f);
	else
		ColorCubeDesc.vColor = _float4(0.f, 1.f, 1.f, 1.f);

	ColorCubeDesc.vPos = vPos;
	ColorCubeDesc.vScale = _float3(.5f, .5f, .5f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_RenderCube"), TM->Get_CurLevel(), TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_RenderCubes.push_back((CRenderCube*)pTempObj);

	Safe_Release(pGameInstance);
}

void CCutScene_Tool::Create_ChaseLookCube()
{
	if (nullptr != m_pChaseLookCube)
	{
		m_pChaseLookCube->Set_Dead();
		m_pChaseLookCube = nullptr;
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CColorCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CColorCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.5f, 0.5f, 0.5f, 1.f);
	ColorCubeDesc.vPos = _float3(0.f, 0.f, 0.f);
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_ColorCube"), TM->Get_CurLevel(), TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_pChaseLookCube = (CColorCube*)pTempObj;

	Safe_Release(pGameInstance);
}


void CCutScene_Tool::Delete_MarkCube()
{
	for (list<CMarkCube*>::iterator iter = m_MarkCubes.begin(); iter != m_MarkCubes.end();)
	{

		if (m_sSelectedMarkCubeTag == (*iter)->Get_Tag())
		{
			(*iter)->Set_Dead();
			Safe_Release(*iter);
			iter = m_MarkCubes.erase(iter);
		}
		else
			++iter;

	}

	MakeRenderPos();
}

void CCutScene_Tool::Delete_LookCube()
{
	for (list<CLookCube_HR*>::iterator iter = m_LookCubes.begin(); iter != m_LookCubes.end();)
	{

		if (m_sSelectedMarkCubeTag == (*iter)->Get_Tag())
		{
			(*iter)->Set_Dead();
			Safe_Release(*iter);
			iter = m_LookCubes.erase(iter);
		}
		else
			++iter;
	}

	MakeRenderPos();
}




void CCutScene_Tool::Set_Start(_bool bStart)
{
	m_bStart = bStart;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;
	m_TempMarkCubes.clear();
	m_pTempBasiMarkCubes.clear();

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;
	m_TempLookCubes.clear();
	m_pTempBasiLookCubes.clear();

	if (bStart)
	{
		m_TempMarkCubes = m_MarkCubes;
		MakeMarkPos();
		m_vMarkTempPos = CalculMarkBasi();
		CRM->Get_Cam()->Set_Pos(m_vMarkTempPos);


		Create_ChaseLookCube();
		m_TempLookCubes = m_LookCubes;
		MakeLookPos();
		m_vLookTempPos = CalculLookBasi();
		m_pChaseLookCube->Set_Pos(m_vLookTempPos);
	}

	CRM->Get_Cam()->Set_CutScene(bStart);

}
void CCutScene_Tool::PlayCutScene(_float fTimeDelta)
{
	if (!m_bStart)
		return;

	PlayMark(fTimeDelta);
	PlayLook(fTimeDelta);

}

void CCutScene_Tool::PlayMark(_float fTimeDelta)
{

	// 도착 했냐
	if (1.f < m_fMarkT)
	{
		if (1 > m_TempMarkCubes.size())
		{
			m_bStart = false;
			return;
		}


		// 고른다. 
		m_pTempBasiMarkCubes.clear();
		MakeMarkPos();

		m_fMarkT = 0.f;
		m_fMarkTimeAcc = 0.f;

		m_vMarkTempPos = CalculMarkBasi();

		CRM->Get_Cam()->Set_Pos(m_vMarkTempPos);

		return;
	}




	_vector vTempPos = XMLoadFloat3(&m_vMarkTempPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);

	if (1 == m_pTempBasiMarkCubes.size())
	{
		m_fMarkTimeAcc += fTimeDelta;
		if (m_fMarkSpeed < m_fMarkTimeAcc)
		{
			m_fMarkT = 2.f;
			m_fMarkTimeAcc = 0.f;
		}

	}
	else
	{
		if (CRM->Get_Cam()->Move(vTempPos, m_fMarkSpeed, fTimeDelta, m_fMarkDestDis, true))
		{
			m_fMarkT += m_fMarkMoveSens;
			m_vMarkTempPos = CalculMarkBasi();
		}
	}

	_vector vPos = XMLoadFloat3(&m_pChaseLookCube->Get_Pos());
	vPos = XMVectorSetW(vPos, 1.f);
	CRM->Get_Cam()->LookAt(vPos);


}
void CCutScene_Tool::PlayLook(_float fTimeDelta)
{
	// 도착 했냐
	if (1.f < m_fLookT)
	{
		if (1 > m_TempLookCubes.size())
		{
			// m_bStart = false;
			return;
		}


		// 고른다. 
		m_pTempBasiLookCubes.clear();
		MakeLookPos();

		m_fLookT = 0.f;
		m_fLookTimeAcc = 0.f;

		m_vLookTempPos = CalculLookBasi();
		m_pChaseLookCube->Set_Pos(m_vLookTempPos);

		return;
	}




	_vector vTempPos = XMLoadFloat3(&m_vLookTempPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);

	if (1 == m_pTempBasiLookCubes.size())
	{
		m_fLookTimeAcc += fTimeDelta;
		if (m_fLookSpeed < m_fLookTimeAcc)
		{
			m_fLookT = 2.f;
			m_fLookTimeAcc = 0.f;
		}

	}
	else
	{
		if (m_pChaseLookCube->Move(vTempPos, m_fLookSpeed, fTimeDelta, m_fLookDestDis, true))
		{
			m_fLookT += m_fLookMoveSens;
			m_vLookTempPos = CalculLookBasi();
		}
	}
}


void CCutScene_Tool::MakeMarkPos()
{
	if (1 > m_TempMarkCubes.size())
		return;


	list<CMarkCube*>::iterator Cur_iter = m_TempMarkCubes.begin();
	list<CMarkCube*>::iterator Fro_iter = ++(m_TempMarkCubes.begin());

	for (; Cur_iter != m_TempMarkCubes.end(); ++Cur_iter)
	{

		if (1 == m_TempMarkCubes.size())
		{
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_TempMarkCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
		}
		else
		{
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
			break;
		}
	}


	for (_int i = 0; i < m_pTempBasiMarkCubes.size(); ++i)
		m_TempMarkCubes.pop_front();

	// 스피드를 설정한다.
	if (0 < m_pTempBasiMarkCubes.size())
	{
		CMarkCube* pTempCube = m_pTempBasiMarkCubes.front();
		m_fMarkSpeed = pTempCube->Get_SpeedTime();
		m_fMarkMoveSens= pTempCube->Get_SensTime();
		m_fMarkDestDis = pTempCube->Get_DestDis();
	}
}
void CCutScene_Tool::MakeLookPos()
{
	if (1 > m_TempLookCubes.size())
		return;


	list<CLookCube_HR*>::iterator Cur_iter = m_TempLookCubes.begin();
	list<CLookCube_HR*>::iterator Fro_iter = ++(m_TempLookCubes.begin());

	for (; Cur_iter != m_TempLookCubes.end(); ++Cur_iter)
	{

		if (1 == m_TempLookCubes.size())
		{
			m_pTempBasiLookCubes.push_back(*Cur_iter);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_TempLookCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			m_pTempBasiLookCubes.push_back(*Cur_iter);
		}
		else
		{
			m_pTempBasiLookCubes.push_back(*Cur_iter);
			break;
		}
	}


	for (_int i = 0; i < m_pTempBasiLookCubes.size(); ++i)
		m_TempLookCubes.pop_front();

	// 스피드를 설정한다.
	if (0 < m_pTempBasiLookCubes.size())
	{
		CLookCube_HR* pTempCube = m_pTempBasiLookCubes.front();
		m_fLookSpeed = pTempCube->Get_SpeedTime();
		m_fLookMoveSens = pTempCube->Get_SensTime();
		m_fLookDestDis = pTempCube->Get_DestDis();
	}
}
_float3 CCutScene_Tool::CalculMarkBasi()
{
	_int iSize = (_int)m_pTempBasiMarkCubes.size();

	_float3 vPos1, vPos2, vPos3, vPos4;

	list<CMarkCube*>::iterator iter = m_pTempBasiMarkCubes.begin();

	switch (iSize)
	{
	case 1:
		vPos1 = (*iter)->Get_Pos();
		return vPos1;
		break;
	case 2:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, m_fMarkT);
	case 3:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, m_fMarkT);
	case 4:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		vPos4 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, vPos4, m_fMarkT);
	}

	return _float3();
}

_float3 CCutScene_Tool::CalculLookBasi()
{
	_int iSize = (_int)m_pTempBasiLookCubes.size();

	_float3 vPos1, vPos2, vPos3, vPos4;

	list<CLookCube_HR*>::iterator iter = m_pTempBasiLookCubes.begin();

	switch (iSize)
	{
	case 1:
		vPos1 = (*iter)->Get_Pos();
		return vPos1;
		break;
	case 2:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, m_fLookT);
	case 3:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, m_fLookT);
	case 4:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		vPos4 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, vPos4, m_fLookT);
	}

	return _float3();
}







void CCutScene_Tool::MakeRenderPos()
{
	m_PlayPosTemp.clear();
	MakeMarkRenderPos();
	MakeLookRenderPos();
}

void CCutScene_Tool::MakeMarkRenderPos()
{
	if (1 > m_MarkCubes.size())
		return;

	list<CAMDATA> TempPoss;

	list<CMarkCube*>::iterator Cur_iter = m_MarkCubes.begin();
	list<CMarkCube*>::iterator Fro_iter = ++(m_MarkCubes.begin());

	for (; Cur_iter != m_MarkCubes.end(); ++Cur_iter)
	{
		if (1 == m_MarkCubes.size())
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = true;

			TempPoss.push_back(CamData);

			// 포스 계산

			CalculRenderBasi(TempPoss);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_MarkCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.fSens = (*Cur_iter)->Get_SensTime();
			CamData.bMark = true;

			TempPoss.push_back(CamData);
		}
		else
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.fSens = (*Cur_iter)->Get_SensTime();
			CamData.bMark = true;

			TempPoss.push_back(CamData);

			// 포스 계산
			CalculRenderBasi(TempPoss);

			TempPoss.clear();
		}
	}


	RenderPos(1);
}
void CCutScene_Tool::MakeLookRenderPos()
{
	if (1 > m_LookCubes.size())
		return;

	list<CAMDATA> TempPoss;

	list<CLookCube_HR*>::iterator Cur_iter = m_LookCubes.begin();
	list<CLookCube_HR*>::iterator Fro_iter = ++(m_LookCubes.begin());

	for (; Cur_iter != m_LookCubes.end(); ++Cur_iter)
	{
		if (1 == m_LookCubes.size())
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.bMark = false;

			TempPoss.push_back(CamData);

			// 포스 계산

			CalculRenderBasi(TempPoss);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_LookCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.fSens = (*Cur_iter)->Get_SensTime();
			CamData.bMark = false;

			TempPoss.push_back(CamData);
		}
		else
		{
			CAMDATA CamData;
			ZeroMemory(&CamData, sizeof(CAMDATA));
			CamData.vPos = (*Cur_iter)->Get_Pos();
			CamData.fSpeedAndStopTime = (*Cur_iter)->Get_SpeedTime();
			CamData.fSens = (*Cur_iter)->Get_SensTime();
			CamData.bMark = false;

			TempPoss.push_back(CamData);

			// 포스 계산
			CalculRenderBasi(TempPoss);

			TempPoss.clear();
		}
	}


	RenderPos(1);
}

void CCutScene_Tool::RenderPos(_int iSens)
{
	for (auto& pCube : m_RenderCubes)
		pCube->Set_Dead();
	m_RenderCubes.clear();


	_int iCount = 0;
	_float fCutDis = 0.f;
	_float3 vPrePos;
	ZeroMemory(&vPrePos, sizeof(_float3));
	for (auto& Data : m_PlayPosTemp)
	{
		if (iCount % iSens == 0)
		{
			_vector DisVec = XMLoadFloat3(&vPrePos) - XMLoadFloat3(&Data.vPos);
			fCutDis = XMVectorGetX(XMVector3Length(DisVec));

			if (0.1f < fCutDis)
			{
				Create_RenderPosCube(Data.vPos, Data.bMark);
				vPrePos = Data.vPos;
			}

		}

		++iCount;
	}

}

void CCutScene_Tool::CalculRenderBasi(list<CAMDATA> Poss)
{
	_int iSize = (_int)Poss.size();

	CAMDATA vPos1, vPos2, vPos3, vPos4;

	switch (iSize)
	{
	case 1:
		vPos1 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1);
		break;
	case 2:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1, vPos2);
		break;
	case 3:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		vPos3 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1, vPos2, vPos3);
		break;
	case 4:
		vPos1 = Poss.front(); Poss.pop_front();
		vPos2 = Poss.front(); Poss.pop_front();
		vPos3 = Poss.front(); Poss.pop_front();
		vPos4 = Poss.front(); Poss.pop_front();
		GetBesierRenderPos(vPos1, vPos2, vPos3, vPos4);
		break;
	default:
		break;
	}

}
void CCutScene_Tool::GetBesierRenderPos(CAMDATA vPos1)
{

}
void CCutScene_Tool::GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2)
{
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	if (0.f == vPos1.fSens)
		vPos1.fSens = 0.01f;
	_float fT = 0.f;
	while (1.f >= fT)
	{
		CamData.vPos = CToolManager::Get_Instance()->GetBesierPos(vPos1.vPos, vPos2.vPos, fT);
		CamData.bMark = vPos1.bMark;

		m_PlayPosTemp.push_back(CamData);
		fT += vPos1.fSens;
	}
}
void CCutScene_Tool::GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3)
{
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	if (0.f == vPos1.fSens)
		vPos1.fSens = 0.01f;
	_float fT = 0.f;
	while (1.f >= fT)
	{
		CamData.vPos = CToolManager::Get_Instance()->GetBesierPos(vPos1.vPos, vPos2.vPos, vPos3.vPos, fT);
		CamData.bMark = vPos1.bMark;

		m_PlayPosTemp.push_back(CamData);
		fT += vPos1.fSens;
	}
}
void CCutScene_Tool::GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3, CAMDATA vPos4)
{
	CAMDATA CamData;
	ZeroMemory(&CamData, sizeof(CAMDATA));

	if (0.f == vPos1.fSens)
		vPos1.fSens = 0.01f;
	_float fT = 0.f;
	while (1.f >= fT)
	{
		CamData.vPos = CToolManager::Get_Instance()->GetBesierPos(vPos1.vPos, vPos2.vPos, vPos3.vPos, vPos4.vPos, fT);
		CamData.bMark = vPos1.bMark;

		m_PlayPosTemp.push_back(CamData);
		fT += vPos1.fSens;
	}
}




void CCutScene_Tool::Save_Data()
{
	CDataManager::Get_Instance()->Save_Cam(m_iSaveIndex);
}

void CCutScene_Tool::Load_Data()
{

	CDataManager::DATA_CAMS* Data = CDataManager::Get_Instance()->Load_Cam(m_iSaveIndex);

	Clear_Data();

	if (nullptr == Data)
		return;

	for (_uint i = 0; i < Data->iPosNum; ++i)
	{
		Create_MarkCube(&Data->pPosDatas[i]);
	}

	for (_uint i = 0; i < Data->iLookNum; ++i)
	{
		Create_LookCube(&Data->pLookDatas[i]);
	}

	Safe_Delete_Array(Data->pLookDatas);
	Safe_Delete_Array(Data->pPosDatas);
	Safe_Delete(Data);

}





void CCutScene_Tool::Clear_PosData()
{
	for (auto& pCube : m_MarkCubes)
		pCube->Set_Dead();

	m_MarkCubes.clear();
	m_TempMarkCubes.clear();
	m_pTempBasiMarkCubes.clear();


	m_bStart = false;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;

}

void CCutScene_Tool::Clear_LookData()
{
	for (auto& pCube : m_LookCubes)
		pCube->Set_Dead();

	m_LookCubes.clear();
	m_TempLookCubes.clear();
	m_pTempBasiLookCubes.clear();


	m_bStart = false;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;
}
void CCutScene_Tool::Clear_Data()
{
	Clear_PosData();
	Clear_LookData();
}





void CCutScene_Tool::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

#pragma region 중략

	const list<CMarkCube*>* pMarkCubes = CRM->Get_CutSceneTool()->Get_MarkCubes();
	const list<CLookCube_HR*>* pLookCubes = CRM->Get_CutSceneTool()->Get_LookCubes();


	if (ImGui::BeginListBox("MarkCubeList"))
	{

		for (auto& pMarkCube : *pMarkCubes)
		{
			bool isSelected = CRM->Get_CutSceneTool()->Get_SelectedMarkCubeTag() == pMarkCube->Get_Tag();
			ImVec2 vSize{ 80.f, 10.f };
			if (ImGui::Selectable(pMarkCube->Get_Tag().c_str(), isSelected, 0, vSize))
				CRM->Get_CutSceneTool()->Set_SelectedMarkCubeTag(pMarkCube->Get_Tag());


			ImGui::SameLine();


			string sButtonTag = "Index: " + pMarkCube->Get_Tag();

			ImGui::PushItemWidth(100.f);
			_int iTemp = pMarkCube->Get_LinkIndex();


			if (ImGui::InputInt(sButtonTag.c_str(), &iTemp))
			{
				pMarkCube->Set_LinkIndex(iTemp);
				CRM->Get_CutSceneTool()->MakeRenderPos();
			}



			string sTimeTag = "Time: " + pMarkCube->Get_Tag();

			_float fTemp = pMarkCube->Get_SpeedTime();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pMarkCube->Set_SpeedTime(fTemp);

				CRM->Get_CutSceneTool()->MakeRenderPos();
			}

			sTimeTag = "Sence: " + pMarkCube->Get_Tag();
			fTemp = pMarkCube->Get_SensTime();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pMarkCube->Set_SensTime(fTemp);

				CRM->Get_CutSceneTool()->MakeRenderPos();
			}

			sTimeTag = "DestDis: " + pMarkCube->Get_Tag();
			fTemp = pMarkCube->Get_DestDis();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pMarkCube->Set_DestDis(fTemp);

				CRM->Get_CutSceneTool()->MakeRenderPos();
			}




			ImGui::PopItemWidth();


			//if (0 > pMarkCube->Get_LinkIndex() || 3 < pMarkCube->Get_LinkIndex())
			//{
			//	pMarkCube->Set_LinkIndex(0);
			//	Set_WarningBox("Index range must be 0-3 !!");
			//}


			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete_MarkCube"))
		CRM->Get_CutSceneTool()->Delete_MarkCube();

	if(Get_SelectedCube())
		IG->Debug_Transform((CTransform*)Get_SelectedCube()->Get_ComponentPtr(TEXT("Com_Transform")));



	if (ImGui::BeginListBox("LookCubeList"))
	{

		for (auto& pLookCube : *pLookCubes)
		{
			bool isSelected = CRM->Get_CutSceneTool()->Get_SelectedMarkCubeTag() == pLookCube->Get_Tag();
			ImVec2 vSize{ 80.f, 10.f };
			if (ImGui::Selectable(pLookCube->Get_Tag().c_str(), isSelected, 0, vSize))
				CRM->Get_CutSceneTool()->Set_SelectedMarkCubeTag(pLookCube->Get_Tag());


			ImGui::SameLine();


			string sButtonTag = "Index: " + pLookCube->Get_Tag();

			ImGui::PushItemWidth(100.f);
			_int iTemp = pLookCube->Get_LinkIndex();


			if (ImGui::InputInt(sButtonTag.c_str(), &iTemp))
			{
				pLookCube->Set_LinkIndex(iTemp);
				CRM->Get_CutSceneTool()->MakeRenderPos();
			}




			// ImGui::SameLine();
			string sTimeTag = "Time: " + pLookCube->Get_Tag();

			_float fTemp = pLookCube->Get_SpeedTime();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pLookCube->Set_SpeedTime(fTemp);

				CRM->Get_CutSceneTool()->MakeRenderPos();
			}

			sTimeTag = "Sence: " + pLookCube->Get_Tag();
			fTemp = pLookCube->Get_SensTime();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pLookCube->Set_SensTime(fTemp);

				CRM->Get_CutSceneTool()->MakeRenderPos();
			}

			sTimeTag = "DestDis: " + pLookCube->Get_Tag();
			fTemp = pLookCube->Get_DestDis();
			if (ImGui::InputFloat(sTimeTag.c_str(), &fTemp))
			{
				pLookCube->Set_DestDis(fTemp);

				CRM->Get_CutSceneTool()->MakeRenderPos();
			}


			ImGui::PopItemWidth();


			//if (0 > pMarkCube->Get_LinkIndex() || 3 < pMarkCube->Get_LinkIndex())
			//{
			//	pMarkCube->Set_LinkIndex(0);
			//	Set_WarningBox("Index range must be 0-3 !!");
			//}


			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete_LookCube"))
		CRM->Get_CutSceneTool()->Delete_LookCube();


	if (ImGui::Button("ReLoad"))
		CRM->Load_Data();
















	if (CRM->Get_CutSceneTool()->Get_Start())
	{
		if (ImGui::ColorButton("Stop", ImVec4{ 255.f, 0.f, 0.f, 255.f }))
			CRM->Get_CutSceneTool()->Set_Start(false);
	}
	else
	{
		if (ImGui::Button("Play"))
			CRM->Get_CutSceneTool()->Set_Start(true);
	}

	ImGui::SameLine();

	if (ImGui::Button("SetPos"))
		CRM->Get_CutSceneTool()->Set_SelectingCubePosToSelectedCube();
	ImGui::SameLine();
	if (ImGui::Button("SetCamPos"))
		CRM->Get_CutSceneTool()->Set_SelectingCubePosToCamPos();

	ImGui::SameLine();

	_bool bShowCube = CRM->Get_CutSceneTool()->Get_ShowCube();
	if (ImGui::Checkbox("ShowCube", &bShowCube))
		CRM->Get_CutSceneTool()->Set_ShowCube(bShowCube);

	ImGui::DragFloat("CubeSize", &m_fCubeSize);
	if (0.01f > m_fCubeSize)
		m_fCubeSize = 0.01f;

	if (ImGui::Button("ClearMarkCube"))
		CRM->Get_CutSceneTool()->Clear_PosData();
	ImGui::SameLine();
	if (ImGui::Button("ClearLookCube"))
		CRM->Get_CutSceneTool()->Clear_LookData();

	_int iSaveIndex = CRM->Get_CutSceneTool()->Get_Index();
	if (ImGui::InputInt("Save Index", &iSaveIndex))
		CRM->Get_CutSceneTool()->Set_Index(iSaveIndex);


	if (ImGui::Button("Save"))
		CRM->Get_CutSceneTool()->Save_Data();
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		CRM->Get_CutSceneTool()->Load_Data();



#pragma endregion

	ImGui::End();
}
