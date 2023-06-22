#include "colony.hpp"


void parse_file_to_places_colony()
{


  ifstream input(std::string("communicate/algo_input") + std::string(ant_colony.ID) + ".txt");
  int lines_counter = 0;
  for (std::string line; getline(input, line);)
  {
    int counter = 0;
    int id;
    double x;
    double y;
    vector<double> distance;
    vector<double> tau0;
    string delimiter = " ";
    size_t pos = 0;
    std::string token;
    //prvních 11 řádku je nastavení 
    if(lines_counter < 11){
      switch (lines_counter)
      {
      case 0:
        ant_colony.alpha = stod(line);
        break;
      case 1:
        ant_colony.beta = stod(line);
        break;
      case 2:
        ant_colony.rho = stod(line);
        break;
      case 3:
        ant_colony.Q = stod(line);
        break;
      case 4:
        ant_colony.q0 = stod(line);
        break;
      case 5:
        ant_colony.tau0 = stod(line);
        break;
      case 6:
        ant_colony.nn_multiplayer = stod(line);
        break;
      case 7:
        ant_colony.number_iterations = stoi(line);
        break;
      case 8:
        ant_colony.number_agents = stoi(line);
        break;
      case 9:
        ant_colony.nn_path_len = stoi(line);
        break;
      default:
        break;
      }

    }else{
      while ((pos = line.find(delimiter)) != std::string::npos)
      {
        token = line.substr(0, pos);
        switch (counter)
        {

        case 0:
          id = stoi(token);
          break;

        case 1:
          x = stod(token);
          break;

        case 2:
          y = stod(token);
          break;

        default:
          distance.emplace_back(stod(token));
          tau0.emplace_back(ant_colony.tau0);
        }
        line.erase(0, pos + delimiter.length());
        counter++;
      }
      // push last token¨
      distance.emplace_back(stod(line.erase(0, pos + delimiter.length())));
      global_distances.emplace_back(distance);
      tau0.emplace_back(ant_colony.tau0);
      Place place(id, x, y, distance, tau0);
      ant_colony.list_places.emplace_back(place);
    }
    
    lines_counter++;
    
  }
  input.close();
}






Tour nearest_neighbor(int path_length, int start_place)
{
  Tour best;
  const int size = ant_colony.list_places.size();
  vector<int> visited_places(size);
  fill(visited_places.begin(), visited_places.end(), -1);
  // while we dont have full path
  int actual_place = start_place;
  best.distance = 0;
  for (int i = 0; i < path_length; ++i)
  {
    visited_places[i] = actual_place;
    int next_place = -1;
    double best_distance = inf;
    for (int d = 0; d < size; ++d)
    {
      if (actual_place != d && find(visited_places.begin(), visited_places.end(), d) == visited_places.end())
      {
        double distance = ant_colony.list_places[actual_place].distance[d];
        if (distance < best_distance)
        {
          best_distance = distance;
          best.distance += best_distance;
          next_place = d;
        }
      }
    }
    actual_place = next_place;
  }
  best.path = visited_places;
  return best;
}


void cout_vector(vector<double> path){
  for (int i: path)
    cout << i << '\n';
}

void vvv(vector<int> path){
  for (int i: path)
    cout << i << ' ';
  cout << "FF\n";
}

inline double compute_visibility(double distance, double beta)
{
  return pow(1.0 / distance, beta);
}

inline double compute_tau(double tau, double alpha)
{
  return pow(tau, alpha);
}

int choose_element(vector<double> probabilities,double probabilities_sum) {

    std::uniform_real_distribution<double> dist(0.0, probabilities_sum);
    double rand_num = dist(gen);

    // Traverse the vector of probabilities and accumulate the probabilities
    double cumulative_prob = 0.0;
    for (int i = 0; i < probabilities.size(); ++i) {
        cumulative_prob += probabilities[i];
        if (cumulative_prob >= rand_num) {
            return i;
        }
    }
    // Should never reach here, but return -1 as an error value
    return -1;
}




Tour agent_chose_path(Agent agent)
{
  double distance = 0;
  const int size = ant_colony.list_places.size();
  uniform_int_distribution<> get_start_place(0, size - 1);
  vector<int> visited_places(size + 1);
  vector<int> no_visited_places(size);
  int start_place = get_start_place(gen);
  int actual_place = start_place;
  fill(visited_places.begin(), visited_places.end(), -1);
  iota(no_visited_places.begin(), no_visited_places.end(), 0);
  int next_place = -1;
  for (int i = 0; i < size; ++i)
  {
    visited_places[i] = actual_place;
    double probabilities_sum = 0.0;
    double visibility_tau_sum = ant_colony.list_places[actual_place].setup_visibility_tau();
    no_visited_places.erase( remove(no_visited_places.begin(),no_visited_places.end(), actual_place ),no_visited_places.end()); //možná špatně jde to bez odstranění 
    vector<double> probabilities(no_visited_places.size());
    //vypočítání pravděpodobnosti že půjdu nějakou cestou na základě vzorce 
    for (int d = 0; d < no_visited_places.size(); ++d)
    {
      double visibility = compute_visibility(global_distances[actual_place][no_visited_places[d]], ant_colony.beta);
      double tau = compute_tau(ant_colony.list_places[actual_place].tau[no_visited_places[d]], ant_colony.alpha);
      probabilities[d] = (visibility * tau) / visibility_tau_sum;
      probabilities_sum+=probabilities[d];
    }
    
    // používání q z knížky půjdou cestou která má největší pravděpodobnost
    // jinak vybírám cestu podle pravděpodobnosti 
    uniform_real_distribution<> uniform_q(0, 1);
    double q = uniform_q(gen);
    if (q < ant_colony.q0){
      actual_place = no_visited_places[std::distance(probabilities.begin(),max_element(probabilities.begin(), probabilities.end()))]; // max element index 
    }else{
      int q = choose_element(probabilities,probabilities_sum);
      if(q==-1) break;
      
      actual_place = no_visited_places[q];
    }

    distance += global_distances[visited_places[i]][actual_place];
  }
  //ještě musím jít zpatky do počátečního místa 
  visited_places[size] = start_place;
  distance += global_distances[visited_places[size-1]][start_place];
  Tour best;
  best.path = visited_places;
  best.distance = distance;
  return best;
}



inline double compute_delta_tau(double tau, double delta_tau){
  return tau+delta_tau;
}

void agent_update_tau(Tour best){
  //je tu mutex pro postupný přístup z jiných vláken 
  ant_colony.tau_mutex.lock();
  const int N =  ant_colony.list_places.size();
  double delta_tau = ant_colony.Q / best.distance;
  //pokud jsme cestu navštívili přičtu delta tau jinak nic 
  // je tam i poslední cesta nemusím ji přidávat ručně 
  for (int i = 0; i < N ; ++i)
  { 
    ant_colony.list_places[best.path[i]].tau[best.path[i+1]]+= delta_tau;
  }
  ant_colony.tau_mutex.unlock();
  
}

void setup_tau_start_NN(Tour best, int N){
  ant_colony.tau_mutex.lock();
  double delta_tau = ant_colony.Q / best.distance;
  //pokud jsme cestu navštívili přičtu delta tau jinak nic 
  for (int i = 0; i < N-1 ; ++i)
  {
    ant_colony.list_places[best.path[i]].tau[best.path[i+1]]+= delta_tau;
  }
  ant_colony.tau_mutex.unlock();
}


void agent_check_best_distance(Tour best){
    ant_colony.distance_mutex.lock();
    if(best.distance < ant_colony.best_distance){
      
      ant_colony.best_distance = best.distance;
      ant_colony.best_path = best.path;
    }
    ant_colony.distance_mutex.unlock();
  
}

void write_update_into_file(int i){
  ofstream f("communicate/update.txt",ios::trunc);
  if (f.is_open())
  {
    f << i;
    f.close();
  }
}

inline double compute_pheromone_evaporation(double tau, double rho){
  return tau*(1-rho);
}

void pheromone_evaporation(){
  //vypařování feromonů pro každé tau 
  const int N =  ant_colony.list_places.size();
  ant_colony.tau_mutex.lock();
   for (int i = 0; i < N; ++i){
    for (int j = i+1; j < N; ++j){
      double evaporation = compute_pheromone_evaporation(ant_colony.list_places[i].tau[j],ant_colony.rho);
      ant_colony.list_places[i].tau[j] = evaporation;
      ant_colony.list_places[j].tau[i] = evaporation;
    }
  } 
  ant_colony.tau_mutex.unlock();
}


void create_agent(int id)
{
  seed+=id;
  
  Agent agent(id);
  for(int i = 0; i < ant_colony.number_iterations; ++i){
    Tour best;
    best = agent_chose_path(agent);
    agent_update_tau(best);
    agent_check_best_distance(best); 
    if(id=ant_colony.number_agents-1){
      pheromone_evaporation();
    }
  }
}

void setup_threads()
{
  for (size_t i = 0; i < ant_colony.number_agents; i++)
  {
    ant_colony.agents.emplace_back(thread(create_agent, i));
  }
  for (size_t i = 0; i < ant_colony.number_agents; i++)
  {
    ant_colony.agents[i].join();
  }
}

void setup_nn_tau(){
  Tour best;
  uniform_int_distribution<> start_place(0, ant_colony.list_places.size() - 1);
  vector<int> chosen_places = {-1}; 
  for(int j = 0; j < ant_colony.list_places.size()*ant_colony.nn_multiplayer; ++j){ 
      int actual_place = start_place(gen);
      // vybírám nové místo pokud jsem ho už vybral
      // find je ekvivaletní actual place not in chosen_places
      while(find(chosen_places.begin(), chosen_places.end(), actual_place) != chosen_places.end()){
        actual_place = start_place(gen);
      }
      chosen_places.emplace_back(actual_place);
      best = nearest_neighbor(ant_colony.nn_path_len,actual_place);
      setup_tau_start_NN(best,ant_colony.nn_path_len);
  }
}


void write_Tour(){
  ofstream f(std::string("communicate/best_path") + std::string(ant_colony.ID) + ".txt", ios::trunc);
  if (f.is_open())
  {
    for (int i: ant_colony.best_path)
      f << i << ' ';
    f << "\n";
    f << ant_colony.best_distance <<"\n";
    f.close();
  }
}




int main(int argc, char** argv)
{ 
  std::uniform_int_distribution<int> dist(1, UINT32_MAX);
  seed = dist(gen);
  ant_colony.ID = argv[1];
  parse_file_to_places_colony();
  setup_nn_tau();
  setup_threads();
  cout << ant_colony.best_distance <<  "QQ "<<"\n";
  Tour best; 
  best.path = ant_colony.best_path;
  best.distance = ant_colony.best_distance;
  best = twoOpt(ant_colony.best_path,ant_colony.best_distance);
  ant_colony.best_path = best.path;
  ant_colony.best_distance = best.distance;
  cout << ant_colony.best_distance << "ZZ";
  write_Tour();

}

