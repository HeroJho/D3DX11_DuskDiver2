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

class CStreetStore : public CGameObject
{
public:
	typedef struct tagStreetStoreDesc {
		_uint		iStoreIndex;
	}STREETSTOREDESC;

private:
	CStreetStore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStreetStore(const CStreetStore& rhs);
	virtual ~CStreetStore() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	STREETSTOREDESC			m_StreetStoreDesc;
	_bool					m_bShopping = false;


private:
	HRESULT Ready_Components();

public:
	static CStreetStore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END