//include required headers
#include "tuple.h"
#include <pthread.h>
#include <iostream>

// define constants
#define TUPLE_NUM 10
#define THREAD_NUM 4

// initialize database that contains (TUPLE_SIZE) tuples
void init_database(std::vector<tuple *> &database) {
    // create (TUPLE_SIZE) tuples
    for (int i = 0; i < TUPLE_NUM; i++) {
        tuple *t = new tuple;
        // initialize key
        t->key = i;
        // initialize version array
        init_version_array(t);
        // add tuple to database
        database.push_back(t);
    }
}

// update value to designated tuple in database
void update_value(std::vector<tuple *> &database, uint64_t key, uint64_t value) {
    database[key]->value = value;
    // version increment by 1 after each update
    if (database[key]->version_array.size() == 0)
        database[key]->version_array.push_back(0);
    else
        database[key]->version_array.push_back(database[key]->version_array.back() + 1);
}

/* Reserve for Concurrent Operations
void worker (std::vector<tuple *> &database, tuple *t){
    // lock tuple
    t->mutex.lock();
    // update value
    update_value(t, rand() % 100);
    // unlock tuple
    t->mutex.unlock();
}
*/

// print tuple after worker threads finish
void print_tuple(tuple *t) {
    // print key and value
    std::cout << "key: " << t->key << std::endl;
    std::cout << "value: " << t->value << std::endl;
    // print version array
    std::cout << "version array: ";
    for (int i = 0; i < t->version_array.size(); i++)
        std::cout << t->version_array[i] << " ";
    std::cout << std::endl;
    // print newest version number
    std::cout << "newest version number: " << t->version_array.back() << std::endl;
}

// main function
int main() {
    // initialize database
    std::vector<tuple *> database;
    init_database(database);
    // get tuple from database
    tuple *t = database[0];
    // initialize version array
    init_version_array(t);
    // random value change every timie 
    srand(time(NULL));
    // add random value to all tuples in database
    for (int i = 0; i < TUPLE_NUM; i++)
        update_value(database, i, rand() % 100);
    
    // print tuple for each tuples in database
    for (int i = 0; i < TUPLE_NUM; i++)
        print_tuple(database[i]);

    /* Reserve for Concurrent Operations
    // create worker threads using pthread
    pthread_t worker[THREAD_NUM];
    for (int i = 0; i < TUPLE_NUM; i++)
        pthread_create(&worker[i], NULL, (void *(*)(void *))worker, t);
    // join worker threads
    for (int i = 0; i < TUPLE_NUM; i++)
        pthread_join(worker[i], NULL);
    */

    return 0;
}