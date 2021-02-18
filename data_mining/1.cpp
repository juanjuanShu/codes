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

map< string, unsigned int > create_C1(vector < vector<string> >& dataSet) {
    map< string, unsigned int >C1;
    //迭代器
    vector < vector<string> >::iterator it_data;
    vector<string>::iterator it_item;

    for (it_data = dataSet.begin(); it_data != dataSet.end(); it_data++) {
        for (it_item = (*it_data).begin(); it_item != (*it_data).end(); it_item++) {
            auto ret = C1.insert({ *it_item,1 });
            //insert插入时候，由于map的key要求唯一，如果已经插入了，则返回值中的second的值为False
            //说明插入这个元素了
            if (!ret.second)
                ++ret.first->second;
        }
    }

    return C1;
}

map< string, unsigned int > create_L1_by_C1(map< string, unsigned int >& C1, unsigned int min_sup_count) {
    map< string, unsigned int >::iterator it_C1;
    it_C1 = C1.begin();
    while (it_C1 != C1.end()) {
        if (it_C1->second < min_sup_count) {
            C1.erase(it_C1++);
        }
        else {
            it_C1++;
        }
    }

    return C1;
}

map<vector<string>, vector<string>>  create_Ck(vector < vector<string> >& dataSet, map< string, unsigned int > L, int k) {
    //void  create_Ck(vector < vector<string> >& dataSet, map< string, unsigned int > L, int k) {
        //Ck map(k-1,k) 方便自连接 也剪去了一部分候选集（需要Lk-1）
        //Ck 用hash存储，获取计数


    vector<string> Lj;
    //vector<string>::iterator it_item;
    vector<string>::iterator it_Ck, it_Ck1;

    map<vector<string>, vector<string>> Ck;
    //map<vector<string>, vector<string>>::iterator it_map;

    //获取map中的key值
    for (auto& item : L) {
        Lj.push_back(item.first);
    }
    sort(Lj.begin(), Lj.end());


    //利用性质 预剪枝
    //用前缀项集存储，key:k-1项 value：第k项
    //基于前缀项集的连接 相同前缀中的value任意两项进行连接
    //基于前缀项集的剪枝：根据性质：频繁项集的所有非空子集也必须是频繁的。即得到的 k 项集的子集（k - 1）项集也必须是频繁的；
    //                    value( k - 1)项是否在 L（k-1）项中 如果不在 则删除
    if (k == 2) {
        for (it_Ck = Lj.begin(); it_Ck != Lj.end() - 1; it_Ck++) {
            for (it_Ck1 = it_Ck + 1; it_Ck1 != Lj.end(); it_Ck1++) {
                //cout << *it_Ck << " " << *it_Ck1 << endl;
                vector<string> tmp_key;
                vector<string> tmp_value;
                tmp_key.push_back(*it_Ck);
                tmp_value.push_back(*it_Ck1);
                Ck.insert(map<vector<string>, vector<string>>::value_type(tmp_key, tmp_value));
            }
        }
    }
    else {
        //拿到的是map  key相同 value两两组合


    }

    return Ck;
}

void generate_Lk(vector < vector<string> >& dataSet, unsigned int min_sup_count) {
    map< string, unsigned int >C1;
    map< string, unsigned int >L1;
    vector<  map< string, unsigned int > > L;
    //基于前缀存储的Ck  map(k-1 ,k)
    map<vector<string>, vector<string>>  prefix_Ck;

    //生成C1  map<项集  支持度计数>
    C1 = create_C1(dataSet);
    //L1
    L1 = create_L1_by_C1(C1, min_sup_count);

    L.push_back(L1);

    // L1是map类型 key是 0 value是项集，生成C2 自连接，剪枝
    // 计算支持度计数：1）由事务生成 k 项集-Tk  2）将Lk 存储到 unorderedMap中，默认为0；
    //                 3) 遍历 Tk去map中找，得到统计数



    int k = 1;
    while (L[k - 1].size() > 0) {


        prefix_Ck = create_Ck(dataSet, L[k -1], k+1);
        break;
        //L1 = generate_Lk_by_Ck(dataSet, C1, min_sup_count);

    }



}

int main()
{
    vector < vector<string> > dataSet;
    unsigned int min_sup_count = 2;

    dataSet = loadDataset();
    generate_Lk(dataSet, min_sup_count);

}

