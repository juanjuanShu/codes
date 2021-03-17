#include "MultiResolution.h"
#include "Common.h"
#include <set>

MultiResolution::MultiResolution(vector<InstanceType>& instances, double min_prev, double cellSize,double distance, map<FeatureType, unsigned int> numOfInstances)
:_min_prev(min_prev),
_cellSize(cellSize),
_distance(distance),
_numOfInstances(numOfInstances){
	for (auto it = instances.begin(); it != instances.end(); it++) {
		auto instanceId = get<InstanceIdType>(*it);
		auto feature = get<FeatureType>(*it);
		auto location = get<LocationType>(*it);

		double realX = location.first, realY = location.second;
		int x = realX / cellSize, y = realY / cellSize;

		_tableInstances[1][{feature}].push_back({ { x,y } });
		//记录该单元格中的相关元素，反向求解！！！！
		_instances[{ x, y }][feature].push_back({ instanceId });
	}
}

ColocationSetType MultiResolution::_selectPrevalentColocations(MultiResolution_ColocationPackage &candidatePackages, int k) {
	ColocationSetType prevalence;

	if (!empty(candidatePackages)) {
		for (auto candidatePackage : candidatePackages) {
			ColocationType candidate = candidatePackage.first;
			MultiResolution_TableInstanceType tableInstances = candidatePackage.second;
			
			//判断每个candidate参与率是否大于等于_min_prev
			bool isPrevalent = true;
			//每个feature开始统计
			for (unsigned int i = 0; i < candidate.size(); i++) {
				int count = 0;
				FeatureType feature = candidate[i];
				set<CellPositionType> cellPositionSet;// Record whether the instances in the cell are counted.
				for (auto& rowInstance : tableInstances) {
					CellPositionType cellPosition = rowInstance[i];
					//如果找不到，那么插入该元素，并统计一次表格中的元素
					if (cellPositionSet.find(cellPosition) == cellPositionSet.end()) {
						cellPositionSet.insert(cellPosition);
						count += (int)_instances[cellPosition][feature].size();
					}
				}

				double conf = count * 1.0 / _numOfInstances[feature];
				if (conf < _min_prev) {
					isPrevalent = false;
					break;
				}
			}

			if (isPrevalent) {
				_tableInstances[k][candidate] = tableInstances;
				prevalence.push_back(candidate);
			}
		}
	}
	
	return prevalence;
}

MultiResolution_ColocationPackage MultiResolution::_generateTableInstances(ColocationSetType& candidates, int k) {
	MultiResolution_ColocationPackage multiResolution_ColocationPackage;

	for (auto candidate : candidates) {
		ColocationType candidate1(candidate.begin(), candidate.end() - 1);
		ColocationType candidate2(candidate.begin(), candidate.end() - 2);
		candidate2.push_back(candidate.back());

		MultiResolution_TableInstanceType  tableInstance1 = _tableInstances[k - 1][candidate1];
		MultiResolution_TableInstanceType  tableInstance2 = _tableInstances[k - 1][candidate2];

		for (auto it1 = tableInstance1.begin(); it1 != tableInstance1.end(); it1++) {
			MultiResolution_RowInstanceType& rowInstance1 = *it1;
			for (auto it2 = tableInstance2.begin(); it2 != tableInstance2.end(); it2++) {
				MultiResolution_RowInstanceType& rowInstance2 = *it2;

				bool canMerge = true;
				for (int idx = 0; idx < k - 2; idx++) {
					if (rowInstance1[idx] != rowInstance2[idx]) {
						canMerge = false;
						break;
					}
				}

				Common* a = new Common(_distance, _cellSize);
				if (canMerge) {
					CellPositionType& cell1 = rowInstance1.back(), & cell2 = rowInstance2.back();
					if (a->multi_rel(cell1, cell2)) {
						MultiResolution_RowInstanceType newRowInstance(rowInstance1);
						newRowInstance.push_back(rowInstance2.back());
						multiResolution_ColocationPackage[candidate].push_back(move(newRowInstance));
					}
				}
			}
		}
	}

	return multiResolution_ColocationPackage;
}

void MultiResolution::multiResolutionPruning(ColocationSetType& candidates, int k) {
	MultiResolution_ColocationPackage candidatePackages =  _generateTableInstances(candidates, k);
	candidates = _selectPrevalentColocations(candidatePackages,k);
}
