#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CNumResultCombo final : public CUI
{
protected:
	CNumResultCombo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNumResultCombo(const CNumResultCombo& rhs);
	virtual ~CNumResultCombo() = default;

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

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool Test = false;

	_float m_fDeadTime = 0.f;
public:
	static CNumResultCombo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END