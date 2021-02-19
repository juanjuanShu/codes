#include "MultiResolution.h"
#include "Common.h"

MultiResolution::MultiResolution(vector<InstanceType>& instances, double min_prev, double cellSize) 
:_min_prev(min_prev),
_cellSize(cellSize){
	for (auto it = instances.begin(); it != instances.end(); it++) {
		auto instanceId = get<InstanceIdType>(*it);
		auto feature = get<FeatureType>(*it);
		auto location = get<LocationType>(*it);

		double realX = location.first, realY = location.second;
		int x = realX / cellSize, y = realY / cellSize;

		_tableInstances[1][{feature}].push_back({ { x,y } });

	}
}

MultiResolution_ColocationPackage MultiResolution::_generateTableInstances(ColocationSetType& candidates, int k) {
	MultiResolution_ColocationPackage multiResolution_ColocationPackage;

	for (auto candidate : candidates) {
		ColocationType candidate1(candidate.begin(), candidate.end() - 1);
		ColocationType candidate2(candidate.begin(), candidate.end() - 2);
		candidate2.push_back(candidate.back());

		MultiResolution_TableInstanceType  tableInstance1 = _tableInstances[k][candidate1];
		MultiResolution_TableInstanceType  tableInstance2 = _tableInstances[k][candidate2];

		for (auto it1 = tableInstance1.begin(); it1 != tableInstance1.end(); it1++) {
			MultiResolution_RowInstanceType& rowInstance1 = *it1;
			for (auto it2 = tableInstance2.begin(); it2 != tableInstance2.end(); it2++) {
				MultiResolution_RowInstanceType& rowInstance2 = *it2;

				bool canMerge = true;
				for (unsigned int idx = 0; idx < k - 1; idx++) {
					if (rowInstance1[idx] != rowInstance2[idx]) {
						canMerge = false;
						break;
					}
				}

				Common* a = new Common();
				if (canMerge) {
					CellPositionType& cell1 = rowInstance1.back(), & cell2 = rowInstance2.back();
					if (a->multi_rel(cell1, cell1)) {
						rowInstance1.push_back(rowInstance2.back());
						multiResolution_ColocationPackage[candidate].push_back(move(rowInstance1));
					}
				}
			}
		}
	}

}

void MultiResolution::multiResolutionPruning(ColocationSetType& candidates, int k) {
	MultiResolution_ColocationPackage multiResolution_ColocationPackage =  _generateTableInstances(candidates, k);
	auto candidates = _selectPrevalentColocations(multiResolution_ColocationPackage);
}
