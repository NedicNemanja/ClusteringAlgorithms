#ifndef METRIC_HPP
#define METRIC_HPP

#include <string>
#include <cmath>

#include "myvector.hpp"

class Metric {
  public:
    const std::string name;
    Metric(std::string);
    virtual ~Metric();
    virtual unsigned int Hash(myvector& p)=0;
    virtual int dim()=0;
    virtual std::vector<long int> get_g(myvector &p)=0;
    //vector distance based on metric (euclidean,cosine,...)
    virtual double vectorDistance(std::vector<coord>::iterator first,
                                  std::vector<coord>::iterator last,
                                  std::vector<coord>::iterator first2)=0;
};

#endif
