#include "stdafx.h"
#include "..\Public\Level_Tool.h"
#include "Stage5.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "DataManager.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Tool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	//if (TK_DOWN(DIK_F7))
	//	DM->Conv_Bin_Model_All();

}

HRESULT CLevel_Tool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Tool_Level"));


	return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage1"), LEVEL_TOOL, L"Layer_BackGround")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CLevel_Tool * CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool*		pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();

}


