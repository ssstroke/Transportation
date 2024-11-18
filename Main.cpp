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

    auto plan = Plan(initial_table_0);

    std::cout << "Vogel's approximation." << std::endl << "================================" << std::endl << std::endl;

    plan.Start_VogelsApproximation();

    std::cout << "MODI optimization." << std::endl << "================================" << std::endl << std::endl;

    plan.Optimize_MODI();

    std::cout << "Total cost: " << plan.GetTotalCost();

    return 0;
}
