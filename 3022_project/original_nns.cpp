#include<bits/stdc++.h>

#include<randomkeyword.h>
using namespace std;
typedef long long ll;
typedef vector<int> vi;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
#define pb push_back
#define mp make_pair
#define fi first
#define se second
#define trav(i, x) for (auto &i : x)
#define sz(x) ((int)(x).size())

#define LEN_DATA 50 // size of total point group
#define NUM_POINTS 100000
#define MAX_LEN_KEYWORD 1000
#define NUM_RAW 100000


/**
 * main.cpp
 * totally, we have 6199528 point groups
 * range of group size		number of point groups
 * [1,9] 							3393196
 * [10,99]							2606332
 * [100,999]						200000
 * 
 * gs = group size
 * gs = 1 -> choose 10 query points from [1,9]
 * gs = 2 -> choose 10 query points from [10,99]
 * gs = 3 -> choose 10 query points from [100,999]
 * 
 * k = k best results
 * 
 * beta is the sampling ratio, beta >= k, which is used to obtain the threshold distkBest
 * beta = 10%, 20%, 30%, 40%, 50%
 * Usage: ./main gs k beta
 * Example: ./main 1 10 10
*/

FILE *fp, *fopen();
double drand48(void) {
	return rand() / (RAND_MAX + 1.0);
}

typedef struct point {
	double x;
	double y;
} pt;

typedef struct raw { // raw data
	int len_keyword;
	int keyword[LEN_KEYWORD + 1];
	double longitude_x;
	double latitude_y;
} raw_t;

pt point[NUM_POINTS + 1];
raw_t rawdata[SZ_DATA + SZ_DATA + 1]; // one rawdata represents one post. index starting from 1

/**
 * 
 * paper里面对raw data的预处理是:
 * each tweet = <date, time, keyword-list, longitude_x and latitude_y>
 * for i in all tweets
 *     for j in i.keyword-list:
 * 		   point_group_j.insert({longitude_x, latitude_y}) // 能这样做的前提是我们需要对所有的keyword进行离散化处理, 处理完之后的大小其实就是dataset的大小
 * 每个point group都对应一个keyword, point group的大小 = 含有这个keyword的所有message的个数
*/


// 从5种 group size 里面每一种取 10 个point groups. 
typedef struct point_group {
	int keyword; // each point group corresponds to a keyword
	int size; // group size
	int point_id[10]; // id in pt point[NUM_POINTS + 1];
} pg_t;

vector<pg_t> resultSet;
int dif_keyword[KEYWORD_TYPE + KEYWORD_TYPE_2 + 1]; // map - Key: keyword value, Value: point_group_id
vector<int> point_group_item[KEYWORD_TYPE + KEYWORD_TYPE_2 + 1]; // values in vecotr: index to rawdata (posts)

pg_t all_point_group[KEYWORD_TYPE + KEYWORD_TYPE_2 + 1]; // array to store the point group id. each point group corresponds to one keyword. 



/**
 * @brief calculate the centroid of a query point group Q
*/
void calculate_centroid(pt* centroid, pg_t* query_point_group) {
	int n = query_point_group->size;
	double centroid_x = 0.0;
	double centroid_y = 0.0;
	for (int i = 1; i <= n; i++) {
		centroid_x += point[query_point_group->point_id[i]].x;
		centroid_y += point[query_point_group->point_id[i]].y;
	}
	centroid->x = centroid_x / n;
	centroid->y = centroid_y / n;
}

bool isEqual(double x, double y) {
    return std::fabs(x - y) < std::numeric_limits<double>::epsilon();
}
bool isLessThan(double x, double y) {
    return (x + std::numeric_limits<double>::epsilon()) < y;
}
bool isGreaterThan(double x, double y) {
    return (x - std::numeric_limits<double>::epsilon()) > y;
}

double calc_dis(pt* centroid, pt* point_in_Q) {
	return sqrt(pow(point_in_Q->x - centroid->x, 2) + pow(point_in_Q->y - centroid->y, 2));
}

/**
 * @brief return the position of the point in Q that is nearest to centroid
*/
int nearest_to_centroid(pt* centroid, pg_t* query_point_group) {
	int id;
	double near = DBL_MAX;
	for (int i = 1; i <= query_point_group->size; i++) {
		double tmp_dis = calc_dis(centroid, &point[query_point_group->point_id[i]]);
		if (isLessThan(tmp_dis, near)) {
			near = tmp_dis;
			id = i;
		}
	}
	return id;
}

/**
 * @param representatives representative subset Q' of the query point gropu Q
 * @param query_point_group query point group Q
 * @param alpha number of representatives to be chosen
*/
void greedy_representative(vector<int>* representatives, pg_t* query_point_group, int alpha) {
	pt centroid;
	calculate_centroid(&centroid, query_point_group);
	bool alive[query_point_group->size + 1];
	memset(alive, true, sizeof(alive));
	
	int first_representative = nearest_to_centroid(&centroid, query_point_group); // the point in Q nearest to the centroid of Q
	alive[first_representative] = false; // remove q_rep from Q
	// representative里面存的是pt point[NUM_POINTS + 1];的索引
	representatives->push_back(query_point_group->point_id[first_representative]); // append q_rep to Q'
	
	double RepDist[query_point_group->size + 1];
	for (int i = 1; i <= query_point_group->size; i++) 
		RepDist[i] = DBL_MAX; // 
	
	while (representatives->size() < alpha) {
		double largest_repdist = -1.0;
		int next_representative = 0;
		for (int i = 1; i <= query_point_group->size; i++) {
			if (alive[i]) {
				for (int j = 0; j < representatives->size(); j++) {
					RepDist[i] = min(calc_dis(&point[(*representatives)[j]], &point[query_point_group->point_id[i]]), RepDist[i]);
				}
				if (isGreaterThan(RepDist[i], largest_repdist)) {
					largest_repdist = RepDist[i];
					next_representative = i; // q_rep = the point in Q with the largest RepDist(q_i)
				}
			}
		}
		representatives->push_back(query_point_group->point_id[next_representative]); // append q_rep to Q'
		alive[next_representative] = false; // remove q_rep from Q
	}
}

/**
 * @param kbest 应该是用大根堆去维护的
*/
double incremental_distance(pg_t* query_point_group, pg_t* point_group_G, double kbest) {
	double d_max = 0.0;
	for (int i = 1; i <= query_point_group->size; i++) {
		double d_j = DBL_MAX;
		for (int j = 1; j <= point_group_G->size; j++) {
			double dist = calc_dis(&point[query_point_group->point_id[i]], &point[query_point_group->point_id[j]]);
			if (isLessThan(dist, d_j)) {
				d_j = dist;
			}
		}
		if (isGreaterThan(d_j, d_max) || isEqual(d_j, d_max)) {
			d_max = d_j;
			if (isGreaterThan(d_max, kbest) || isEqual(d_max, kbest)) { // pruning part
				return d_max;
			}
		}
	}
	return d_max;
}

/**
 * @param gamma used in choosing probing query point to apply range serach with radius distkBest
*/
void similarity_search(pg_t query_point, int K, int beta, int gamma, int sz_dataset, int sz_rawdata) {
	/**
	 * Candidate Retrieval
	*/
	resultSet.clear();
	priority_queue<pair<double,int>> distKbest;
	vector<int> beta_groups;
	int num_sample = 0;
	bool is_sample[LEN_DATA +1];
	while (num_sample != beta) {
		int rand_pos = (int) (drand48() * sz_dataset) + 1; // sampling on all_point_group[LEN_DATA + 1];
		if (!is_sample[rand_pos]) {
			is_sample[rand_pos] = true;
			beta_groups.push_back(rand_pos);
			num_sample++;
		}
	}
	// extract the first k point group in beta_groups, and calculate the dist(Q, G)
	for (int i = 0; i < K; i++) {
		distKbest.push({incremental_distance(&query_point, &all_point_group[beta_groups[i]], DBL_MAX), beta_groups[i]});
	}
	// obtain the threshold distkBest
	double dist_k_Best = distKbest.top().first;
	for (int i = K; i < num_sample; i++) {
		double tmp = incremental_distance(&query_point, &all_point_group[beta_groups[i]], dist_k_Best);
		if (tmp < dist_k_Best) {
			distKbest.pop();
			distKbest.push({tmp, beta_groups[i]});
			dist_k_Best = tmp;
		}
	}
	vector<int> gamma_rep;
	greedy_representative(&gamma_rep, &query_point, gamma);
	// for each chosen query point, we issue a range search 
	for (int i = 0; i < gamma_rep.size(); i++) {
		
	}
	return;
}
