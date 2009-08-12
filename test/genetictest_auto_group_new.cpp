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
  ifstream indata("090805_exponential.dat",ios::in);
  ofstream datafile1;
  ofstream datafile2;
  datafile1.open("new_small.dat");
  datafile2.open("new_big.dat");

  
  int nodes = 100;
  int candidate1 = 2;
  int candidate2 = 8;
  int node_ttl;
  int connection_limit;
  int k = 100; //the nunber of items
  //int forl = 0;
  int generation = 3000;
  int group1_portion = 10;
  int group2_portion = 90;
  
  //float a[nodes]; 
  float b[nodes]; //fitness
  float c[nodes]; //probability
  int d[nodes]; //connection
  int e[nodes]; //TTL

  float c_new[nodes]; // probability for child
  int d_new[nodes]; // connection for child
  int e_new[nodes]; // TTL for child
  
  Ran1Random ran_no(-1);
  EventScheduler sched;
  
  for(int i = 0; i < nodes ; i++) {
    indata >> b[i] >> c[i] >> d[i] >> e[i];
  }

  int father;
  float father_prob;
  int father_connection;
  int father_TTL;

  int mother;
  float mother_prob;
  int mother_connection;
  int mother_TTL;

  float pure_cross_prob;
  float pure_cross_ttl;
  float pure_cross_connection;

  //for crossover of query ignore probability
  for(int ge = 1; ge < generation; ge++) {
      datafile1 << "#-----------------------------" << endl;
      datafile2 << "#-----------------------------" << endl;
      int ctime, qtime;
      double time = 0;

      //empty network generation
      auto_ptr<SimpleNetwork> Net_ptr (new SimpleNetwork(ran_no));
      //cout << 'a' << endl;
      for(int i = 0 ; i < group1_portion ; i++) {
        int name_parent;
        //bool check_candidate = false;
        bool check_parent = false;
        int group1_candidate[candidate1];
        //selection of father
        for(int f = 0 ; f < candidate1 ; f++) {
          group1_candidate[f] = ran_no.getInt(group1_portion-1, 0);
        }
        for (int f = 0 ; f < candidate1-1 ; f++) {
          if (b[group1_candidate[f]] >= b[group1_candidate[f+1]]) {
            father = group1_candidate[f];
          }
          else {
              father = group1_candidate[f+1];
          }
        }

        father_prob = c[father];
        father_connection = d[father];
        father_TTL = e[father];
        
        while(!check_parent) {
          for(int m = 0 ; m < candidate1 ; m++) {
            group1_candidate[m] = ran_no.getInt(group1_portion-1, 0);
          }
          for (int m = 0 ; m < candidate1-1 ; m++) {
            if (b[group1_candidate[m]] >= b[group1_candidate[m+1]]) {
              mother = group1_candidate[m];
            }
            else {
                mother = group1_candidate[m+1];
            }
          }
          if(father != mother) {
              check_parent = true;
          }
        }

        mother_prob = c[mother];
        mother_connection = d[mother];
        mother_TTL = e[mother];

      pure_cross_prob = (father_prob + mother_prob)/2; //crossover without mutation
          //mutation process for query ignore probability
          //maximum mutation is 10% of crossover, minimum mutation is 1% of crossover
          if (ran_no.getDouble01() > 0.5) {
              c_new[i] = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (c_new[i] > 1) {
              c_new[i] = 0.9999999;
              }
          }
          else {
              c_new[i] = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (c_new[i] < 0) {
                  c_new[i] = 0.0000001;
              }
          }
          //for TTL
          if (mother_TTL > father_TTL) {
              pure_cross_ttl = ran_no.getInt(mother_TTL, father_TTL);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_ttl = ran_no.getInt(father_TTL, mother_TTL);
              //e_new[i] = pure_cross_ttl;
          }

          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  e_new[i] = pure_cross_ttl + 1;
              }
              else {
                  e_new[i] = pure_cross_ttl - 1;
                  if (e_new[i] < 1) {
                      e_new[i] = 1;
                  }
              }
          }
          else {
              e_new[i] = pure_cross_ttl;
          }

          //for connection
          if (mother_connection > father_connection) {
              pure_cross_connection = ran_no.getInt(mother_connection, father_connection);
          //    d_new[i] = pure_cross_connection;
          }
          else {
              pure_cross_connection = ran_no.getInt(father_connection, mother_connection);
            //  d_new[i] = pure_cross_connection;
          }
          
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  d_new[i] = pure_cross_connection + 1;
              }
              else {
                  d_new[i] = pure_cross_connection - 1;
                  if (d_new[i] < 1) {
                      d_new[i] = 1;
                  }
              }
          }
          else {
              d_new[i] = pure_cross_connection;
          }

          Net_ptr->create(i, e_new[i], d_new[i], c_new[i], true);
       }
      
      for(int i = group1_portion ; i < nodes ; i++) {
        int name_parent;
        //bool check_candidate = false;
        bool check_parent = false;
        int group2_candidate[candidate2];
        //selection of father
        for(int f = 0 ; f < candidate2 ; f++) {
          group2_candidate[f] = ran_no.getInt(nodes-1, group1_portion);
        }
        for (int f = 0 ; f < candidate2-1 ; f++) {
          if (b[group2_candidate[f]] >= b[group2_candidate[f+1]]) {
            father = group2_candidate[f];
          }
          else {
              father = group2_candidate[f+1];
          }
        }

        father_prob = c[father];
        father_connection = d[father];
        father_TTL = e[father];
        
        while(!check_parent) {
          for(int m = 0 ; m < candidate2 ; m++) {
            group2_candidate[m] = ran_no.getInt(nodes-1, group1_portion);
          }
          for (int m = 0 ; m < candidate2-1 ; m++) {
            if (b[group2_candidate[m]] >= b[group2_candidate[m+1]]) {
              mother = group2_candidate[m];
            }
            else {
                mother = group2_candidate[m+1];
            }
          }
          if(father != mother) {
              check_parent = true;
          }
        }

        mother_prob = c[mother];
        mother_connection = d[mother];
        mother_TTL = e[mother];

        //check_candidate = false;
        //name_parent = f;
        //while(!check_parent) {
        //seletion of mother

      pure_cross_prob = (father_prob + mother_prob)/2; //crossover without mutation
          //mutation process for query ignore probability
          //maximum mutation is 10% of crossover, minimum mutation is 1% of crossover
          if (ran_no.getDouble01() > 0.5) {
              c_new[i] = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (c_new[i] > 1) {
              c_new[i] = 0.9999999;
              }
          }
          else {
              c_new[i] = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (c_new[i] < 0) {
                  c_new[i] = 0.0000001;
              }
          }
          //for TTL
          if (mother_TTL > father_TTL) {
              pure_cross_ttl = ran_no.getInt(mother_TTL, father_TTL);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_ttl = ran_no.getInt(father_TTL, mother_TTL);
              //e_new[i] = pure_cross_ttl;
          }

          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  e_new[i] = pure_cross_ttl + 1;
              }
              else {
                  e_new[i] = pure_cross_ttl - 1;
                  if (e_new[i] < 1) {
                      e_new[i] = 1;
                  }
              }
          }
          else {
              e_new[i] = pure_cross_ttl;
          }

          //for connection
          if (mother_connection > father_connection) {
              pure_cross_connection = ran_no.getInt(mother_connection, father_connection);
//              d_new[i] = pure_cross_connection;
          }
          else {
              pure_cross_connection = ran_no.getInt(father_connection, mother_connection);
  //            d_new[i] = pure_cross_connection;
          }
          
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  d_new[i] = pure_cross_connection + 1;
              }
              else {
                  d_new[i] = pure_cross_connection - 1;
                  if (d_new[i] < 1) {
                      d_new[i] = 1;
                  }
              }
          }
          else {
              d_new[i] = pure_cross_connection;
          }

          Net_ptr->create(i, e_new[i], d_new[i], c_new[i], false);
       }

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

      while (totn->moveNext() ) {
          SimpleNode* inNode = dynamic_cast<SimpleNode*> (totn->current() );
          hitrate = (float)inNode->getQueryhits()/(float)inNode->getQuerymessage();
          cpu_cost = inNode->getrxmessage()+(inNode->getprmessage())*(inNode->getItem()).size();
          disk_cost = (inNode->getItem()).size();
          //fitness = ((5*hitrate)-(1.1-(disk_cost/100))+(1.1-(cpu_cost/15000)))+5;
          //fitness = ((5*hitrate)-((disk_cost/100))+((cpu_cost/15000)))+5;
          fitness = exp((10*hitrate)-((disk_cost/100)+(cpu_cost/15000)));
          //fitness = fitness * fitness;
          b[inNode->getAddress()] = fitness;
          c[inNode->getAddress()] = c_new[inNode->getAddress()];
          d[inNode->getAddress()] = d_new[inNode->getAddress()];
          e[inNode->getAddress()] = e_new[inNode->getAddress()];
          if (inNode -> getGroupid() == true){
              datafile1<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << endl; 
          }
          else {
              datafile2<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << endl; 
          }
      }
    }
    datafile1.close();
    datafile2.close();
    return 0;
}
