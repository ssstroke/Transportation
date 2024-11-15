#include "Plan.h"

#include <iostream>

int main()
{
    // Number of columns is a number of consumers.
    // Number of rows is a number of producers.
    // Cells in a last column interpret as a total amount for producer Ai (i = 0, ..., number_of_consumers - 1).
    // Cells in a last row interpret as a total need for consumer Bi (i = 0, ..., number_of_producers - 1).
    // Other cells interpret as a cost of transportation from producer Ai to consumer Bi.
    // Last south-east corner cell is ignored.
    const std::vector<std::vector<size_t>> initial_table = {
    //  Nizhniy Novgorod Perm  Krasnodar  Amount 
        {4893,           4280, 6213,      1000}, // Omsk
        {5327,           4296, 6188,       200}, // Novosibirsk
        {6006,           5030, 7224,       600}, // Tomsk
        { 700,            600,  500,         0}  // Needs
    };

    auto plan = Plan(initial_table);

    plan.Start_VogelsApproximation();

    plan.Print();
}
