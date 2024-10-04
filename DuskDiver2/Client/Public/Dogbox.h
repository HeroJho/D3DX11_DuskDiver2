#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CRigid;
END

BEGIN(Client)

class CDogbox : public CCreture
{
public:
	typedef struct tagDogboxDesc
	{
		_float4 vPosition;
	}DOGBOXDESC;
private:
	CDogbox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDogbox(const CDogbox& rhs);
	virtual ~CDogbox() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

	void Add_Force(_float3 vForce);
	const _bool& Get_Hited(void) { return m_bHited; }
	const _bool& Get_Delete(void) { return m_bDelete; }

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRigid*					m_pRigidCom = nullptr;

	DOGBOXDESC				m_DogboxDesc;

	_bool					m_bAddForce = false; // 박스한테 힘을 주었는가
	_bool					m_bHited = false; //박스가 맞았는가
	_bool					m_bDelete = false; // 박스를 삭제하는가

private:
	HRESULT Ready_Components();
	void Set_InitState();

	void Create_Item(); // 단서를 제공한다

public:
	static CDogbox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END