#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CStage4Escalator : public CCreture
{
private:
	enum STATE {
		STATE_ENTRANCE, STATE_EXIT, STATE_END
	};
private:
	CStage4Escalator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage4Escalator(const CStage4Escalator& rhs);
	virtual ~CStage4Escalator() = default;


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
	_bool					m_bOn = false;
	_bool					m_bEscalator = false;
	_uint					m_iModelIndex = 0;

	_float					m_fOnAcc = 0.f;
	_float4					m_fPlayerPos = { 0.f, 0.f, 0.f, 0.f };
	//================================
	_float  NewX = 0.f;
	_float  NewZ = 0.f;
	//================================

private:
	HRESULT Ready_Components();
	_uint PlayerNearby();
	void Set_Position(_uint iModelIndex);
	void Move_Player(_float fTImeDelta);

	//================================
	void Move(_float fTimeDelta);
	//================================

public:
	static CStage4Escalator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END