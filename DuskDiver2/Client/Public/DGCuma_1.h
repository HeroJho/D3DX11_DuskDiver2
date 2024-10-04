#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)


class CDGCuma_1 final : public CCreture
{
public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };


private:
	CDGCuma_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDGCuma_1(const CDGCuma_1& rhs);
	virtual ~CDGCuma_1() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;


public:
	void Set_Face(FACE eFace) { m_eFace = eFace; }

	void Set_Down();


private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();


private:
	FACE m_eFace;
	_float m_fTimeAcc = 0.f;
	_bool m_bDown = false;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


public:
	static CDGCuma_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END