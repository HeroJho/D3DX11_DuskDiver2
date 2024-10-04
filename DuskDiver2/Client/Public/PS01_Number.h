#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CPS01_Number final : public CUI
{
protected:
	CPS01_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPS01_Number(const CPS01_Number& rhs);
	virtual ~CPS01_Number() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;

public:
	void Set_Number(_uint iNum);

protected:
	HRESULT Ready_Components();

private:
	_uint m_iNumber = 0;
	string m_sNumber = "";
	
	_uint m_iTestNum = 0;

	_bool m_bSizeUp = true;
	_float m_fSizeUpAcc = 3.f;

	_bool m_bSizeOnce = false;
	_float m_fSizeX = 0.f;
	_float m_fSizeY = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

public:
	static CPS01_Number* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END