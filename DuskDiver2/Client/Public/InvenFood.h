#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CInvenFood final : public CUI
{
private:
	enum SIDE
	{
		R, L
	};
	CInvenFood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvenFood(const CInvenFood& rhs);
	virtual ~CInvenFood() = default;

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
	_bool m_bRender = false;
	SIDE m_Side = L;
	_uint m_iNum = 10;

	_float m_fTime = 0.f;
	_float m_fTime2 = 0.f;
	_bool m_bTexture = false;

public:
	static CInvenFood* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END