#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <future>
#include <numeric>


#define LIMIT 10000000
#define INT_MAX 2147483647

std::mutex primes_mutex; // Declare a mutex to protect the shared resource

/*
This function checks if an integer n is prime.

Parameters:
n : int - integer to check

Returns true if n is prime, and false otherwise.
*/
bool check_prime(const int& n);
void prime_checker(int start, int end, std::shared_ptr<std::promise<int>> resultPromise);
int get_upperbound();
int get_num_threads();

int main() {
    auto start_time = std::chrono::high_resolution_clock::now(); // Record the start time
    std::vector<std::thread> threads;
    std::vector<std::shared_future<int>> futures;
    std::vector<std::shared_ptr<std::promise<int>>> promises;

    int upperbound;
    int num_threads;

    upperbound = get_upperbound();
    num_threads = get_num_threads();

    int numb_per_thread = upperbound / num_threads;
    // Launch threads
    for (int i = 0; i < num_threads; ++i) {
        int start = i * numb_per_thread + 1;
        int end = (i == num_threads - 1) ? upperbound : (i + 1) * numb_per_thread;

        // Create a shared promise and shared future
        auto promise = std::make_shared<std::promise<int>>();
        std::shared_future<int> resultFuture = promise->get_future();

        // Use emplace_back to add a thread with the lambda function
        threads.emplace_back(prime_checker, start, end, promise);

        // Store the shared_future and shared_promise in the vectors
        futures.emplace_back(resultFuture);
        promises.emplace_back(std::move(promise));
    }
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // Record the end time
    std::chrono::duration<double> elapsed_time = end_time - start_time;

    std::cout << "Total runtime: " << elapsed_time.count() << " ms" << std::endl;

    // Retrieve and accumulate the results
    int sum_prime = std::accumulate(futures.begin(), futures.end(), 0,
        [](int sum, const std::shared_future<int>& future) {
            return sum + future.get();
        });
    std::cout << "Total prime numbers: " << sum_prime-1 << std::endl;


    return 0;
}

void prime_checker(int start, int end, std::shared_ptr<std::promise<int>> resultPromise) {
    int count = 0;

    for (int current_num = start; current_num <= end; current_num++) {
//        std::cout << "Thread " << threadID << ": Processing " << current_num << std::endl;
        if (check_prime(current_num)) {
            // Use a lock_guard to lock the mutex and ensure mutual exclusion
            std::lock_guard<std::mutex> lock(primes_mutex);
            ++count;

        }
    }
    resultPromise->set_value(count);
}

int get_upperbound() {
    int upperbound;
    int key = 0;

    while (!key) {
        std::cout << "Enter an integer (up to " << INT_MAX << "): ";

        std::cin >> upperbound;

        if (std::cin.fail() || upperbound < 0 || upperbound > INT_MAX) {
            std::cout << "Invalid input. Please enter a valid integer within the specified range." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            std::cout << "Upperbound: " << upperbound << std::endl;
            key = 1;
        }
    }
    return upperbound;
}

int get_num_threads() {
    int key = 0;

    int maxThreads = 1024;
    int num_threads;

    while (!key) {
        std::cout << "Enter an integer (up to " << maxThreads << "): ";

        std::cin >> num_threads;

        if (std::cin.fail() || num_threads < 0 || num_threads > maxThreads) {
            std::cout << "Invalid input. Please enter a valid integer within the specified range." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            std::cout << "Number of threads: " << num_threads << std::endl;
            key = 1;
        }
    }
    return num_threads;
}

bool check_prime(const int& n) {
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}
