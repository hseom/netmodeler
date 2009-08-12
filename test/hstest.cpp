#include <netmodeler.h>
#include <memory>

using namespace Starsky;
using namespace std;

//int main(int argc, char *argv[]) {
int main(void) {
  auto_ptr<SimpleNetwork> sn (new SimpleNetwork());
  sn->create(10);
  auto_ptr<NodeIterator> i (sn->getNodeIterator() );
  while (i->moveNext() ) {
    SimpleNode* this_node = dynamic_cast<SimpleNode*> (i->current() );
    cout << "this node's address: " << this_node->getAddress() <<endl; 
  }
}
