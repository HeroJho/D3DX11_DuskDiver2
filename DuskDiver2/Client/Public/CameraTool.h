#pragma once
#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)

class CCameraTool final : public CBase
{
public:
	CCameraTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCameraTool() = default;

public:
	void Initalize();
public:
	void ShowCameraWindow(_float fTimeDelta);
	
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;


public:
	virtual void Free() override;
};

END

