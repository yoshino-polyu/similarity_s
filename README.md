# similarity_s

main.cpp contains the implementation of my version and competitor's version [1].

randomkeyword.cpp is used to generate rawdata (i.e. the input file input.txt).

After the data is generated, main.cpp will read input.txt by using get_keyword() and build the corresponding index.

experiment_vary_pg.py is the python script we use to run the experiement automatically, and the result (response time) was stored in corresponding txt file.

Co_xxx means the results of competitor. My_xxx means the result of mine. For example, Co_OUTPUT_10_1_2 means K = 10, query point is retrieved from group 1 (size [0,9]). We generate totally 3 (3 different types of group, [0,9],[10,99],[100,999]) * 3 (3 trials, each trial with different query points, for obtaining general results) = 9 query set (10 query points form a query set). Therefore, 2 means is from Q12 (group 1 trial 2).

query_points_0x.txt store the query points retrieved from group x.

[1] Z. Li, Y. Li, and M. L. Yiu, ”Fast similarity search on keyword-induced point groups,” in Proceedings of the 26th ACM SIGSPATIAL Inter- national Conference on Advances in Geographic Information Systems, Nov. 2018, pp. 109-118.
