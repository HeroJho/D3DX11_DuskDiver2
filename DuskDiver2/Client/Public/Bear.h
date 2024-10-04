#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CCreture;

class CBear : public CGameObject
{
public:
	enum BEARTYPE { BEAR1, BEAR2, BEAR3, BEAR4, BEAR5, BEAR6, BEAR7, BEAR8 };
private:
	CBear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBear(const CBear& rhs);
	virtual ~CBear() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

public:

	vector<CCreture*>* Get_BearList() { return &m_pBear; }
	void	Dead_AllBear();

	//hp
	_float Get_MaxHp() { return m_fMaxHp; }
	void Set_MaxHp(_float fMaxHp) { m_fMaxHp = fMaxHp; }

	_float Get_NowHp() { return m_fNowHp; }
	void Set_NowHp(_float fNowHp) { m_fNowHp = fNowHp; }

	_float Get_PreHp() { return m_fPreHp; }

	//pg
	_float Get_PrePatternGauge() { return m_fPrePatternGauge; }
	_float Get_PatternGauge() { return m_fPatternGauge; }

private:
	HRESULT	Ready_Bear(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Find_NearstBear(_float fTimeDelta);
	void	Set_PreBar(_float fTimeDelta);
	void	Check_TotalHp();

private:
	vector<CCreture*>				m_pBear;
	_float4							m_fPlayerPos;

	_float							m_fMaxHp = 0.f;
	_float							m_fNowHp = 0.f;
	_float							m_fPreHp = 0.f;
	_float							m_fMinusHp = 0.f;


	_float					m_fMaxPg = 0.f;
	_float					m_fPatternGauge = 0.f;
	_float					m_fPrePatternGauge = 0.f;
	_float					m_fMinusPg = 0.f;

	_float					m_fSameHp = 0.f;
	_uint					m_iBearIndex = 0;

	_bool					m_bStartEndCut = false;

public:
	static CBear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END