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

class CStage3Terrain : public CGameObject
{
private:
	CStage3Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3Terrain(const CStage3Terrain& rhs);
	virtual ~CStage3Terrain() = default;


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

	

protected:
	HRESULT Ready_Components();

private:
	_float m_fTimeAcc = 0.f;
	_float m_fBlurPow = 0.15f;
	_float4 m_vDiffuseDir;
	_float m_fDirRatio = 1.f;
	_uint test = 0;
	_float fTest = 0.f;

	_float m_fGama = 1.f;

public:
	static CStage3Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END