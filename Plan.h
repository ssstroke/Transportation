#pragma once

#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

struct PlanCell
{
    size_t cost = 0;
    size_t amount = 0;
};

class Plan
{
public:
    Plan(const std::vector<std::vector<size_t>>& initial_table)
    {
        assert(initial_table.size() > 1);
        assert(initial_table[0].size() > 1);

        this->producers_count = initial_table.size() - 1;
        this->consumers_count = initial_table[0].size() - 1;

        producers_amounts = std::vector<size_t>(this->producers_count);
        for (size_t prod = 0; prod < this->producers_count; ++prod)
        {
            producers_amounts[prod] = initial_table[prod][this->consumers_count];
        }

        consumers_needs = std::vector<size_t>(this->consumers_count);
        for (size_t cons = 0; cons < this->consumers_count; ++cons)
        {
            consumers_needs[cons] = initial_table[this->producers_count][cons];
        }

        this->grid = std::vector<std::vector<PlanCell>>(this->producers_count, std::vector<PlanCell>(this->consumers_count));
        for (size_t prod = 0; prod < this->producers_count; ++prod)
        {
            for (size_t cons = 0; cons < this->consumers_count; ++cons)
            {
                this->grid[prod][cons] = PlanCell{
                    initial_table[prod][cons],
                    0
                };
            }
        }
    }

    void Start_LeastCost()
    {
        size_t total_producers_amount = std::accumulate(this->producers_amounts.begin(), this->producers_amounts.end(), (size_t)0);
        size_t total_consumers_needs = std::accumulate(this->consumers_needs.begin(), this->consumers_needs.end(), (size_t)0);

        while (total_producers_amount != 0 && total_consumers_needs != 0)
        {
            size_t prod_idx = 0;
            size_t cons_idx = 0;
            size_t least_cost = SIZE_MAX;

            for (size_t prod = 0; prod < this->producers_count; ++prod)
            {
                for (size_t cons = 0; cons < this->consumers_count; ++cons)
                {
                    if (this->grid[prod][cons].cost < least_cost && this->producers_amounts[prod] != 0 && this->consumers_needs[cons] != 0)
                    {
                        least_cost = this->grid[prod][cons].cost;
                        prod_idx = prod;
                        cons_idx = cons;
                    }
                }
            }

            const size_t SUPPLY_AMOUNT = std::min(this->producers_amounts[prod_idx], this->consumers_needs[cons_idx]);

            this->grid[prod_idx][cons_idx].amount = SUPPLY_AMOUNT;
            this->producers_amounts[prod_idx] -= SUPPLY_AMOUNT;
            this->consumers_needs[cons_idx] -= SUPPLY_AMOUNT;

            total_producers_amount = std::accumulate(this->producers_amounts.begin(), this->producers_amounts.end(), (size_t)0);
            total_consumers_needs = std::accumulate(this->consumers_needs.begin(), this->consumers_needs.end(), (size_t)0);

            this->Print();
        }
    }

    void Start_VogelsApproximation()
    {
        struct DiffCell
        {
            size_t cost;
            size_t prod_idx;
            size_t cons_idx;
        };

        size_t total_producers_amount = std::accumulate(this->producers_amounts.begin(), this->producers_amounts.end(), (size_t)0);
        size_t total_consumers_needs = std::accumulate(this->consumers_needs.begin(), this->consumers_needs.end(), (size_t)0);

        size_t iteration = 0;
        while (total_producers_amount != 0 && total_consumers_needs != 0)
        {
            auto diffs = std::vector<DiffCell>();
            diffs.reserve(this->producers_count + this->consumers_count);

            for (size_t prod = 0; prod < this->producers_count; ++prod)
            {
                DiffCell min_0 = { SIZE_MAX, 0, 0 };
                DiffCell min_1 = { SIZE_MAX, 0, 0 };

                if (this->producers_amounts[prod] != 0)
                {
                    for (size_t cons = 0; cons < this->consumers_count; ++cons)
                    {
                        if (this->consumers_needs[cons] != 0)
                        {
                            if (this->grid[prod][cons].cost < min_1.cost)
                            {
                                min_1.cost = this->grid[prod][cons].cost;
                                min_1.prod_idx = prod;
                                min_1.cons_idx = cons;
                                if (min_0.cost > min_1.cost)
                                {
                                    std::swap(min_0, min_1);
                                }
                            }
                        }
                    }

                    if (min_0.cost == SIZE_MAX && min_1.cost != SIZE_MAX)
                    {
                        diffs.push_back(DiffCell{min_1.cost, min_1.prod_idx, min_1.cons_idx });
                    }
                    else if (min_0.cost != SIZE_MAX && min_1.cost == SIZE_MAX)
                    {
                        diffs.push_back(DiffCell{ min_0.cost, min_0.prod_idx, min_0.cons_idx });
                    }
                    else if (min_0.cost != SIZE_MAX && min_1.cost != SIZE_MAX)
                    {
                        diffs.push_back(DiffCell{ min_1.cost - min_0.cost, min_0.prod_idx, min_0.cons_idx });
                    }
                }
            }

            for (size_t cons = 0; cons < this->consumers_count; ++cons)
            {
                DiffCell min_0 = { SIZE_MAX, 0, 0 };
                DiffCell min_1 = { SIZE_MAX, 0, 0 };

                if (this->consumers_needs[cons] != 0)
                {
                    for (size_t prod = 0; prod < this->producers_count; ++prod)
                    {
                        if (this->producers_amounts[prod] != 0)
                        {
                            if (this->grid[prod][cons].cost < min_1.cost)
                            {
                                min_1.cost = this->grid[prod][cons].cost;
                                min_1.prod_idx = prod;
                                min_1.cons_idx = cons;
                                if (min_0.cost > min_1.cost)
                                {
                                    std::swap(min_0, min_1);
                                }
                            }
                        }
                    }

                    if (min_0.cost != SIZE_MAX && min_1.cost == SIZE_MAX)
                    {
                        diffs.push_back(DiffCell{ min_0.cost, min_0.prod_idx, min_0.cons_idx });
                    }
                    else if (min_0.cost != SIZE_MAX && min_1.cost != SIZE_MAX)
                    {
                        diffs.push_back(DiffCell{ min_1.cost - min_0.cost, min_0.prod_idx, min_0.cons_idx });
                    }
                }
            }

            assert(diffs.size() != 0);

            // Here we interpret Diff in another way.
            // Cost is the difference between min_0 and min_1.
            // Indices are indices of min_0 (i.e. minimum cost).
            const auto MAX_DIFF = *std::max_element(diffs.begin(), diffs.end(), [](const DiffCell& a, const DiffCell& b)
                {
                    return a.cost < b.cost;
                });
            const auto SUPPLY_AMOUNT = std::min(this->producers_amounts[MAX_DIFF.prod_idx], this->consumers_needs[MAX_DIFF.cons_idx]);

            this->grid[MAX_DIFF.prod_idx][MAX_DIFF.cons_idx].amount = SUPPLY_AMOUNT;
            this->producers_amounts[MAX_DIFF.prod_idx] -= SUPPLY_AMOUNT;
            this->consumers_needs[MAX_DIFF.cons_idx] -= SUPPLY_AMOUNT;

            total_producers_amount = std::accumulate(this->producers_amounts.begin(), this->producers_amounts.end(), (size_t)0);
            total_consumers_needs = std::accumulate(this->consumers_needs.begin(), this->consumers_needs.end(), (size_t)0);

            std::cout << "Iteration " << iteration++ << ":" << std::endl;
            this->Print();
            std::cout << std::endl;
        }
    }

    // TODO
    void Optimize_1() {}
    void Optimize_2() {}

    void Print()
    {
        for (size_t prod = 0; prod < this->producers_count; ++prod)
        {
            for (size_t cons = 0; cons < this->consumers_count; ++cons)
            {
                std::printf("%-8zu ", this->grid[prod][cons].amount);
            }
            std::printf("%-8zu\n\n", this->producers_amounts[prod]);
        }
        for (size_t cons = 0; cons < this->consumers_count; ++cons)
        {
            std::printf("%-8zu ", this->consumers_needs[cons]);
        }
        std::cout << std::endl;
    }

private:
    size_t producers_count;
    size_t consumers_count;

    std::vector<size_t> producers_amounts;
    std::vector<size_t> consumers_needs;

    std::vector<std::vector<PlanCell>> grid;
};


