#include<bits/stdc++.h>
#include<unistd.h> // for using getopt
#include<randomkeyword.h>
#include <sys/time.h>

using namespace std;
typedef long long ll;
typedef vector<int> vi;
typedef pair<int, int> pii;
#define pb push_back
#define mp make_pair
#define fi first
#define se second

#define LEN_DATA (SZ_DATA * 2) // size of total point group

/**
 * main.cpp
 * totally, we have 6199528 point groups
 * range of group size		number of point groups
 * [1,9] 							3393216
 * [10,99]							2606312
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
 * q specifies choose which set of query points
 * Example: gs = 1 and q = 1 choose Q11 (vector that stores 10 query points from group size [1,9]) as query points.
 * Usage: ./main -g gs -k k -b beta -q q
*/
FILE *fp, *fopen();
int size_of_raw;
int num_point_group;

double drand48(void) {
	return rand() / (RAND_MAX + 1.0);
}

typedef struct raw { // raw data
	int len_keyword;
	int keyword[LEN_KEYWORD + 1]; // index starting from 1
	double longitude_x;
	double latitude_y;
} raw_t;

raw_t rawdata[LEN_DATA + 1]; // one rawdata represents one post. index starting from 1
int dif_keyword[KEYWORD_TYPE + KEYWORD_TYPE_2 + 1]; // map - Key: keyword value, Value: point_group_id
vector<int> point_group_item[KEYWORD_TYPE + KEYWORD_TYPE_2 + 1]; // point_group_item[keyword] = keywords' point group. values in vecotr: index to rawdata (posts)
// double q2p[LEN_DATA + 1]; // Key: reference to rawdata. Value: normalised Euclidean distance from query point to the other points.
double cache_similarity[LEN_DATA + 1]; // Key: reference to rawdata. Values: 

/**
 * @brief calculate the spatial proximity of two posts by using normalized euclidean distance
 * @param query_point index to rawdata (query point)
 * @param other_point other keyword
 * @return range = [0, infinity] 0 means two points are overlap. The further the two points are from each other, the greater the value.
*/
double calc_sp(int query_point, int other_point) {
	double x1 = rawdata[query_point].longitude_x;
	double y1 = rawdata[query_point].latitude_y;
	
	double x2 = rawdata[other_point].longitude_x;
	double y2 = rawdata[other_point].latitude_y;
	
	double mean_x1_x2 = (x1 + x2) / 2;
	double var_x1_x2 = (x1 - mean_x1_x2) * (x1 - mean_x1_x2) + (x2 - mean_x1_x2) * (x2 - mean_x1_x2);
	double mean_y1_y2 = (y1 + y2) / 2;
	double var_y1_y2 = (y1 - mean_y1_y2) * (y1 - mean_y1_y2) + (y2 - mean_y1_y2) * (y2 - mean_y1_y2);
	
	double square_x = (x1 - x2) * (x1 - x2);
	double square_y = (y1 - y2) * (y1 - y2);
	double res = std::sqrt((square_x/var_x1_x2) + (square_y/var_y1_y2));
	return res;
}

/**
 * @brief calculate the text relevance
 * @param query_point first: index to rawdata (posts), second: exact keyword (index to point group)
 * @param other_point other keyword
 * @return Jaccard Index: https://en.wikipedia.org/wiki/Jaccard_index    0: 100% the same 1: 0% the same
*/
double calc_rel(pii query_point, int other_point) {
	int n = rawdata[other_point].len_keyword;
	int num = 0;
	for (int i = 1; i <= n; i++) {
		if (rawdata[other_point].keyword[i] == query_point.se) num++;
	}
	double res = 1.0 - ((double) num / n);
	return res;
}

/**
 * @brief directly calculate similarity distance without using LB.
 * @param query_point first: index to rawdata (posts), second: exact keyword (index to point group)
 * @param point_group_G single point group
*/
// double incremental_distance(pg_t* query_point_group, pg_t* point_group_G, double kbest) {
double direct_calc_distance(pii query_point, vector<int>* point_group_G) {
	double res = 0.0;
	int n = point_group_G->size();
	for (int i = 0; i < n; i++) {
		double spatial_proximity = calc_sp(query_point.fi, (*point_group_G)[i]);
		double text_relevance = calc_rel(query_point, (*point_group_G)[i]);
		res += spatial_proximity * text_relevance;
	}
	return res;
}

/**
 * @param query_point first: index to rawdata (posts), second: exact keyword (index to point group)
 * @param point_group_G single point group
*/
// double incremental_distance(pg_t* query_point_group, pg_t* point_group_G, double kbest) {
double my_direct_calc_distance(pii query_point, vector<int>* point_group_G) {
	double res = 0.0;
	int n = point_group_G->size();
	for (int i = 0; i < n; i++) {
		int other_point = (*point_group_G)[i];
		// double spatial_proximity = calc_sp(query_point.fi, other_point);
		// double text_relevance = calc_rel(query_point, other_point);
		// res += spatial_proximity * text_relevance;
		res += cache_similarity[other_point];
	}
	return res;
}

/**
 * @param kbest The farthest distance from the query point in the k best results. Used to perform pruning. 
*/
double incremental_distance(pii query_point, vector<int>* point_group_G, double kbest) {
	double res = 0.0;
	int n = point_group_G->size();
	for (int i = 0; i < n; i++) {
		double spatial_proximity = calc_sp(query_point.fi, (*point_group_G)[i]);
		double text_relevance = calc_rel(query_point, (*point_group_G)[i]);
		res += spatial_proximity * text_relevance;
		if (res > kbest) return res;
	}
	return res;
}

/**
 * 
 * @param kbest The farthest distance from the query point in the k best results. Used to perform pruning. 
*/
double my_incremental_distance(pii query_point, vector<int>* point_group_G, double kbest) {
	double res = 0.0;
	int n = point_group_G->size();
	for (int i = 0; i < n; i++) {
		int other_point = (*point_group_G)[i];
		// double spatial_proximity = calc_sp(query_point.fi, other_point);
		// double text_relevance = calc_rel(query_point, other_point);
		// res += spatial_proximity * text_relevance;
		res += cache_similarity[other_point];
		if (res > kbest) return res;
	}
	return res;
}

/**
 * @brief competitors' implementation
 * @param query_point first: index to rawdata (posts), second: exact keyword (index to point group)
 * @param K number of results
 * @param beta sampling ratio, 10 = 10%
*/
void similarity_search(pii query_point, int K, int beta, vector<int>* ans) {
	// step 1: sample beta% point groups to obtain the threshold
	double sampling_ratio = (double) beta / 100;
	int num_sample = (int) (sampling_ratio * num_point_group);
	vector<int> beta_groups; // index to point group that is sampled. 
	// the first item in beta_groups is a dummy value -1, which is used to perform Reservoir Sampling Algorithm
	beta_groups.pb(-1); // this make beta_groups's index starting from 1
	
	// Reservoir Sampling Algorithm
	// first fill the reservoir array
	int cnt = 0;
	int ptr = 1;
	for (; cnt < num_sample; ptr++) {
		if (ptr != query_point.se) {
			beta_groups.pb(ptr);
			cnt++;
		}
	}
	
	// replace elements with gradually decreasing probability
	for (; ptr <= num_point_group; ptr++) {
		if (ptr != query_point.se) {
			int prob = (int) (drand48() * ptr) + 1; // choose one from [1, ptr]
			if (prob <= num_sample) {
				beta_groups[prob] = ptr;
			}
		}
	}

	set<int> beta_set; // for removing candidate point groups that have been processed in step 1.
	for (int i = 1; i <= num_sample; i++) {
		beta_set.insert(beta_groups[i]);
	}

	
	// step 2: extract the first k point group in beta_groups to obtain init threshold kbest. Then use this kbest to continue to calculate the distance of remaining point group.
	priority_queue<pair<double,int>> candSet; // max heap to maintain the current k nearest results.
	for (int i = 1; i <= K; i++) {
		double dist = direct_calc_distance(query_point, &point_group_item[beta_groups[i]]);
		candSet.push({dist, beta_groups[i]});
	}
	double dist_k_Best = candSet.top().fi;
	for (int i = K + 1; i <= num_sample; i++) {
		double dist = incremental_distance(query_point, &point_group_item[beta_groups[i]], dist_k_Best);
		if (dist < dist_k_Best) {
			candSet.pop();
			candSet.push({dist, beta_groups[i]});
			dist_k_Best = candSet.top().fi;
		}
	}

	/**
	 * @attention now the size of candSet = K
	 * */ 
	
	// step 3: perform range search at query point with radius as distkBest to retrive the collection of keywords within the range.
	// range search to retrieve the collection of keywords within the range.
	dist_k_Best = candSet.top().fi;
	for (int i = 1; i <= num_point_group; i++) {
		if (beta_set.find(i) != beta_set.end()) { // already consider this point group.
			continue;
		}
		double dist = incremental_distance(query_point, &point_group_item[i], dist_k_Best);
		if (dist < dist_k_Best) {
			candSet.push({dist, i});
		}
	}
	
	/**
	 * @attention now the size of candSet > K
	 * */	
	
	// sort candSet in ascending order of lower bound values.
	vector<pair<double,int>> sorted_candSet;
	while (!candSet.empty()) {
		sorted_candSet.pb(candSet.top());
		candSet.pop();
	}
	sort(sorted_candSet.begin(), sorted_candSet.end());
	for (int i = 0; i < K; i++) {
		ans->pb(sorted_candSet[i].second);		
	}
#ifdef STAT
	printf("results: \n");
	// 取sorted_candSet的前K个作为结果
	for (int i = 0; i < K; i++) {
		cout << "distance: " << sorted_candSet[i].first << " pg: " << sorted_candSet[i].second << endl;
	}
#endif
}

/**
 * @brief brute force to calculate the similarity distance of query point and other points. This is used for preprocessing.
*/
double force_calc(pii query_point, int other_point) {
	double spatial_proximity = calc_sp(query_point.fi, other_point);
	double text_relevance = calc_rel(query_point, other_point);
	return spatial_proximity * text_relevance;
}

/**
 * @brief my implementation
 * @param query_point first: index to rawdata (posts), second: exact keyword (index to point group)
 * @param K number of results
 * @param beta sampling ratio, 10 = 10% (by default)
*/
void my_similarity_search(pii query_point, int K, int beta, vector<int>* ans) {
	/**
	 * @attention Optimization: preprocessing to calculte all the similarity distance
	*/
	for (int i = 1; i <= size_of_raw; i++) {
		cache_similarity[i] = force_calc(query_point, i);
	}
	
	// step 1: sample beta% point groups to obtain the threshold
	double sampling_ratio = (double) beta / 100;
	int num_sample = (int) (sampling_ratio * num_point_group);
	vector<int> beta_groups; // index to point group that is sampled. 
	// the first item in beta_groups is a dummy value -1, which is used to perform Reservoir Sampling Algorithm
	beta_groups.pb(-1); // this make beta_groups's index starting from 1
	
	// Reservoir Sampling Algorithm
	// first fill the reservoir array
	int cnt = 0;
	int ptr = 1;
	for (; cnt < num_sample; ptr++) {
		if (ptr != query_point.se) {
			beta_groups.pb(ptr);
			cnt++;
		}
	}
	
	
	// replace elements with gradually decreasing probability
	for (; ptr <= num_point_group; ptr++) {
		if (ptr != query_point.se) {
			int prob = (int) (drand48() * ptr) + 1; // choose one from [1, ptr]
			if (prob <= num_sample) {
				beta_groups[prob] = ptr;
			}
		}
	}

	set<int> beta_set; // for removing candidate point groups that have been processed in step 1.
	for (int i = 1; i <= num_sample; i++) {
		beta_set.insert(beta_groups[i]);
	}
	
	// step 2: extract the first k point group in beta_groups to obtain init threshold kbest. Then use this kbest to continue to calculate the distance of remaining point group.
	priority_queue<pair<double,int>> candSet; // max heap to maintain the current k nearest results.
	for (int i = 1; i <= K; i++) {
		double dist = my_direct_calc_distance(query_point, &point_group_item[beta_groups[i]]);
		candSet.push({dist, beta_groups[i]});
	}
	
	double dist_k_Best = candSet.top().fi;
	for (int i = K + 1; i <= num_sample; i++) {
		double dist = my_incremental_distance(query_point, &point_group_item[beta_groups[i]], dist_k_Best);
		if (dist < dist_k_Best) {
			candSet.pop();
			candSet.push({dist, beta_groups[i]});
			dist_k_Best = candSet.top().fi;
		}
	}
	
	/**
	 * @attention now the size of candSet = K
	 * */
	// step 3: perform range search at query point with radius as distkBest to retrive the collection of keywords within the range.
	// range search to retrieve the collection of keywords within the range.
	dist_k_Best = candSet.top().fi;
	// Optimization: First consider the point set (group) of q.keyword, so that dist_k_Best can converge faster. 
	double dist = my_incremental_distance(query_point, &point_group_item[query_point.se], dist_k_Best);
	if (dist < dist_k_Best) {
		candSet.pop();
		candSet.push({dist, query_point.se});
		dist_k_Best = candSet.top().fi;
		beta_set.insert(query_point.second);
	}
	/**
	 * @attention now the size of candSet = K
	 * */
	dist_k_Best = candSet.top().fi;
	for (int i = 1; i <= num_point_group; i++) {
		if (beta_set.find(i) != beta_set.end()) { // already consider this point group.
			continue;
		}
		double dist = my_incremental_distance(query_point, &point_group_item[i], dist_k_Best);
		if (dist < dist_k_Best) {
			candSet.pop();
			candSet.push({dist, i});
			dist_k_Best = candSet.top().fi;
		}
	}
	
	/**
	 * @attention now the size of candSet = K
	 * */
	
	// sort candSet in ascending order of lower bound values.
	
	vector<pair<double,int>> sorted_candSet;
	while (!candSet.empty()) {
		sorted_candSet.pb(candSet.top());
		candSet.pop();
	}
	sort(sorted_candSet.begin(), sorted_candSet.end());
	for (int i = 0; i < K; i++) {
		ans->pb(sorted_candSet[i].second);
	}
#ifdef STAT
	printf("results: \n");
	// 取sorted_candSet的前K个作为结果
	for (int i = 0; i < K; i++) {
		cout << "distance: " << sorted_candSet[i].first << " pg: " << sorted_candSet[i].second << endl;
	}
#endif
}

/**
 * @brief load the data into our memory space.
 * 
*/
void get_keyword(int gs, vector<pii>* query_points) {
/**
 * 
 * paper里面对raw data的预处理是:
 * each tweet = <date, time, keyword-list, longitude_x and latitude_y>
 * for i in all tweets
 *     for j in i.keyword-list:
 * 		   point_group_j.insert({longitude_x, latitude_y}) // 能这样做的前提是我们需要对所有的keyword进行离散化处理, 处理完之后的大小其实就是dataset的大小
 * 每个point group都对应一个keyword, point group的大小 = 含有这个keyword的所有message的个数
*/
	int n;
	fscanf(fp, "%d", &n); // n posts
	size_of_raw = n;
	memset(dif_keyword, 0, sizeof(dif_keyword));
	int point_group_id = 0;
	for (int i = 1; i <= n; i++) { // for each post = <date, time, keyword-list, longitude_x, latitude_y>
		int len_keyword;
		fscanf(fp, "%d", &len_keyword);
		rawdata[i].len_keyword = len_keyword;
		for (int j = 1; j <= len_keyword; j++) {
			int keyword;
			fscanf(fp, "%d", &keyword);
			/**
			 * if there is 100 keyword in our dataset, then point_group_id will increment from 1 to 100.
			*/
			if (!dif_keyword[keyword]) { // new keyword
				++point_group_id;
				dif_keyword[keyword] = point_group_id;
			}
			rawdata[i].keyword[j] = keyword;
			point_group_item[dif_keyword[keyword]].pb(i); // push rawdata index to dif_keyword[keyword] point set. 
		}
		fscanf(fp, "%lf", &rawdata[i].longitude_x);
		fscanf(fp, "%lf", &rawdata[i].latitude_y);
	}
	vi range_9;
	vi range_99;
	vi range_999;
	int cnt_9 = 0, cnt_99 = 0, cnt_999 = 0;
	for (int i = 1; i <= point_group_id; i++) {
		if (1 <= point_group_item[i].size() && point_group_item[i].size() <= 9) {
			range_9.pb(i);
			cnt_9++;
		}
		else if (10 <= point_group_item[i].size() && point_group_item[i].size() <= 99) {
			range_99.pb(i);
			cnt_99++;
		}
		else {
			range_999.pb(i);
			cnt_999++;
		}
	}
	// printf("10: %d\n", cnt_9);
	// printf("100: %d\n", cnt_99);
	// printf("1000: %d\n", cnt_999);
	vi* tmp;
	if (gs == 1) tmp = &range_9;
	else if (gs == 2) tmp = &range_99;
	else tmp = &range_999;
	// load query points. 
	for (int i = 1; i <= NUM_QUERY_POINTS; i++) {
		int pos_in_tmp = (int) (drand48() * tmp->size()); // choose a point group in tmp. 
		int candidate_pg = (*tmp)[pos_in_tmp];
		tmp->erase(tmp->begin() + pos_in_tmp); // remove this point group from tmp. 
		int sz = point_group_item[candidate_pg].size();
		int candicate_posts = point_group_item[candidate_pg][(int) (drand48() * sz)]; // choose a point within point group.
		query_points->pb({candicate_posts, candidate_pg}); // load the point into point group. 
	}
	num_point_group = point_group_id; // cache the size of total point group.
}


int main(int argc, char *argv[]) {
	fp = fopen("input.txt", "r");
	int o;
	int choice_of_group_size, K, beta, which_query_points;
	
	while ((o = getopt(argc, argv, "g:k:b:q:")) != -1)
	{
		switch (o)
		{
			case 'g':
			choice_of_group_size = atoi(optarg);
			break;
			case 'k':
			K = atoi(optarg);
			break;
			case 'b':
			beta = atoi(optarg);
			break;
			case 'q':
			which_query_points = atoi(optarg);
			default:
			break;
		}
	}
	
	vector<pii> Q11 = {{10068970, 200051}, {19119148, 5137851}, {11239910, 3896608}, 
	{17228762, 563132}, {19482939, 4737955}, {16071144, 2957427}, {12345212, 5505009}, 
	{15428458, 612220}, {11505171, 4693480}, {10671045, 2956863}};
	vector<pii> Q12 = {{19097891, 3178441}, {17247393, 4230598}, {16459216, 5575232}, {17958400, 903662}, 
	{19469319, 3168902}, {17723557, 5857586}, {10425748, 2137401}, {12413284, 5038299}, 
	{19435824, 4479496}, {15935601, 6168823}};
	vector<pii> Q13 = {{19953023, 2029559}, {17573107, 4968764}, {11429766, 4585207}, {18241819, 4473859}, 
	{12604475, 2208573}, {16717080, 5192582}, {10935693, 1964363}, {13239006, 2797447}, 
	{19161023, 3614588}, {13095023, 5852964}};

	vector<pii> Q21 = {{11924149, 200047}, {15209923, 4082353}, {12059624, 2818251}, {16008521, 418489}, 
	{17501509, 3629317}, {14935903, 2045623}, {11414848, 4562874}, {15873996, 448371}, 
	{10905097, 3582573}, {10401215, 2045166}};
	vector<pii> Q22 = {{17514810, 2218081}, {19184022, 3121812}, {13725377, 4665410}, {14969079, 628460}, 
	{17579256, 2210576}, {17402049, 5133688}, {10254057, 1444871}, {12259218, 3964306}, 
	{17608287, 3364555}, {15934468, 5939106}};
	vector<pii> Q23 = {{19640151, 1369531}, {16235877, 3884451}, {10859123, 3470619}, {18211162, 3358821}, 
	{13786311, 1494981}, {15986287, 4149041}, {13406010, 1324510}, {14459998, 1923457}, 
	{18920695, 2573940}, {11872017, 5124957}};
	
	vector<pii> Q31 = {{1119437, 2}, {4797275, 151122}, {1985224, 106554}, {6987063, 9410}, {9446238, 135860}, {4798594, 76701}, {648266, 166195}, {4981011, 10694}, {78040, 134230}, {580619, 76683}};
	vector<pii> Q32 = {{6382416, 83499}, {9373653, 117796}, {5741477, 169236}, {6199756, 18395}, {7049078, 83201}, {7398748, 182066}, {592425, 52491}, {3161318, 147217}, {7581717, 126528}, {3971142, 198208}};
	vector<pii> Q33 = {{9772447, 49407}, {7158715, 144532}, {950202, 130303}, {8869642, 126325}, {4219018, 54542}, {4460276, 153301}, {2456383, 47553}, {1618812, 71852}, {9248980, 97303}, {939040, 181843}};
	
	vector<pii> query_points; // first: index to rawdata (posts), second: exact keyword
	get_keyword(choice_of_group_size, &query_points); // load data into our space. 	
	query_points.clear(); // clear the query points, use the prepared one. 
	if (choice_of_group_size == 1) {
		if (which_query_points == 1) {
			for (auto item : Q11) {
				query_points.pb(item);
			}
		}
		else if (which_query_points == 2) {
			for (auto item : Q12) {
				query_points.pb(item);
			}
		}
		else {
			for (auto item : Q13) {
				query_points.pb(item);
			}
		}
	}
	else if (choice_of_group_size == 2) {
		if (which_query_points == 1) {
			for (auto item : Q21) {
				query_points.pb(item);
			}
		}
		else if (which_query_points == 2) {
			for (auto item : Q22) {
				query_points.pb(item);
			}
		}
		else {
			for (auto item : Q23) {
				query_points.pb(item);
			}
		}
	}
	else if (choice_of_group_size == 3) {
		if (which_query_points == 1) {
			for (auto item : Q31) {
				query_points.pb(item);
			}
		}
		else if (which_query_points == 2) {
			for (auto item : Q32) {
				query_points.pb(item);
			}
		}
		else {
			for (auto item : Q33) {
				query_points.pb(item);
			}
		}
	}
	vi ans;
	// memset(q2p, 0, sizeof(q2p));
	memset(cache_similarity, 0, sizeof(cache_similarity));
	// start the timer.
	struct timeval t1, t2;
	double timeuse;
	gettimeofday(&t1, NULL);
	for (int i = 0; i < query_points.size(); i++) {
#ifdef MY
		my_similarity_search(query_points[i],K,beta, &ans);
		// memset(q2p, 0, sizeof(q2p));
		memset(cache_similarity, 0, sizeof(cache_similarity));
#endif
#ifdef OG // origional
		similarity_search(query_points[i], K, beta, &ans);
#endif
		ans.clear();
	}
    gettimeofday(&t2,NULL);
    timeuse = (t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000000.0;
	printf("%lu queries with timeuse = %f", query_points.size(), timeuse);
// #endif
    return 0;
}