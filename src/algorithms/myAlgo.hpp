#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <random> 
#include <chrono>
#include <array>
#include <algorithm>
#include <thread>
#include <unordered_set>
#include <numeric> 
#include <algorithm>  
#include <cstdlib>     

using namespace std;
double inf_a = std::numeric_limits<double>::infinity();
extern vector<vector<double>> global_distances;
pair<int,int> segment_boarders;
// https://stackoverflow.com/questions/39288595/why-not-just-use-random-device
std::mt19937::result_type seed = std::random_device()() ^
 std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() ^
 std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()
    /* ^ more_external_random_stuff */;
int choose_element(vector<double> probabilities,double probabilities_sum, mt19937_64 gen);
std::mt19937_64 gen(seed);
class PlaceA
{
public:
  int id;
  double x;
  double y;
  vector<double> distance;
  PlaceA(int id, double x, double y, vector<double> distance)
  {
    this->id = id;
    this->x = x;
    this->y = y;
    this->distance = distance;
  }
};
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
struct Settings
{ 
  int start_nn_count;
  int start_nn_size;
  int midle_nn_size;
  int last_nn_size;
  int rounding; 
  int segments; 

};

Tour threeOpt(vector<int> path, double distance);

vector<PlaceA> list_places; 
vector<PlaceA> list_places_sorted; 
double get_distance_from_list_places(vector<int>path);
double get_distance_from_path(vector<int>path);
Tour run_colony_from_myAlgo(vector <int> my_places, vector<vector<double>> glob_dis);
Tour twoOpt(vector<int> path, double distance);


vector<vector<int>> all_permutations = {
{0, 1, 2, 3, 4},
{0, 1, 2, 4, 3},
{0, 1, 3, 2, 4},
{0, 1, 3, 4, 2},
{0, 1, 4, 2, 3},
{0, 1, 4, 3, 2},
{0, 2, 1, 3, 4},
{0, 2, 1, 4, 3},
{0, 2, 3, 1, 4},
{0, 2, 3, 4, 1},
{0, 2, 4, 1, 3},
{0, 2, 4, 3, 1},
{0, 3, 1, 2, 4},
{0, 3, 1, 4, 2},
{0, 3, 2, 1, 4},
{0, 3, 2, 4, 1},
{0, 3, 4, 1, 2},
{0, 3, 4, 2, 1},
{0, 4, 1, 2, 3},
{0, 4, 1, 3, 2},
{0, 4, 2, 1, 3},
{0, 4, 2, 3, 1},
{0, 4, 3, 1, 2},
{0, 4, 3, 2, 1},
{1, 0, 2, 3, 4},
{1, 0, 2, 4, 3},
{1, 0, 3, 2, 4},
{1, 0, 3, 4, 2},
{1, 0, 4, 2, 3},
{1, 0, 4, 3, 2},
{1, 2, 0, 3, 4},
{1, 2, 0, 4, 3},
{1, 2, 3, 0, 4},
{1, 2, 3, 4, 0},
{1, 2, 4, 0, 3},
{1, 2, 4, 3, 0},
{1, 3, 0, 2, 4},
{1, 3, 0, 4, 2},
{1, 3, 2, 0, 4},
{1, 3, 2, 4, 0},
{1, 3, 4, 0, 2},
{1, 3, 4, 2, 0},
{1, 4, 0, 2, 3},
{1, 4, 0, 3, 2},
{1, 4, 2, 0, 3},
{1, 4, 2, 3, 0},
{1, 4, 3, 0, 2},
{1, 4, 3, 2, 0},
{2, 0, 1, 3, 4},
{2, 0, 1, 4, 3},
{2, 0, 3, 1, 4},
{2, 0, 3, 4, 1},
{2, 0, 4, 1, 3},
{2, 0, 4, 3, 1},
{2, 1, 0, 3, 4},
{2, 1, 0, 4, 3},
{2, 1, 3, 0, 4},
{2, 1, 3, 4, 0},
{2, 1, 4, 0, 3},
{2, 1, 4, 3, 0},
{2, 3, 0, 1, 4},
{2, 3, 0, 4, 1},
{2, 3, 1, 0, 4},
{2, 3, 1, 4, 0},
{2, 3, 4, 0, 1},
{2, 3, 4, 1, 0},
{2, 4, 0, 1, 3},
{2, 4, 0, 3, 1},
{2, 4, 1, 0, 3},
{2, 4, 1, 3, 0},
{2, 4, 3, 0, 1},
{2, 4, 3, 1, 0},
{3, 0, 1, 2, 4},
{3, 0, 1, 4, 2},
{3, 0, 2, 1, 4},
{3, 0, 2, 4, 1},
{3, 0, 4, 1, 2},
{3, 0, 4, 2, 1},
{3, 1, 0, 2, 4},
{3, 1, 0, 4, 2},
{3, 1, 2, 0, 4},
{3, 1, 2, 4, 0},
{3, 1, 4, 0, 2},
{3, 1, 4, 2, 0},
{3, 2, 0, 1, 4},
{3, 2, 0, 4, 1},
{3, 2, 1, 0, 4},
{3, 2, 1, 4, 0},
{3, 2, 4, 0, 1},
{3, 2, 4, 1, 0},
{3, 4, 0, 1, 2},
{3, 4, 0, 2, 1},
{3, 4, 1, 0, 2},
{3, 4, 1, 2, 0},
{3, 4, 2, 0, 1},
{3, 4, 2, 1, 0},
{4, 0, 1, 2, 3},
{4, 0, 1, 3, 2},
{4, 0, 2, 1, 3},
{4, 0, 2, 3, 1},
{4, 0, 3, 1, 2},
{4, 0, 3, 2, 1},
{4, 1, 0, 2, 3},
{4, 1, 0, 3, 2},
{4, 1, 2, 0, 3},
{4, 1, 2, 3, 0},
{4, 1, 3, 0, 2},
{4, 1, 3, 2, 0},
{4, 2, 0, 1, 3},
{4, 2, 0, 3, 1},
{4, 2, 1, 0, 3},
{4, 2, 1, 3, 0},
{4, 2, 3, 0, 1},
{4, 2, 3, 1, 0},
{4, 3, 0, 1, 2},
{4, 3, 0, 2, 1},
{4, 3, 1, 0, 2},
{4, 3, 1, 2, 0},
{4, 3, 2, 0, 1},
{4, 3, 2, 1, 0},
};