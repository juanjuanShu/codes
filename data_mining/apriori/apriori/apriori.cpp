// apriori.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <set>
#include <vector>
#include <map>
#include<algorithm>


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

map< vector<string> , unsigned int > create_C1(vector < vector<string> >& dataSet) {
    map< vector<string>, unsigned int >C1;
    map< vector<string>, unsigned int >::iterator it_C1;
    //迭代器
    vector < vector<string> >::iterator it_data;
    vector<string>::iterator it_item;

    for (it_data = dataSet.begin(); it_data != dataSet.end(); it_data++) {
        for (it_item = (*it_data).begin(); it_item != (*it_data).end(); it_item++) {
            vector<string> tmp_item = {};
            tmp_item.push_back(*it_item);
            auto ret = C1.insert({ tmp_item,1 });
            //insert插入时候，由于map的key要求唯一，如果已经插入了，则返回值中的second的值为False
            //说明插入这个元素了
            if (!ret.second)
                ++ret.first->second;
        }
    }

   
    return C1;
}

map< vector<string>, unsigned int > create_L1_by_C1(map< vector<string>, unsigned int >& C1, unsigned int min_sup_count) {
    map< vector<string>, unsigned int >::iterator it_C1;
    it_C1 = C1.begin();
    while (it_C1 != C1.end()) {
        if (it_C1->second < min_sup_count) {
            C1.erase(it_C1++);
        }
        else {
            it_C1++;
        }
    }

    for (auto& w : C1) {
        cout << w.first[0] << " " << w.second << endl;
    }
    return C1;
}

map< vector<string>, vector<string> > create_map_L1(map< vector<string>, unsigned int > L1) {
    //获取key
    map< vector<string>, vector<string> > map_L1;
    map< vector<string>, unsigned int >::iterator it_L1;
    vector<string>::iterator it_item;
    vector<string> map_value;
    vector<string> map_key;

    for (it_L1 = L1.begin(); it_L1 != L1.end(); it_L1++) {
        map_value.push_back((it_L1->first)[0]);
    }

    map_key.push_back("0");
    map_L1.insert(map<vector<string>, vector<string>>::value_type(map_key, map_value));

    return map_L1;
}

void create_Ck(map< vector<string>, vector<string> >map_L, int k) {
    map< vector<string>, vector<string> >::iterator it_map_L;
    vector<string>::iterator it_value,it_value2;
    vector<string> combine_item;

    

    //for (it_map_L = map_L.begin(); it_map_L != map_L.end(); it_map_L++) {
    //    for (it_value = (it_map_L->second).begin(); it_value != (it_map_L->second).end() - 1; it_value++)
    //        for (it_value2 = it_value + 1; it_value2 != (it_map_L->second).end(); it_value2++) {
    //            //k = 2 不进行预剪枝
    //            combine_item.push_back(*it_value);
    //            combine_item.push_back(*it_value2);
    //        }       
    //}
}

void generate_Lk(vector < vector<string> >& dataSet, unsigned int min_sup_count) {
    map< vector<string>, unsigned int >C1;
    map< vector<string>, unsigned int >L1;
    map< vector<string>, vector<string> >map_L1;
    vector<  map< vector<string>, unsigned int > > L;
    //基于前缀存储的Ck  map(k-1 ,k)
    map<vector<string>, vector<string>>  prefix_Ck;

    //生成C1  map<项集  支持度计数>
    C1 = create_C1(dataSet);
    //L1
    L1 = create_L1_by_C1(C1, min_sup_count);
    //获得map存储
    map_L1 = create_map_L1(L1);

    L.push_back(L1);
    int k = 1;

     create_Ck(map_L1, k);
    /*while (L[k - 1].size() > 0) {
        prefix_Ck = create_Ck(map_L1,k + 1);
    }*/






    // L1是map类型 key是 0 value是项集，生成C2 自连接，剪枝
    // 计算支持度计数：1）由事务生成 k 项集-Tk  2）将Lk 存储到 unorderedMap中，默认为0；
    //                 3) 遍历 Tk去map中找，得到统计数
   
}

int main()
{
    vector < vector<string> > dataSet;
    unsigned int min_sup_count = 2;

    dataSet = loadDataset();
    generate_Lk(dataSet, min_sup_count);

}

