#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "myvector.hpp"
#include "Metric.hpp"

class Cluster{
    Metric* metric;
    myvector center;
    std::unordered_set<string> assigned;  //id's of points that belong to the cluster, excluding centers
  public:
    Cluster(myvector &center);
    ~Cluster();
    myvector getCenter();
    void Print();
    void AddVector(myvector &v);
};

#endif
