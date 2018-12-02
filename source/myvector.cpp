#include "myvector.hpp"

#include <iostream>

MyVectorContainer AllVectors;

myvector::myvector(){}

myvector::myvector(const myvector& obj)
:values(obj.values),id(obj.id){}

myvector::myvector(std::vector<coord> &v, std::string &id)
:values(v), id(id) {}

myvector::myvector(std::vector<coord> &v)
:values(v), id("") {}

myvector::~myvector(){}

void myvector::print(std::ostream &out) const{
  for(size_t i=0; i<this->values.size(); i++){
    out << this->values.at(i) << " ";
  }
  //out << std::endl << "-------------------" <<  std::endl;
}

int myvector::size() const { return values.size(); }

std::string myvector::get_id() const {return id;}

std::vector<coord> myvector::getCoords() const { return values;}

std::vector<coord>::const_iterator myvector::begin() const {
  return values.begin();
}

std::vector<coord>::const_iterator myvector::end() const {
  return values.end();
}
