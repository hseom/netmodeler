#ifndef starsky__parent_h
#define starsky__parent_h

#include <iostream>
#include "node.h"

namespace Starsky {
    class Parent : public Node {
        protected:
        float _min;
        float _max;
        float _probability;
        
        public:
        Parent();

        Parent(const float min, const float max, const float ignore_prob);

        float getmin();
        float getmax();
        float getprob();
    };
}
#endif


