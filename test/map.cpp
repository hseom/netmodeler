#include <vector>
#include <iostream>
#include "netmodeler.h"

using namespace std;
using namespace Starsky;
//random string generator
std::vector<std::string> rstringGenerator (int howmany, int length, Random& r ){
  std::vector<std::string> items;
  for (int no=0; no < howmany; no++){
    std::string item;
    for (int i = 0; i < length; i++){
      int rand_no = (int) (r.getDouble01() * 122);
      if ( rand_no < 65 ) { rand_no = 65 + rand_no % 56;}
      if ( (rand_no > 90) && (rand_no < 97) ) { rand_no += 6; }
        item += (char)rand_no;
        }
    items.push_back(item);
  }
  return items;
}

int main(void) {
  Ran1Random r(-1);
  vector<string> my_vec = rstringGenerator(10,5,r);
  int ran_no = r.getInt(10);
  cout << "ran_no: " << ran_no << endl;
  string selected_str = my_vec[ran_no];
  cout << "selected_str: " << selected_str << endl;
  ran_no = r.getInt(10);
  cout << "ran_no: " << ran_no << endl;
  selected_str = my_vec[ran_no];
  cout << "selected_str: " << selected_str << endl;
}
