#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <optional>
#include <vector>

struct PlanCell
{
    size_t cost = 0;
    size_t amount = 0;
    bool fake = false;
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

        size_t iteration = 0;
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

            std::cout << "Least cost. Iteration " << iteration++ << ":" << std::endl;
            this->Print();
            std::cout << std::endl;
        }
    }

    // NOTE: cost can not be equal to SIZE_MAX.
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

            std::cout << "Vogel's approximation. Iteration " << iteration++ << ":" << std::endl;
            this->Print();
            std::cout << std::endl;
        }
    }

    void Optimize_MODI()
    {
        static size_t iteration = 0;

        auto u = std::vector<std::optional<int64_t>>(this->producers_count);
        auto v = std::vector<std::optional<int64_t>>(this->consumers_count);

        u[0] = 0;

        while (
            std::any_of(u.begin(), u.end(), [](const std::optional<int64_t>& x)
                {
                    return x.has_value() == false;
                })
            ||
            std::any_of(v.begin(), v.end(), [](const std::optional<int64_t>& x)
                {
                    return x.has_value() == false;
                })
            ) // i.e. if there are any unset elements
        {
            for (size_t prod = 0; prod < this->producers_count; ++prod)
            {
                for (size_t cons = 0; cons < this->consumers_count; ++cons)
                {
                    if (this->grid[prod][cons].amount != 0 || this->grid[prod][cons].fake == true)
                    {
                        const auto COST = this->grid[prod][cons].cost;
                        if (u[prod].has_value())
                        {
                            v[cons] = COST - u[prod].value();
                        }
                        else if (v[cons].has_value())
                        {
                            u[prod] = COST - v[cons].value();
                        }
                    }
                }
            }
        }

        auto delta = std::vector<std::vector<int64_t>>(this->producers_count, std::vector<int64_t>(this->consumers_count));
        int64_t delta_max = std::numeric_limits<int64_t>().min();
        size_t delta_max_prod_idx = 0;
        size_t delta_max_cons_idx = 0;
        for (size_t prod = 0; prod < this->producers_count; ++prod)
        {
            for (size_t cons = 0; cons < this->consumers_count; ++cons)
            {
                delta[prod][cons] = u[prod].value() + v[cons].value() - this->grid[prod][cons].cost;
                if (delta[prod][cons] > delta_max)
                {
                    delta_max = delta[prod][cons];
                    delta_max_prod_idx = prod;
                    delta_max_cons_idx = cons;
                }
            }
        }

        // This happens only if plan is not optimal.
        // Find cycle.
        if (delta_max > 0)
        {
            this->cycle_cell = std::pair<size_t, size_t>(delta_max_prod_idx, delta_max_cons_idx);

            // assert() as it should never ever fail.
            assert(this->CycleHorizontal(this->cycle_cell) == true);

            size_t min_cost = std::numeric_limits<size_t>().max();
            size_t min_cost_prod_idx = 0;
            size_t min_cost_cons_idx = 0;
            for (const auto& cell : this->cycle)
            {
                const auto PROD = cell.first;
                const auto CONS = cell.second;
                if (
                    (PROD != delta_max_prod_idx || CONS != delta_max_cons_idx) &&
                    this->grid[PROD][CONS].amount < min_cost
                    )
                {
                    min_cost = this->grid[PROD][CONS].amount;
                    min_cost_prod_idx = PROD;
                    min_cost_cons_idx = CONS;
                }
            }

            for (size_t i = 0; i < this->cycle.size(); ++i)
            {
                const auto PROD = this->cycle[i].first;
                const auto CONS = this->cycle[i].second;
                if (i % 2 == 0) // the cell has "plus" sign.
                {
                    this->grid[PROD][CONS].amount += min_cost;
                }
                else // the cell has "minus" sign.
                {
                    this->grid[PROD][CONS].amount -= min_cost;
                }
            }

            this->cycle.clear();

            std::cout << "MODI optimization. Iteration " << iteration++ << ":" << std::endl;
            this->Print();
            std::cout << std::endl;

            size_t non_empty_cells = 0;
            for (size_t prod = 0; prod < this->producers_count; ++prod)
            {
                for (size_t cons = 0; cons < this->consumers_count; ++cons)
                {
                    if (this->grid[prod][cons].amount != 0)
                    {
                        ++non_empty_cells;
                    }
                }
            }
            if (non_empty_cells < this->producers_count + this->consumers_count - 1)
            {
                size_t min_cost = std::numeric_limits<size_t>().max();
                size_t min_cost_prod_idx = 0;
                size_t min_cost_cons_idx = 0;
                for (size_t prod = 0; prod < this->producers_count; ++prod)
                {
                    for (size_t cons = 0; cons < this->consumers_count; ++cons)
                    {
                        if (this->grid[prod][cons].cost < min_cost)
                        {
                            min_cost = this->grid[prod][cons].cost;
                            min_cost_prod_idx = prod;
                            min_cost_cons_idx = cons;
                        }
                    }
                }
                this->grid[min_cost_prod_idx][min_cost_cons_idx].fake = true;
            }

            this->Optimize_MODI();
        }
        else
        {
            return;
        }
    }

    // TODO
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

    size_t GetTotalCost()
    {
        size_t total_cost = 0;
        for (size_t prod = 0; prod < this->producers_count; ++prod)
        {
            for (size_t cons = 0; cons < this->producers_count; ++cons)
            {
                total_cost += this->grid[prod][cons].amount * this->grid[prod][cons].cost;
            }
        }
        return total_cost;
    }

private:
    size_t producers_count;
    size_t consumers_count;

    std::vector<size_t> producers_amounts;
    std::vector<size_t> consumers_needs;

    std::vector<std::vector<PlanCell>> grid;

    std::vector<std::pair<size_t, size_t>> cycle;
    std::pair<size_t, size_t> cycle_cell;

    bool CycleHorizontal(const std::pair<size_t, size_t>& cell)
    {
        for (size_t cons = 0; cons < this->consumers_count; ++cons)
        {
            // cell.first  is prod
            // cell.second is cons
            if (cons != cell.second && this->grid[cell.first][cons].amount != 0)
            {
                const auto TURN_CELL = std::pair<size_t, size_t>(cell.first, cons);
                if (CycleVertical(TURN_CELL) == true)
                {
                    this->cycle.push_back(TURN_CELL);
                    return true;
                }
            }
        }
        return false;
    }

    bool CycleVertical(const std::pair<size_t, size_t>& cell)
    {
        for (size_t prod = 0; prod < this->producers_count; ++prod)
        {
            // Found cycle.
            if (prod == cycle_cell.first && cell.second == cycle_cell.second)
            {
                const auto TURN_CELL = std::pair<size_t, size_t>(prod, cell.second);
                this->cycle.push_back(TURN_CELL);
                return true;
            }
            else if (prod != cell.first && this->grid[prod][cell.second].amount != 0)
            {
                const auto TURN_CELL = std::pair<size_t, size_t>(prod, cell.second);
                if (CycleHorizontal(TURN_CELL) == true)
                {
                    this->cycle.push_back(TURN_CELL);
                    return true;
                }
            }
        }
        return false;
    }
};
