#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <cmath>
#include <netmodeler.h>

using namespace Starsky;
using namespace std;

//random string generator
std::set<std::string> rstringGenerator (int howmany, int length, Random& r){
    std::set<std::string> items;
    for(int no=0; no < howmany; no++) {
        std::string item;
        for(int i=0; i<length; i++) {
            int rand_no = (int) (r.getDouble01()* 122);
            if ( rand_no < 65) { rand_no = 65 + rand_no % 56;}
            if ( (rand_no > 90) && (rand_no < 97) ) { rand_no += 6;}
            item += (char)rand_no;
        }
        items.insert(item);
    }
    return items;
}


int main(void) {
  ifstream indata("090802_test.dat",ios::in);
  //ofstream outdata;
  //outdata.open("reason.dat");
  
  int nodes = 100;
  int k = 100;
  float a[nodes]; //fitness
  float b[nodes]; //probability
  int c[nodes]; //connection 
  int d[nodes]; //ttl
  
  Ran1Random ran_no(-1);
  EventScheduler sched;
  auto_ptr<SimpleNetwork> Net_ptr (new SimpleNetwork(ran_no));
  float check_prob = 0.5;

  for(int i = 0; i < 100; i++) {
    //ifstream indata("final_test.dat",ios::in);
    indata >> a[i] >> b[i] >> c[i] >> d[i];
  }
  
  //for(int i = 0; i < 100 ; i++) {
    for(int j = 0; j < nodes ; j++) {
      if(j == 3) {
        //ifstream indatja("final_test_modified", ios::in);
        //indata >> a[j] >> b[j] >> c[j] >> d[j];
        Net_ptr->create(j, d[j], c[j], check_prob, false);
        //cout << a[j] << "\t" << b[j] << "\t" << c[j] << "\t" << d[j] << endl;

        //Net_ptr->create(1, d[j], c[j], check_prob, true);
      }
      else {
        //ifstream indata("final_test_modified", ios::in);
        //indata >> a[j] >> b[j] >> c[j] >> d[j];
        //ifstream indata("final_test_modified", ios::in);
        Net_ptr->create(j, d[j], c[j], b[j], true);
        //cout << a[j] << "\t" << b[j] << "\t" << c[j] << "\t" << d[j] << endl;
      }
    }
    //check_prob = check_prob + 0.01;
    int ctime, qtime;
    double time = 0;
  
    std::set<std::string> items = rstringGenerator(k, 10, ran_no);
    std::set<std::string>::const_iterator item_it;

    time += 10;
    UniformNodeSelector uns(ran_no);
    for (item_it = items.begin(); item_it != items.end(); item_it++) {
      std::string item = *item_it;
      ctime = time + ran_no.getExp(100.0);
      Action* c_action = new CacheAction(sched, ran_no, uns, *Net_ptr.get(), item);
      sched.at(ctime, c_action);
/*
      jtime = ctime + ran_no.getExp(720.0);
      Action* j_action = new JoinAction(sched, ran_no, *Net_ptr.get());
      sched.at(jtime, j_action);
*/
      UniformNodeSelector q_start(ran_no);
      for (int iter = 0; iter < 100; iter++) {
        Action* q_action = new QueryAction(sched, ran_no, q_start, *Net_ptr.get(), item);
        qtime = ctime +ran_no.getExp(3600.0);
        sched.at(qtime, q_action);
      }
/*
      forl++;
      forl=forl%2;
  
      if(forl == 0) {
        ftime = qtime + ran_no.getExp(1080.0);
        Action *f_action = new FailAction(sched, ran_no, *Net_ptr.get());
        sched.at(ftime, f_action);
      }
      else {
        ltime = qtime +ran_no.getExp(1080.0);
        Action* l_action = new LeaveAction(sched, ran_no, *Net_ptr.get());
        sched.at(ltime, l_action);
      }
  */ 
      time = ctime;
    }

    Action* stop = new StopAction(sched);
    sched.at(360000, stop);
    sched.start();
  
    auto_ptr<NodeIterator> totn (Net_ptr->getNodeIterator());
    float hitrate;
    int cpu_cost;
    int disk_cost;
    float prob;
    float fitness;
    //int new_nodes = 0;
    
    while (totn->moveNext() ) {
      SimpleNode* inNode = dynamic_cast<SimpleNode*> (totn->current() );
      //if (inNode -> getGroupid() ==false) {
      hitrate = (float)inNode->getQueryhits()/(float)inNode->getQuerymessage();
      //total_hits = total_hits + (inNode->getQueryhits());
      cpu_cost = inNode->getrxmessage()+(inNode->getprmessage())*(inNode->getItem()).size();
      disk_cost = (inNode->getItem()).size();
      fitness = ((5*hitrate)-(1.1-(disk_cost/100))-(1.1-(cpu_cost/50000)))+5;
      fitness = fitness * fitness;
      //fitness = exp((10*hitrate)-((disk_cost/100)+(cpu_cost/50000)));
      if (inNode -> getGroupid() == false) {
        //cout << fitness << "\t" << inNode -> getprob() << "\t" << inNode -> getConnectionlimit() << "\t" <<  inNode -> getTTL() << endl;
        cout << inNode -> getprob() << "\t" << fitness << "\t" << hitrate << "\t" << cpu_cost << "\t" <<  disk_cost << endl;
      }
      //new_nodes++;
    }
  //}
  //outdata.close();
  return 0;
}
