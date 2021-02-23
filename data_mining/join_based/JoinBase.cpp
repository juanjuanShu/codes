#include "JoinBase.h"
#include "MultiResolution.h"
#include "Common.h"
#include <set>

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

				Common* a = new Common();
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

ColocationSetType apriori_gen(ColocationSetType& consequentSet) {
	vector<FeatureType> combItem;
	set<ColocationType> tmpSet;
	vector<ColocationType> consequentNewSet;

	for (auto it_cs = consequentSet.begin(); it_cs != consequentSet.end() - 1; it_cs++) {
		for (auto it_cs1 = (it_cs)+1; it_cs1 != consequentSet.end(); it_cs1++) {
			//bc bd=>bcd bd cd=>bcd
			combItem = {};
			set_union((*it_cs).begin(), (*it_cs).end(), (*it_cs1).begin(), (*it_cs1).end(), back_inserter(combItem));
			if (combItem.size() == (*it_cs).size() + 1) {
				tmpSet.insert(combItem);
			}
		}
	}
	//set转vector
	consequentNewSet.assign(tmpSet.begin(), tmpSet.end());

	return consequentNewSet;
}

void JoinBase::_generateRuleByColocation(const ColocationType& colocation,
	ColocationSetType& consequentSet,
	vector<Rule>& ans,
	int consequent_num,
	int i,
	int itemLength) {
	double conf;
	ColocationSetType consequentNewSet = {};
	ColocationSetType consequentTempSet = {};

	//前件至少有1个，前件加后件如果大于当前事务，则无法生成
	if (consequent_num + 1 > itemLength) return;

	for (auto consequent : consequentSet) {
		ColocationType antecedent = {};
		//itemSet - consequent = antecedent
		set_difference(colocation.begin(), colocation.end(), consequent.begin(), consequent.end(), back_inserter(antecedent));
		unsigned int antecedent_num = antecedent.size();
		conf = (_numOfColocations[i][colocation]) * 1.0 / (_numOfColocations[antecedent_num][antecedent]);
		conf = round(conf * 100) / 100.0;
		if (conf >= _min_conf) {
			ans.push_back(move(Rule{ antecedent, consequent, conf }));
			consequentTempSet.push_back(consequent);
		}
		//else {
		//    //如果bcd=>a置信度不满足，则剪枝；即从后件项集中删除该元素
		//    it = consequentSet.erase(it);
		//}
	}

	if (!empty(consequentTempSet)) {
		consequentNewSet = apriori_gen(consequentTempSet);
		_generateRuleByColocation(colocation, consequentNewSet, ans, consequent_num + 1, i, itemLength);
	}
}

/*vector<Rule> JoinBase::_generateRules() {
	vector<Rule> ans; 
	int k1 = 1;
	while (_prevalentColocation.count(k1) && !_prevalentColocation[k1].empty()) {
		int k2 = 1;
		// Don't generate R_1
		if (k1 == 1 && k2 == 1) {
			++k2;
		}

		while (_prevalentColocation.count(k2) && !_prevalentColocation[k2].empty()) {
			for (auto it1 = _prevalentColocation[k1].begin(); it1 != _prevalentColocation[k1].end(); ++it1) {
				const ColocationType& colocation1 = (*it1).first;
				for (auto it2 = _prevalentColocation[k2].begin(); it2 != _prevalentColocation[k2].end(); ++it2) {
					const ColocationType& colocation2 = (*it2).first;

					// Colocation patterns are sorted, so use two pointers to merge them.
					ColocationType unionColocation;
					set_union(colocation1.begin(), colocation1.end(),
						colocation2.begin(), colocation2.end(),
						std::back_inserter(unionColocation));

					// colocation1 ∪ colocation2 = ∅
					if (unionColocation.size() != colocation1.size() + colocation2.size()) continue;

					unsigned int k = unionColocation.size();
					if (_prevalentColocation.count(k) && _prevalentColocation[k].count(unionColocation)) {
						// The new colocation pattern is prevalent.
						unsigned int sizeOfNew = _prevalentColocation[k][unionColocation].size();
						unsigned int sizeOfColocation1 = _prevalentColocation[k1][colocation1].size();

						std::map<std::vector<InstanceIdType>, bool> bitmap;
						auto& tableInstance = _prevalentColocation[k1][colocation1];
						for (auto& rowInstance : tableInstance) {
							std::vector<InstanceIdType> ids; // Collect instance ids.
							for (auto& instanceId : rowInstance) {
								ids.push_back(instanceId);
							}
							bitmap[ids] = false;
						}


						int p = 0;
						std::vector<unsigned int> featureIdx; // Collect feature id in vector, not the same as instance ids.
						for (unsigned int i = 0; i < unionColocation.size(); ++i) {
							if (unionColocation[i] == colocation1[p]) {
								featureIdx.push_back(i);
								p++;
							}
						}

						auto& cliquesUnion = _prevalentColocation[k][unionColocation];
						for (auto& clique : cliquesUnion) {
							std::vector<InstanceIdType> ids; // instance ids.
							for (auto idx : featureIdx) {
								ids.push_back(clique[idx]);
							}
							bitmap[ids] = true;
						}

						int trueCnt = 0;
						for (auto bit : bitmap) {
							if (bit.second) {
								trueCnt++;
							}
						}
						double conf = trueCnt * 1.0 / bitmap.size();
						if (conf > _min_conf) {
							
							ans.push_back(move(Rule{ colocation1, colocation2, conf }));
						}


					}
				}
			}
			k2++;
		}
		k1++;
	}

	return ans;
}*/

vector<Rule> JoinBase::_generateRules() {
	vector<ColocationType> consequentSet;
	vector<Rule> ans;

	/*for (int k = 1; k <= _numOfColocations.size(); k++) {
		for (auto& _numOfColocation : _numOfColocations[k]) {
			auto colocations = _numOfColocation.first;
			for (auto feature : colocations) {
				cout << feature << " ";
			}
			cout << endl;
		}
	}*/

	for (int k = 1; k <= _numOfColocations.size(); k++) {
		for (auto& _numOfColocation : _numOfColocations[k]) {
			consequentSet = {};
			auto colocations = _numOfColocation.first;
			//规则的1-项后件
			for (auto& colocation : colocations) {
				consequentSet.push_back({ colocation });
			}
			_generateRuleByColocation(colocations, consequentSet, ans, 1, k, k);
		}
	}

	return ans;
}

void visualization(vector<Rule> rules) {
	ColocationType antecedent;
	ColocationType consequent;
	//ofstream ofs("output.txt");


	for (auto& rule : rules)
	{
		antecedent = rule.antecedent;
		for (auto& item : antecedent) {
			cout << item;
			int size = antecedent.size() - 1;
			if (item == antecedent[size])
				cout << " ";
			else
				cout << "^";
		}

		cout << " => ";

		consequent = rule.consequent;
		for (auto& item : consequent) {
			cout << item;
			int size = consequent.size() - 1;
			if (item == consequent[size])
				cout << " ";
			else
				cout << "^";
		}

		cout << "   confidence  : " << rule.conf;

		cout << endl;
	}
}

void JoinBase::execute() {
	int k = 2;
	while (_prevalentColocation.count(k - 1) && !_prevalentColocation[k - 1].empty()) {
		vector<ColocationType> candidates = _generateCandidateColocations_k(k);
		if (_fmul) {
			MultiResolution multiResolution(_true_instances,_min_prev, _cellSize, numOfInstances);
			multiResolution.multiResolutionPruning(candidates, k);
		}
		ColocationPackage candidatePackages = _generateTableInstances(candidates, k);
		_selectPrevalentColocations(candidatePackages, k);
		k++;
	}

	vector<Rule> rules = _generateRules();

	visualization(rules);
}