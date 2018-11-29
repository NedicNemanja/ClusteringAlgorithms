#include "myvector.hpp"

#include <iostream>

MyVectorContainer AllVectors;

myvector::myvector(){}

myvector::myvector(const myvector& obj)
:values(obj.values),id(obj.id){}

myvector::myvector(std::vector<coord> &v, std::string &id)
:values(v), id(id) {}

myvector::~myvector(){}

void myvector::print(std::ostream &out){
  for(size_t i=0; i<this->values.size(); i++){
    out << this->values.at(i) << " ";
  }
  //out << std::endl << "-------------------" <<  std::endl;
}

int myvector::size(){ return values.size(); }

std::string myvector::get_id(){return id;}

std::vector<coord>::iterator myvector::begin(){ return values.begin(); }
std::vector<coord>::iterator myvector::end(){ return values.end(); }
