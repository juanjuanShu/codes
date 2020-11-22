// apriori.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <set>
#include <vector>
#include <map>


using namespace std;

//加载数据集
vector < vector<string>> loadDataset()
{
    vector < vector<string> > dataSet;

    vector <string> s1 = { "I1","I2","I5" };
    vector <string> s2 = { "I2","I4" };
    vector <string> s3 = { "I2","I3" };
    vector <string> s4 = { "I1","I2","I4" };
    vector <string> s5 = { "I1","I3" };
    vector <string> s6 = { "I2","I3" };
    vector <string> s7 = { "I1","I3" };
    vector <string> s8 = { "I1","I3","I2","I5" };
    vector <string> s9 = { "I2","I3","I1" };

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

map< string, unsigned int > create_C1(vector < vector<string> >& dataSet) {
    map< string, unsigned int >C1;
    //迭代器
    vector < vector<string> >::iterator it_data;
    vector<string>::iterator it_item;

    for (it_data = dataSet.begin(); it_data != dataSet.end(); it_data++) {
        for (it_item = (*it_data).begin(); it_item != (*it_data).end(); it_item++) {
            auto ret = C1.insert({*it_item,1});
            //insert插入时候，由于map的key要求唯一，如果已经插入了，则返回值中的second的值为False
            //说明插入这个元素了
            if (!ret.second)
                ++ret.first->second; 
        }
    }

    return C1;
}

map< string, unsigned int > create_L1_by_C1(map< string, unsigned int > C1,int min_sup_count) {
    //map< string, unsigned int >L1;
    for (auto& item : C1) {
        if (item.second < min_sup_count) {
            C1.erase(item.first);
        }
    }

    for (auto& item : C1) {
        cout << item.first << ":" << item.second << endl;
    }

    return C1;

}

void generate_Lk(vector < vector<string> > dataSet, int min_sup_count) {
    map< string, unsigned int >C1;
    map< string, unsigned int >L1;
    //存储频繁项集合和对应的支持度计数
    //support_data = {}

    //生成C1
    C1 = create_C1(dataSet);
    L1 = create_L1_by_C1(C1, min_sup_count);

    //L1 = generate_Lk_by_Ck(dataSet, C1, min_sup_count);


}

int main()
{
    vector < vector<string> > dataSet;
    int min_sup_count = 3;

    dataSet = loadDataset();
    generate_Lk(dataSet, min_sup_count);

}

