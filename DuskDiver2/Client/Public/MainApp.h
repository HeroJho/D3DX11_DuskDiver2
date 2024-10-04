#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END


BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();	
	void Get_KeyInput();

private:
	CGameInstance*				m_pGameInstance = nullptr;
	CRenderer*					m_pRenderer = nullptr;
	RECT m_Clip;
	POINT m_p1;
	POINT m_p2;

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

#ifdef _DEBUG
private:
	_float			m_fTimeAcc = 0.f;
	_uint			m_iNumDraw = 0;
	_bool			m_bClip = false;

	_tchar			m_szFPS[256] = TEXT("");

#endif // _DEBUG

private:
	HRESULT Open_Level();


	//====================
	//    For. Collider
	//====================
private:
	/* 어떤 오브젝트들이 충돌할지 설정합니다. */
	void Check_Collider();


public:
	static CMainApp* Create();
	virtual void Free();
};


END