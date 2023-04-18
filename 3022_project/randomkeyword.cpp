#include<bits/stdc++.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include"randomfuncs.h"
#include"randomkeyword.h"

/**
 * First we the point sets of different sizes. Query points are selected from these point sets of different sizes.
 * The size of our dataset is 2e7, which means we have a total of 2e7 posts. 
 * Assume the number of keywords in a keyword list is at most 10 -> range of length of keyword list = [1,10]
 * post = <len_of_keyword, keyword_list, longitude(x), latitude(y)>. (assume x and y are integer [0, 1e8])
 * 1e7 posts' keywrods are randomly generated from 2e5 types of keyword, which is for creating point group with large size.
 * another 1e7 posts' keyword are randomly generated from 6e6 types of keyword, which is for creating point group with samll size.
 * 
 * Output:
 * The first line contains a single integer n (n = 2e7) - the number of posts
 * Each of the next n lines describes a post. l - the length of keyword list, ki - the keyword, x - x-coordinate, y - y-coordinate.
 * n 
 * l k1 k2 ... kl x y (1-th line)
*/

int main(int argc, char *argv[]) 
{
    printf("%d\n", SZ_DATA * 2);
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