#pragma once
#include "stdafx.h"

//ʵ��<ʵ��ID,�ռ��������ͣ�λ��>
using InstanceIdType = unsigned int;
using FeatureType = unsigned char;
using LocationType = pair<double, double>;
using InstanceType = tuple<InstanceIdType, FeatureType, LocationType>;

//��ʵ���ͱ�ʵ��
using RowInstanceType = vector<InstanceIdType>;
using TableInstanceType = vector<RowInstanceType>;

//co-locationģʽ
using ColocationType = vector<FeatureType>;
//��ѡģʽ,ֻ�洢����    vector<vector<FeatureType>>  
using ColocationSetType = vector<ColocationType>;
using ColocationPackage = map<ColocationType, TableInstanceType>; 

//MultiResolution part
using CellPositionType = pair<int,int>;
using MultiResolution_RowInstanceType = vector<CellPositionType>;
using MultiResolution_TableInstanceType = vector<MultiResolution_RowInstanceType>;
using MultiResolution_ColocationPackage = map<ColocationType, MultiResolution_TableInstanceType>;

//struct Rule {
//	ColocationType antecedent;
//	ColocationType consequent;
//	double conf;
//};
//using RuleType = set<Rule>;
