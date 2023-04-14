#include<bits/stdc++.h>
#include<unistd.h> // for using getopt

int main(int argc, char *argv[]) {
    int choice_of_group_size, K, beta;
    int o;
    while ((o = getopt(argc, argv, "g:k:b:")) != -1)
	{
		switch (o)
		{
			case 'g':
			choice_of_group_size = atoi(optarg);
            std::cout << "choice " << choice_of_group_size << std::endl;
			break;
			case 'k':
			K = atoi(optarg);
            std::cout << "K " << K << std::endl;
			break;
			case 'b':
			beta = atoi(optarg);
            std::cout << "beta " << beta << std::endl;
			break;
			default:
            std::cout << atoi(optarg) << std::endl;
			break;
		}
	}
}