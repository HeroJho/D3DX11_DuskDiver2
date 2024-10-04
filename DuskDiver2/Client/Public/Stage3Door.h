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

class CStage3Door : public CGameObject
{
public:
	typedef struct tagDoorDesc {
		_uint iDoorIndex; //0�� ����, 1�� ������
		_float fMinZ; // �ּ�z��ġ
		_float fMaxZ; // ����z��ġ
	}DOORDESC;
private:
	CStage3Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3Door(const CStage3Door& rhs);
	virtual ~CStage3Door() = default;


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

	DOORDESC				m_DoorDesc;

private:
	HRESULT Ready_Components();
	_bool CheckPlayerOnPos();
	void Update_Door(_float fTimeDelta);

	void MoveLeft(_float fTimeDelta); // ������ �������� ������
	void MoveRight(_float fTimeDelta); // ������ ���������� ������

private:
	_bool					m_bOpen = false;

public:
	static CStage3Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END