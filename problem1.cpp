/* The solution is as follows:
*  
*  Before the game starts, one of the guests will randomly be chosen to be the "counter."
*  
*  All non-counter guests will behave as follows:
*  - When they reach the end of the labyrinth and there is a cupcake, AND it is the first time that they have seen a cupcake at the end, they will eat it.
*  - Otherwise, when they reach the end, they will do nothing and simply leave.
*  
*  The counter guest will behave as follows:
*  - When they reach the end of the labyrinth, if there is not a cupcake, they will add 1 to a counter. Then, they will request a cupcake, but not eat it.
*  - When they reach the end of the labyrinth, if there is a cupcake, they will not eat it, and they will not update their counter.
*  - When the counter reaches N - 1, they will declare that every guest has visited the labyrinth.
*/

#include <omp.h>
#include <iostream>
#include <random>
using namespace std;

// The variable representing whether there is a cupcake on the plate must be accessed atomically, as it may be accessed and/or changed by multiple threads.
bool cupcake_on_plate = true;

// This variable will represent whether the guests think they have all visited the labyrinth. This also must be accessed atomically.
bool all_visited = false;

// For this example, we will use N=5.
const int N = 5;

// Implementing the protocol:
int main()
{
    random_device rd;
    mt19937 current_guest(rd());
    uniform_int_distribution<> dist(0, N - 1);
    int iam, counter;
    bool seen_cupcake;
    #pragma omp parallel shared(cupcake_on_plate, all_visited, current_guest) private(iam, counter, seen_cupcake) num_threads(N)
    {
        iam = omp_get_thread_num(); // The counter guest will be guest "0"
        counter = 0;
        seen_cupcake = false;
        while(!all_visited)
        {
            #pragma omp critical
            if (iam == dist(current_guest))
            {
                //Counter guest
                if (iam == 0)
                {
                    if (!cupcake_on_plate)
                    {
                        counter++;
                        cupcake_on_plate = true; // Requesting a cupcake
                        if (counter == N - 1)
                            all_visited = true;
                    }
                }
                //Other guests
                else
                {
                    if (cupcake_on_plate && !seen_cupcake)
                    {
                        cupcake_on_plate = false; // Eating the cupcake
                        seen_cupcake = true;
                    }
                }
            }
        }
    }

    cout << all_visited;
    return 0;
}