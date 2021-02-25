#pragma once
#include "stdafx.h"
#include "Types.h"
#include "Common.h"
#include "MultiResolution.h"
class Common;

struct Rule {
	ColocationType antecedent;
	ColocationType consequent;
	double conf;
};

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

private:
	double _min_prev;
	double _min_conf;
	bool _fmul;
	double _cellSize;
	map<FeatureType, map<InstanceIdType, LocationType>> _instances;
	map<FeatureType, unsigned int> numOfInstances;
	map<unsigned int,map<ColocationType, unsigned int>> _numOfColocations;
	map<unsigned int, ColocationPackage> _prevalentColocation;
	vector<InstanceType> _true_instances;

	void _generateCandidateColocations(int k);

	vector<ColocationType> _generateCandidateColocations_2();

	vector<ColocationType> _generateCandidateColocations_k(int k);

	ColocationPackage _generateTableInstances(ColocationSetType& candidates, int k);

	void  _selectPrevalentColocations(ColocationPackage& candidates, int k);

	bool  _isSubsetPrevalent(ColocationType& candidates, int k);

	vector<Rule> _generateRules();

	//void _generateRuleByColocation(const ColocationType& colocations,ColocationSetType& consequentSet,vector<Rule>& ans,int consequent_num,int i,int itemLength);

	unsigned int  getRowInstancesOfColocationSub(const ColocationType& colocationSub);
};  
