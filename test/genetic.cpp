#include <netmodeler.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <memory>

using namespace Starsky;
using namespace std;

std::set<std::string> rstingGenerator (int howmany, int length, Random& r) {
    std::set<std::string> items;
    for (int no=0; no < howmany; no++){
        std::string item;
        for (int i = 0; i < length; i++){

