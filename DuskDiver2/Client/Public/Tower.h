#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CTower : public CCreture
{
private:
	CTower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTower(const CTower& rhs);
	virtual ~CTower() = default;

private:

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	virtual void Render_Debug() override;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	

protected:
	HRESULT Ready_Components();

private:
	_int m_iNum = 0;
	_float m_fBlurPower = 0.f;
	CCreture::CREATEINFO m_CretureInfo;
public:
	static CTower* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END