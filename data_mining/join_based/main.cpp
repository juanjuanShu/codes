#include "stdafx.h"
#include "Types.h"
#include "JoinBase.h"
#include <iostream>
#include <sstream>
#include <chrono>

using chrono::high_resolution_clock;
using chrono::milliseconds;


void loadData(vector<InstanceType>& instances,string inputPath);
void visualization(set<Rule> rules);

int main(int argc,char ** argv) {
    if (argc != 6) {
        cout << "Argument number must be 5" << endl;
        cout << "./JoinBase minimum_prevalence£¬ minimum_rule_probability£¬ maximum_neighbourhood_distance£¬ cell_resolution(0 when turn off)£¬ inputPath" << endl;
        return 0;
    }
    double minPre = stod(argv[1]), minRuleProbability = stod(argv[2]), maxDistance = stod(argv[3]), cellResolution = stod(argv[4]);
    string inputPath(argv[5]);

    vector<InstanceType> instances;
    loadData(instances, inputPath);

    high_resolution_clock::time_point beginTime = high_resolution_clock::now();

    JoinBase joinBase(instances, minPre, minRuleProbability, maxDistance, cellResolution != 0, cellResolution);
    //JoinBase joinBase(instances, 0.3, 0, 3, cellResolution != 0, cellResolution);
    set<Rule> rules =  joinBase.execute();

    high_resolution_clock::time_point endTime = high_resolution_clock::now();
    milliseconds timeInterval = chrono::duration_cast<milliseconds>(endTime - beginTime);
    cout << timeInterval.count() << "ms" << endl;

    visualization(rules);

    return 0;
}

void visualization(set<Rule> rules) {
    ColocationType antecedent;
    ColocationType consequent;
    //ofstream ofs("output.txt");

    for (auto rule : rules)
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

void loadData(vector<InstanceType> &instances, string inputPath) {
    /*instances.push_back({ 1, 'C', {12,1.5} });
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
    instances.push_back({ 4, 'A', {10, 4} });*/

    ifstream ifs(inputPath , ios::in);

    string line;
    while (getline(ifs, line)) {
        for (int i = 0; i < line.size(); ++i) {
            if (line[i] == ',') {
                line[i] = ' ';
            }
        }

        stringstream ss(line);
        FeatureType feature;  InstanceIdType instanceId; double x,y;
        ss >> feature >> instanceId >> x >> y;
        instances.push_back(make_tuple(instanceId, feature,make_pair(x,y)));
    }
}