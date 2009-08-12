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
  
  ifstream indata("090615.dat",ios::in);

  int nodes = 100;
  int ttl = 4;
  int k = 100; //the nunber of items
  int forl = 0;
  int ctime, qtime, jtime, ftime, ltime;
  double time = 0;

  Ran1Random ran_no(-1);
  EventScheduler sched;
  
  auto_ptr<SimpleNetwork> Net_ptr (new SimpleNetwork(ran_no));

  float fitness_sum = 0;
  float a[nodes]; //fitness
  float b[nodes];
  float c[nodes]; //probability
  
  for(int i = 0; i < nodes ; i++) {
    indata >> b[i] >> c[i];
    //cout << b[i] << "\t" << c[i] << endl;
    a[i] = 0;
    fitness_sum += b[i];
  }
  for(int i = 1 ; i < nodes ; i++) {
      a[i] = b[i-1];
      b[i] += a[i];
      //cout << a[i] << "\t" << b[i] << endl;
  }

  float select_father;
  float select_mother;
  float father_prob;
  float mother_prob;

  
  for(int i = 0 ; i < nodes ; i++) {
      float pure_cross;
      //selection of father
      select_father = ran_no.getDouble01()*fitness_sum;
      for(int f = 0 ; f < nodes ; f++) {
          if(a[f]<select_father && b[f]>select_father) {
              father_prob = c[f];
              //cout<<c[f]<<endl;
          }
     // else {}
      }
      //seletion of mother
      select_mother = ran_no.getDouble01()*fitness_sum;
      for(int m = 0 ; m < nodes ; m++) {
          if(a[m]<select_mother && b[m]>select_mother) {
              mother_prob = c[m];
              //cout<<c[m]<<endl;
          }
      //else{} 
      }
      pure_cross = (father_prob+ mother_prob)/2; //crossover without mutation
      //cout<<pure_cross<<endl;
      //cout << "before mutation : "<< pure_cross << endl;
      //mutation process
      //maximum mutation is 10% of crossover, minimum mutation is 1% of crossover
      if (ran_no.getDouble01() > 0.5) {
         // cout << 'a' << endl;
          c[i] = pure_cross + ran_no.getDouble((pure_cross)*0.01, 0); 
          if (c[i] > 1) {
              c[i] = 0.9999999;
              //cout << 'a' << endl;
          }
      }
      else {
          //cout << 'b' <<endl;
          c[i] = pure_cross - ran_no.getDouble((pure_cross)*0.01, 0);
          if (c[i] < 0) {
              c[i] = 0.0000001;
              //cout << 'b' << endl;
          }
      }
     //cout << c[i] << endl;
     //c[i] = pure_cross;
   }
   
   // nodes creation of next generation
   for(int i=0; i< nodes; i++) {
       Net_ptr->create(1, c[i]);
       //cout << c[i] << endl;
   }

  std::set<std::string> items = rstringGenerator(k, 10, ran_no);
  std::set<std::string>::const_iterator item_it;

  time += 10;

  UniformNodeSelector uns(ran_no);
  for (item_it = items.begin(); item_it != items.end(); item_it++) {
      std::string item = *item_it;
      ctime = time + ran_no.getExp(100.0);
      Action* c_action = new CacheAction(sched, ran_no, uns, *Net_ptr.get(), item, ttl);
      sched.at(ctime, c_action);
/*
      jtime = ctime + ran_no.getExp(720.0);
      Action* j_action = new JoinAction(sched, ran_no, *Net_ptr.get());
      sched.at(jtime, j_action);
*/
   
      UniformNodeSelector q_start(ran_no);
      for (int iter = 0; iter < 100; iter++) {
          Action* q_action = new QueryAction(sched, ran_no, q_start, *Net_ptr.get(), item, ttl);
          qtime = jtime +ran_no.getExp(3600.0);
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
      int total_hits = 0;
      while (totn->moveNext() ) {
          SimpleNode* inNode = dynamic_cast<SimpleNode*> (totn->current() );

          float hitrate;
          int cpu_cost;
          int disk_cost;
          float prob;
          float fitness;

          hitrate = (float)inNode->getQueryhits()/(float)inNode->getQuerymessage();
          total_hits = total_hits + (inNode->getQueryhits());
          cpu_cost = inNode->getrxmessage()+(inNode->getprmessage())*(inNode->getItem()).size();
          disk_cost = (inNode->getItem()).size();

          fitness = 10*(exp((0.5*hitrate)-((0.2*disk_cost/100)+(0.3*cpu_cost/1000000))));
          fitness = fitness * fitness;
          //cout<<fitness<<"\t"<<inNode->getprob() << endl; 
          //cout << hitrate << "\t" << disk_cost << "\t" << cpu_cost << endl;
      }
      cout << total_hits << endl;
      return 0;
}
