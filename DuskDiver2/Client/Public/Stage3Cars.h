#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CTransform;
END

BEGIN(Client)

class CStage3Cars : public CCreture
{
public:
	typedef struct tagCarsDesc {
		_uint iLaneIndex; // ��������� Ż���ΰ�? 0,5 : ���� 1~4 : �Ϲ� ���� ,,,,, 0~2 : ���±� 3~5 : ���±�
	}CARSDESC;
private:
	CStage3Cars(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3Cars(const CStage3Cars& rhs);
	virtual ~CStage3Cars() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
	CTransform*				m_pTransformCom2 = nullptr;

	CARSDESC				m_CarsDesc;
	_float					m_fCurPosX = 0.f;
	_float					m_fHighSpeed = 25.f;
	_float					m_fLowSpeed = 15.f;																	
	_bool					m_bAttackCol = true;
	_float					m_fAttackColAcc = 0.f;

	_float m_fDissolveAcc = 0.f;
	_bool m_bRender = false;

	_float					m_fFriction = 3.f; // �ӵ��� ���ҽ�ų ������� 
	_float					m_fSpeedFric = 0.f; // ���ӿ�� �ϱ����� �ӵ�

private:
	HRESULT Ready_Components();

	void Update_Cars(_float fTimeDelta);

	void MoveBack(_float fTimeDelta); // �ڷ� ���� �� 
	void MoveFront(_float fTimeDelta); // ������ ���� ��

	void MoveBackFric(_float fTimeDelta); // �ڷ� �����ϴ� ��
	void MoveFrontFric(_float fTimeDelta); // ������ �����ϴ� ��

	void Reset_Fric();

public:
	static CStage3Cars* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END