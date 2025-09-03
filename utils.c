#include "utils.h"

size_t round_log(size_t value){
    // Non optimal way to way to do it but will keep it this way
    // for making the entire code simple and easy to understand
    // If interested, look at the way to round up to next highest power of 2 with bit shifting
    size_t power = 0;
    size_t cvalue = 1;
    while(cvalue <= value){
        cvalue = cvalue * 2;
        power++;
    }
    return power;
}