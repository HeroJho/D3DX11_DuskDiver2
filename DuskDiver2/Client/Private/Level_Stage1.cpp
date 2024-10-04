#include "stdafx.h"
#include "..\Public\Level_Stage1.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "ToolManager.h"


CLevel_Stage1::CLevel_Stage1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage1::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Stage1::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

HRESULT CLevel_Stage1::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Stage1_Level"));

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_BackGround(const _tchar * pLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Stage1::Ready_Layer_Player(const _tchar * pLayerTag)
{


	return S_OK;
}

CLevel_Stage1 * CLevel_Stage1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage1*		pInstance = new CLevel_Stage1(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Stage1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage1::Free()
{
	__super::Free();

}


