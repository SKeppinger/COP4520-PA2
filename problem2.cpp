/*
*  For obvious reasons, strategy 1 has a lot of issues, some of which the minotaur specifically wants to avoid.
*  
*  Both solutions 2 and 3 are viable to solve the problem, each with their own advantages and disadvantages.
*  
*  Solution 2 allows guests to roam freely when the showroom is marked BUSY, and they do not need to devote any time to waiting outside. Instead, they can just enter
*  when it is marked AVAILABLE. However, this allows a single guest to enter and exit the showroom multiple times very quickly, monopolizing it from the other guests.
*  
*  I believe solution 3 is the best solution. While guests do have to wait outside the showroom, it prevents the same guest from entering and exiting the showroom over and
*  over again without letting other guests enter.
*/

#include <omp.h>
#include <iostream>
#include <random>
using namespace std;

// This variable will represent whether the guests have all seen the vase.
bool all_visited = false;

// For this example, we will use N=5.
const int N = 5;

// Implementing the protocol:
int main()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> queue_dist(0, 1); //Since it isn't specified, giving guests a 50% chance to join the queue
    uniform_int_distribution<> leave_dist(0, 1); //Since it isn't specified, giving guests a 50% chance to leave the showroom
    int iam, queue_pos, queue_len = 0, unique_visits = 0;
    bool showroom_busy = false, seen_vase;
    #pragma omp parallel shared(queue_len, showroom_busy, all_visited, unique_visits) private(iam, queue_pos, seen_vase) num_threads(N)
    {
        iam = omp_get_thread_num();
        queue_pos = -1; //-1 represents that the guest is not in the queue, while 0 means they are in the showroom
        seen_vase = false;

        while(!all_visited)
        {
            // If there is a queue, and the showroom is available
            if (queue_len > 0 && !showroom_busy && queue_pos != -1)
                queue_pos--;

            // Determine whether should join queue
            if (queue_pos == -1 && queue_dist(gen) == 1)
            {
                queue_pos = queue_len;
                queue_len++;
            }
            // If in showroom
            else if (queue_pos == 0)
            {
                showroom_busy = true;

                if (!seen_vase)
                {
                    unique_visits++;
                    seen_vase = true;
                }

                // Determine whether should leave showroom
                if (leave_dist(gen) == 1)
                {
                    queue_pos = -1;
                    showroom_busy = false;
                }
            }

            // If all guests have visited the showroom
            if (unique_visits == N)
                all_visited = true;
        }
    }

    cout << all_visited;
    return 0;
}