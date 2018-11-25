#include <iostream>
#include <fstream>

#include "ReadInput.hpp"
#include "myvector.hpp"
#include "HashTable.hpp"
#include "utility.hpp"
#include "ErrorCodes.hpp"

using namespace std;

#define dimension 203

int main(int argc, char** argv){
  /*****************PREPROCESSING*************************************/
  ParseArguments(argc,argv);
  //open input file
  ifstream data = OpenInFile(CmdArgs::InputFile);
  //read data from file
  list<myvector> vectors = ReadDataset(data,dimension);
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
  /****************LSH****************************************************/
  //cleanup
  for(int i=0; i<CmdArgs::L; i++){
    delete LSH_Hashtables[i];
  }
  outfile.close();
  return OK;
}
