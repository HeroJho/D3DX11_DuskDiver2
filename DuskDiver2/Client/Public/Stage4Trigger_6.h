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

class CStage4Trigger_6 : public CCreture
{
private:
	CStage4Trigger_6(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage4Trigger_6(const CStage4Trigger_6& rhs);
	virtual ~CStage4Trigger_6() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

	virtual void Render_Debug() override;

	void Open();

protected:
	HRESULT Ready_Components();

private:
	_bool m_bOpen = false;
	_float m_fTimeAcc = 0.f;

public:
	static CStage4Trigger_6* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END