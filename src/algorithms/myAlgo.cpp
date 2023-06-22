#include "myAlgo.hpp"


int myrandom (int i) { return std::rand()%i;}
vector <int> path_improving; 
Settings settings;
void parse_file_to_places()
{

  ifstream input(std::string("communicate/algo_input.txt"));
  int lines_counter = 0;
  for (std::string line; getline(input, line);)
  {
    int counter = 0;
    int id;
    double x;
    double y;
    vector<double> distance;
    string delimiter = " ";
    size_t pos = 0;
    std::string token;
    //prvních 11 řádku je nastavení 
    if(lines_counter < 6){
      switch (lines_counter)
      {
      case 0:
        settings.start_nn_size = stoi(line);
        break;
      case 1:
        settings.midle_nn_size = stoi(line);
        break;
      case 2:
        settings.last_nn_size = stoi(line);
        break;
      case 3:
        settings.start_nn_count = stoi(line);
        break;
      case 4:
        settings.rounding = stoi(line);
        break;
      case 5:
        settings.segments = stoi(line);
        break;
      case 6:

        break;
      case 7:

        break;
      case 8:

        break;
      case 9:

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
        }
        line.erase(0, pos + delimiter.length());
        counter++;
      }
        // push last token
        distance.emplace_back(stod(line.erase(0, pos + delimiter.length())));
        global_distances.emplace_back(distance);
        PlaceA place(id, x, y, distance);
        path_improving.emplace_back(id);
        list_places.emplace_back(place);
    }
    lines_counter++;
  }
  cout << " DD"<< settings.rounding << "FF";
  input.close();
}


int cycle(int i,bool right,int size){
    if(right){ // 0 1 2 0 cyklus 
        if(i==size){
            return 0;
        }else{
            return i+1;
        }
    }else{
        if(i==0){ // 0 2 1 0 2 cyklus 
            return size;
        }else{
            return i-1;
        }
    }
}

int cycle_for_segment(int i,bool right,int size){
    if(right){ // 0 1 2 0 cyklus 
        if(i==size){
            return -1;
        }else{
            return i+1;
        }
    }else{
        if(i==0){ // 0 2 1 0 2 cyklus 
            return -2;
        }else{
            return i-1;
        }
    }
}


//nemusíme počítat celou vzdálenost pouze přepojení bodů 
double calculate_distance_after_remove_point(int remove_point_index,int remove_point){
    
    double distance = 0.0;
    int left_boarder,right_boarder; 
    int size = path_improving.size()-1;
    left_boarder = path_improving[cycle(remove_point_index,false,size)];
    right_boarder = path_improving[cycle(remove_point_index,true,size)];
    distance-= global_distances[left_boarder][remove_point];
    distance-= global_distances[remove_point][right_boarder];
    distance+= global_distances[left_boarder][right_boarder];
    //cout << left_boarder << " " << remove_point << " " << right_boarder << " " << remove_point_index << " " << distance << "\n";
    //cout << "OO" << global_distances[left_boarder][remove_point] + global_distances[remove_point][right_boarder] << "OO"  << global_distances[left_boarder][right_boarder] << "\n";
    return distance;
}

Tour tsp_greedy( int start_place) {
  Tour best;
  const int size = path_improving.size();
  vector<int> visited_places;

  int actual_place = start_place;
  visited_places.push_back(start_place);
  best.distance = 0;
  
  while(1){
    double min_distance = inf_a;
    int next_place = -1;
    
    for (int d = 0; d < visited_places.size(); ++d) {

      for (size_t i = 0; i < path_improving.size(); i++)
      {

        if (visited_places[d] != path_improving[i] && find(visited_places.begin(), visited_places.end(), path_improving[i]) == visited_places.end()){
          double distance = global_distances[visited_places[d]][path_improving[i]];
          if (distance < min_distance) {
            min_distance = distance;
            next_place = path_improving[i];
          }
        }
      }
      
    }
    
    if (next_place == -1) {
      // No unvisited places left, go back to the start place
      best.distance += global_distances[actual_place][start_place];
      visited_places.push_back(start_place);
      break;
    }
    
    best.distance += min_distance;
    visited_places.push_back(next_place);
    actual_place = next_place;
  }
  best.path = visited_places;
  return best;
}


//pokud jsou místa utříděná od 0-endplace tak to bude vracet index 
//slouží k vrácení možných kandidátů 
int nearest_neighbor(int start_place, vector<int> visited_places)
{
    int size = path_improving.size();
    int actual_place = start_place;
    int next_place = -1;
    double best_distance = inf_a;
    for (int d = 0; d < size; ++d)
    {
        if ( find(visited_places.begin(), visited_places.end(), path_improving[d]) == visited_places.end() )
        {
            double distance = list_places[actual_place].distance[path_improving[d]];
            if (distance < best_distance)
            {
                best_distance = distance;
                next_place = d;
            }
        }
    }
    return next_place;
}

int nearest_neighbor_superior(int start_place, vector<int> visited_places)
{
    int size = path_improving.size();
    int actual_place = start_place;
    int next_place = -1;
    double best_distance = inf_a;
    vector <int> e;
    for (size_t i = 0; i < 7; i++)
    {
      for (int d = 0; d < size; ++d)
      {
        if ( find(visited_places.begin(), visited_places.end(), path_improving[d]) == visited_places.end() )
        {
            double distance = list_places[actual_place].distance[path_improving[d]];
            if (distance < best_distance)
            {
                best_distance = distance;
                next_place = d;
                visited_places.emplace_back(d);
                e.emplace_back(d);
            }
        }
      }
    }
    
    
    return next_place;
}



double calculate_distance_between_inserted_points( int left_boarder,int right_boarder,vector<int>inserted_path, double distance ){
  
  for (size_t i = 0; i < inserted_path.size()-1; i++)
  { 
    distance+=global_distances[inserted_path[i]][inserted_path[i+1]];
  }
  return distance;
  
}


double get_distance_from_places(){
    double distance = 0;
    for (size_t i = 0; i < path_improving.size()-1; i++)
    {
        distance+=global_distances[path_improving[i]][path_improving[i+1]];
    }
    return distance;
}


//testovano 
vector <int> construct_new_path(vector<int> inserted_path, vector<int> old_path, int z ){
    vector<int> new_path;
    
    inserted_path.resize(inserted_path.size()); //kvůli přidání pravé hranice zde byl problém k přístupu k paměti kde nemáme co dělat
    for (size_t i = 0; i < old_path.size(); i++) 
    {
        if (std::find(inserted_path.begin()+1, inserted_path.end(), old_path[i]) == inserted_path.end()){ // old_path[i] not in inserted_path jako v pythonu 
            //začínáme od 1 protože první prvek je levá hranice
            new_path.emplace_back(old_path[i]);
        }
    }
    //najdeme levou hranici 
    
    auto it = find(new_path.begin(), new_path.end(), inserted_path[0]);
    // insertujeme po levé hranici 
    new_path.insert(it+1 , inserted_path.begin()+1, inserted_path.end());

    return new_path;
}

int check_boarder(int boarder_index, int current_index, bool right , int boarder, int current){
  if(boarder==current){
    return cycle(current_index,right,path_improving.size()-1);
  }
  return -1;
}

bool is_same_number(double a, double b)
{   
    return fabs(a - b) < 0.00000001;
}

#define INSERTION_NEIGHBOURS 30

double core_insertion(double old_distance){
  for (size_t i = 0; i < list_places.size(); i++)
  {
    auto it = find(path_improving.begin(), path_improving.end(), list_places[i].id);
    int current_place_index = it - path_improving.begin();
    int current_place = path_improving[current_place_index];
    double distance_min = old_distance;  
    Tour t; 
    double dis_after_remove =  calculate_distance_after_remove_point(current_place_index,current_place);
    int distance_min_index = -1; 
    for (size_t z = 0; z < list_places.size()-1; z++)
    {
      if(path_improving[z] == current_place || path_improving[z+1] == current_place) continue;
      double distance = old_distance; 

      distance+= global_distances[path_improving[z]][current_place] + global_distances[current_place][path_improving[cycle(z,true,list_places.size()-1)]] 
      - global_distances[path_improving[z]][path_improving[cycle(z,true,list_places.size()-1)]];
      distance+=dis_after_remove;
      //cout << distance << " "<< distance_min <<"\n";
      if(distance < distance_min){
        distance_min = distance;
        distance_min_index = path_improving[z]; 
      }
    }
    if(distance_min < old_distance && distance_min_index != -1){
      path_improving.erase(path_improving.begin()+current_place_index);
      old_distance = distance_min; 
      auto it = find(path_improving.begin(), path_improving.end(), distance_min_index);
      // insertujeme po levé hranici 
      path_improving.insert(it+1 ,current_place );
    }

    
  }
  return old_distance; 
  
}

Improve get_all_posible_indexes(vector<int> inserted_path,double distance){
  
  Improve im; 
  im.improved = false;
  distance+= global_distances[inserted_path[0]][inserted_path.back()];
  distance-= global_distances[inserted_path[0]][inserted_path[1]];
  distance-= global_distances[inserted_path[inserted_path.size()-2]][inserted_path.back()];
  int left_boarder = inserted_path[0];
  int rigt_boarder = inserted_path.back();

  for (size_t i = 0; i < path_improving.size()-1; i++)
  {  
      if(path_improving[i] == inserted_path[0]) continue;
      double new_distance = distance; 
      new_distance-= global_distances[path_improving[i]][path_improving[i+1]];
      new_distance+= global_distances[path_improving[i]][inserted_path[1]];
      new_distance+= global_distances[path_improving[inserted_path.size()-2]][path_improving[i+1]];
      if(new_distance < distance && (!is_same_number(new_distance,distance))){
        
        inserted_path[0] = i;
        //cout << path_improving[i] << "QQQ"; 
        inserted_path[inserted_path.end()[-1]] = i+1; //uložíme si index 
        im.improved = true;
      }
  }
 
 im.distance = distance; 
 im.path = inserted_path; 
 return im;
 
}

Tour core_insertion_one(double old_distance, int current_place, vector<int> input_path){

    auto it = find(input_path.begin(), input_path.end(), current_place);
    int current_place_index = it - input_path.begin();
    current_place = input_path[current_place_index];
    double distance_min = old_distance;  
    Tour t; 
    double dis_after_remove =  calculate_distance_after_remove_point(current_place_index,current_place);
    int distance_min_index = -1; 
    for (size_t z = 0; z < list_places.size()-1; z++)
    {
      if(input_path[z] == current_place || input_path[z+1] == current_place) continue;
      double distance = old_distance; 

      distance+= global_distances[input_path[z]][current_place] + global_distances[current_place][input_path[cycle(z,true,list_places.size()-1)]] 
      - global_distances[input_path[z]][input_path[cycle(z,true,list_places.size()-1)]];
      distance+=dis_after_remove;
      //cout << distance << " "<< distance_min <<"\n";
      if(distance < distance_min){
        distance_min = distance;
        distance_min_index = input_path[z]; 
      }
    }
    if(distance_min < old_distance && distance_min_index != -1){
      input_path.erase(input_path.begin()+current_place_index);
      old_distance = distance_min; 
      auto it = find(input_path.begin(), input_path.end(), distance_min_index);
      // insertujeme po levé hranici 
      input_path.insert(it+1 ,current_place );
    }
  t.path = input_path;
  t.distance = old_distance;
  return t; 
  
}







Improve core(vector<int> aco_builded_path, int nn_size, bool use_insertion){


    //mohl bych použít reverse v každé iteraci ale příjde mi to zbytečné když to můžu udělat 1x jen musím mít víc proměných 
    vector <int> it_path = path_improving;// pořadí míst kterýma budem procházet 
    vector <int> noreversed_it_path = it_path; 
    vector <int> reversed_it_path = it_path; // obracené pořadí míst kterýma procházíme 
    reverse(reversed_it_path.begin(),reversed_it_path.end());
    vector <vector<int>> history;
    bool improved = true;
    //path_improving = aco_builded_path;
    double old_distance;
    
    improved = false;

    for (size_t i = 0; i < path_improving.size(); i++) 
    {   
        if(use_insertion){
          old_distance = get_distance_from_path(path_improving);
        }else{
          old_distance = get_distance_from_path(path_improving)+global_distances[path_improving[path_improving.size()-1]][path_improving[0]];
        }
        auto it = find(path_improving.begin(), path_improving.end(), it_path[i]);
        int current_place_index = it - path_improving.begin();
        int current_place = path_improving[current_place_index];
        int right_boarder_index = cycle(current_place_index,true,path_improving.size()-1);
        int right_boarder = path_improving[right_boarder_index];
        int start_cur = current_place;
        int left_boarder_index = cycle(current_place_index,false,path_improving.size()-1);
        int left_boarder = path_improving[left_boarder_index];
        vector <int> inserted_path = {current_place};
        vector <pair<int,int>> backup_boarders;
        double best_distance = inf_a; 
        vector <int> best_inserted_path; 
        vector <vector<int>> backup_path;
        int best_z = 0;
        double distance_after_remove_point = 0;
        double distance_boarders = global_distances[left_boarder][current_place] + global_distances[current_place][right_boarder]; //původní spojení
        backup_path.emplace_back(path_improving);
        
        for (size_t z = 0; z < nn_size; z++) //používáme 8sousedů takže celkem 9 prvků 
        { 
          
            if((z %2)==0){
              current_place_index = nearest_neighbor(start_cur,inserted_path);
            }else{
              current_place_index = nearest_neighbor(current_place,inserted_path);
            }
      
            current_place = path_improving[current_place_index];
            bool boarder_flag = false; 
            int new_boarder = check_boarder(left_boarder_index,current_place_index,false,left_boarder,current_place);
            if(new_boarder!=-1){
              
              left_boarder_index = new_boarder;
              left_boarder = path_improving[left_boarder_index]; 
              path_improving.erase(path_improving.begin()+current_place_index); //vymažu starou hranici 
              distance_after_remove_point -= global_distances[left_boarder][current_place]; //odpojím levou hranici 
              boarder_flag = true;
            }
            new_boarder = check_boarder(right_boarder_index,current_place_index,true,right_boarder,current_place);
            if(new_boarder!=-1){
              
              right_boarder_index = new_boarder;
              right_boarder = path_improving[right_boarder_index]; 
              path_improving.erase(path_improving.begin()+current_place_index); //vymažu starou hranici 
              distance_after_remove_point -= global_distances[right_boarder][current_place]; //odpojím pravou hranici 
              boarder_flag = true;
              
            }
            if(!boarder_flag){
              distance_after_remove_point+= calculate_distance_after_remove_point(current_place_index,current_place); //mělo by to být záporné číslo proto + 
              path_improving.erase(path_improving.begin()+current_place_index);
              
            }
            
            double distance = old_distance; 
            backup_path.emplace_back(path_improving); //zachování pro nejlepší cestu 
            distance += distance_after_remove_point; //přičteme záporné číslo takže odčítáme vzdálenost 
            inserted_path.insert(inserted_path.begin(), left_boarder);
            inserted_path.emplace_back(current_place);  //vkládání nově nalazeného 
            inserted_path.emplace_back(right_boarder);
            Tour t;
            t = twoOpt(inserted_path, inf_a); //algoritmus na nejlepší cestu 
            
      
            inserted_path.erase(inserted_path.begin()); //odstranění levé hranice
            inserted_path.pop_back(); //odstranění pravé hranice
            distance+= calculate_distance_between_inserted_points(left_boarder,right_boarder,t.path,0);
            //cout << calculate_distance_between_inserted_points(left_boarder,right_boarder,t.path,0) << " " << distance_after_remove_point-distance_boarders << "\n"; 
            distance-=distance_boarders;
            if(best_distance>distance){ //ukladání nejlepší iterace 
                best_distance = distance; 
                best_inserted_path = t.path;
                best_z = z ; 
            }
        }
        
        if((best_distance < old_distance) && (!is_same_number(best_distance,old_distance))){ //float přesnost 
            //cout << best_z << "EEE\n";
            if(use_insertion){
              old_distance = get_distance_from_path(path_improving);
            }else{
              old_distance = get_distance_from_path(path_improving)+global_distances[path_improving[path_improving.size()-1]][path_improving[0]];
            }
            

            //cout << old_distance << "\n";
            path_improving = construct_new_path(best_inserted_path,backup_path[best_z],best_z); //vytovření nové cesty pokud je zlepšení  
            improved = true;
            history.emplace_back(path_improving);
        }else{
          path_improving = backup_path[0];
        }
                     

    }

  Improve i;
  i.improved = improved; 
  i.distance = old_distance; 
  i.path = path_improving; 
  return i; 
}



int check_cycle_for_segment(int index, int right_segment, int left_segment, vector <int> path){
  if(index == -1){
    return right_segment;
  }else if (index == -2)
  {
    return left_segment;
  }
  return path[index];
}


int check_boarder_for_segment(int current_index, bool right , int boarder, int current){
  if(boarder==current){
    return cycle_for_segment(current_index,right,path_improving.size()-1);
  }
  return -3;
}

double calculate_distance_after_remove_point_for_segment(int remove_point_index,int remove_point, int right_segment, int left_segment){
    
    double distance = 0.0;
    int left_boarder,right_boarder; 
    int size = path_improving.size()-1;
    left_boarder = check_cycle_for_segment(cycle_for_segment(remove_point_index,false,size),right_segment,left_segment,path_improving);
    right_boarder = check_cycle_for_segment(cycle_for_segment(remove_point_index,true,size),right_segment,left_segment,path_improving);
    distance-= global_distances[left_boarder][remove_point];
    distance-= global_distances[remove_point][right_boarder];
    distance+= global_distances[left_boarder][right_boarder];
    //cout << left_boarder << " " << remove_point << " " << right_boarder << " " << remove_point_index << " " << distance << "\n";
    //cout << "OO" << global_distances[left_boarder][remove_point] + global_distances[remove_point][right_boarder] << "OO"  << global_distances[left_boarder][right_boarder] << "\n";
    return distance;
}

//testovano 
vector <int> construct_new_path_for_segment(vector<int> inserted_path, vector<int> old_path, int z, int right_segment, int left_segment ){
    vector<int> new_path;
    if(inserted_path.back() == right_segment){ //odstranění pravé hranice pokud je segment 
      inserted_path.pop_back();
    }


    for (size_t i = 0; i < old_path.size(); i++) 
    {
        if (std::find(inserted_path.begin()+1, inserted_path.end(), old_path[i]) == inserted_path.end()){ // old_path[i] not in inserted_path jako v pythonu 

            new_path.emplace_back(old_path[i]);
        }
    }
    //pokud je to segment insertujeme na začátek 
    if(inserted_path[0] == left_segment){
      new_path.insert(new_path.begin(),inserted_path.begin()+1, inserted_path.end()); // vynecháváme insertovaní levé hranice +1 
    }else{
        //najdeme levou hranici 
      auto it = find(new_path.begin(), new_path.end(), inserted_path[0]);
      // insertujeme po levé hranici 
      new_path.insert(it+1 , inserted_path.begin()+1, inserted_path.end());
    }
    
    return new_path;
}



Improve core_for_segment(vector<int> aco_builded_path, int nn_size, bool use_insertion, int right_segment, int left_segment){
    
    vector <int> it_path = path_improving;// pořadí míst kterýma budem procházet 
    bool improved = true;
    //path_improving = aco_builded_path;
    double old_distance;
    improved = false;
    for (size_t i = 0; i < path_improving.size(); i++) 
    {   
        old_distance = get_distance_from_path(path_improving)+global_distances[left_segment][path_improving[0]]+
        global_distances[path_improving.back()][right_segment];
        auto it = find(path_improving.begin(), path_improving.end(), it_path[i]);
        int current_place_index = it - path_improving.begin();
        int current_place = path_improving[current_place_index];
        int right_boarder_index = cycle_for_segment(current_place_index,true,path_improving.size()-1);
        int right_boarder = check_cycle_for_segment(right_boarder_index,right_segment,left_segment,path_improving);
        int start_cur = current_place;
        int left_boarder_index = cycle_for_segment(current_place_index,false,path_improving.size()-1);
        int left_boarder = check_cycle_for_segment(left_boarder_index,right_segment,left_segment,path_improving);
        vector <int> inserted_path = {current_place};
        vector <pair<int,int>> backup_boarders;
        double best_distance = inf_a; 
        vector <int> best_inserted_path; 
        vector <vector<int>> backup_path;
        int best_z = 0;
        double distance_after_remove_point = 0;

        double distance_boarders = global_distances[left_boarder][current_place] + global_distances[current_place][right_boarder]; //původní spojení
        backup_path.emplace_back(path_improving);
        
        for (size_t z = 0; z < nn_size; z++) //používáme 8sousedů takže celkem 9 prvků 
        { 
          
          
            if((z %2)==0){
              current_place_index = nearest_neighbor_superior(start_cur,inserted_path);
            }else{
              current_place_index = nearest_neighbor(start_cur,inserted_path);
            }
            
            current_place = path_improving[current_place_index];
            bool boarder_flag = false; 

            int new_boarder = check_boarder_for_segment(current_place_index,false,left_boarder,current_place); //nemusím replacovat

            if(new_boarder!=-3){
              left_boarder_index = new_boarder;
              left_boarder = check_cycle_for_segment(left_boarder_index,right_segment,left_segment,path_improving); 
              path_improving.erase(path_improving.begin()+current_place_index); //vymažu starou hranici 
              distance_after_remove_point -= global_distances[left_boarder][current_place]; //odpojím levou hranici 
              boarder_flag = true;
            }
            new_boarder = check_boarder_for_segment(current_place_index,true,right_boarder,current_place);
            if(new_boarder!=-3){
              right_boarder_index = new_boarder;
              right_boarder = check_cycle_for_segment(right_boarder_index,right_segment,left_segment,path_improving); 
              path_improving.erase(path_improving.begin()+current_place_index); //vymažu starou hranici 
              distance_after_remove_point -= global_distances[right_boarder][current_place]; //odpojím pravou hranici 
              boarder_flag = true;
              
            }
            if(!boarder_flag){
              distance_after_remove_point+= calculate_distance_after_remove_point_for_segment(current_place_index,current_place,right_segment,left_segment); //mělo by to být záporné číslo proto + 
              path_improving.erase(path_improving.begin()+current_place_index);
              
            }
            
            double distance = old_distance; 
            backup_path.emplace_back(path_improving); //zachování pro nejlepší cestu 
            distance += distance_after_remove_point; //přičteme záporné číslo takže odčítáme vzdálenost 
            
            inserted_path.insert(inserted_path.begin(), left_boarder);
            inserted_path.emplace_back(current_place);  //vkládání nově nalazeného 
            inserted_path.emplace_back(right_boarder);
            
            Tour t;
            t = twoOpt(inserted_path, inf_a); //algoritmus na nejlepší cestu 
            inserted_path.erase(inserted_path.begin()); //odstranění levé hranice
            inserted_path.pop_back(); //odstranění pravé hranice
           
            distance+= calculate_distance_between_inserted_points(left_boarder,right_boarder,t.path,0);
            distance-=distance_boarders;
            if(best_distance>distance){ //ukladání nejlepší iterace 
                best_distance = distance; 
                best_inserted_path = t.path;
                best_z = z ; 
            }
        }
        
        if((best_distance < old_distance) && (!is_same_number(best_distance,old_distance))){ 
            old_distance = get_distance_from_path(path_improving)+global_distances[left_segment][path_improving[0]]+
            global_distances[path_improving.back()][right_segment];
            path_improving = construct_new_path_for_segment(best_inserted_path,backup_path[best_z],best_z,right_segment,left_segment); //vytovření nové cesty pokud je zlepšení  
            improved = true;
        }else{
          path_improving = backup_path[0];
        }
                     

    }

  Improve i;
  i.improved = improved; 
  i.distance = old_distance; 
  i.path = path_improving; 
  return i; 
}










void start_core_insertion(Improve im){
  while(1){
    double new_distance = core_insertion(im.distance);
    if(new_distance < im.distance){
      cout << new_distance << "EEE\n";
      im.distance = new_distance;
    }else{
      break;
    }
  }
}



void best5(){
  int iter = 0; 
  while(1){
    int left_boarder = path_improving[iter+0];
    int right_boarder = path_improving[iter+6]; 
    double best_permutation_distance = global_distances[path_improving[iter+0]][path_improving[iter+1]] + 
    global_distances[path_improving[iter+1]][path_improving[iter+2]] +
    global_distances[path_improving[iter+2]][path_improving[iter+3]] +
    global_distances[path_improving[iter+3]][path_improving[iter+4]] +
    global_distances[path_improving[iter+4]][path_improving[iter+5]] +
    global_distances[path_improving[iter+5]][path_improving[iter+6]];
    bool improvement = false; 
    vector <int> best_permutation; 
    for (size_t i = 1; i < all_permutations.size(); i++)
    {
      double distance = global_distances[left_boarder][path_improving[iter+1+all_permutations[i][0]]] + 
      global_distances[path_improving[iter+1+all_permutations[i][0]]][path_improving[iter+1+all_permutations[i][1]]] + 
      global_distances[path_improving[iter+1+all_permutations[i][1]]][path_improving[iter+1+all_permutations[i][2]]] + 
      global_distances[path_improving[iter+1+all_permutations[i][2]]][path_improving[iter+1+all_permutations[i][3]]] +
      global_distances[path_improving[iter+1+all_permutations[i][3]]][path_improving[iter+1+all_permutations[i][4]]] +
      global_distances[path_improving[iter+1+all_permutations[i][4]]][right_boarder];
      if(distance<best_permutation_distance){
        best_permutation_distance = distance; 
        best_permutation = all_permutations[i];
        improvement = true; 
      }
    }
    if(improvement){
      vector <int> tmp_path = path_improving; 
      path_improving[iter+1] = tmp_path[iter+1+best_permutation[0]];
      path_improving[iter+2] = tmp_path[iter+1+best_permutation[1]];
      path_improving[iter+3] = tmp_path[iter+1+best_permutation[2]];
      path_improving[iter+4] = tmp_path[iter+1+best_permutation[3]];
      path_improving[iter+5] = tmp_path[iter+1+best_permutation[4]]; 
    }
    if(iter+15>=path_improving.size()){
      break;

    }
    iter+=5; 

  }
  
}


double check_improved(Improve im){
  if(!im.improved){
      Tour t = twoOpt(path_improving,im.distance);
      t.distance = get_distance_from_path(t.path)+global_distances[t.path[t.path.size()-1]][t.path[0]]; 
      //cout << t.distance; 
      //cout << "FFFFFFFF";
      path_improving = t.path;
      im.distance = t.distance;
    
  }
  return im.distance;
}

vector<int> start_segment_improving(vector<int> path, int path_size){
  vector<vector<int>> segment_paths;  
  vector<vector<int>> segment_paths_indexes;  
  vector<int> old_path = path;
  vector<int> tmp_path; 
  vector<int> tmp_path_index; 
  bool improved = true;
  vector<int> best_path = path_improving;
  Improve im;
  settings.start_nn_size = 2;
  settings.midle_nn_size = 7;
  settings.last_nn_size = 29;
  cout << path_size;
  for (size_t i = 0; i < path.size(); i++) //končí o jeden prvek dřív protože poslední je right boarder 
  {
    tmp_path.emplace_back(path[i]);
    tmp_path_index.emplace_back(i);
    if(((i+1)%path_size)==0)
    {
      segment_paths.emplace_back(tmp_path);
      segment_paths_indexes.emplace_back(tmp_path_index);
      tmp_path.clear();
      tmp_path_index.clear();
    }
  }

  segment_paths_indexes.emplace_back(tmp_path_index);
  segment_paths.emplace_back(tmp_path); // jemožné že ty prvky nebudou přesně takže tam vložím ten zbytek co mám 

  
  for (size_t v = 0; v < segment_paths.size(); v++) // začínám od 1 jelikož první prvek je leftboarder a poslední right boarder takže -1
  {
    path = best_path;
    int left_segment_index = cycle((find(path.begin(), path.end(), segment_paths[v][0]) - path.begin()),false,path.size()-1);
    int left_segment = path[left_segment_index];
    int right_segment = path[cycle((find(path.begin(), path.end(), segment_paths[v].back()) - path.begin()),true,path.size()-1)];
    for (size_t q = 0; q < 10; q++)
    {
      double old_distance = get_distance_from_path(old_path)+global_distances[old_path[old_path.size()-1]][old_path[0]];
      path_improving = segment_paths[v];
      
      random_shuffle( path_improving.begin(), path_improving.end(),myrandom );
      if(segment_paths[v].size() < 5){
        break;
      }
      if(settings.midle_nn_size >= segment_paths[v].size()){
        settings.midle_nn_size = segment_paths[v].size()/2;
      }
      if(settings.last_nn_size >= segment_paths[v].size()){
        settings.last_nn_size = segment_paths[v].size()-2;
      }
      improved = true;
      while (improved)
      {
        im = core_for_segment({},settings.start_nn_size,false,right_segment,left_segment);
        improved = im.improved; 
      }
      
      improved = true;
      
      while (improved)
      {
        im = core_for_segment({},settings.midle_nn_size,false,right_segment,left_segment);
        improved = im.improved; 
      }
      im.distance = check_improved(im); 
      
      improved = true;
      while (improved)
      {
        im = core_for_segment({},settings.last_nn_size,false,right_segment,left_segment);
        improved = im.improved; 
      }
    
      for (size_t i = 0 ; i < segment_paths[v].size(); i++)
      {
        path[segment_paths_indexes[v][i]]=path_improving[i];
      }

      double new_distance = get_distance_from_path(path)+global_distances[path[path.size()-1]][path[0]];
      if(new_distance<old_distance){
        cout << "  "<<  get_distance_from_path(path)+global_distances[path[path.size()-1]][path[0]] << "NEW" << "\n";
        best_path = path;
        old_path = path;
      }
    
    }
  }
   cout << "FINISH";
  return  best_path;
  

}








void start_algo(){

  int last_life = 1;
  bool improved = true;
  Improve im;
  im.distance = get_distance_from_path(path_improving)+global_distances[path_improving[path_improving.size()-1]][path_improving[0]];
  vector <vector<int>> backup_path;
  backup_path.emplace_back(path_improving);
  backup_path.emplace_back(path_improving);
  double best_distance = inf_a; 
  int best_index; //zde je to ready na vlakna 
  cout << "F";
  for (size_t i = 0; i < settings.start_nn_count; i++)
  {
    path_improving = backup_path[0];
    while (improved)
    {
      im = core({},settings.start_nn_size+i,false);
      improved = im.improved; 
    }
    improved = true;
    cout << im.distance << "\n";
    while (improved)
    {
      im = core({},settings.midle_nn_size,false);
      improved = im.improved; 
    }
    im.distance = check_improved(im); 
    if (best_distance > im.distance)
    {
      best_distance = im.distance; 
      backup_path[1] = path_improving;
    }
    improved = true; 
  }
  path_improving = backup_path[1];
  improved = true;
  while (improved)
  {
    im = core({},settings.last_nn_size,false);
    improved = im.improved; 


  }
  if(settings.segments){
    path_improving =  start_segment_improving(path_improving,120);
    path_improving =  start_segment_improving(path_improving,117);
    path_improving =  start_segment_improving(path_improving,114);
    path_improving =  start_segment_improving(path_improving,111);
    path_improving =  start_segment_improving(path_improving,108);
    path_improving =  start_segment_improving(path_improving,105);
    path_improving =  start_segment_improving(path_improving,102);
  }
}
void write_best_tour(){
  ofstream f(std::string("communicate/best_path.txt"), ios::trunc);
  if (f.is_open())
  {
    for (int i: path_improving)
      f << i << ' ';
    f << "\n";
    f << get_distance_from_path(path_improving)+global_distances[path_improving[path_improving.size()-1]][path_improving[0]] <<"\n";
    f.close();
  }
}


void calc_distance()
{
  double distance = 0; 
  
  vector<int>ee;
  for (size_t i = 0; i < ee.size()-1; i++)
  {
    distance+=global_distances[ee[i]-1][ee[i+1]-1];
  }
  distance+=global_distances[ee[ee.size()-1]-1][ee[0]-1];
  cout << distance << "EEE";
  exit(1);
  
}

int main(int argc, char** argv) {
  cout << "A";
  parse_file_to_places();
  //calc_distance();
  srand ( unsigned ( std::time(0) ) );
  const clock_t begin_time = clock();
  cout << get_distance_from_path(path_improving)+global_distances[path_improving[path_improving.size()-1]][path_improving[0]] <<"\n";
  start_algo();
  best5();
  cout << get_distance_from_path(path_improving)+global_distances[path_improving[path_improving.size()-1]][path_improving[0]] <<"\n";
   cout << "\n";
   std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC << "\n";
   cout << get_distance_from_path(path_improving)+global_distances[path_improving[path_improving.size()-1]][path_improving[0]] <<"\n";
   write_best_tour();
   return 0;
}