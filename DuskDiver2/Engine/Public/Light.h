#pragma once

#include "Base.h"

BEGIN(Engine)
/* �� ������ �����Ѵ�. */

class ENGINE_DLL CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	LIGHTDESC* Get_LightDesc() {
		return &m_LightDesc;
	}
	LIGHTDESC* Get_OriLightDesc() {
		return &m_OriLightDesc;
	}

public:
	HRESULT Initialize(const LIGHTDESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void Set_Dead() { m_bDead = true; }
	_bool Get_Dead() { return m_bDead; }

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	_bool					m_bDead = false;

private:
	LIGHTDESC				m_LightDesc;
	LIGHTDESC				m_OriLightDesc;

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	virtual void Free() override;
};

END