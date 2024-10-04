#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CMetalTrashbinBody : public CCreture
{
public:
	typedef struct tagMetalTrashbinDesc {
		_uint iTrashbinIndex;
	}METALTRASHBINDESC;
private:
	CMetalTrashbinBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMetalTrashbinBody(const CMetalTrashbinBody& rhs);
	virtual ~CMetalTrashbinBody() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;


private:
	CShader*					m_pShaderCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CRigid*						m_pRigidCom = nullptr;
	
	METALTRASHBINDESC			m_MetalTrashbinDesc;
	class CMetalTrashbinTop*	m_pTrashbinTop = nullptr;
	class CTrash*				m_pTrash = nullptr;

	_bool						m_bHit = false;
	_bool						m_bRenderState = false;
	_bool						m_bCreateItem = false;

	//for Debug
	//========================================
	_float					NewX = 0.f;
	_float					NewY = 0.f;
	_float					NewZ = 0.f;
	_float					Speed = 0.1f;

private:
	HRESULT Ready_Components();
	void InitState();
	void Check_RenderState();
	void Create_Item();
	void AddForceToTop();
	void Check_DeadTIme(); // 맞았고 플레이어의 시야를 벗어나면 삭제한다

public:
	static CMetalTrashbinBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END