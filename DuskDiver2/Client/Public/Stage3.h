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

class CStage3 : public CGameObject
{
private:
	CStage3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3(const CStage3& rhs);
	virtual ~CStage3() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	void Crowd_Sound(_float fTimeDelta); // ¿õ¼º¿õ¼º

private:
	HRESULT Ready_Components();

	_bool	m_bSound = false;
	_float	m_fSoundAcc = 0.f;

public:
	static CStage3* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END