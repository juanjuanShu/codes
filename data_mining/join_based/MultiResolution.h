#pragma once
#include "stdafx.h"
#include "Types.h"

class MultiResolution
{
public:
	MultiResolution(vector<InstanceType>& instances,double min_prev,double cellSize,double distance, map<FeatureType, unsigned int> NumOfInstances);
	void multiResolutionPruning(ColocationSetType  &candidates,int k);

private:
	double _min_prev;
	double _cellSize;
	double _distance;
	map<unsigned int,map<ColocationType,MultiResolution_TableInstanceType>>  _tableInstances;
	map<CellPositionType,map<FeatureType,vector<InstanceIdType>>>  _instances;
	map<FeatureType, unsigned int> _numOfInstances;

	MultiResolution_ColocationPackage _generateTableInstances(ColocationSetType& candidates, int k);
	ColocationSetType _selectPrevalentColocations(MultiResolution_ColocationPackage &candidatePackages,int k);
};

