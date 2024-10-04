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


class CDGCuma_2 final : public CCreture
{
public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };


private:
	CDGCuma_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDGCuma_2(const CDGCuma_2& rhs);
	virtual ~CDGCuma_2() = default;

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

private:
	_bool m_bUp = false;

private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();





private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


	FACE m_eFace;
	_bool m_bDown = false;
	_bool m_bRealDown = true;
	_bool m_bTickUp = true;
	_bool m_bTickDown = false;

	_float m_fDownTimeAcc = 0.f;

public:
	static CDGCuma_2* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END