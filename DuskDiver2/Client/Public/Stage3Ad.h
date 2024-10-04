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

class CStage3Ad : public CGameObject
{
public:
	typedef struct tagADDesc {
		_uint iADType; // 어떤 타입의 광고인가
		_uint iADIndex; // 같은 타입의 몇번째 인가
		_uint iPassIndex; // 몇번 패스를 쓸것인가
	}ADDESC;
	enum ADTYPE { TYPE_UV, TYPE_ACC, TYPE_END };
private:
	CStage3Ad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3Ad(const CStage3Ad& rhs);
	virtual ~CStage3Ad() = default;


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

	ADDESC					m_ADDesc;

	_float					m_fMoveTime = 0.f;//uv또는 컴포넌트를 섞는가

	_float					m_fMoveUVu = 0.f;
	_float					m_fLastUVu = 0.f;

	_float					m_fAlpha = 0.f; // 컴포넌트를 섞는 비율
	_bool					m_bReverse = false; // 컴포넌트를 합하는지 빼는지

private:
	HRESULT Ready_Components();
	void Update_AD(_float fTimeDelta);
	void Update_ADUV(_float fTimeDelta);
	void Update_ADACC(_float fTimeDelta);

public:
	static CStage3Ad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END