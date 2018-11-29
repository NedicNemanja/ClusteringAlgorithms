#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "myvector.hpp"
#include "Metric.hpp"

#include <vector>

class Cluster{
    vector_index center;
    //vector_indexes of points that belong to the cluster, excluding centers
    std::vector<vector_index> members;
  public:
    Cluster(vector_index center);
    ~Cluster();
    myvector getCenter();
    void Print();
    void AddVector(vector_index vindex);
};

#endif
