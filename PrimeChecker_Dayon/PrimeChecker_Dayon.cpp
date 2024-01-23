#include <iostream>
#include <vector>
#include <thread>


#define LIMIT 10000000
#define INT_MAX 2147483647 

/*
This function checks if an integer n is prime.

Parameters:
n : int - integer to check

Returns true if n is prime, and false otherwise.
*/
bool check_prime(const int& n);
void prime_checker(int start, int end, int threadID);
int get_upperbound();
int get_num_threads();

int main() {
    std::vector<std::thread> threads;
    int upperbound;
    int num_threads;

    upperbound = get_upperbound();
    num_threads = get_num_threads();

    int numb_per_thread = upperbound / num_threads;
    // Launch threads
    for (int i = 0; i < num_threads; ++i) {
        int start = i * numb_per_thread + 1;
        int end = (i == num_threads - 1) ? upperbound : (i + 1) * numb_per_thread;

        // Create a thread and pass the range to process
        threads.emplace_back(prime_checker, start, end, i + 1);
    }
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}

void prime_checker(int start, int end, int threadID) {
    std::vector<int> primes;

    for (int current_num = start; current_num <= end; current_num++) {
        std::cout << "Thread " << threadID << ": Processing " << current_num << std::endl;
        if (check_prime(current_num)) {
            primes.push_back(current_num);
        }
    }

    std::cout << primes.size() << " primes were found." << std::endl;
}

int get_upperbound() {
    int upperbound;
    int key = 0;

    while (!key) {
        std::cout << "Enter an integer (up to " << INT_MAX << "): ";

        std::cin >> upperbound;

        if (std::cin.fail() || upperbound < 0 || upperbound > upperbound) {
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

    int maxThreads = std::thread::hardware_concurrency();
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