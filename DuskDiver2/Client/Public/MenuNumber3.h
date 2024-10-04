#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)

class CMenuNumber3 final : public CUI
{
protected:
	CMenuNumber3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMenuNumber3(const CMenuNumber3& rhs);
	virtual ~CMenuNumber3() = default;

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



public:
	static CMenuNumber3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END