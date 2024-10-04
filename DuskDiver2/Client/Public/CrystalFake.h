#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CCrystalFake : public CCreture
{
public:
	typedef struct tagCrystalFakeDesc {
		_float4 vPosition;
	}FAKEDESC;
private:
	CCrystalFake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrystalFake(const CCrystalFake& rhs);
	virtual ~CCrystalFake() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	FAKEDESC				m_FakeDesc;

	_bool					m_bRender = false;
	_bool					m_bHited = false;

	_float					m_fBlurPower = 0.5f;
	_bool					m_bReset = false;
	_float					m_fDeadTime = 0.f;

private:
	HRESULT Ready_Components();
	void Update_RenderState();
	void Decrease_Emissive(_float fTimeDelta);

	void CrystalFake_Reset(_float fTimeDelta);

public:
	static CCrystalFake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END