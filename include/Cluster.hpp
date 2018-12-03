#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "myvector.hpp"
#include "Metric.hpp"

#include <vector>

class Cluster{
    myvector centroid;
    vector_index medoid; //-1 if center is not part of the dataset
    //vector_indexes of points that belong to the cluster, excluding centers
    std::vector<vector_index> members;
  public:
    Cluster(myvector &center);
    //in case center is part of the dataset, save its index
    Cluster(vector_index c_index);
    ~Cluster();
    myvector getCenter();//get the vector that used as center of the cluster
    vector_index getMedoid();
    void setMedoid(vector_index);
    void setCentroid(myvector);
    vector_index setMedoid();
    std::vector<vector_index> getMembers();
    void Print();
    void AddVector(vector_index vindex);
    /*Calc dissimilarity of v from cluster*/
    double ClusterDissimilarity(myvector &v);
};

#endif
