#include "HashTable.hpp"
#include "utility.hpp"
#include "LSHEuclidean.hpp"
#include "HypercubeEuclidean.hpp"
#include "CosineSimilarity.hpp"
#include "ErrorCodes.hpp"
#include "ReadInput.hpp"  //cmdargs

#include <cmath> //pow()

using namespace std;

using CmdArgs::K;

/*num_of_vectors/div_tablesize is the tablesize if metric=euclidean.
Suggested: =2,=4,=8,=16 */
int div_tablesize = 4;

unsigned int TableSize(string metric, int num_vectors){
  if(metric == "euclidean")
    return num_vectors / div_tablesize;
  else if(metric == "cosine")
    return pow(2,CmdArgs::K);
  else
    exit(UNKNOWN_METRIC);
}

/*Use this constructor if you did not know the num of vectors and you stored
them temporarily in a list. This will insert all vectors from the list.*/
HashTable::HashTable(list<myvector>& vlist, string metric_name,int dimension,string tabletype)
:tabletype(tabletype),buckets(TableSize(metric_name,vlist.size()))
{
  if(metric_name == "euclidean"){
    if(tabletype=="lsh")
      metric = new LSH::Euclidean(dimension, buckets.size());
    else if(tabletype=="hypercube")
      metric = new Hypercube::Euclidean(dimension, buckets.size());
    else{
      cout << "Unknown tabletype: " << tabletype << endl;
      exit(UNKNOWN_METRIC);
    }
  }
  else if("cosine"){
    metric = new CosineSimilarity(dimension);
  }
  else{
    cout << "Unknown metric: " << metric_name << endl;
    exit(UNKNOWN_METRIC);
  }
  cout << "Creating HashTable with " << buckets.size() << "buckets"<< endl;
  InsertList(vlist);
}

//Same as before, but tablesize is set manually (used in hypercube)
HashTable::HashTable(list<myvector> &vlist, string metric_name,int dimension,
int tablesize,string tabletype)
:tabletype(tabletype),buckets(tablesize)
{
  if(metric_name == "euclidean"){
    if(tabletype=="lsh")
      metric = new LSH::Euclidean(dimension, buckets.size());
    else if(tabletype=="hypercube")
      metric = new Hypercube::Euclidean(dimension, buckets.size());
    else{
      cout << "Unknown tabletype: " << tabletype << endl;
      exit(UNKNOWN_METRIC);
    }
  }
  else if("cosine"){
    metric = new CosineSimilarity(dimension);
  }
  else{
    cout << "Unknown metric: " << metric_name << endl;
    exit(UNKNOWN_METRIC);
  }
  cout << "Creating HashTable with " << buckets.size() << "buckets"<< endl;
  InsertList(vlist);
}

/*Use this constructor if you know the size (num of vectors) for your HashTable
before you create a temporary list to store the vectors.
That way you can immediately HashTable::Insert them here*/
HashTable::HashTable(int size, string metric_name, int dimension,string tabletype)
:tabletype(tabletype),buckets(TableSize(metric_name,size))
{
  if(metric_name == "euclidean"){
    if(tabletype=="lsh")
      metric = new LSH::Euclidean(dimension, buckets.size());
    else if(tabletype=="hypercube")
      metric = new Hypercube::Euclidean(dimension, buckets.size());
    else{
      cout << "Unknown tabletype: " << tabletype << endl;
      exit(UNKNOWN_METRIC);
    }
  }
  else if("cosine"){
    metric = new CosineSimilarity(dimension);
  }
  else{
    cout << "Unknown metric: " << metric_name << endl;
    exit(UNKNOWN_METRIC);
  }
  cout << "Creating HashTable with " << buckets.size() << "buckets"<< endl;
}

HashTable::~HashTable(){
  cout << "HashTable destroyed" << endl;
  delete metric;
}




/*Insert a new vector to the table*/
void HashTable::Insert(myvector& p){
  unsigned int h = metric->Hash(p);
  //cout << h << " ";
  buckets[h].push_back(p);
}

/*Insert all elements of a list to hashtable*/
void HashTable::InsertList(list<myvector>& vlist){
  for(list<myvector>::iterator it=vlist.begin(); it != vlist.end(); it++){
    Insert(*it);
  }
}




/*Find in which bucket a vector should belong.*/
Bucket HashTable::get_bucket(myvector& v){
  return buckets[metric->Hash(v)];
}

/*Find in which bucket q should belong
and filter out any vectors with a different g*/
Bucket HashTable::get_bucket_filtered(myvector& q){
  Bucket bucket = get_bucket(q);  //find the corresponding bucket for q
  if(metric->name != "euclidean") //filter only for euclidean metric
    return bucket;

  Bucket result;
  std::vector<long int> g_of_q = metric->get_g(q);
  for(Bucket::iterator p=bucket.begin(); p != bucket.end(); p++){
      //for ever p check g's with q
      std::vector<long int> g_of_p = metric->get_g(*p);
      if(vectorCompare(g_of_q,g_of_p) == true) //same g's
        result.push_back(*p);
  }
  return result;
}

/*get bucket at pos*/
Bucket HashTable::get_bucket_at(int pos){
  return buckets[pos];
}

Metric* HashTable::get_metric(){
  return metric;
}


//Print bucket sizes
void HashTable::PrintBuckets(){
  cout << "Printing buckets" << endl;
  for(int i=0; i<buckets.size(); i++){
    cout << "bucket" << i << ":" << buckets[i].size() << endl;
  }
}

/*Get the hash value for p*/
int HashTable::get_hash(myvector &p){
  return metric->Hash(p);
}
