#include "tools.hpp"

double get_distance_from_path(vector<int>path){
    double distance = 0;
    for (size_t i = 0; i < path.size()-1; i++)
    {
        distance+=global_distances[path[i]][path[i+1]];
    }
    return distance;
}

double get_distance_from_indexes(vector<int>path, vector<int>indexes){
    double distance = 0;
    for (size_t i = 0; i < indexes.size()-1; i++)
    {
        distance+=global_distances[path[indexes[i]]][path[indexes[i+1]]];
    }
    
 
  return distance;
}

double get_distance(int a, int b){
    return global_distances[a][b];
}




