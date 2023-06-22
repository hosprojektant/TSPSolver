#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <random> 
#include <chrono>
#include <array>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace std;
double get_distance_from_path(vector<int>path);
struct Tour
{
  vector<int> path;
  double distance;
};
struct Improve
{
  bool improved; 
  vector<int> path;
  double distance;
};
Tour threeOpt(vector<int> path, double distance);
Tour twoOpt(vector<int> path, double distance);
vector<vector<double>> global_distances;
int cycle(int i,bool right,int size);