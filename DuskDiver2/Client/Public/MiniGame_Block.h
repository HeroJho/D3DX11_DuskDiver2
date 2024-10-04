#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CMiniGame_Block : public CGameObject
{
private:
	CMiniGame_Block(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMiniGame_Block(const CMiniGame_Block& rhs);
	virtual ~CMiniGame_Block() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;



public:
	void MakeBlock(_float fSpeed, _float fMaxDis, _bool bBome = false);
	void Set_Fail();
	void Set_Reset();

	void Check_Level();

	_float3 Get_InitPos() { return m_vInitPos; }
	_bool Get_Bome() { return m_bBome; }

private:
	class CBlock* m_pCurBlock = nullptr;
	class CBlock* m_pPreBlock = nullptr;

	// For. Block Op
private:
	_uint m_iBlockCount = 1; // 현재 블럭 개수
	_bool m_bBlockDir = false; // 왓다 갓다리 방향
	_float m_fSpeed = 3.f; // 왓다 갓다리 속도
	_float m_fMaxDis = 5.f; // 블럭 끝 범위
	_float3 m_vInitPos; // 블럭 초기 위치
	_float3 m_vScale;

	_float m_fR = 0.f;
	_float m_fG = 0.f;
	_float m_fB = 1.f;


	_float m_fY = 0.25f;
	_float m_fUpSpeed = 0.f;

	_float m_fBlockHight = 0.f;

	_bool m_bBome = false;
	_bool m_bPreBome = false;
	_float3 m_vPreInitPos;



private:
	CTransform*				m_pTransformCom = nullptr;


private:
	HRESULT Ready_Components();


public:
	static CMiniGame_Block* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END