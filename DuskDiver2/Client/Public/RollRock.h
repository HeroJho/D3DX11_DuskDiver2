#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CRollRock : public CGameObject
{
public:
	typedef struct tagRollRcokInfo
	{
		_float4x4 vWorldMatrix;
		_bool	bJump = false;
		_float  fScale;
		_float	fJumpPower; 
		_uint	iIndex = 0; // ¸ðµ¨ ÀÎµ¦½º
		_bool	bMove = false;
		_uint	iNaviNum;
	}ROLLROCKINFO;

private:
	CRollRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRollRock(const CRollRock& rhs);
	virtual ~CRollRock() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

private:
	void Check_Death(_float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;

private:
	_float				m_fDeathAcc = 0.f;
	_float				m_fDissolveAcc = 0.f;
	_bool				m_bDissolve = false;
	_uint				m_iShaderPath = 0;
	ROLLROCKINFO		m_RockInfo;
public:
	static CRollRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END