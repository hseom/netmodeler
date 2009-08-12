#include "parent.h"

using namespace Starsky;
using namespace std;

Parent::Parent()
{
}

Parent::Parent(const float min, const float max, const float ignore_prob)
{
    _min = min;
    _max = max;
    _probability = ignore_prob;
}

float Parent::getmin()
{
    return _min;
}

float Parent::getmax()
{
    return _max;
}

float Parent::getprob()
{
    return _probability;
}


