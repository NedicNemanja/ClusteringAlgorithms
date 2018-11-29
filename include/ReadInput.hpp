#ifndef READINPUT_HPP
#define READINPUT_HPP

#include <iostream>
#include <fstream>
#include <string>

#include "myvector.hpp"

namespace CmdArgs{
    extern std::string InputFile;
    extern std::string ConfFile;
    extern std::string OutFile;
    extern std::string Metric;
    extern int K,L,number_of_clusters;
};

/*Parse cmd line arguments and read from cin any that are missing*/
void ParseArguments(int argc, char** argv);

/*Parse .conf file*/
void ReadConfigurationFile(std::string filename);

MyVectorContainer ReadDataset(std::ifstream &data,int dim);

//Check the first vector from stream and find its dimension
int FindDimension(std::ifstream &data);

//read coordinates+id of a myvector and return true for success, else false
bool GetVector(std::ifstream &data,std::vector<coord> &coords,std::string&, int);

std::ifstream OpenInFile(std::string &filename);
std::ofstream OpenOutFile(std::string &filename);


#endif
