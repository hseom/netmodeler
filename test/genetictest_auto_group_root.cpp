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
  ifstream indata("090808_root.dat",ios::in);
  ofstream datafile1;
  ofstream datafile2;
  datafile1.open("test_small_1.dat");
  datafile2.open("test_big_1.dat");

  //int nodes = 100;

  float q_ignore;
  int node_ttl;
  int connection_limit;
  int k = 100; //the nunber of items
  //int forl = 0;
  int generation = 1000;
  int group1 = 9;
  int group2 = 81;

  float small_fit[group1]; //fitness 
  float small_prob[group1]; //probability
  int small_connection[group1]; //connection
  int small_TTL[group1]; //TTL

  float big_fit[group2]; //fitness
  float big_prob[group2]; //probability
  int big_connection[group2]; //connection 
  int big_TTL[group2]; //TTL


  for (int i = 0; i < group1+group2; i++) {
      if (i < group1) {
          indata >> small_fit[i] >> small_prob[i] >> small_connection[i] >> small_TTL[i];
      }
      else { 
          indata >> big_fit[i-group1] >> big_prob[i-group1] >> big_connection[i-group1] >> big_TTL[i-group1];
      }
  }

  Ran1Random ran_no(-1);
  EventScheduler sched;

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
      //small group fitness sorting 
      float temp_fit = 0;
      float temp_prob = 0;
      int temp_connection = 0;
      int temp_TTL = 0;

      for(int i = 0; i < group1 ; i++) {
          for(int j = 0; j < (group1-i) ; j++) {
              if (small_fit[j] < small_fit[j+1]) {
                  //fitness sorting
                  temp_fit = small_fit[j];
                  small_fit[j] = small_fit[j+1];
                  small_fit[j+1] = temp_fit;
                  //probability sorting
                  temp_prob = small_prob[j];
                  small_prob[j] = small_prob[j+1];
                  small_prob[j+1] = temp_prob;
                  //connection sorting
                  temp_connection = small_connection[j];
                  small_connection[j] = small_connection[j+1];
                  small_connection[j+1] = temp_connection;
                  //TTL sorting
                  //cout << j << "\t" << small_TTL[j] << endl;
                  //cout << j+1 << "\t" << small_TTL[j+1] << endl;
                  temp_TTL = small_TTL[j];
                  small_TTL[j] = small_TTL[j+1];
                  small_TTL[j+1] = temp_TTL;
                  //cout << j << "\t" << small_TTL[j] << endl;
                  //cout << j+1 << "\t" << small_TTL[j+1] << endl;
              }
          }
      }
      //for(int i = 0 ; i < group1 ; i++) {
        //  cout << small_TTL[i] << endl;
      //}
      for(int i = 0 ; i < 3 ; i++) {
          pure_cross_prob = (small_prob[i] + small_prob[i]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //for TTL
          pure_cross_ttl = ran_no.getInt(small_TTL[i], small_TTL[i]);
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }
          //for connection
          pure_cross_connection = ran_no.getInt(small_connection[i], small_connection[i]);
          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(3*i, node_ttl, connection_limit, q_ignore, true);
          pure_cross_prob = (small_prob[i] + small_prob[i+1]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (small_TTL[i] > small_TTL[i+1]) {
              pure_cross_ttl = ran_no.getInt(small_TTL[i], small_TTL[i+1]);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_ttl = ran_no.getInt(small_TTL[i+1], small_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }
          //for connection
           if (small_connection[i] > small_connection[i+1]) {
              pure_cross_connection = ran_no.getInt(small_connection[i], small_connection[i+1]);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_connection = ran_no.getInt(small_connection[i+1], small_connection[i]);
              //e_new[i] = pure_cross_ttl;
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(3*i+1, node_ttl, connection_limit, q_ignore, true);
          pure_cross_prob = (small_prob[i] + small_prob[i+2]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (small_TTL[i] > small_TTL[i+2]) {
              pure_cross_ttl = ran_no.getInt(small_TTL[i], small_TTL[i+2]);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_ttl = ran_no.getInt(small_TTL[i+2], small_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }
          //for connection
           if (small_connection[i] > small_connection[i+2]) {
              pure_cross_connection = ran_no.getInt(small_connection[i], small_connection[i+2]);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_connection = ran_no.getInt(small_connection[i+2], small_connection[i]);
              //e_new[i] = pure_cross_ttl;
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(3*i+2, node_ttl, connection_limit, q_ignore, true);
       }
       for(int i = 0; i < group2 ; i++) {
          for(int j = 0; j < (group2-i) ; j++) {
              if (big_fit[j] < big_fit[j+1]) {
                  //fitness sorting
                  temp_fit = big_fit[j];
                  big_fit[j] = big_fit[j+1];
                  big_fit[j+1] = temp_fit;
                  //probability sorting
                  temp_prob = big_prob[j];
                  big_prob[j] = big_prob[j+1];
                  big_prob[j+1] = temp_prob;
                  //connection sorting
                  temp_connection = big_connection[j];
                  big_connection[j] = big_connection[j+1];
                  big_connection[j+1] = temp_connection;
                  //TTL sorting
                  temp_TTL = big_TTL[j];
                  big_TTL[j] = big_TTL[j+1];
                  big_TTL[j+1] = temp_TTL;
              }
          }
      }

     for(int i = 0 ; i < 9 ; i++) {
          pure_cross_prob = (big_prob[i] + big_prob[i]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //for TTL
          pure_cross_ttl = big_TTL[i];
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
          pure_cross_connection = big_connection[i];
          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+9, node_ttl, connection_limit, q_ignore, false);
          
          pure_cross_prob = (big_prob[i] + big_prob[i+1]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
                  q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (big_TTL[i] > big_TTL[i+1]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+1]);
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+1], big_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+1]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+1]);
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+1], big_connection[i]);
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+10, node_ttl, connection_limit, q_ignore, false);
          
          pure_cross_prob = (big_prob[i] + big_prob[i+2]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL

          if (big_TTL[i] > big_TTL[i+2]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+2]);
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+2], big_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+2]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+2]);
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+2], big_connection[i]);
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+11, node_ttl, connection_limit, q_ignore, false);

          pure_cross_prob = (big_prob[i] + big_prob[i+3]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (big_TTL[i] > big_TTL[i+3]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+3]);
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+3], big_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+3]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+3]);
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+3], big_connection[i]);
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+12, node_ttl, connection_limit, q_ignore, false);

          pure_cross_prob = (big_prob[i] + big_prob[i+4]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (big_TTL[i] > big_TTL[i+4]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+4]);
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+4], big_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+4]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+4]);
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+4], big_connection[i]);
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+13, node_ttl, connection_limit, q_ignore, false);
          
          pure_cross_prob = (big_prob[i] + big_prob[i+5]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (big_TTL[i] > big_TTL[i+5]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+5]);
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+5], big_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+5]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+5]);
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+5], big_connection[i]);
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+14, node_ttl, connection_limit, q_ignore, false);
        
          pure_cross_prob = (big_prob[i] + big_prob[i+6]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (big_TTL[i] > big_TTL[i+6]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+6]);
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+6], big_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+6]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+6]);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+6], big_connection[i]);
              //e_new[i] = pure_cross_ttl;
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+15, node_ttl, connection_limit, q_ignore, false);
        
          pure_cross_prob = (big_prob[i] + big_prob[i+7]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (big_TTL[i] > big_TTL[i+7]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+7]);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+7], big_TTL[i]);
              //e_new[i] = pure_cross_ttl;
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+7]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+7]);
              //e_new[i] = pure_cross_ttl;
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+7], big_connection[i]);
              //e_new[i] = pure_cross_ttl;
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+16, node_ttl, connection_limit, q_ignore, false);
        
          pure_cross_prob = (big_prob[i] + big_prob[i+8]) / 2;
          //mutation process for query ignore probability
          //maximum mutation is 5% of crossover, minimum mutation is 0% of crossover
          if (ran_no.getDouble01() > 0.5) {
              q_ignore = pure_cross_prob + ran_no.getDouble((pure_cross_prob)*0.05, 0); 
              if (q_ignore > 1) {
              q_ignore = 0.9999999;
              }
          }
          else {
              q_ignore = pure_cross_prob - ran_no.getDouble((pure_cross_prob)*0.05, 0);
              if (q_ignore < 0) {
                  q_ignore = 0.0000001;
              }
          }
          //Crossover for TTL
          if (big_TTL[i] > big_TTL[i+8]) {
              pure_cross_ttl = ran_no.getInt(big_TTL[i], big_TTL[i+8]);
          }
          else {
              pure_cross_ttl = ran_no.getInt(big_TTL[i+8], big_TTL[i]);
          }
          //mutation process for TTL
          //probability that mutation occurs
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  node_ttl = pure_cross_ttl + 1;
              }
              else {
                  node_ttl = pure_cross_ttl - 1;
                  if (node_ttl < 1) {
                      node_ttl = 1;
                  }
              }
          }
          else {
              node_ttl = pure_cross_ttl;
          }

          //for connection
           if (big_connection[i] > big_connection[i+8]) {
              pure_cross_connection = ran_no.getInt(big_connection[i], big_connection[i+8]);
          }
          else {
              pure_cross_connection = ran_no.getInt(big_connection[i+8], big_connection[i]);
          }

          //mutation process for connection limit
          if (ran_no.getDouble01() < 0.001) {
              if (ran_no.getDouble01() > 0.5) {
                  connection_limit = pure_cross_connection + 1;
              }
              else {
                  connection_limit = pure_cross_connection - 1;
                  if (connection_limit < 1) {
                      connection_limit = 1;
                  }
              }
          }
          else {
              connection_limit = pure_cross_connection;
          }

          Net_ptr->create(9*i+17, node_ttl, connection_limit, q_ignore, false);
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
          //fitness = ((5*hitrate)-(1.1-(disk_cost/100))-(1.1-(cpu_cost/50000)))+5;
          //fitness = ((5*hitrate)-((disk_cost/100))+((cpu_cost/15000)))+5;
          fitness = exp((10*hitrate)-((disk_cost/100)+(cpu_cost/50000)));
          //fitness = fitness * fitness;
          if(inNode->getGroupid() == true) {
              small_fit[inNode->getAddress()] = fitness;
              small_prob[inNode->getAddress()] = inNode->getprob();
              small_TTL[inNode->getAddress()] = inNode->getTTL();
              cout << "address " << inNode->getAddress() << "\t" << "TTL " << small_TTL[inNode->getAddress()] << endl;
              small_connection[inNode->getAddress()] = inNode->getConnectionlimit();
              datafile1<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << inNode->getConnectionlimit() << "\t" << inNode->getTTL() << "\t" << endl;
          }
          
          else {
              big_fit[inNode->getAddress()-group1] = fitness;
              big_prob[inNode->getAddress()-group1] = inNode->getprob();
              big_TTL[inNode->getAddress()-group1] = inNode->getTTL();
              big_connection[inNode->getAddress()-group1] = inNode->getConnectionlimit();
              datafile2<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << inNode->getConnectionlimit() << "\t" << inNode->getTTL() << "\t" << endl; 
          }
          /*
          if(inNode->getGroupid() == true){
              //datafile1 << fitness << endl;
              //datafile1<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << endl; 
              datafile1<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << inNode->getConnectionlimit() << "\t" << inNode->getTTL() << "\t" << endl;
          }
          else {
              //datafile2 << fitness << endl;
              //datafile2<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << endl; 
              datafile2<<fitness<<"\t"<<inNode->getprob() << "\t" << hitrate << "\t" << cpu_cost << "\t" << disk_cost << "\t" << inNode->getConnectionlimit() << "\t" << inNode->getTTL() << "\t" << endl; 
          }
          */
       }
       for (int i = 0 ; i < group1 ; i++) {
           cout << "data push completed " << small_TTL[i] << endl;
       }

    }
    datafile1.close();
    datafile2.close();
    return 0;
}
