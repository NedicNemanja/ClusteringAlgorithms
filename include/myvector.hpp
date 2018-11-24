#ifndef MYVECTOR_H
#define MYVECTOR_H

#include <vector>
#include <string>
#include <iostream>

typedef double coord;

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

#endif
