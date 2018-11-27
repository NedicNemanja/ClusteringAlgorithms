#include <iostream>
#include <fstream>

#include "ReadInput.hpp"
#include "myvector.hpp"
#include "HashTable.hpp"
#include "utility.hpp"
#include "ErrorCodes.hpp"
#include "ClusterSpace.hpp"

using namespace std;

#define dimension 203

int main(int argc, char** argv){
  /*****************PREPROCESSING*************************************/
  ParseArguments(argc,argv);
  //open input file
  ifstream data = OpenInFile(CmdArgs::InputFile);
  //read data from file
  vector<myvector> vectors = ReadDataset(data,dimension);
  cout << "Read input set of " << vectors.size() << "vectors" << endl;
  data.close();
  //Initialize Hashtables
  vector<HashTable*> LSH_Hashtables(CmdArgs::L);
  for(int i=0; i<CmdArgs::L; i++){
    LSH_Hashtables[i]=new HashTable(vectors,CmdArgs::Metric,dimension,"lsh");
    //LSH_Hashtables[i]->PrintBuckets();
  }
  HashTable HypercubeHashtable(vectors,CmdArgs::Metric,dimension,"hypercube");
  //HypercubeHashtable.PrintBuckets();
  //open outfile
  if(CmdArgs::OutFile.empty()){
    cout << endl << "Provide outfile path:" << endl;
    cin >> CmdArgs::OutFile;
  }
  ofstream outfile = OpenOutFile(CmdArgs::OutFile);
  /****************CLUSTERING**************************************************/
  for(int i=0; i<12; i++){
    cout << "Starting ";
    //Initialization**********************************
    ClusterSpace* S;
    if(i<=5){  //0-5
      cout << "Random Init | ";
      S = new ClusterSpace(vectors,"Random");
    }
    else{      //6-11
      cout << "K-means++ Init | ";
      S = new ClusterSpace(vectors,"K-means++");
    }
    //Assignment**************************************
    if(i%6<=1){       //0,1 6,7 Lloyd's
      cout << "Lloyd's Assignment | ";
      //S->LloydsAssignment();
    }
    else if(i%6<=3){  //2,3 8,9 Range Search LSH
      cout << "Range Search LSH Assignment | ";
    }
    else{             //4,5 10,11 Range Search Hypercube
      cout << "Range Search Hypercube Assignment | ";
    }
    //Update*******************************************
    if(i%2 == 0){ //0,2,4,6,8,10 K-Means
      cout << "K-means Update" << endl;
    }
    else{         //1,3,5,7,9,11 PAM
      cout << "Pam improved like Lloyd's Update" << endl;
    }
    delete S;
  }
  //cleanup
  for(int i=0; i<CmdArgs::L; i++){
    delete LSH_Hashtables[i];
  }
  outfile.close();
  return OK;
}
