#pragma once
#include "stdafx.h"

//实例<实例ID,空间特征类型，位置>
using InstanceIdType = unsigned int;
using FeatureType = unsigned char;
using LocationType = pair<double, double>;
using InstanceType = tuple<InstanceIdType, FeatureType, LocationType>;

//行实例和表实例
using RowInstanceType = vector<InstanceIdType>;
using TableInstanceType = vector<RowInstanceType>;

//co-location模式
using ColocationType = vector<FeatureType>;
//候选模式,只存储特征    vector<vector<FeatureType>>  
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
