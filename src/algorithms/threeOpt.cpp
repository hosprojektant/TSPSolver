#include "threeOpt.hpp"


void reverse_path_section(vector<int>& path, int start, int end) {
    int i = start, j = end;
    while(i < j) {
        int tmp = path[i];
        path[i] = path[j];
        path[j] = tmp;
        i++;
        j--;
    }
}

double check_reverse_path_section(vector<int> path, int start, int end) {
    double distance = 0; 
    distance-=global_distances[path[start-1]][path[start]];
    distance-=global_distances[path[end]][path[end+1]];
    for (size_t z = 0; z < path.size()-1; z++)
    {
        distance-=global_distances[path[z]][path[z+1]];
    }
    
    int i = start, j = end;
    while(i < j) {
        int tmp = path[i];
        path[i] = path[j];
        path[j] = tmp;
        i++;
        j--;
    }
    distance+=global_distances[path[start-1]][path[start]];
    distance+=global_distances[path[end]][path[end+1]];
    for (size_t z = 0; z < path.size()-1; z++)
    {
        distance+=global_distances[path[z]][path[z+1]];
    }
    return distance;
}

void reverse(vector<int>& path, int start, int end, int n) { 

	int half = ((n + end - start + 1) % n) / 2;

	int left = start;
	int right = end;
	int temp;

	int i;
	for (i = 1; i < half + 1; i++) {
		temp = path[right];
		path[right] = path[left];
		path[left] = temp;

		left = (left + 1) % n;
		right = (n + right - 1) % n;
	}
}

void makeMove(vector<int>& path, int bestCase, int i, int j, int k, int n) {
    if(bestCase == 0){
        return; 
    }
    n = n+1;
	if (bestCase == 1) {

		reverse(path, (k + 1) % n, i, n);

	} else if (bestCase == 2) {

		reverse(path, (j + 1) % n, k, n);

	} else if (bestCase == 3) {

		reverse(path, (i + 1) % n, j, n);

	} else if (bestCase == 4) {

		reverse(path, (j + 1) % n, k, n);
		reverse(path, (i + 1) % n, j, n);

	} else if (bestCase == 5) {

		reverse(path, (k + 1) % n, i, n);
		reverse(path, (i + 1) % n, j, n);

	} else if (bestCase == 6) {

		reverse(path, (k + 1) % n, i, n);
		reverse(path, (j + 1) % n, k, n);

	} else if (bestCase == 7) {

		reverse(path, (k + 1) % n, i, n);
		reverse(path, (i + 1) % n, j, n);
		reverse(path, (j + 1) % n, k, n);

	}
}



Tour threeOpt(vector<int> path, double distance)
{   
    Tour best;
    best.distance = distance;
    best.path = path;
    int n = path.size() - 1;
    bool iterations_improvement = true;
    int a, b, c, d, e, f;
    while(iterations_improvement) {
        iterations_improvement = false;
        for (int i = 0; i < n; i++) {
            a = i;
            b = i + 1;
            for (int j = i + 2; j < n; j++) {
                c = j;
                d = j + 1;
                for (int k = j + 2; k < n; k++) {
                    e = k;
                    f = k + 1;
                    double gains[8];
                    gains[0] = 0;
                    gains[1] = global_distances[a][e] + global_distances[b][f] - global_distances[a][b] - global_distances[e][f];
                    gains[2] = global_distances[c][e] + global_distances[d][f] - global_distances[c][d] - global_distances[e][f];
                    gains[3] = global_distances[a][c] + global_distances[b][d] - global_distances[a][b] - global_distances[c][d];
                    double old_delta = global_distances[a][b] + global_distances[c][d] + global_distances[e][f];
                    gains[4] = global_distances[a][c] + global_distances[b][e] + global_distances[d][f] - old_delta;
                    gains[5] = global_distances[a][e] + global_distances[d][b] + global_distances[c][f] - old_delta;
                    gains[6] = global_distances[a][d] + global_distances[e][c] + global_distances[b][f] - old_delta;
                    gains[7] = global_distances[a][d] + global_distances[e][b] + global_distances[c][f] - old_delta;
                    double min_gain = 0;
                    int min_v = 0; 
                    for (size_t v = 0; v < 8; v++)
                    {
                        if(gains[v]<min_gain){
                            min_gain = gains[v];
                            min_v = v;
                        }
                    }
                    makeMove(path,min_v,i,j,k,n);
                    best.distance-= min_gain;

                }
            }
        }
    }
    cout << best.distance << "\n";
    best.path = path;
    return best;
}





// Function to perform a 3-opt swap in a path
void threeOptSwap(vector<int>& path, int a, int b, int c, int d, int e) {
    reverse_path_section(path, a, b);
    reverse_path_section(path, c, d);
    reverse_path_section(path, e, path.size() - 1);
}
int is_path_shorter(int a, int b, int c, int d, double &distance)
{
	if ((global_distances[a][c] + global_distances[b][d]) < (global_distances[a][b] + global_distances[c][d]))
	{
		distance -= (global_distances[a][b] + global_distances[c][d] - global_distances[a][c]
				- global_distances[b][d]);
		return 1;
	}
	return 0;
}

Tour twoOpt(vector<int> path, double distance)
{   
    Tour best;
    best.distance = distance;
    best.path = path;
    int n = path.size() - 1;
    int iterations_improvement = true;
    int a,b,c,d;
    while(iterations_improvement) {
        iterations_improvement = false;
        for (int i = 0; i < n; i++) {
            a = i;
            b = (i+1);
            for (int j = i + 2; j < n; j++) {
                c = j;
                d = (j+1);
                // Reverse the segment between i+1 and j
                
                if (is_path_shorter(best.path[a],best.path[b],best.path[c],best.path[d],best.distance)) {
                    reverse_path_section(best.path, i + 1, j);
                    iterations_improvement = true;
                }
            }
        }
    }
    return best;
}
