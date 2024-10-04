#include "stdafx.h"
#include "..\Public\ActiveQuest.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player_Manager.h"
#include "QuestDist.h"

CActiveQuest::CActiveQuest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CActiveQuest::CActiveQuest(const CActiveQuest & rhs)
	:CUI(rhs)
{
}

HRESULT CActiveQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActiveQuest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	UM->BackUp_ActiveQuest(this);
	Set_QuestPoint({ 11.5f, 1.4f, 25.3f }, 0);

	InitDist();

	return S_OK;
}

void CActiveQuest::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	// 퀘스트의 위치를 다른 함수로 지정해준다.

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float4x4 TargetViewMatrix;
	XMStoreFloat4x4(&TargetViewMatrix,
		(XMLoadFloat4x4(&m_QuestPoint) * pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW)) * pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	_vector ScreenPos = XMVector3Project(
		XMVectorSet(0.f, 0.f, 0.f, 1.f),
		0,
		0,
		1280.f,
		720.f,
		0.f,
		1.f,
		pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ),
		pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW),
		XMLoadFloat4x4(&m_QuestPoint)
	);

	_float x = XMVectorGetX(ScreenPos);
	_float y = XMVectorGetY(ScreenPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((x - g_iWinSizeX * 0.5f), (-y + g_iWinSizeY * 0.5f), 0.0f, 1.f));

	Check_PlayerPos();

	if (false == m_bHideDistance)
	{
		Setting_Distance(x, y, fTimeDelta);
	}
}

void CActiveQuest::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if (!UM->PartyOn_State() && !UM->Get_QuestOn() && UM->Get_FrontUI())
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLDUI, this);

		if ((false == m_bHideDistance) && (false == m_bBehind) && (true == m_bRender))
		{
			for (auto& iter : m_QuestDistsVec)
				iter->LateTick(fTimeDelta);
		}

	}
		
}

HRESULT CActiveQuest::Render()
{
	if ((true == m_bRender) && (false == m_bBehind))
	{
		if (nullptr == m_pVIBufferCom ||
			nullptr == m_pShaderCom)
			return E_FAIL;


		m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iIconIndex)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;


		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (false == m_bHideDistance)
		{
			for (auto& iter : m_QuestDistsVec)
				iter->Render();
		}
	}
	//UI_RenderDebug();

	return S_OK;
}

void CActiveQuest::Render_Debug()
{
}

void CActiveQuest::Set_QuestPoint(_float3 vPos, _uint iQuestIconIndex)
{
	m_bRender = true;
	m_iIconIndex = iQuestIconIndex;

	m_QuestPoint._11 = 1;
	m_QuestPoint._12 = 0;
	m_QuestPoint._13 = 0;
	m_QuestPoint._14 = 0;

	m_QuestPoint._21 = 0;
	m_QuestPoint._22 = 1;
	m_QuestPoint._23 = 0;
	m_QuestPoint._24 = 0;

	m_QuestPoint._31 = 0;
	m_QuestPoint._32 = 0;
	m_QuestPoint._33 = 1;
	m_QuestPoint._34 = 0;

	m_QuestPoint._41 = vPos.x;
	m_QuestPoint._42 = vPos.y;
	m_QuestPoint._43 = vPos.z;
	m_QuestPoint._44 = 1;

	m_Pos = vPos;
}

HRESULT CActiveQuest::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ActiveQuest"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CActiveQuest::InitDist()
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 20.f;
	UiInfoDesc.fSizeY = 20.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	for (_uint i = 0; 4 > i; ++i)
	{
		CGameObject* pGameObject = nullptr;
		pGameObject = GI->Clone_GameObject(TEXT("Prototype_GameObject_QuestDist"), &UiInfoDesc);

		m_QuestDistsVec.push_back((CQuestDist*)pGameObject);
	}
	
}

void CActiveQuest::Check_PlayerPos()
{
	_float3 vCamLook3 = GI->Get_CamDir(CPipeLine::DIR_LOOK);
	_vector vWorldUp = { 0.f,1.f,0.f };
	_vector vCamRight = XMVector3Cross(vWorldUp, XMLoadFloat3(&vCamLook3));
	XMStoreFloat3(&vCamLook3, XMVector3Cross(vCamRight, vWorldUp));

	_float4 vPlayerPos = PM->Get_PlayerPos();
	_float4 vDir = { 0.f,0.f,0.f,1.f };
	_float4 vQuestPos = { 0.f,0.f,0.f,1.f };
	vPlayerPos.y = 0.f;
	vQuestPos.x = m_QuestPoint._41;
	vQuestPos.y = 0.f;
	vQuestPos.z = m_QuestPoint._43;

	XMStoreFloat4(&vDir, XMLoadFloat4(&vQuestPos) - XMLoadFloat4(&vPlayerPos));
	vDir.w = 0.f;
	m_fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vDir)));
	if (999.f < m_fDistance)
		m_bHideDistance = true;
	else
		m_bHideDistance = false;

	_float fCos = vCamLook3.x * vDir.x + vCamLook3.y * vDir.y + vCamLook3.z * vDir.z;

	if (0.f > fCos)
	{
		m_bBehind = true;
	}
	else
	{
		m_bBehind = false;
	}
		
}

void CActiveQuest::Setting_Distance(_float x, _float y, _float fTimeDelta)
{
	_uint iHun = (_uint)(m_fDistance * 0.01f) % 10;
	_uint iTen = (_uint)(m_fDistance * 0.1f) % 10;
	_uint iOne = (_uint)(m_fDistance) % 10;

	if (0 == iHun)
	{
		if (0 != iTen)
		{
			m_QuestDistsVec[1]->Set_DistPos(_float2(x - 18.f, y + 40.f), iTen);
			m_QuestDistsVec[2]->Set_DistPos(_float2(x - 3.f, y + 40.f), iOne);
			m_QuestDistsVec[3]->Set_DistPos(_float2(x + 13.f, y + 40.f), 10);
		}
		else if (0 == iTen)
		{
			m_QuestDistsVec[2]->Set_DistPos(_float2(x - 10.5f, y + 40.f), iOne);
			m_QuestDistsVec[3]->Set_DistPos(_float2(x + 6.f, y + 40.f), 10);
			m_QuestDistsVec[1]->Set_DistPosRender(false);
		}
		m_QuestDistsVec[0]->Set_DistPosRender(false);
	}
	else
	{
		m_QuestDistsVec[0]->Set_DistPos(_float2(x - 20.f, y + 40.f), iHun);
		m_QuestDistsVec[1]->Set_DistPos(_float2(x - 11.f, y + 40.f), iTen);
		m_QuestDistsVec[2]->Set_DistPos(_float2(x + 0.f, y + 40.f), iOne);
		m_QuestDistsVec[3]->Set_DistPos(_float2(x + 13.f, y + 40.f), 10);
	}
	

	for (auto& iter : m_QuestDistsVec)
		iter->Tick(fTimeDelta);
}


CActiveQuest * CActiveQuest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CActiveQuest*		pInstance = new CActiveQuest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActiveQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CActiveQuest::Clone(void * pArg)
{
	CActiveQuest*		pInstance = new CActiveQuest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActiveQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActiveQuest::Free()
{
	__super::Free();

	for (auto& iter : m_QuestDistsVec)
		Safe_Release(iter);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
