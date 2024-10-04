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

class CStage3AzitEvent : public CGameObject
{
public:
	typedef struct tagAzitEventDesc {
		_uint iAzitEventIndex;
	}AZITEVENTDESC;
private:
	CStage3AzitEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3AzitEvent(const CStage3AzitEvent& rhs);
	virtual ~CStage3AzitEvent() = default;


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
	CTexture*				m_pTextureCom = nullptr;

	AZITEVENTDESC			m_AzitEventDesc;
	class CCluePic*			m_pCluePic = nullptr;

	_bool					m_bActivate = false;
	_bool					m_bOn = false;
	_bool					m_bClueFound = false;

private:
	HRESULT Ready_Components();
	void InitStage();


	//================debug
	void Move();

	_float fX = 0.f;
	_float fY = 0.f;
	_float fZ = 0.f;
	_float fSpeed = 0.1f;

public:
	static CStage3AzitEvent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END