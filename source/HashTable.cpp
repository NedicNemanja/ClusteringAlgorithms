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

unsigned int TableSize(string tabletype,string metric, int num_vectors){
  if(tabletype == "lsh"){
    if(metric == "euclidean")
      return num_vectors / div_tablesize;
    else if(metric == "cosine")
      return pow(2,CmdArgs::K);
    else
      exit(UNKNOWN_METRIC);
  }
  if(tabletype == "hypercube")
    return pow(2,CmdArgs::K);
}

/*Use this constructor if you did not know the num of vectors and you stored
them temporarily in a list. This will insert all vectors from the list.*/
HashTable::HashTable(MyVectorContainer& vectors,string metric_name,int dimension,
  string tabletype)
:tabletype(tabletype),buckets(TableSize(tabletype,metric_name,vectors.size()))
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
  cout<<"Creating "<< tabletype<<" HashTable with "<<buckets.size()<<"buckets"<<endl;
  InsertVector(vectors);
}

//Same as before, but tablesize is set manually (used in hypercube)
HashTable::HashTable(MyVectorContainer &vectors,string metric_name,int dimension,
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
  InsertVector(vectors);
}

/*Use this constructor if you know the size (num of vectors) for your HashTable
before you create a temporary list to store the vectors.
That way you can immediately HashTable::Insert them here*/
HashTable::HashTable(int size, string metric_name, int dimension,string tabletype)
:tabletype(tabletype),buckets(TableSize(tabletype,metric_name,size))
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
  //cout << "HashTable destroyed" << endl;
  delete metric;
}




/*Insert a new vector to the table*/
void HashTable::Insert(MyVectorContainer &vectors,vector_index vindex){
  unsigned int h = metric->Hash(vectors[vindex]);
  //cout << h << " ";
  buckets[h].push_back(vindex);
}

/*Insert all elements of a vector to hashtable*/
void HashTable::InsertVector(MyVectorContainer &vectors){
  for(int i=0; i<vectors.size(); i++){
    Insert(vectors,i);
  }
}



/*Find in which bucket a vector should belong.*/
Bucket HashTable::get_bucket(myvector& v){
  return buckets[metric->Hash(v)];
}

int HashTable::num_buckets(){
  return buckets.size();
}
/*Find in which bucket q should belong
and filter out any vectors with a different g*/
Bucket HashTable::get_bucket_filtered(MyVectorContainer &vectors, myvector& q){
  Bucket bucket = get_bucket(q);  //find the corresponding bucket for q
  if(metric->name != "euclidean") //filter only for euclidean metric
    return bucket;

  Bucket result;
  std::vector<long int> g_of_q = metric->get_g(q);
  for(vector_index vindex=0; vindex<bucket.size(); vindex++){
      //p := vectors[vindex];
      //for ever p in bucket check g's with q
      std::vector<long int> g_of_p = metric->get_g(vectors[vindex]);
      if(vectorCompare(g_of_q,g_of_p) == true) //same g's
        result.push_back(vindex);
  }
  return result;
}

/*Check if two vectors have the same hash.*/
bool HashTable::CompareHashes(myvector& q, myvector& p){
  std::vector<long int> g_of_q = metric->get_g(q);
  std::vector<long int> g_of_p = metric->get_g(p);
  return vectorCompare(g_of_q,g_of_p);
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
int HashTable::get_hash(const myvector &p){
  return metric->Hash(p);
}

/*Return all index to vectors of the bucket that are in the range of center.
Returns only unassigned vectors indexes*/
vector<vector_index> HashTable::RangeSearch(int b, myvector center,
  double radius, MyVectorContainer &vectors,vector<bool> &AssignedVectorBitMap){
  vector<vector_index> results;
  //find corresponding bucket (and filter for g's in case of euclidean)
  Bucket bucket;
  if(metric->name != "euclidean")
    bucket = get_bucket_filtered(vectors,center);
  else
    bucket = get_bucket_at(b);
  //for each p in bucket
  double dist;
  for(auto v=bucket.begin(); v != bucket.end(); v++){
    if(AssignedVectorBitMap[*v]) continue;  //already assigned
    dist=metric->vectorDistance(center.begin(),center.end(),vectors[*v].begin());
    if( dist < radius){
      results.push_back(*v);
    }
  }
  return results;
}
