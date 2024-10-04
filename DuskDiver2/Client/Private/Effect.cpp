#include "stdafx.h"
#include "..\Public\Effect.h"
#include "GameInstance.h"

CEffect::CEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{
}

_bool CEffect::Set_UVIndex(_float fTimeDelta)
{
	m_fUVIndexAcc += 1.f * fTimeDelta;
	if (m_fUVIndexAcc >= m_fUVSpeed)
	{
		m_fUVIndexAcc = 0.f;
		m_fUVIndexX += 1.f;
		if (m_fUVIndexX >= m_fMaxUVIndexX)
		{
			m_fUVIndexX = 0.f;
			m_fUVIndexY += 1.f;
			if (m_fUVIndexY >= m_fMaxUVIndexY)
			{
				m_fUVIndexY = 0.f;
				return true;
			}
		}
	}
	return false;
}

_bool CEffect::Set_UVReverse(_float fTimeDelta)
{
	m_fUVIndexAcc += 1.f * fTimeDelta;
	if (m_fUVIndexAcc >= m_fUVSpeed)
	{
		m_fUVIndexAcc = 0.f;
		m_fUVIndexX -= 1.f;
		if (m_fUVIndexX < 0.f)
		{
			m_fUVIndexX = m_fMaxUVIndexX;
			m_fUVIndexY -= 1.f;
			if (m_fUVIndexY < 0.f)
			{
				m_fUVIndexY = m_fMaxUVIndexY;
				return true;
			}
		}
	}
	return false;
}

void CEffect::Set_UVFix(_float fTimeDelta)
{
	m_vUVFixAcc.x += m_vUVFixSpeed.x * fTimeDelta;
	m_vUVFixAcc.y += m_vUVFixSpeed.y * fTimeDelta;
}


CEffect* CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect*		pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect::Clone(void * pArg)
{
	CEffect*		pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();
}
