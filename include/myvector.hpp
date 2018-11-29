#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <vector>
#include <string>
#include <iostream>

typedef double coord; //myvector coordinates

class myvector {
    std::vector<coord> values;
    std::string id;
  public:
    myvector();
    myvector(const myvector& obj);
    myvector(std::vector<coord> &v, std::string &id);
    ~myvector();
    void print(std::ostream &out);
    int size();
    std::string get_id();
    std::vector<coord>::iterator begin();
    std::vector<coord>::iterator end();
};

typedef std::vector<myvector> MyVectorContainer;
typedef int vector_index;  //used to locate a myvector in MyVectorPool

extern MyVectorContainer AllVectors;

#endif
