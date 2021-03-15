#include "JoinBase.h"
#include "MultiResolution.h"
#include "Common.h"
#include <set>

JoinBase::JoinBase(vector<InstanceType>& instances,double min_prev, double min_conf, double distance, bool fmul, double cellSize)
	:_min_prev(min_prev),
	_min_conf(min_conf),
	_distance(distance),
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
		auto ret = numOfInstances.insert({ feature,1 });
		if (!ret.second) { ++ret.first->second; }

		_numOfColocations[1][{feature}] ++;
	}
}

vector<ColocationType> JoinBase::_generateCandidateColocations_2() {
	vector<FeatureType> colocations;
	vector<ColocationType> candidateColocations;

	//获取到实例类型，排序
	for (auto it_data = numOfInstances.begin(); it_data != numOfInstances.end(); it_data++) {
		colocations.push_back((*it_data).first);
	}
	sort(colocations.begin(), colocations.end());
	//A B C
	for (unsigned int i = 0; i < colocations.size() - 1; i++) {
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
	if (k == 2)  return _generateCandidateColocations_2();

	vector<ColocationType> candidateColocations;
	ColocationPackage& colocationPackage  = _prevalentColocation[k - 1];
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
					ColocationType tmpCandidate(candidate);
					tmpCandidate.push_back(*it_value);
					tmpCandidate.push_back(*it_value1);
					if (_isSubsetPrevalent(tmpCandidate,k)) {
						candidateColocations.push_back(tmpCandidate);
					}
				}
			}
		}
	}

	return candidateColocations;
}

ColocationPackage  JoinBase::_generateTableInstances(ColocationSetType &candidates, int k) {
	ColocationPackage candidatePackage;

	for (auto candidate : candidates) {
		//A B
		//A B C一定是由 A B(前 k - 1项)和 A C（k -2 加上最后一个）组成得到的
		ColocationType candidate1(candidate.begin(),candidate.end() -1);
		ColocationType candidate2(candidate.begin(),candidate.end() -2); 
		candidate2.push_back(candidate.back());

		TableInstanceType  tableInstance1= _prevalentColocation[k - 1][candidate1];
		TableInstanceType  tableInstance2= _prevalentColocation[k - 1][candidate2];

		for (auto it1 = tableInstance1.begin(); it1 != tableInstance1.end(); it1++) {
			RowInstanceType& rowInstance1 = *it1;
			for (auto it2 = tableInstance2.begin(); it2 != tableInstance2.end(); it2++) {
				RowInstanceType& rowInstance2 = *it2;

				bool canMerge = true;
				for (unsigned int idx = 0; idx < k - 2; idx++) {
					if (rowInstance1[idx] != rowInstance2[idx]) {
						canMerge = false;
						break;
					}
				}

				Common* a = new Common(_distance, _cellSize);
				if (canMerge) {
					LocationType location1 = _instances[candidate1.back()][rowInstance1.back()];
					LocationType location2 = _instances[candidate2.back()][rowInstance2.back()];
					if (a->isRReachable(location1, location2)) {
						RowInstanceType rowNewInstance(rowInstance1);
						rowNewInstance.push_back(rowInstance2.back());
						candidatePackage[candidate].push_back(move(rowNewInstance));
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
		for (unsigned int i = 0; i < colocations.size(); i++) {
			FeatureType feature = colocations[i];
			//numOfInstances[feature]是feature的实例数
			bitMap[feature] = vector<bool>(numOfInstances[feature],false);
		}
		//A B:1 1,2 4 
		for (auto rowInstance : tableInstances) {
			for (unsigned int i = 0; i < colocations.size(); i++) {
				FeatureType feature = colocations[i];
				//rowInstance[i]是1,2...，从1开始，但是位图下标从0开始
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

	//记录频繁模式和出现的次数
	auto prevalentPackages = _prevalentColocation[k];
	for (auto prevalentPackage : prevalentPackages) {
		ColocationType colocations = prevalentPackage.first;
		TableInstanceType tableInstances = prevalentPackage.second;

		set<RowInstanceType> rowInstanceSet;
		for (auto rowInstance:tableInstances) {
			if (rowInstanceSet.find(rowInstance) == rowInstanceSet.end()) {
				rowInstanceSet.insert(rowInstance);
				_numOfColocations[k][colocations] ++;
			}
		}
	}
}

bool issubset(ColocationType colocation_sub, ColocationType  colocation) {
	ColocationType unionColocation;
	set_union(colocation.begin(), colocation.end(),colocation_sub.begin(), colocation_sub.end(),
		back_inserter(unionColocation));

	return !empty(unionColocation);
}

vector<unsigned int>  getFeatureIdx(const ColocationType &colocation,const ColocationType &colocationSub) {
	vector<unsigned int> featureIdx;

	int pos = 0;
	//A B ;A B C 
	for (unsigned int i = 0; i < colocationSub.size(); i++) {
		if (colocation[i] == colocationSub[pos]) {
			featureIdx.push_back(i);
			pos++;
		}
	}

	return featureIdx;
}

unsigned int getProjectNumOfColocation(TableInstanceType tableInstance, vector<unsigned int> featureIdx) {
	unsigned int projectNumOfColocation;

	set<RowInstanceType> rowInstanceProjectSet;
	//A:0
	for (auto rowInstance : tableInstance) {
		RowInstanceType rowInstanceIds;
		//得到投影的模式的行实例个数
		for (unsigned int i = 0; i < featureIdx.size(); i++) {
			for (unsigned int j = 0; j < rowInstance.size(); j++) {
				if (j == featureIdx[i]) {
					rowInstanceIds.push_back(rowInstance[j]);
				}
			}
		}
		if (!empty(rowInstanceIds)) {
			//去重
			if (rowInstanceProjectSet.find(rowInstanceIds) == rowInstanceProjectSet.end()) {
				rowInstanceProjectSet.insert(rowInstanceIds);
			}
		}
	}

	return rowInstanceProjectSet.size();
}

unsigned int  JoinBase::getRowInstancesOfColocationSub(const ColocationType& colocationSub) {
	unsigned int rowInstancesOfColocationSub;
	
	int k = colocationSub.size();
	return _prevalentColocation[k][colocationSub].size();
}

void JoinBase::_generateRules() {
	//获取colocationSubSet
	ColocationSetType colocationSubSet;
	ColocationPackage colocationOnePackages = _prevalentColocation[1];
	for (auto colocationPackage : colocationOnePackages) {
		ColocationType colocation = colocationPackage.first;
		colocationSubSet.push_back(colocation);
	}

	//a =>bc abc(投影bc)/bc
	int length = _prevalentColocation.size();
	for (unsigned int k = 2; k <= length;k++) {
		ColocationPackage colocationPackages = _prevalentColocation[k];
	
		//abc
		for (auto colocationPackage : colocationPackages) {
			ColocationType colocation = colocationPackage.first;
			TableInstanceType tableInstance = colocationPackage.second;

			for (auto colocationSub : colocationSubSet) {
				if (!issubset(colocationSub, colocation)) {
					continue;
				}
				//abc=>colocation  bc=>colocationSub
				//找出colocationSub在colocation中的feature的下标（按照字典序排序）
				//例如：colocation:A B C ;colocationSub:A C,则是 0,2
				vector<unsigned int> featureIdx = getFeatureIdx(colocation, colocationSub);
				
				//获得分子：abc在ab投影下的行实例数
				unsigned int projectNumOfColocation = getProjectNumOfColocation(tableInstance, featureIdx);
				
				//分母
				unsigned int rowInstancesOfColocationSub = getRowInstancesOfColocationSub(colocationSub);
				
				double conf = projectNumOfColocation * 1.0 / rowInstancesOfColocationSub;
				if (conf >= _min_conf) {
					ColocationType antecedent;
					//abc - bc = a, a =>bc
					set_difference(colocation.begin(), colocation.end(), colocationSub.begin(), colocationSub.end(), back_inserter(antecedent));
					_rules.insert(move(Rule{ antecedent, colocationSub, conf }));
				}
			}

			//这一轮得到的低级放入下一轮 作为 colocationSubSet
			colocationSubSet.push_back(colocation);
		}
	}
}


set<Rule>  JoinBase::execute() {
	int k = 2;
	while (_prevalentColocation.count(k - 1) && !_prevalentColocation[k - 1].empty()) {
		vector<ColocationType> candidates = _generateCandidateColocations_k(k);
		if (_fmul) {
			MultiResolution multiResolution(_true_instances, _min_prev, _cellSize, _distance, numOfInstances);
			multiResolution.multiResolutionPruning(candidates, k);
		}
		ColocationPackage candidatePackages = _generateTableInstances(candidates, k);
		_selectPrevalentColocations(candidatePackages, k);
		k++;
	}

	_generateRules();

	return _rules;
}