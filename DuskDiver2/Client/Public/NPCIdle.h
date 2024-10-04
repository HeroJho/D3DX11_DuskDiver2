#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CNPCIdle : public CCreture
{
public:
	typedef struct tagNPCIdleDesc {
		_uint iNPCIndex; // 몇번 NPC인가 고유인덱스
		_uint iIdleIndex; // 몇번 idle상태인가
		_uint iModelIndex;// 어느 모델을 쓸것인가
	}NPCIDLEDESC;
private:
	CNPCIdle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCIdle(const CNPCIdle& rhs);
	virtual ~CNPCIdle() = default;

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

	_bool					m_bAction = false;
	_bool					m_bRenderState = false;
	NPCIDLEDESC				m_NPCIdleDesc;
	_float					m_fNPCAlpha = 1.f; // 플레이어와 충돌하면 alpha값을 낮추어 렌더하게끔

private:
	HRESULT Ready_Components();
	void Check_Playernearby();

public:
	static CNPCIdle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END