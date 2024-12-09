#include "Plan.h"

#include <iostream>

int main()
{
    const std::vector<std::vector<size_t>> initial_table_0 = {
    //  Nizhniy Novgorod Perm  Krasnodar  Amount 
        {4893,           4280, 6213,      1000}, // Omsk
        {5327,           4296, 6188,       200}, // Novosibirsk
        {6006,           5030, 7224,       600}, // Tomsk
        { 700,            600,  500,         0}  // Needs
    };
    const std::vector<std::vector<size_t>> initial_table_1 = {
        {10, 7, 2, 5, 5, 100},
        {4, 9, 8, 1, 3, 300},
        {5, 12, 16, 8, 7, 180},
        {7, 4, 6, 3, 11, 320},
        {200, 250, 120, 130, 200, 0}
    };
    const std::vector<std::vector<size_t>> initial_table_2 = {
        {10, 19, 17, 18, 16, 21, 450},
        {13, 14, 11, 17, 18, 19, 400},
        {15, 11, 7, 19, 19, 22, 150},
        {14, 13, 12, 18, 21, 23, 150},
        {21, 23, 10, 20, 15, 16, 250},
        {200, 300, 400, 250, 150, 100, 0}
    };
    const std::vector<std::vector<size_t>> initial_table_3 = {
        {7, 8, 5, 3, 11},
        {2, 4, 5, 9, 11},
        {6, 3, 1, 2, 8},
        {5, 9, 9, 7, 0}
    };
    const std::vector<std::vector<size_t>> initial_table_4 = {
        {3, 4, 6, 4, 10, 140},
        {9, 10, 15, 12, 6, 50},
        {8, 8, 14, 8, 5, 260},
        {150, 170, 30, 90, 10, 0}
    };

    auto plan = Plan(initial_table_0);

    /*std::cout << "Vogel's approximation." << std::endl << "================================" << std::endl << std::endl;

    plan.Start_VogelsApproximation();*/
    
    /*std::cout << "Least cost." << std::endl << "================================" << std::endl << std::endl;

    plan.Start_LeastCost();

    std::cout << "Total cost: " << plan.GetTotalCost() << std::endl;*/

    /*std::cout << "MODI optimization." << std::endl << "================================" << std::endl << std::endl;

    plan.Optimize_MODI();*/

    std::cout << "Hungarian optimization." << std::endl << "================================" << std::endl << std::endl;

    plan.Optimize_Hungarian();

    plan.Print();

    std::cout << "Total cost: " << plan.GetTotalCost();

    return 0;
}
