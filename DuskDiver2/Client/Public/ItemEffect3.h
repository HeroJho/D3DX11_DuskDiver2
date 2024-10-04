#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CItemEffect3 final : public CUI
{
private:
	CItemEffect3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemEffect3(const CItemEffect3& rhs);
	virtual ~CItemEffect3() = default;

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

	_float m_fTime = 1.29f;

	_bool m_bRender = false;

	_bool m_bRed = false;


public:
	static CItemEffect3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END