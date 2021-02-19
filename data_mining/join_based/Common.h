#pragma once
#include "stdafx.h"
#include "Types.h"
#include "JoinBase.h"

class JoinBase;

class Common
{
public:
	bool isRReachable(LocationType& loc1, LocationType& loc2);
	bool multi_rel(CellPositionType &cell1, CellPositionType &cell2);

	JoinBase* joinBase;
};

