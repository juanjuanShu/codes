#include "Common.h"

bool Common::isRReachable(LocationType& loc1, LocationType& loc2) {
    return pow(loc1.first - loc2.first, 2) + pow(loc1.second - loc2.second, 2) <= 2  * _distance;
}

bool Common::multi_rel(CellPositionType& p1, CellPositionType& p2) {
    //格子划分为1，那么最大
    return abs(p1.first - p2.first) <= 1 && abs(p1.second - p2.second) <= _cellResolution * 2;
}

Common::Common(double distance, double cellResolution)
    :_distance(distance),
    _cellResolution(cellResolution)
{

}
