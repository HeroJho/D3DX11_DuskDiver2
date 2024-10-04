#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Component.h"



_uint CGameObject::m_iDebugTagCount = 0;

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CComponent * CGameObject::Get_ComponentPtr(const _tchar * pComponentTag)
{
	CComponent*		pComponent = Find_Component(pComponentTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{


	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::LateTick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	
	CComponent*			pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);	

	return S_OK;
}

HRESULT CGameObject::Compute_CamZ(_fvector vWorldPos)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_fvector		vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;

}


CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}



 // For. ImGui
void CGameObject::Make_ImGui()
{
	char sClassName[MAX_PATH];
	strcpy(sClassName, typeid(*this).name());
	string sTempStr = sClassName + string("_") + to_string(m_iDebugTagCount);
	m_sImGuiTag = sTempStr;

	++m_iDebugTagCount;
}



// For. Collider

CCollider * CGameObject::Get_Colliders(string sTag)
{
	for (auto& pCol : m_Colliders)
	{
		if (sTag == pCol->Get_Desc()->sTag)
			return pCol;
	}

	return nullptr;
}

void CGameObject::Tick_Col(_fmatrix TransformMatrix)
{
	for (auto& pCollider : m_Colliders)
		pCollider->Update(TransformMatrix);
}

void CGameObject::Render_Col(class CRenderer* pRenderer)
{
	for (auto& pCollider : m_Colliders)
	{
		if (nullptr != pCollider)
			pRenderer->Add_DebugGroup(pCollider);
	}
}

HRESULT CGameObject::AddCollider(CCollider::TYPE eType, CCollider::COLLIDERDESC Desc)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*			pCollider = nullptr;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		pCollider = (CCollider*)pGameInstance->Clone_Component(10, TEXT("Prototype_Component_Collider_AABB"), &Desc);
		break;
	case CCollider::TYPE_OBB:
		pCollider = (CCollider*)pGameInstance->Clone_Component(10, TEXT("Prototype_Component_Collider_OBB"), &Desc);
		break;
	case CCollider::TYPE_SPHERE:
		pCollider = (CCollider*)pGameInstance->Clone_Component(10, TEXT("Prototype_Component_Collider_Sphere"), &Desc);
		break;
	}

	if (nullptr == pCollider)
		return E_FAIL;

	m_Colliders.push_back(pCollider);
	m_ProColliders.emplace(pCollider->Get_Desc()->sTag, pCollider);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CGameObject::Choose_Col(const char * sName)
{
	if (sName)
	{
		auto	iter = find_if(m_ProColliders.begin(), m_ProColliders.end(), CTag_Finder_C(sName));

		if (iter == m_ProColliders.end())
			return E_FAIL;

		(*iter).second->Set_IsCurTrigger(true);
	}
	else
	{
		for (auto& pCol : m_Colliders)
			pCol->Set_IsCurTrigger(true);
	}

	return S_OK;
}

void CGameObject::Emp_Col()
{
	for (auto& pCol : m_Colliders)
		pCol->Set_IsCurTrigger(false);
}



void CGameObject::Fix_Timer(_float fRatio, _float fTimeLimit)
{
	if (m_fTMRatio < fRatio)
		return;

	m_fTMRatio = fRatio;
	m_fTMLimit = fTimeLimit;
	m_fTMAcc = 0.f;
	m_bFixTimer = true;
}

void CGameObject::Tick_Timer(_float fTimeDelta)
{
	if (m_bFixTimer)
	{
		m_fTMAcc += 1.f * fTimeDelta;
		if (m_fTMAcc >= m_fTMLimit)
		{
			m_bFixTimer = false;
			m_fTMRatio = 1.f;
			m_fTMAcc = 0.f;
		}
	}
}

void CGameObject::Reset_Timer()
{
	m_bFixTimer = false;
	m_fTMRatio = 1.f;
	m_fTMAcc = 0.f;
}



void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	for (auto& pCollider : m_Colliders)
	{
		Safe_Release(pCollider);
	}


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
