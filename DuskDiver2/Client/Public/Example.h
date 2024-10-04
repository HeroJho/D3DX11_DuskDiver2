#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)

class CExample final : public CGameObject
{
private:
	CExample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExample(const CExample& rhs);
	virtual ~CExample() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	CShader*				m_pShaderCom = nullptr;

	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	vector<CGameObject*>				m_Parts;
	typedef vector<CGameObject*>		PARTS;

	vector<class CHierarchyNode*>		m_Sockets;



private:
	HRESULT Ready_Components();

	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();

	HRESULT Update_Weapon();

public:
	static CExample* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END