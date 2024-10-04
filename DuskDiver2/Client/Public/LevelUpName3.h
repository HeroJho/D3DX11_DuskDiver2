#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CLevelUpName3 final : public CUI
{
private:
	CLevelUpName3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLevelUpName3(const CLevelUpName3& rhs);
	virtual ~CLevelUpName3() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	HRESULT Ready_Components();


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool m_bDebug = false;

	_bool m_bAppear = false;
	_float m_fCut = 0.f;
	_uint m_iCurrentNum = 0;

	_bool m_bCut = false;

	_bool m_bDD = false;
public:
	static CLevelUpName3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END