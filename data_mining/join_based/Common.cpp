#include "Common.h"

bool Common::isRReachable(LocationType& loc1, LocationType& loc2) {
    /*bool flag =  pow(loc1.first - loc2.first, 2) + pow(loc1.second - loc2.second, 2) < 1;
    if (flag) {
        cout << "666" << endl;
    }*/
    return pow(loc1.first - loc2.first, 2) + pow(loc1.second - loc2.second, 2) <= 4;
}

bool Common::multi_rel(CellPositionType& p1, CellPositionType& p2) {
    return abs(p1.first - p2.first) <= 1 && abs(p1.second - p2.second) <= 1;
}