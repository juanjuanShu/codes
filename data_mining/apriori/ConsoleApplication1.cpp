// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <set>
#include <vector>
#include "ConsoleApplication1.h"

using namespace std;

//加载数据集
vector < set<string>> loadDataset()
{
   vector < set<string> > dataSet;

    set <string> s1 = { "I1","I2","I5" };
    set <string> s2 = { "I2","I4" };
    set <string> s3 = { "I2","I3" };
    set <string> s4 = { "I1","I2","I4" };
    set <string> s5 = { "I1","I3" };
    set <string> s6 = { "I2","I3" };
    set <string> s7 = { "I1","I3" };
    set <string> s8 = { "I1","I3","I2","I5" };
    set <string> s9 = { "I2","I3","I1" };
    
    dataSet.push_back(s1);
    dataSet.push_back(s2);
    dataSet.push_back(s3);
    dataSet.push_back(s4);
    dataSet.push_back(s5);
    dataSet.push_back(s6);
    dataSet.push_back(s7);
    dataSet.push_back(s8);
    dataSet.push_back(s9);

    return dataSet;
}

set <string> create_C1(vector < set<string> > &dataSet) {
    set< string > C1;
    //迭代器
    vector < set<string> >::iterator it_data;
    set<string>::iterator it_set;

    for (it_data = dataSet.begin(); it_data != dataSet.end(); it_data++) {
        for (it_set = (*it_data).begin(); it_set != (*it_data).end(); it_set++) {
            C1.insert(*it_set);
        }
    }

    return C1;
}

void generate_Lk(vector < set<string> > dataSet, int min_sup_count) {
    set <string> C1;
    //存储频繁项集合和对应的支持度计数
     support_data = {}

    //生成C1
    C1 = create_C1(dataSet);
  
    L1 = generate_Lk_by_Ck(dataSet, C1, min_sup_count);


}

int main()
{
    vector < set<string> > dataSet;
    int min_sup_count = 3;

    dataSet = loadDataset();
    generate_Lk(dataSet, min_sup_count);

}

