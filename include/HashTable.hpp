#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <list>
#include <vector>

#include "myvector.hpp"
#include "Metric.hpp"

typedef std::vector<vector_index> Bucket;

class HashTable
{
    std::string tabletype;  //lsh or hypercube
    std::vector<Bucket> buckets;
    Metric* metric;
  public:
    HashTable(MyVectorContainer&,std::string,int dim, std::string);
    HashTable(MyVectorContainer&,std::string,int dim,int tsize,std::string);
    HashTable(int size, std::string metric_name, int dim, std::string);
    ~HashTable();
    Bucket get_bucket(myvector& v);
    /*get bucket, but filter out the vectors with different g()'s*/
    Bucket get_bucket_filtered(MyVectorContainer &vectors, myvector &q);
    /*get bucket at pos*/
    Bucket get_bucket_at(int pos);
    Metric* get_metric();
    /*Get the hash value for p*/
    int get_hash(myvector &p);
    void Insert(MyVectorContainer &vectors,vector_index vindex);
    void InsertVector(MyVectorContainer &vectors);
    void InitBuckets(int size);
    void PrintBuckets();
};

unsigned int TableSize(std::string tabletype,std::string metric,int numvectors);

#endif
