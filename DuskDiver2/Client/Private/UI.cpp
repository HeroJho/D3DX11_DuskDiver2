#include "stdafx.h"
#include "..\Public\UI.h"
#include "GameInstance.h"

#include "ToolManager.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_UiInfo, sizeof(UIINFODESC));
	ZeroMemory(&m_TotalInfo, sizeof(UIINFODESC));
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(&m_UiInfo, sizeof(UIINFODESC));
	ZeroMemory(&m_TotalInfo, sizeof(UIINFODESC));
}





_vector CUI::Get_TotalPos()
{
	_vector vPos = { m_UiInfo.pParent->Get_Info().fX + m_UiInfo.fX, m_UiInfo.pParent->Get_Info().fY + m_UiInfo.fY };

	return vPos;
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_UiInfo = *((UIINFODESC*)pArg);
	}
	else
	{
		m_UiInfo.pParent = nullptr;

		m_UiInfo.fSizeX = g_iWinSizeX;
		m_UiInfo.fSizeY = g_iWinSizeY;

		m_UiInfo.fX = g_iWinSizeX * 0.5f;
		m_UiInfo.fY = g_iWinSizeY * 0.5f;
	}


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));


	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	// 부모가 있다면 부모의 포지션 값을 적용한다.
	if (nullptr != m_UiInfo.pParent)
	{
		m_TotalInfo.fX = m_UiInfo.pParent->Get_Info().fX + m_UiInfo.fX;
		m_TotalInfo.fY = m_UiInfo.pParent->Get_Info().fY + m_UiInfo.fY;
	}
	else
		m_TotalInfo = m_UiInfo;

	m_pTransformCom->Set_Scale(XMVectorSet(m_UiInfo.fSizeX, m_UiInfo.fSizeY, 1.f, 0.f));
	// 스크린 공간의 좌표를 클립 공간의 좌표로 변환한다.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, 
		XMVectorSet(m_TotalInfo.fX - g_iWinSizeX * 0.5f, 
			-m_TotalInfo.fY + g_iWinSizeY * 0.5f, 0.0f, 1.f));

	// 자식들의 Tick을 호출 
	for (vector<class CUI*>::iterator iter = m_pChildUIs.begin(); iter != m_pChildUIs.end();)
	{
		if (nullptr != *iter)
			(*iter)->Tick(fTimeDelta);

		if ((*iter)->Get_Dead())
		{
			Safe_Release(*iter);
			iter = m_pChildUIs.erase(iter);
		}
		else
			++iter;
	}
}

void CUI::LateTick(_float fTimeDelta)
{
	// 자식의 LateTick 호출
	for (auto& pChildUI : m_pChildUIs)
		pChildUI->LateTick(fTimeDelta);

	// 렉트 충돌처리
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	RECT		rcUI; m_eState = UI_END;
	SetRect(&rcUI, _int(m_TotalInfo.fX - m_UiInfo.fSizeX * 0.5f), 
		_int(m_TotalInfo.fY - m_UiInfo.fSizeY * 0.5f), 
		_int(m_TotalInfo.fX + m_UiInfo.fSizeX * 0.5f), 
		_int(m_TotalInfo.fY + m_UiInfo.fSizeY * 0.5f));
	// 충돌했다면 OnCollision 호출!
	if (PtInRect(&rcUI, ptMouse))
		OnCollision(CCollider::OTHERTOMECOLDESC());
}

HRESULT CUI::Render()
{
	for (auto& pChildUI : m_pChildUIs)
		pChildUI->Render();

	return S_OK;
}

HRESULT CUI::Make_ChildUI(_float fX, _float fY, 
	_float fSizeX, _float fSizeY, _tchar * pTag, void* pArg)
{
	UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));
	UiInfoDesc.pParent = this;
	UiInfoDesc.fSizeX = fSizeX; 
	UiInfoDesc.fSizeY = fSizeY;
	UiInfoDesc.fX = fX; UiInfoDesc.fY = fY;
	UiInfoDesc.pDesc = pArg;

	// 자식 복사본 생성
	CGameObject* pObj = nullptr;
	pObj = GI->Clone_GameObject(pTag, &UiInfoDesc);
	if (pObj == nullptr)
		return E_FAIL;
	// 자식 추가
	m_pChildUIs.push_back((CUI*)pObj);

	return S_OK;
}

void CUI::Delete_AllChildUI()
{
	for (auto& pUI : m_pChildUIs)
	{
		pUI->Set_Dead();
		Safe_Release(pUI);
	}
	m_pChildUIs.clear();
}




void CUI::Handle_Collision()
{
	switch (m_eState)
	{
	case UI_HOVER:
		
		break;
	case UI_DOWN:
		
		break;
	case UI_PRESS:
		
		break;
	case UI_CLICK:
		
		break;
	}
}



void CUI::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if (CM_DOWN(DIMK_LBUTTON))
	{ // 눌렀을 때
		m_eState = UI_DOWN;
		Handle_Down();
	}
	else if (CM_PRESS(DIMK_LBUTTON))
	{ // 누르고 있을 때
		m_eState = UI_PRESS;
		Handle_Press();
	}
	else if (CM_UP(DIMK_LBUTTON))
	{ // 땔 때
		m_eState = UI_CLICK;
		Handle_Click();
	}
	else if (CM_UP_F(DIMK_LBUTTON))
	{ // 마우스가 렉트 위에 있을 때
		m_eState = UI_HOVER;
		Handle_Hover();
	}
}

void CUI::UI_RenderDebug()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	string sTemp = to_string(m_UiInfo.fX);
	TCHAR cTemp[MAX_PATH];
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 310.f));

	sTemp = to_string(m_UiInfo.fY);
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 330.f));

	sTemp = to_string(m_UiInfo.fSizeX);
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 350.f));

	sTemp = to_string(m_UiInfo.fSizeY);
	CToolManager::Get_Instance()->CtoTC(sTemp.data(), cTemp);
	pGameInstance->Render_Fonts(TEXT("Font_Nexon"), cTemp, _float2(5.f, 370.f));

	RELEASE_INSTANCE(CGameInstance);
}

void CUI::UI_InputDebug(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (CK_PRESS(DIK_UP))
	{
		m_UiInfo.fY -= 50.f * fTimeDelta;
	}
	else if (CK_PRESS(DIK_DOWN))
	{
		m_UiInfo.fY += 50.f  * fTimeDelta;
	}
	else if (CK_PRESS(DIK_LEFT))
	{
		m_UiInfo.fX -= 50.f  * fTimeDelta;
	}
	else if (CK_PRESS(DIK_RIGHT))
	{
		m_UiInfo.fX += 50.f  * fTimeDelta;
	}
	else if (CK_PRESS(DIK_X))
	{
		if (CK_PRESS((DIK_LSHIFT)))
			m_UiInfo.fSizeX -= 50.f  * fTimeDelta;
		else if(CK_PRESS_F(DIK_LSHIFT))
			m_UiInfo.fSizeX += 50.f  * fTimeDelta;
	}
	else if (CK_PRESS(DIK_Y))
	{
		if (CK_PRESS(DIK_LSHIFT))
			m_UiInfo.fSizeY -= 50.f  * fTimeDelta;
		else if (CK_PRESS_F(DIK_LSHIFT))
			m_UiInfo.fSizeY += 50.f  * fTimeDelta;
	}
	else if (CK_PRESS(DIK_U))
	{
		m_UiInfo.fSizeX += 50.f  * fTimeDelta;
		m_UiInfo.fSizeY += 50.f  * fTimeDelta;
	}
	else if (CK_PRESS(DIK_D))
	{
		m_UiInfo.fSizeX -= 50.f  * fTimeDelta;
		m_UiInfo.fSizeY -= 50.f  * fTimeDelta;
	}



	RELEASE_INSTANCE(CGameInstance);

}





HRESULT CUI::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

void CUI::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());




	ImGui::End();
}

CUI * CUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUI*		pInstance = new CUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI::Clone(void * pArg)
{
	CUI*		pInstance = new CUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_pChildUIs)
		Safe_Release(pChildUI);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
