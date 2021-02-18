#include "Common.h"

bool Common::isRReachable(LocationType& loc1, LocationType& loc2) {
    return pow(loc1.first - loc2.first, 2) + pow(loc1.second - loc2.second, 2) < 1;
}
