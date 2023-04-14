#include<bits/stdc++.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include"randomfuncs.h"
#include"randomkeyword.h"

/**
 * 首先我们需要有不同的大小的point set. 最后的query point就是从这些不同大小的point set里面进行选择的
 * 我们的dataset的大小是1e7, 也就是说我们一共有1e7个posts. 
 * 我们假设每个post的keyword list最长为10, range of length of keyword list = [1,10]
 * post = <len_of_keyword, keyword_list, longitude(x), latitude(y)>. (assume x and y are integer [0, 1e8])
 * 我们假设我们一共有2e5种keyword, 编号从 1 到2e5
 * Output of randomkeyword:
 * n
 * l k1 k2 ... kl x y - 1-th line
 * 
*/

int main(int argc, char *argv[]) 
{
    printf("%d\n", SZ_DATA * 2);
    // 一份sz data是用来映射到2e5上面的, 最后会产生大约2e5个point group每个point group的大小大概是200
    for (int i = 1; i <= SZ_DATA; i++) {
        int num_keyword = (int) (drand48() * LEN_KEYWORD) + 1;
        printf("%d ", num_keyword);
        std::set<int> kw; // to ensure one posts could not have two same keyword tags.
        for (int j = 1; j <= num_keyword; j++) {
            int keyword;
            while (true) {
                keyword = (int) (drand48() * KEYWORD_TYPE) + 1;
                if (kw.find(keyword) == kw.end()) {
                    kw.insert(keyword);
                    break;
                }
            }
            printf("%d ", keyword);
        }
        int x = (int) (drand48() * COORDINATES);
        int y = (int) (drand48() * COORDINATES);
        printf("%d %d\n", x,y);
    }
    // 一份sz data是用来映射到6e6上面的, 最后会产生大约6e6个point group每个point group的大小大概是10左右
    for (int i = 1; i <= SZ_DATA; i++) {
        int num_keyword = (int) (drand48() * LEN_KEYWORD) + 1;
        printf("%d ", num_keyword);
        std::set<int> kw;
        for (int j = 1; j <= num_keyword; j++) {
            int keyword;
            while (true) {
                keyword = (int) (drand48() * KEYWORD_TYPE_2) + KEYWORD_TYPE;
                if (kw.find(keyword) == kw.end()) {
                    kw.insert(keyword);
                    break;
                }
            }
            printf("%d ", keyword);
        }
        int x = (int) (drand48() * COORDINATES);
        int y = (int) (drand48() * COORDINATES);
        printf("%d %d\n", x,y);
    }
} // main