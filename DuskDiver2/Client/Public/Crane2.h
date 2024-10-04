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

class CCrane2 : public CCreture
{
	enum SIDE
	{
		SIDE_LEFT,SIDE_RIGHT,SIDE_END
	};
	enum CraneState
	{
		STATE_NARROW, STATE_WIDEN, STATE_END
	};
private:
	CCrane2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrane2(const CCrane2& rhs);
	virtual ~CCrane2() = default;

private:

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	virtual void Render_Debug() override;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	

protected:
	HRESULT Ready_Components();

private:
	void State_Narrow(_float fTimeDelta);
	void State_Widen(_float fTimeDelta);

private:
	XMVECTOR m_OriginPos;
	XMVECTOR m_Pos;

	_int m_iNum = 0;

	_float x = 0, y = 0, z = 0;

	_float m_fTure = 0.f;
	//집게 접히게 잇는 시간
	_float m_fAngleTime = 0.f;

	SIDE m_Side = SIDE_RIGHT;
	CraneState m_State = STATE_NARROW;

	_float m_fAracadeTime = 0.f;
	_float m_fTickTime = 0.f;
public:
	static CCrane2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END