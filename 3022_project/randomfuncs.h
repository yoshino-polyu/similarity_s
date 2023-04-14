#if !(_SVID_SOURCE || _XOPEN_SOURCE)

/**
 * @return double in [0, 1) 
*/
double drand48(void) {
    return rand() / (RAND_MAX + 1.0);
}

long int lrand48(void) {
    return rand();
}

long int mrand48(void) {
    return rand() > RAND_MAX / 2 ? rand() : -rand();
}

void srand48(long int seedval) {
    srand(seedval);
}
#endif