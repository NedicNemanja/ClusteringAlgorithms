#include <iostream>
#include <fstream>
#include <time.h>

#include "ReadInput.hpp"
#include "myvector.hpp"
#include "HashTable.hpp"
#include "utility.hpp"
#include "ErrorCodes.hpp"
#include "ClusterSpace.hpp"
#include "myvector.hpp" //so i can set AllVectors
#include "WriteOutput.hpp"

using namespace std;
int SetAlgorithmChoices(int i,string &init, string &assign,string &update);

int main(int argc, char** argv){
  /*****************PREPROCESSING*************************************/
  ParseArguments(argc,argv);
  //open input file
  ifstream data = OpenInFile(CmdArgs::InputFile);
  //read data from file
  AllVectors = ReadDataset(data,CmdArgs::dimension);
  cout << "Read input set of " << AllVectors.size() << "vectors" << endl;
  data.close();
  //Initialize Hashtables
  vector<HashTable*> LSH_Hashtables(CmdArgs::L);
  for(int i=0; i<CmdArgs::L; i++){
    LSH_Hashtables[i]=new HashTable(AllVectors,CmdArgs::Metric,CmdArgs::dimension,"lsh");
    //LSH_Hashtables[i]->PrintBuckets();
  }
  HashTable HypercubeHashtable(AllVectors,CmdArgs::Metric,CmdArgs::dimension,"hypercube");
  //HypercubeHashtable.PrintBuckets();
  //open outfile
  if(CmdArgs::OutFile.empty()){
    cout << endl << "Provide outfile path:" << endl;
    cin >> CmdArgs::OutFile;
  }
  ofstream outfile = OpenOutFile(CmdArgs::OutFile);
  /****************CLUSTERING**************************************************/
  for(int i=0; i<12; i++){
    //if(i%2 == 1) continue; //skip PAM
    string init,assign,update;
    int code = SetAlgorithmChoices(i,init,assign,update);
    double clustering_time;
    struct timespec start,end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    //Initialize
    ClusterSpace S(AllVectors,init,assign,update);
    //Assign and Update
    S.RunClusteringAlgorithms(AllVectors,LSH_Hashtables,&HypercubeHashtable);
    //calculate time and write results to outfile
    clock_gettime(CLOCK_MONOTONIC,&end);
    double t=(end.tv_sec-start.tv_sec + (end.tv_nsec-start.tv_nsec)/1000000000.0);
    cout << "\t" << "clustering_time:" << t << "sec" << endl;
    //WriteResult(outfile, code, S, t);
  }
  //cleanup
  for(int i=0; i<CmdArgs::L; i++){
    delete LSH_Hashtables[i];
  }
  outfile.close();
  return OK;
}


int SetAlgorithmChoices(int i,string &init, string &assign,string &update){
    int code=0;
    cout << i << ".";
    if(i<=5){  //0-5
      init.assign("Random Init");
      code += 100;
    }
    else{      //6-11
      init.assign("K-means++");
      code += 200;
    }
    //Assignment&Update**************************************
    if(i%6<=1){       //0,1 6,7 Lloyd's
      assign.assign("Lloyd's");
      code += 10;
      if(i%2 == 0){               //0,6 K-Means
        code +=1;
        update.assign("K-means");
      }
      else{                       //1,7 PAM
        code +=2;
        update.assign("PAM");
      }
    }
    else if(i%6<=3){  //2,3 8,9 Range Search LSH
      assign.assign("RangeSearchLSH");
      code += 20;
      if(i%2 == 0){               //2,8 K-Means
        code +=1;
        update.assign("K-means");
      }
      else{                       //3,9 PAM
        code +=2;
        update.assign("PAM");
      }
    }
    else{             //4,5 10,11 Range Search Hypercube
      assign.assign("RangeSearchHypercube");
      code += 30;
      if(i%2 == 0){               //4,10 K-Means
        code +=1;
        update.assign("K-means");
      }
      else{                       //5,11 PAM
        code +=2;
        update.assign("PAM");
      }
    }
    return code;
}
