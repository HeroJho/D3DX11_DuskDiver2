#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CNPCWalk : public CCreture
{
public:
	typedef struct tagNPCWalkDesc {
		_uint iRouteIndex;
		_uint iModelIndex;
		_uint iStartNodeIndex;
		_float4 vPosition;
	}NPCWALKDESC;
private:
	CNPCWalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCWalk(const CNPCWalk& rhs);
	virtual ~CNPCWalk() = default;


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
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;

	_bool					m_bRenderState = false;
	NPCWALKDESC				m_NPCWalkDesc;
	_uint					m_iNodeIndex = 0; // 현재 몇번 노드인가
	_float					m_fNPCAlpha = 1.f; // 플레이어와 충돌하면 alpha값을 낮추어 렌더하게끔
	_bool					m_bAction = false;
	_uint					m_iShaderPass = 0;

	_float m_fDissolveAcc = 0.f;
	_bool m_bRender = false;

private:
	HRESULT Ready_Components();
	void Check_Playernearby();


public:
	static CNPCWalk* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END