#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCrack_Manager final : public CBase
{
	DECLARE_SINGLETON(CCrack_Manager)

private:
	CCrack_Manager();
	virtual ~CCrack_Manager() = default;

#pragma region �տ� 
public:
	void Create_Crack(_float4 vPosition, _float fRange, _uint iFissureIndex, _uint iCurLevel);
	void BackUp_Fissure(class CFissure* pFissure);
	void BackUp_CrackStone(class CCrackStone* pCrackStone);
	void Clear_Fissure();
	void Clear_CrackStone();

	_float4 Get_CrackPos(); // �տ��� ��ġ�� ��´�
	_bool Get_CrackExist(); // �տ��� �����ϴ��� ��´�

	void Set_GotoStage5(_bool b) { m_bGotoStage5 = b; }
	const _bool& Get_GotoStage5() { return m_bGotoStage5; }

private:
	class CFissure*		m_pFissure = nullptr;
	class CCrackStone*	m_pCrackStone = nullptr;

	_bool				m_bGotoStage5 = false;
#pragma endregion �տ�

#pragma region ��
public:
	void Delete_Dog(void) { m_bDeleteDog = true; }
	const _bool& Get_DeleteDog() { return m_bDeleteDog; }

private:
	_bool				m_bDeleteDog = false;
#pragma endregion ��

#pragma region ��Ű��
public:
	void Activate_Hockey() { m_bActivateHockey = true; }
	void Deactivate_Hockey() { m_bActivateHockey = false; }
	const _bool& Get_ActivateHockey() { return m_bActivateHockey; }

	void Delete_Hockey() { m_bDeleteHockey = true; }
	const _bool& Get_DeleteHockey() { return m_bDeleteHockey; }
private:
	_bool				m_bActivateHockey = false;
	_bool				m_bDeleteHockey = false;
#pragma endregion ��Ű��

public:
	virtual void Free() override;
};

END