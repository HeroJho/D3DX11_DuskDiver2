#pragma once

#include "Base.h"

BEGIN(Engine)

class CValueManager final : public CBase
{
	DECLARE_SINGLETON(CValueManager)
public:
	typedef struct tagValueDesc
	{
		_float		fLinePower = 0.01f;
		_float		fSpacPower = 10.f;
		_int		iLUT_Index = 6;
		_float      fShadowPow = 0.5f;
	}VALUEDESC;

public:
	CValueManager();
	virtual ~CValueManager() = default;


public:
	VALUEDESC* Get_ValueDesc() { return &m_ValueDesc; }



private:
	VALUEDESC		m_ValueDesc;



public:
	virtual void Free() override;
};

END