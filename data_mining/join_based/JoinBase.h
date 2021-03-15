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

	friend  bool operator < (struct Rule const&a, struct Rule const &b)
	{
		if (a.antecedent == b.antecedent) {
			return a.consequent < b.consequent;
		}
		else {
			return a.antecedent < b.antecedent;
		}
	}
};

class JoinBase {
public:
	JoinBase(
		vector<InstanceType>& instances,
		double min_prev,
		double min_conf,
		double distance,
		bool fmul = true,
		double cellSize = 1
	);

	set<Rule>  execute();

private:
	double _min_prev;
	double _min_conf;
	double _distance;
	bool _fmul;
	double _cellSize;
	map<FeatureType, map<InstanceIdType, LocationType>> _instances;
	map<FeatureType, unsigned int> numOfInstances;
	map<unsigned int,map<ColocationType, unsigned int>> _numOfColocations;
	map<unsigned int, ColocationPackage> _prevalentColocation;
	vector<InstanceType> _true_instances;
	set<Rule> _rules;

	vector<ColocationType> _generateCandidateColocations_2();

	vector<ColocationType> _generateCandidateColocations_k(int k);

	ColocationPackage _generateTableInstances(ColocationSetType& candidates, int k);

	void  _selectPrevalentColocations(ColocationPackage& candidates, int k);

	bool  _isSubsetPrevalent(ColocationType& candidates, int k);

	void _generateRules();

	unsigned int  getRowInstancesOfColocationSub(const ColocationType& colocationSub);
};  
