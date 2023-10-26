//include required headers
#include "tuple.h"
#include <pthread.h>
#include <iostream>
#include <thread>

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

// read value from designated tuple in database
uint64_t read_value(std::vector<tuple *> &database, uint64_t key) {
    return database[key]->value;
}

// read the latest version number from designated tuple in database
int read_latest_version_num(std::vector<tuple *> &database, uint64_t key) {
    return database[key]->version_array.back();
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

// update random value to designated tuple in database
void update_random_value(std::vector<tuple *> &database, uint64_t key){
    uint64_t random_value = rand() % 100;
    update_value(database, key, random_value);
    // print out which key is updated and what value is updated
    std::cout << "key: " << key << " updated to value: " << random_value << std::endl;
}




// thread data structure
struct thread_data {
    int thread_id;
    std::vector<tuple *> *database;
};

// worker thread for read database
void *read_worker(void *arg) {
    // get thread data
    struct thread_data *data = (struct thread_data *) arg;
    // get database
    std::vector<tuple *> *database = data->database;
    // get thread id
    int thread_id = data->thread_id;
    // read value and latest version number from all tuples in database
    for (int i = 0; i < TUPLE_NUM; i++)
        std::cout << "thread " << thread_id << " key: " << i << " value: " << read_value(*database, i) << " latest version number: " << read_latest_version_num(*database, i) << std::endl;
    // print thread id
    std::cout << "thread " << thread_id << " read" << std::endl;

    // exit thread
    pthread_exit(NULL);
}


// worker thread for write database
void *write_worker(void *arg) {
    // get thread data
    struct thread_data *data = (struct thread_data *) arg;
    // get database
    std::vector<tuple *> *database = data->database;
    // get thread id
    int thread_id = data->thread_id;
    // update value to all tuples in database
    for (int i = 0; i < TUPLE_NUM; i++)
        update_random_value(*database, i);
    // print thread id
    std::cout << "thread " << thread_id << " updated" << std::endl;

    // exit thread
    pthread_exit(NULL);
}



// print tuple information
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
    
    // read value and latest version number from all tuples in database
    for (int i = 0; i < TUPLE_NUM; i++)
        std::cout << "key: " << i << " value: " << read_value(database, i) << " latest version number: " << read_latest_version_num(database, i) << std::endl;

    // update some value
    update_value(database, 0, 100);
    update_random_value(database, 1);
    update_random_value(database, 2);
    update_value(database, 3, 222);

    // read value and latest version number from all tuples in database
    for (int i = 0; i < TUPLE_NUM; i++)
        std::cout << "key: " << i << " value: " << read_value(database, i) << " latest version number: " << read_latest_version_num(database, i) << std::endl;   

    
    // print tuple for each tuples in database
    /*
    for (int i = 0; i < TUPLE_NUM; i++)
        print_tuple(database[i]);
    */

    
    // create worker threads
    pthread_t threads[THREAD_NUM];
    struct thread_data thread_data_array[THREAD_NUM];

    // create read worker threads
    for (int i = 0; i < THREAD_NUM; i++) {
        thread_data_array[i].thread_id = i;
        thread_data_array[i].database = &database;
        int rc = pthread_create(&threads[i], NULL, read_worker, (void *) &thread_data_array[i]);
        if (rc) {
            std::cout << "Error:unable to create thread," << rc << std::endl;
            exit(-1);
        }
    }

    // join read worker threads
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(threads[i], NULL);

    // create write worker threads
    for (int i = 0; i < THREAD_NUM; i++) {
        thread_data_array[i].thread_id = i;
        thread_data_array[i].database = &database;
        int rc = pthread_create(&threads[i], NULL, write_worker, (void *) &thread_data_array[i]);
        if (rc) {
            std::cout << "Error:unable to create thread," << rc << std::endl;
            exit(-1);
        }
    }

    // join write worker threads
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
