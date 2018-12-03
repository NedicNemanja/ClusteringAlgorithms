#include "WriteOutput.hpp"
#include "ReadInput.hpp"

#include <iostream>

using namespace std;


void WriteResult(std::ofstream & outfile, int code, ClusterSpace& S,
  double clustering_time){
  outfile << "Algorithm: "<<code/100<<"x"<<(code%100)/10<<"x"<<(code%10)<<endl;

  outfile << "Metric: " << CmdArgs::Metric << endl;

  for(int i=0; i<CmdArgs::number_of_clusters; i++){
    Cluster cluster = S.getCluster(i);
    outfile<<"CLUSTER-"<< i+1 <<"{size: "<<cluster.size()<<", centroid: ";
    string centro_id = cluster.getCenter().get_id();
    if(!centro_id.empty())
      outfile << centro_id << "}" << endl;
    else{
      cluster.getCenter().print(outfile);
      outfile << "}" << endl;
    }
  }

  outfile << "clustering_time: " << clustering_time << endl;

  outfile << "Silhouette: [";
  vector<double> silhouette = S.Silhouette(AllVectors);
  double sum=0;
  for(auto it=silhouette.begin(); it!=silhouette.end(); it++){
    outfile << *it << ",";
    sum += *it;
  }
  outfile << sum/silhouette.size() << "]" << endl;

  for(int i=0; i<CmdArgs::number_of_clusters; i++){
    outfile<<"CLUSTER-"<< i+1 << "{";
    S.getCluster(i).Print(outfile);
    outfile << "}" << endl;
  }
  outfile << endl;
}
