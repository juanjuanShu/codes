#pragma once
#include "stdafx.h"
#include "Types.h"

class MultiResolution
{
public:
	MultiResolution(vector<InstanceType>& instances,double min_prev,double cellSize);
	void multiResolutionPruning(ColocationSetType  &candidates,int k);

private:
	double _min_prev;
	double _cellSize;
	map<unsigned int,map<ColocationType,MultiResolution_TableInstanceType>>  _tableInstances;

	MultiResolution_ColocationPackage _generateTableInstances(ColocationSetType& candidates, int k);
	_selectPrevalentColocations();
};

