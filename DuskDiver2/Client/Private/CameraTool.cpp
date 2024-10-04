#include "stdafx.h"
#include "..\Public\CameraTool.h"
#include "GameInstance.h"


CCameraTool::CCameraTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CCameraTool::Initalize()
{
}

void CCameraTool::ShowCameraWindow(_float fTimeDelta)
{


}




void CCameraTool::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
