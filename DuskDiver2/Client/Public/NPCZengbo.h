#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CNPCZengbo : public CCreture
{
private:
	enum STATE {
		STATE_IDLE, STATE_WALK, STATE_END
	};
private:
	CNPCZengbo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCZengbo(const CNPCZengbo& rhs);
	virtual ~CNPCZengbo() = default;


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

	_bool					m_bAction = false; // 플레이어의 위치에 따라 거리가 멀면 false로 하고 tick,latetick,render를 안한다.
	_uint					m_iTalkCnt = 0; // 첫번째 두번째 대화가 다르다

	//================================
	_float  NewX = 0.f;
	_float  NewZ = 0.f;
	//================================

private:
	HRESULT Ready_Components();
	_bool PlayerNearby();

	//================================
	void Move(_float fTimeDelta);
	//================================

public:
	static CNPCZengbo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END