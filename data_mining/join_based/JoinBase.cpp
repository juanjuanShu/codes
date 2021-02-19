#include "JoinBase.h"
#include "MultiResolution.h"
#include "Common.h"

JoinBase::JoinBase(vector<InstanceType>& instances,double min_prev, double min_conf, bool fmul, double cellSize)
	:_min_prev(min_prev),
	_min_conf(min_conf),
	_fmul(fmul),
	_true_instances(instances),
	_cellSize(cellSize){
	for (auto it = instances.begin(); it != instances.end(); it++) {
		auto instanceId = get<InstanceIdType>(*it);
		auto feature = get<FeatureType>(*it);
		auto location = get<LocationType>(*it);

		_instances[feature][instanceId] = location;

		_prevalentColocation[1][{feature}].push_back({ instanceId });

		//_colocationNum (特征，数量)
		auto ret = _colocationNum1.insert({ feature,1 });
		if (!ret.second) { ++ret.first->second; }
	}
}

vector<ColocationType> JoinBase::_generateCandidateColocations_2() {
	vector<FeatureType> colocations;
	vector<ColocationType> candidateColocations;

	//获取到实例类型，排序
	for (auto it_data = _colocationNum1.begin(); it_data != _colocationNum1.end(); it_data++) {
		colocations.push_back((*it_data).first);
	}
	sort(colocations.begin(), colocations.end());

	for (unsigned int i = 0; i < colocations.size(); i++) {
		for (unsigned int j = i + 1; j < colocations.size(); j++) {
			candidateColocations.push_back({ colocations [i],colocations [j]});
		}
	}

	return candidateColocations;
}

bool  JoinBase::_isSubsetPrevalent(ColocationType& candidates, int k) {
	if (k <= 2) return true;

	for (unsigned int i = 0; i < candidates.size();i++) {
		ColocationType candidatesCopy(candidates);
		candidatesCopy.erase(candidatesCopy.begin() + i);
		if (!_prevalentColocation[k - 1].count(candidatesCopy)) {
			return false;
		}
	}
	
	return true;
}

vector<ColocationType> JoinBase::_generateCandidateColocations_k(int k){
	if (k == 1)  return _generateCandidateColocations_2();

	vector<ColocationType> candidateColocations;
	ColocationPackage& colocationPackage  = _prevalentColocation[k];
	ColocationSetType C;
	vector<FeatureType> colocationSet;
	map < ColocationType, ColocationType> trie_colocationSet = {};

	//get
	for (auto it = colocationPackage.begin(); it != colocationPackage.end(); it++) {
		C.push_back((*it).first);
	}
	sort(C.begin(), C.end());
	
	//存储
	for (unsigned int i = 0; i < C.size(); ++i) {
		colocationSet = C[i];
		FeatureType lastElement = colocationSet.back();
		colocationSet.pop_back();
		if (trie_colocationSet.find(colocationSet) == trie_colocationSet.end()) {
			trie_colocationSet.insert({ colocationSet,{lastElement} });
		}
		else {
			trie_colocationSet[colocationSet].push_back(lastElement);
		}
	}

	//连接
	for (auto& item : trie_colocationSet) {
		ColocationType candidate = item.first;
		//如果后面的k只有一个，则无法连接
		if (item.second.size() >= 2) {
			for (auto it_value = (item.second).begin(); it_value != (item.second).end() - 1; it_value++) {
				for (auto it_value1 = it_value + 1; it_value1 != (item.second).end(); it_value1++) {
					candidate.push_back(*it_value);
					candidate.push_back(*it_value1);
					if (_isSubsetPrevalent(candidate,k)) {
						candidateColocations.push_back(candidate);
					}
				}
			}
		}
	}

	return candidateColocations;
}

ColocationPackage  JoinBase::_generateTableInstances(vector<ColocationType> &candidates, int k) {
	ColocationPackage candidatePackage;

	for (auto candidate : candidates) {
		//A B
		//A B C一定是由 A B(前 k - 1项)和 A C（k -2 加上最后一个）组成得到的
		ColocationType candidate1(candidate.begin(),candidate.end() -1);
		ColocationType candidate2(candidate.begin(),candidate.end() -2); 
		candidate2.push_back(candidate.back());

		TableInstanceType  tableInstance1= _prevalentColocation[k][candidate1];
		TableInstanceType  tableInstance2= _prevalentColocation[k][candidate2];

		for (auto it1 = tableInstance1.begin(); it1 != tableInstance1.end(); it1++) {
			RowInstanceType& rowInstance1 = *it1;
			for (auto it2 = tableInstance2.begin(); it2 != tableInstance2.end(); it2++) {
				RowInstanceType& rowInstance2 = *it2;

				bool canMerge = true;
				for (unsigned int idx = 0; idx < k - 1; idx++) {
					if (rowInstance1[idx] != rowInstance2[idx]) {
						canMerge = false;
						break;
					}
				}

				Common* a = new Common();
				if (canMerge) {
					LocationType location1 = _instances[candidate1.back()][rowInstance1.back()];
					LocationType location2 = _instances[candidate2.back()][rowInstance2.back()];
					if (a->isRReachable(location1, location2)) {
						rowInstance1.push_back(rowInstance2.back());

						candidatePackage[candidate].push_back(move(rowInstance1));
					}
				}
			}
		}
	}

	return candidatePackage;
}

void  JoinBase::_selectPrevalentColocations(ColocationPackage & candidatePackages,int k) {
	if (empty(candidatePackages))  return;

	for (auto candidatePackage : candidatePackages) {
		ColocationType colocations = candidatePackage.first;
		TableInstanceType tableInstances = candidatePackage.second;

		//初始化位图
		map<FeatureType, vector<bool>> bitMap;
		map<FeatureType, vector<bool>>::iterator it_bit;
		for (unsigned int i = 0; i < colocations.size(); i++) {
			FeatureType feature = colocations[i];
			bitMap[feature] = vector<bool>(_colocationNum1[feature],false);
		}
		//A B:1 1,2 4 
		for (auto rowInstance : tableInstances) {
			for (unsigned int i = 0; i < colocations.size(); i++) {
				FeatureType feature = colocations[i];
				bitMap[feature][rowInstance[i] - 1] = true;
			}
		}

		bool isPrevalent = true;
		for (auto it_bit = bitMap.begin(); it_bit != bitMap.end(); it_bit++) {
			FeatureType feature = (*it_bit).first;
			vector<bool> flag = (*it_bit).second;

			int count = 0;
			for (unsigned int i = 0; i < flag.size(); i++) {
				if (flag[i]) count++;
			}
			if (count * 1.0 / flag.size() < _min_prev) {
				isPrevalent = false;
				break;
			}
			
		}
		
		//如果isPrevalent为true，才会保留
		if (isPrevalent) {
			_prevalentColocation[k][colocations] = tableInstances;
		}
	}
}

void _generateRules() {
	//c1 => c2 c1&&C2 / c1
}

void JoinBase::execute() {
	int k = 1;

	while (_prevalentColocation.count(k) && !_prevalentColocation[k].empty()) {
		vector<ColocationType> candidates = _generateCandidateColocations_k(k);
		if (_fmul) {
			MultiResolution multiResolution(_true_instances,_min_prev, _cellSize);
			multiResolution.multiResolutionPruning(candidates, k);
		}
		ColocationPackage candidatePackages = _generateTableInstances(candidates, k);
		_selectPrevalentColocations(candidatePackages, k);
		k++;
	}
	_generateRules();
}