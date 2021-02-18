#pragma once
#include "stdafx.h"
#include "Types.h"
#include "Common.h"
class Common;

class JoinBase {
public:
	JoinBase(
		vector<InstanceType>& instances,
		double min_prev,
		double min_conf,
		bool fmul = true,
		double cellSize = 1
	);

	void execute();
	Common* common;

private:
	double _min_prev;
	double _min_conf;
	bool _fmul;
	double _cellSize;
	map<FeatureType, map<InstanceIdType, LocationType>> _instances;
	map<FeatureType, unsigned int> _colocationNum1;
	map<unsigned int, ColocationPackage> _prevalentColocation;

	void _generateCandidateColocations(int k);

	vector<ColocationType> _generateCandidateColocations_2();

	vector<ColocationType> _generateCandidateColocations_k(int k);

	ColocationPackage _generateTableInstances(ColocationSetType& candidates, int k);

	void  _selectPrevalentColocations(ColocationPackage& candidates, int k);

	bool  _isSubsetPrevalent(ColocationType& candidates, int k);

	void _generateRules();
};  
