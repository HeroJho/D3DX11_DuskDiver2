#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CRock : public CGameObject
{
public:
	typedef struct RockInfo
	{
		_uint iIndex;
		_float fRotation;
		_float4 vPosition;
		_float fScale;
		_float4x4 vWorldMatrix;

	}ROCKINFO;
private:
	enum STATE {START};
private:
	CRock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRock(const CRock& rhs);
	virtual ~CRock() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;

private:
	void End_Animation();
	void Update(_float fTimeDelta);
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);

private:
	HRESULT Ready_Components();

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;

private:
	_tchar		m_cModelkey[MAX_PATH] = TEXT("");;
	_uint		m_iRockIndex;
	STATE		m_eCurState = START;
	_float		m_fDissolveAcc = 0.f;
	_bool		m_bDissolve = false;
	_uint		m_iShaderPath = 0;
public:
	static CRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END