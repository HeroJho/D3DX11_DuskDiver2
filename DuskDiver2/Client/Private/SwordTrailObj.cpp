#include "stdafx.h"
#include "..\Public\SwordTrailObj.h"
#include "GameInstance.h"
#include "SwordTrail.h"

CSwordTrailObj::CSwordTrailObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSwordTrailObj::CSwordTrailObj(const CSwordTrailObj & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordTrailObj::Initialize(void * pArg)
{
	CSwordTrail::TRAILINFO Info = *(CSwordTrail::TRAILINFO*)(pArg);

	m_pTrail = (CSwordTrail*)GI->Clone_Component(LEVEL_STATIC, L"SwordTrail", &Info);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

void CSwordTrailObj::Tick(_float fTimeDelta, _matrix Matrix)
{
	if(m_pTrail->Get_TrailOn())
		m_pTrail->Tick(fTimeDelta, Matrix);
}

void CSwordTrailObj::LateTick(_float fTimeDelta)
{
	if (!m_pTrail->Get_TrailOn())
		return;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CSwordTrailObj::Render()
{
	m_pTrail->Render();
	return S_OK;
}

_bool CSwordTrailObj::Get_TrailOn()
{
	return m_pTrail->Get_TrailOn();
}

void CSwordTrailObj::TrailOn(_matrix Matrix)
{
	m_pTrail->TrailOn(Matrix);
}

void CSwordTrailObj::TrailOff()
{
	m_pTrail->TrailOff();
}

void CSwordTrailObj::Set_Color(_float4 vColor)
{
	m_pTrail->Set_Color(vColor);
}

void CSwordTrailObj::Set_Color2(_float4 vColor)
{
	m_pTrail->Set_Color2(vColor);
}

void CSwordTrailObj::Set_Ratio(_float2 vRatio)
{
	m_pTrail->Set_Ratio(vRatio);
}

void CSwordTrailObj::Set_BlurPower(_float fBlurPower)
{
	m_pTrail->Set_BlurPower(fBlurPower);
}

void CSwordTrailObj::Set_TimeLimit(_float fTimeLimit)
{
	m_pTrail->Set_TimeLimit(fTimeLimit);
}

void CSwordTrailObj::Set_HighLow(_float3 vHigh, _float3 vLow)
{
	m_pTrail->Set_HighLow(vHigh, vLow);
}

CSwordTrailObj* CSwordTrailObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSwordTrailObj*		pInstance = new CSwordTrailObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSwordTrailObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwordTrailObj::Clone(void * pArg)
{
	CSwordTrailObj*		pInstance = new CSwordTrailObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSwordTrailObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordTrailObj::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrail);
}
