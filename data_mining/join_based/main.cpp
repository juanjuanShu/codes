#include "stdafx.h"
#include "Types.h"
#include "JoinBase.h"

/*
* 1.产生候选co-location
* 连接
* 剪枝
* 2.产生候选co-location的表实例
* 几何策略和组合策略，混合策略
* 
*/
int main() {
	vector<InstanceType> instances;
    
    instances.push_back({ 1, 'C', {12,1.5} });
    instances.push_back({ 2, 'C', {0, 3} });
    instances.push_back({ 3, 'C', {30, 10} });

    instances.push_back({ 1, 'B', {0, 1} });
    instances.push_back({ 2, 'B', {13,1.5} });
    instances.push_back({ 3, 'B', {20, 5} });
    instances.push_back({ 4, 'B', {11,2} });
    instances.push_back({ 5, 'B', {31,11} });

    instances.push_back({ 1, 'A', {0, 2} });
    instances.push_back({ 2, 'A', {11, 3} });
    instances.push_back({ 3, 'A', {12,3} });
    instances.push_back({ 4, 'A', {10, 4} });

    JoinBase joinBase(instances,0.5, 0, true, 1);
    joinBase.execute();

    return 0;
}