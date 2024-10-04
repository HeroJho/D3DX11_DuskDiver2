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
		_uint iADType; // � Ÿ���� �����ΰ�
		_uint iADIndex; // ���� Ÿ���� ���° �ΰ�
		_uint iPassIndex; // ��� �н��� �����ΰ�
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

	_float					m_fMoveTime = 0.f;//uv�Ǵ� ������Ʈ�� ���°�

	_float					m_fMoveUVu = 0.f;
	_float					m_fLastUVu = 0.f;

	_float					m_fAlpha = 0.f; // ������Ʈ�� ���� ����
	_bool					m_bReverse = false; // ������Ʈ�� ���ϴ��� ������

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