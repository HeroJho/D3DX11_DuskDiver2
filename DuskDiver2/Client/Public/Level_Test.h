#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Test final : public CLevel
{
private:
	CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Test() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Dialog(const _tchar* pLayerTag);

	HRESULT Stage5_Debug(const _tchar* pLayerTag);

private:
	_bool m_bRender = false;

public:
	static CLevel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END