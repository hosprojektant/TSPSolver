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
#include <numeric> 


using namespace std;

// https://stackoverflow.com/questions/39288595/why-not-just-use-random-device
std::mt19937::result_type seed = std::random_device()() ^
 std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() ^
 std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()
    /* ^ more_external_random_stuff */;
int choose_element(vector<double> probabilities,double probabilities_sum, mt19937_64 gen);
std::mt19937_64 gen(seed);
double inf = std::numeric_limits<double>::infinity();
extern vector<vector<double>> global_distances;
class Place;

class AntColony
{
public:
  char* ID;
  int number_iterations = 200;
  vector<double> best_distance_iteration;
  int number_agents = 40;
  double alpha = 1;
  double beta = 12;
  double rho = 0.3;
  double tau0 = 0.1;
  double Q = 2;
  double q0 = 0.2;
  double nn_multiplayer = 1.0;
  int nn_path_len = 2;
  int agent_iteration = 0;
  mutex tau_mutex;
  mutex iteration_mutex; 
  vector<thread> agents;
  std::vector<Place> list_places;
  int start_place = 0;
  int actual_place = 0;
  std::vector<int> best_path;
  double best_distance = inf;
  mutex distance_mutex;
  std::vector<int> shortest_path;
  double shortest_distance;
  AntColony() {}
};
AntColony ant_colony;

class Place
{
public:
  int id;
  double x;
  double y;
  vector<double> distance;
  vector<double> tau;
  Place(int id, double x, double y, vector<double> distance, vector<double> tau)
  {
    this->id = id;
    this->x = x;
    this->y = y;
    this->distance = distance;
    this->tau = tau;
  }
  // 1 / sum of all distance
  double setup_visibility_tau()
  {
    const int n = this->distance.size();
    double visibility_tau_sum = 0;
    for (int i = 0; i < n; ++i)
    {
      if(this->id != i ){
        visibility_tau_sum += pow(1.0 / this->distance[i], ant_colony.beta) * pow(this->tau[i], ant_colony.alpha);
      }
    }

    return visibility_tau_sum;
  }
};

class Agent
{
public:
  vector<int> best_path;
  vector<int> last_path; 
  double best_distance = inf;
  double last_distance;
  int id;
  Agent(int id)
  {
    this->id = id;
  }
};

struct Tour
{
  vector<int> path;
  double distance;
};
Tour twoOpt(vector<int> path, double distance);
Tour threeOpt(vector<int> path, double distance);
Tour run_LK(Tour tour1);
void vvv(vector<int> path);