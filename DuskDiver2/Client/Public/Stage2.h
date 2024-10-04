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

class CStage2 : public CGameObject
{
private:
	CStage2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage2(const CStage2& rhs);
	virtual ~CStage2() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	void Crowd_Sound(_float fTimeDelta); // ¿õ¼º¿õ¼º

protected:
	HRESULT Ready_Components();
	
	_float m_fAlpa = 0.f;
	_uint test = 0;
	_float m_fGama = 1.3f;

	_bool	m_bSound = false;
	_float	m_fSoundAcc = 0.f;

public:
	static CStage2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END