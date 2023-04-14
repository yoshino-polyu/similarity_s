
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
 * Usage: ./main -g gs -k k -b beta
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

/**
 * 
 * paper里面对raw data的预处理是:
 * each tweet = <date, time, keyword-list, longitude_x and latitude_y>
 * for i in all tweets
 *     for j in i.keyword-list:
 * 		   point_group_j.insert({longitude_x, latitude_y}) // 能这样做的前提是我们需要对所有的keyword进行离散化处理, 处理完之后的大小其实就是dataset的大小
 * 每个point group都对应一个keyword, point group的大小 = 含有这个keyword的所有message的个数
*/


int dif_keyword[KEYWORD_TYPE + KEYWORD_TYPE_2 + 1]; // map - Key: keyword value, Value: point_group_id
vector<int> point_group_item[KEYWORD_TYPE + KEYWORD_TYPE_2 + 1]; // point_group_item[keyword] = keywords' point group. values in vecotr: index to rawdata (posts)
unordered_map<int, double> q2p; // Key: reference to rawdata. Value: normalised Euclidean distance from query point to the other points.

/**
 * @brief load the data into our memory space.
*/
void get_keyword(int gs, vector<pii>* query_points) {
	int n;
	fscanf(fp, "%d", &n);
	size_of_raw = n;
	memset(dif_keyword, 0, sizeof(dif_keyword));
	int point_group_id = 0;
	for (int i = 1; i <= n; i++) {
		int len_keyword;
		fscanf(fp, "%d", &len_keyword);
		rawdata[i].len_keyword = len_keyword;
		for (int j = 1; j <= len_keyword; j++) {
			int keyword;
			fscanf(fp, "%d", &keyword);
			/**
			 * assume there is 100 keyword in our dataset, then point_group_id will increment from 1 to 100.
			*/
			if (!dif_keyword[keyword]) { // new keyword
				++point_group_id;
				dif_keyword[keyword] = point_group_id;
			}
			rawdata[i].keyword[j] = keyword;
			point_group_item[dif_keyword[keyword]].pb(i);
		}
		fscanf(fp, "%lf", &rawdata[i].longitude_x);
		fscanf(fp, "%lf", &rawdata[i].latitude_y);
	}
	vi range_9;
	vi range_99;
	vi range_999;
	for (int i = 1; i <= point_group_id; i++) {
		if (1 <= point_group_item[i].size() && point_group_item[i].size() <= 9) {
			range_9.pb(i);
		}
		else if (10 <= point_group_item[i].size() && point_group_item[i].size() <= 99) {
			range_99.pb(i);
		}
		else {
			range_999.pb(i);
		}
	}
	vi* tmp;
	if (gs == 1) tmp = &range_9;
	else if (gs == 2) tmp = &range_99;
	else tmp = &range_999;
    for (int j = 1; j <= 5; j++) {
        printf("+++++++++++++++\n");
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
        for (int i = 0; i < query_points->size(); i++) {
            cout << (*query_points)[i].first << " " << (*query_points)[i].second << endl;
        }
        query_points->clear();
    }
#ifdef STATS
	printf("num point group: %d\n", point_group_id);
	int range_999 = 0;
	int range_99 = 0;
	int range_9 = 0;
	for (int i = 1; i <= point_group_id; i++) {
		if (1 <= point_group_item[i].size() && point_group_item[i].size() <= 9) {
			range_9++;
		}
		else if (10 <= point_group_item[i].size() && point_group_item[i].size() <= 99) {
			range_99++;
		}
		else {
			range_999++;
		}
	}
	printf("10 : %d\n", range_9);
	printf("100 : %d\n", range_99);
	printf("1000 : %d\n", range_999);
#endif
}


int main(int argc, char *argv[]) {
	fp = fopen("input.txt", "r");
	int o;
	int choice_of_group_size;
	while ((o = getopt(argc, argv, "g:k:b:")) != -1)
	{
		switch (o)
		{
			case 'g':
			choice_of_group_size = atoi(optarg);
			break;
			case 'k':
			break;
			case 'b':
			break;
			
			default:
			break;
		}
	}
	vector<pii> query_points; // first: index to rawdata (posts), second: exact keyword
	get_keyword(choice_of_group_size, &query_points); // load data into our space.
    return 0;
}