#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)
class CPlayer04WhiteBall;
class CBeastTornado;

class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	void Add_Player04WhiteBall(CPlayer04WhiteBall* pWhiteBall);
	void Release_Player04WHiteBall();
	void Create_RageParticleEffect(_uint iIndex);
	_float4 Get_Player04WhiteBallPos();
	_bool Get_RageParticleOn() { return m_bRageParticleOn; }
	void Set_RageParticleOn() { m_bRageParticleOn = true; }
	_uint Get_RageParticleCount() { return m_iRageParticleCount; }
	void Add_RageParticleCount() { m_iRageParticleCount += 1; }
	void Release_RageParticle();
	_bool Get_BeastTornadoEnd();
	void Add_BeastTornado(CBeastTornado* pTornado);
	void ReleaseTornado();
	_bool Get_MarbleCol() { return m_bMarbleCol; }
	void Set_MarbleCol(_bool bMarbleCol) { m_bMarbleCol = bMarbleCol; }
	_bool Get_MarbleEnd() { return m_bMarbleEnd; }
	void Set_MarbleEnd(_bool bMarbleEnd) { m_bMarbleEnd = bMarbleEnd; }
	void Set_MableAcc(_float fMableAcc) { m_fMableAcc = fMableAcc; }
	_float Get_MableAcc() { return m_fMableAcc; }

	
private:
	CPlayer04WhiteBall* m_pPlayer04WhiteBall = nullptr;
	CBeastTornado* m_pBeastTornado = nullptr;
	_uint m_iRageParticleCount = 0;
	_bool m_bRageParticleOn = true;
	_bool m_bMarbleCol = false;
	_bool m_bMarbleEnd = false;
	_float m_fMableAcc = 0.f;

public:
	virtual void Free() override;
};
END

