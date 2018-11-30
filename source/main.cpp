#include <iostream>
#include <fstream>

#include "ReadInput.hpp"
#include "myvector.hpp"
#include "HashTable.hpp"
#include "utility.hpp"
#include "ErrorCodes.hpp"
#include "ClusterSpace.hpp"
#include "myvector.hpp" //so i can set AllVectors

#define dimension 203

using namespace std;
void SetAlgorithmChoices(int i,string &init, string &assign,string &update);

int main(int argc, char** argv){
  /*****************PREPROCESSING*************************************/
  ParseArguments(argc,argv);
  //open input file
  ifstream data = OpenInFile(CmdArgs::InputFile);
  //read data from file
  AllVectors = ReadDataset(data,dimension);
  cout << "Read input set of " << AllVectors.size() << "vectors" << endl;
  data.close();
  //Initialize Hashtables
  vector<HashTable*> LSH_Hashtables(CmdArgs::L);
  for(int i=0; i<CmdArgs::L; i++){
    LSH_Hashtables[i]=new HashTable(AllVectors,CmdArgs::Metric,dimension,"lsh");
    //LSH_Hashtables[i]->PrintBuckets();
  }
  HashTable HypercubeHashtable(AllVectors,CmdArgs::Metric,dimension,"hypercube");
  //HypercubeHashtable.PrintBuckets();
  //open outfile
  if(CmdArgs::OutFile.empty()){
    cout << endl << "Provide outfile path:" << endl;
    cin >> CmdArgs::OutFile;
  }
  ofstream outfile = OpenOutFile(CmdArgs::OutFile);
  /****************CLUSTERING**************************************************/
  for(int i=0; i<12; i++){
    string init,assign,update;
    SetAlgorithmChoices(i,init,assign,update);
    //Initialize
    ClusterSpace S(AllVectors,init,assign,update);
    //Assign and Update
    S.RunClusteringAlgorithms(AllVectors,LSH_Hashtables);
  }
  //cleanup
  for(int i=0; i<CmdArgs::L; i++){
    delete LSH_Hashtables[i];
  }
  outfile.close();
  return OK;
}

void SetAlgorithmChoices(int i,string &init, string &assign,string &update){
  cout << i << ".";
    if(i<=5){  //0-5
      init.assign("Random Init");
    }
    else{      //6-11
      init.assign("K-means++");
    }
    //Assignment&Update**************************************
    if(i%6<=1){       //0,1 6,7 Lloyd's
      assign.assign("Lloyd's");
      if(i%2 == 0){               //0,6 K-Means
        update.assign("K-means");
      }
      else{                       //1,7 PAM
        update.assign("PAM");
      }
    }
    else if(i%6<=3){  //2,3 8,9 Range Search LSH
      assign.assign("RangeSearchLSH");
      if(i%2 == 0){               //2,8 K-Means
        update.assign("K-means");
      }
      else{                       //3,9 PAM
        update.assign("PAM");
      }
    }
    else{             //4,5 10,11 Range Search Hypercube
      assign.assign("RangeSearchHypercube");
      if(i%2 == 0){               //4,10 K-Means
        update.assign("K-means");
      }
      else{                       //5,11 PAM
        update.assign("PAM");
      }
    }
}
