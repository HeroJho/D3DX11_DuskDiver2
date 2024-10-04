#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int            iCurrentIndex = -1;
	}NAVIGATIONDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	_uint Get_CurCellIndex() { return m_NavigationDesc.iCurrentIndex; }
	void Set_NaviIndex(_uint iIndex) { m_NavigationDesc.iCurrentIndex = iIndex; }

	void Set_CurCell(_int iCurCell) { m_iCurCell = iCurCell; }

public:
	_bool isMove(_fvector vPosition, _vector* vNormal);
	/* ���� ���� ��Ҵ��� üũ�մϴ�. ���Ѵٸ� 2���ڷ� �������� ���� ���̵� ���� �� �ֽ��ϴ�.*/
	_bool isGround(_fvector vPosition, _float* Out_fCellY = nullptr);
	/* ���� ���� ���� Y���� ��ȯ�մϴ� (��� ������) */
	_float CNavigation::Compute_Height(_fvector vPos);
	/* �� ��ġ�� �´� �� �ε����� ã���ϴ�. ����� ��α� ������ �ݺ� ȣ�� X !!! */
	_int Find_NaviIndex(_fvector vPos, _bool bNearY = true);




private:
	vector<class CCell*>            m_Cells;
	typedef vector<class CCell*>    CELLS;

	NAVIGATIONDESC                    m_NavigationDesc;

	_int			m_iCurCell = -1;
	class			CCell*		m_pCell = nullptr;

#ifdef _DEBUG
	class CShader*            m_pShader = nullptr;
#endif


private:
	HRESULT Ready_Neighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;







	// =======================
	//       For. Debug
	// =======================
#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG


};

END