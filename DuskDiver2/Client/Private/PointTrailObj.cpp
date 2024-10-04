#include "stdafx.h"
#include "..\Public\PointTrailObj.h"
#include "GameInstance.h"
#include "PointTrail.h"
#include "ToolManager.h"

CPointTrailObj::CPointTrailObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPointTrailObj::CPointTrailObj(const CPointTrailObj & rhs)
	: CGameObject(rhs)
{
}


HRESULT CPointTrailObj::Initialize(void * pArg)
{
	CPointTrail::TRAILCREATEINFO Info = (*(CPointTrail::TRAILCREATEINFO*)pArg);

	m_pTrail = (CPointTrail*)GI->Clone_Component(LEVEL_STATIC, L"PointTrail", &Info);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

void CPointTrailObj::Tick(_float fTimeDelta)
{
	m_pTrail->Tick(fTimeDelta);
}

void CPointTrailObj::LateTick(_float fTimeDelta)
{
	if (!m_pTrail->Get_TrailOn())
		return;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPointTrailObj::Render()
{
	m_pTrail->Render();
	return S_OK;
}

_bool CPointTrailObj::Get_TrailOn()
{
	return m_pTrail->Get_TrailOn();
}

void CPointTrailObj::TrailOn()
{
	m_pTrail->TrailOn();
}

void CPointTrailObj::TrailEnd()
{
	m_pTrail->Set_TrailOff();
}

void CPointTrailObj::TrailOff()
{
	m_pTrail->TrailOff();
}

void CPointTrailObj::Add_Point(_float4 vPos)
{
	m_pTrail->Add_Point(vPos);
}

void CPointTrailObj::Set_Color(_float4 Color)
{
	m_pTrail->Set_Color(Color);
}

void CPointTrailObj::Set_Color2(_float4 Color2)
{
	m_pTrail->Set_Color2(Color2);
}

void CPointTrailObj::Set_Ratio(_float2 vRatio)
{
	m_pTrail->Set_Ratio(vRatio);
}

void CPointTrailObj::Set_BlurPower(_float fBlurPower)
{
	m_pTrail->Set_BlurPower(fBlurPower);
}

void CPointTrailObj::Set_Size(_float fSize)
{
	m_pTrail->Set_Size(fSize);
}

void CPointTrailObj::Set_MaxIndex(_uint iMaxIndex)
{
	m_pTrail->Set_MaxIndex(iMaxIndex);
}

void CPointTrailObj::Set_TimeLimit(_float fTimeLimit)
{
	m_pTrail->Set_TimeLimit(fTimeLimit);
}

CPointTrailObj* CPointTrailObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPointTrailObj*		pInstance = new CPointTrailObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPointTrailObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPointTrailObj::Clone(void * pArg)
{
	CPointTrailObj*		pInstance = new CPointTrailObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPointTrailObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPointTrailObj::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrail);
}
