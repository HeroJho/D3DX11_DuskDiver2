#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CRedWindow : public CGameObject
{
private:
	CRedWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRedWindow(const CRedWindow& rhs);
	virtual ~CRedWindow() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	HRESULT Ready_Components();

private:
	_float m_fAlpha = 0.f;
	_bool m_bBring = false;
	_float m_fSpeed = 0.2f;
	_float m_fTimeAcc = 0.f;
	_float m_fTime = 0.f;
	_int m_iCount = 0;

public:
	static CRedWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END