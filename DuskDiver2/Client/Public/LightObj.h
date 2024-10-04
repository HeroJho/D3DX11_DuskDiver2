#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CLight;
END

BEGIN(Client)

class CLightObj : public CGameObject
{
public:
	typedef struct tagLightObjDesc
	{
		_float4 vPos;
		_float  fRange;
		_float4 vDiffuse;
		_float4 vAmbient;
		_float4 vSpecular;

		_float fEndTime;

	}LIGHTOBJDESC;

private:
	CLightObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightObj(const CLightObj& rhs);
	virtual ~CLightObj() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	CTransform*				m_pTransformCom = nullptr;

	CLight*					m_pLight = nullptr;

	_float m_fTimeAcc = 0.f;
	_float m_fEndTime = 0.f;
	_float m_fUpSpeed = 0.f;
	_float m_fRatio = 0.f;

	_float4 m_vInitDiffuse{0.f, 0.f , 0.f , 0.f };

protected:
	HRESULT Ready_Components();


public:
	static CLightObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END