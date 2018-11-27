#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "myvector.hpp"
#include "Metric.hpp"

class Cluster{
    Metric* metric;
    myvector center;
    std::vector<myvector> vectors;  //points that belong to the cluster, excluding center
  public:
    Cluster(myvector &center);
    ~Cluster();
    myvector getCenter();
    void Print();
};

#endif
