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

class CStage4Machine0 : public CGameObject
{
private:
	CStage4Machine0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage4Machine0(const CStage4Machine0& rhs);
	virtual ~CStage4Machine0() = default;

public:
	void Set_Start(_bool bStart);
	void Set_Gage(_float Ratio);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;



private:
	_float2					m_fUV = { 0.f,0.f }; // y°¡ 0ÀÌ¸é ¸ðµç ºûÀÌ °¡·ÁÁü
	
	_bool					m_bStart = false;
	_float					m_fBlurPow = 0.f;
	_float					m_fColorRatio = 0.2f;

	_float					m_fTimeAcc = 0.f;

private:
	HRESULT Ready_Components();


public:
	static CStage4Machine0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END