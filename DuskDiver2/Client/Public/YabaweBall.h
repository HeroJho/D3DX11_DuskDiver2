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

class CYabaweBall : public CCreture
{
public:
	typedef struct tagYabaweDesc
	{
		_float3 vPos;
		_float4x4 mMatrix;

	}YABAWEDESC;

protected:
	CYabaweBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CYabaweBall(const CYabaweBall& rhs);
	virtual ~CYabaweBall() = default;


public:
	_bool Get_Render() { return m_bRender; }
	void Set_Render(_bool bRender) { m_bRender = bRender; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

	virtual void Render_Debug() override;

	void Reset();


private:
	_bool m_bStart = false;
	_bool m_bBlock = false;
	_float3 m_vGoLook;

	_float3 m_vInitPos;
	_float m_fTimeDeltaAcc = 0.f;

	_bool m_bOne = false;
	_bool m_bRender = false;

	_float m_fTimeCutStartAcc = 0.f;

protected:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	// CTexture*				m_pTextureCom = nullptr;


protected:
	HRESULT Ready_Components();

protected:
	YABAWEDESC m_Desc;


public:
	static CYabaweBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END