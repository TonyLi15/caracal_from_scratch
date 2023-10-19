// include required headers
#include <vector>
#include <cstdint>

// define constants
#define VERSION_ARRAY_MAX 1000

// globally unique 64-bit serial id (chapter 4)
// bit number of each field is not determined yet
struct sid {
    uint64_t epoch: 32;
    uint64_t per_node_seq_cnt: 16;
    uint64_t node_id: 16;
};

// tuple 
struct tuple {
    struct sid sid;
    uint64_t key;
    uint64_t value;
    std::vector<int> version_array;
};

//initialization of version array (chapter 4.1)
void init_version_array(tuple *t) {
   t->version_array.reserve(VERSION_ARRAY_MAX);
}

// adding version to version array
void add_version(tuple *t, int version) {
    int index = 0;
    // sort version array in ascending order (using insertion sort)
    while (index < t->version_array.size() && t->version_array[index] < version)
        index++;
    t->version_array.insert(t->version_array.begin() + index, version);
}
