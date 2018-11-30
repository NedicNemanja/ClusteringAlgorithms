#include "ReadInput.hpp"
#include "ErrorCodes.hpp"
#include "myvector.hpp"
#include "ParsingCSV.hpp"

#include <stdlib.h> //atoi
#include <string.h> //strcmp
#include <sstream>
#include <utility>  //std::move()
#include <ctype.h> //tolower()
using namespace std;

namespace CmdArgs{
    std::string InputFile;
    std::string ConfFile;
    std::string OutFile;
    std::string Metric;
    int number_of_clusters,L,K;
    int MAX_NUM_RANGESEARCH_ITERATIONS;
    int HYPERCUBE_PROBES
}

/*Parse cmd line arguments and read from cin any that are missing*/
void ParseArguments(int argc, char** argv){
  int arg_index=1;
  int Input_flag=0, Conf_flag=0, Out_flag=0, Metric_flag=0;

  //scan all arguements to find identifiers
  while(arg_index < argc){
    //make sure there is an argument to check
    if(argv[arg_index] == NULL)
      break;

    if(strcmp(argv[arg_index],"-i")==0 && Input_flag==0 && argv[arg_index+1]!=NULL){
      CmdArgs::InputFile.assign(argv[++arg_index]);
      Input_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-c")==0 && Conf_flag==0 && argv[arg_index+1]!=NULL){
      CmdArgs::ConfFile.assign(argv[++arg_index]);
      Conf_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-o")==0 && Out_flag==0 && argv[arg_index+1]!=NULL){
      CmdArgs::OutFile.assign(argv[++arg_index]);
      Out_flag = 1;
      arg_index++;
      continue;
    }
    if(strcmp(argv[arg_index],"-d")==0 && Metric_flag==0 && argv[arg_index+1]!=NULL){
      CmdArgs::Metric.assign(argv[++arg_index]);
      Metric_flag = 1;
      arg_index++;
      continue;
    }
    fprintf(stderr, "Unknown argument: %s\n", argv[arg_index]);
    exit(UNKNOWN_CMDARGUMENT);
  }

  //check that you got all the arguements
  if(Input_flag == 0){
    cout << "Provide dataset path: ";
    cin >> CmdArgs::InputFile;
  }
  if(Conf_flag == 0){
    cout << "Provide cluster.conf path: ";
    cin >> CmdArgs::ConfFile;
  }
  if(Out_flag == 0){
    cout << "Provide outfile path: ";
    cin >> CmdArgs::OutFile;
  }
  ReadConfigurationFile(CmdArgs::ConfFile);
}

void ReadConfigurationFile(std::string filename){
  int clusters_flag=0,hash_func_flag=0,hashtables_flag=0;
  ifstream file = OpenInFile(filename);
  string field_name;
  while(!file.eof()){
    file >> field_name;
    if(field_name == "number_of_clusters:"){
      if(!file.eof()){
        file >> CmdArgs::number_of_clusters;
        clusters_flag=1;
        field_name.clear();
        continue;
      }
    }
    if(field_name == "number_of_hash_functions:"){
      if(!file.eof()){
        file >> CmdArgs::K;
        hash_func_flag=1;
        field_name.clear();
        continue;
      }
    }
    if(field_name == "number_of_hashtables:"){
      if(!file.eof()){
        file >> CmdArgs::L;
        hashtables_flag=1;
        field_name.clear();
        continue;
      }
    }
    if(!file.eof()){
      cerr << "Unknown argument in .conf: -" << field_name <<"-" << endl;
      exit(UNKNOWN_CMDARGUMENT);
    }
  }
  //check that all is set, if not use default values
  if(clusters_flag == 0){
    cout << "Default number_of_clusters: 3" << endl;
    CmdArgs::number_of_clusters = 3;
  }
  else
    cout << "number_of_clusters: " << CmdArgs::number_of_clusters << endl;

  if(hash_func_flag == 0 ){
    cout << "Default number_of_hash_functions (K): 4" << endl;
    CmdArgs::K=4;
  }
  else
    cout << "number_of_hash_functions (K): " << CmdArgs::K << endl;

  if(hashtables_flag == 0){
    cout << "Default number_of_hashtables (L): 5" << endl;
    CmdArgs::L=5;
  }
  else
    cout << "number_of_hashtables (L): " << CmdArgs::L << endl;

  file.close();
}


MyVectorContainer ReadDataset(ifstream &data, int dim){
  cout << endl << "Reading dataset from disk..." << endl;
  //read coords from input and initialize vectors
  string id;
  vector<coord> coords(dim);  //temp vector that gets overwritten every loop
  vector<myvector> vectors;
  while(GetVector(data, coords, id, dim)){
      myvector vec(coords,id);
      //vec.print(cout);
      vectors.push_back(vec);
  }
  return vectors;
}

//read coordinates of a vector and return true for success, else false
bool GetVector(ifstream &data,vector<coord> &coords, string &id, int dim){
  //read id
  CSVRow row;
  int row_index=0;
  data >> row;
  if(data.eof()) return false;
  id = row[row_index++];
  //read coords
  for(int i=0; i<dim; i++){
    coords.at(i) = stod(row[row_index++]);
  }
  return true;
}

//Check the first vector and find its dimension
int FindDimension(ifstream &data){
  int dimension=0;
  streampos oldpos = data.tellg();  // stores the stream position
  string line;
  getline(data,line);               //get the whole vector
  istringstream is(line);         //treat line like a stream
  string id;                      //skip first string, that's the id
  is >> id;
  coord n;
  while( is >> n ) {                //count coords in line
    dimension++;
  }
  data.seekg (oldpos);   // get back to the position
  return dimension;
}

ifstream OpenInFile(string &filename){
  ifstream file(filename);
  if(!file.is_open()){
    cerr << "Couldn't open " << filename << endl;
    exit(FILE_ACCESS);
  }
  return file;
}

ofstream OpenOutFile(string &filename){
  ofstream file(filename);
  if(!file.is_open()){
    cerr << "Couldn't open " << filename << endl;
    exit(FILE_ACCESS);
  }
  return file;
}
