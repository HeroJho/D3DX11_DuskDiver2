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

class CStage1 : public CGameObject
{
private:
	CStage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage1(const CStage1& rhs);
	virtual ~CStage1() = default;


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

	_uint test = 0;
	_uint					m_DIndex = 0;
	
	_float m_fLightTimeAcc = 0.f;
	_float m_fRatio = 0.f;
	_bool m_bEndLight = false;

public:
	static CStage1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END