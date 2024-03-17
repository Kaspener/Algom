#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <numeric>

class Matrix
{
private:
    int m_rows, m_columns;
    int maxColumnWidth = 0;
    std::vector<std::vector<unsigned long long>> cost;
    std::vector<std::vector<long long>> count;
    std::vector<unsigned long long> suppliers;
    std::vector<unsigned long long> consumers;

public:
    Matrix(){};
    Matrix(std::ifstream &in);
    int rows() const { return m_rows; }
    int columns() const { return m_columns; }
    unsigned long long costAt(int row, int column) const { return cost[row][column]; }
    long long countAt(int row, int column) const { return count[row][column]; }
    void setCountAt(int row, int column, long long count) { this->count[row][column] = count; }
    unsigned long long supplierAt(int index) const { return suppliers[index]; }
    unsigned long long consumersAt(int index) const { return consumers[index]; }
    void print() const;
    int makeClose();
    std::vector<std::pair<int, int>> getSortedCoords() const;
    void printDistribution() const;
};

Matrix::Matrix(std::ifstream &in)
{
    in >> m_rows >> m_columns;
    cost.resize(m_rows, std::vector<unsigned long long>(m_columns));
    count.resize(m_rows, std::vector<long long>(m_columns, -1));
    suppliers.resize(m_rows);
    consumers.resize(m_columns);
    int currentWidth;
    for (int i = 0; i < m_rows; ++i)
    {
        for (int j = 0; j < m_columns; ++j)
        {
            in >> cost[i][j];
            currentWidth = std::to_string(cost[i][j]).size();
            maxColumnWidth = std::max(maxColumnWidth, currentWidth);
        }
        in >> suppliers[i];
        currentWidth = std::to_string(suppliers[i]).size();
        maxColumnWidth = std::max(maxColumnWidth, currentWidth);
    }
    for (int i = 0; i < m_columns; ++i)
    {
        in >> consumers[i];
        currentWidth = std::to_string(consumers[i]).size();
        maxColumnWidth = std::max(maxColumnWidth, currentWidth);
    }
}

void Matrix::print() const
{
    int spacing = 9;

    auto printBorder = [](int m_columns, int maxColumnWidth, int spacing) -> void
    {
        for (int i = 0; i <= m_columns + 1; ++i)
        {
            std::cout << "+" << std::string((maxColumnWidth + spacing - 1), '-');
        }
        std::cout << "+" << std::endl;
    };
    std::cout << std::string(maxColumnWidth + spacing, ' ');
    printBorder(m_columns - 2, maxColumnWidth, spacing);
    std::cout << std::string(maxColumnWidth + spacing, ' ');
    std::cout << "|";
    int half = (maxColumnWidth + spacing - 2) / 2;
    for (int i = 0; i < m_columns; ++i)
    {
        std::cout << std::string(half, ' ') << "\033[33mB" << i + 1 << "\033[0m" << std::string(maxColumnWidth + spacing - 3 - half, ' ') << "|";
    }
    std::cout << std::endl;
    printBorder(m_columns, maxColumnWidth, spacing);
    for (int i = 0; i < m_rows; ++i)
    {
        std::cout << "|";
        std::cout << std::string(half, ' ') << "\033[33mA" << i + 1 << "\033[0m" << std::string(maxColumnWidth + spacing - 3 - half, ' ') << "|";
        for (int j = 0; j < m_columns; ++j)
        {
            std::cout << std::setw(maxColumnWidth + spacing - 1) << std::right << cost[i][j] << "|";
        }
        std::cout << "\033[33m" << std::setw(maxColumnWidth + spacing - 1) << std::right << suppliers[i] << "\033[0m|" << std::endl;
        std::cout << "|" << std::string(maxColumnWidth + spacing - 1, ' ') << "|";
        for (int j = 0; j < m_columns; ++j)
        {
            if (count[i][j] != -1)
                std::cout << "\033[32m" << std::setw(maxColumnWidth + spacing - 1) << std::right << count[i][j] << "\033[0m|";
            else
                std::cout << std::string(maxColumnWidth + spacing - 1, ' ') << "|";
        }
        std::cout << std::string(maxColumnWidth + spacing - 1, ' ') << "|" << std::endl;
        printBorder(m_columns, maxColumnWidth, spacing);
    }
    std::cout << std::string(maxColumnWidth + spacing, ' ') << "|";
    for (int i = 0; i < m_columns; ++i)
    {
        std::cout << "\033[33m" << std::setw(maxColumnWidth + spacing - 1) << std::right << consumers[i] << "\033[0m|";
    }
    std::cout << std::endl
              << std::string(maxColumnWidth + spacing, ' ');
    printBorder(m_columns - 2, maxColumnWidth, spacing);
    std::cout << std::endl;
}

int Matrix::makeClose()
{
    unsigned long long suppliersCount = std::accumulate(suppliers.begin(), suppliers.end(), 0);
    unsigned long long consumersCount = std::accumulate(consumers.begin(), consumers.end(), 0);
    if (suppliersCount == consumersCount)
    {
        std::cout << "The model of the transport task is closed!" << std::endl;
        return 0;
    }
    std::cout << "The model of the transport task is open! Make close model:" << std::endl;
    if (suppliersCount > consumersCount)
    {
        consumers.push_back(suppliersCount - consumersCount);
        for (int i = 0; i < m_rows; ++i)
        {
            cost[i].push_back(0);
            count[i].push_back(-1);
        }
        m_columns++;
        print();
        return 1;
    }
    suppliers.push_back(consumersCount - suppliersCount);
    cost.push_back(std::vector<unsigned long long>(m_columns, 0));
    count.push_back(std::vector<long long>(m_columns, -1));
    m_rows++;
    print();
    return 2;
}

std::vector<std::pair<int, int>> Matrix::getSortedCoords() const
{
    std::vector<std::pair<unsigned long long, std::pair<int, int>>> vec;
    for (int i = 0; i < m_rows; ++i)
    {
        for (int j = 0; j < m_columns; ++j)
        {
            if (cost[i][j] != 0)
                vec.push_back({cost[i][j], {i, j}});
        }
    }
    std::sort(vec.begin(), vec.end());
    std::vector<std::pair<int, int>> coords(vec.size());
    for (int i = 0; i < vec.size(); ++i)
    {
        coords[i] = vec[i].second;
    }
    return coords;
}

void Matrix::printDistribution() const
{
    std::cout << "Expenses:" << std::endl;
    unsigned long long resSum = 0;
    for (int i = 0; i < m_rows; ++i)
    {
        unsigned long long rowSum = 0;
        for (int j = 0; j < m_columns; ++j)
        {
            rowSum += std::max(0ll, count[i][j]) * cost[i][j];
        }
        resSum += rowSum;
        std::cout << "\033[33mA" << i + 1 << "\033[0m = " << rowSum << std::endl;
    }
    std::cout << std::endl;
    std::cout << "\033[33mResult\033[0m = " << resSum << std::endl;
}

class Solution
{
private:
    Matrix matrix;

public:
    Solution(Matrix matrix)
    {
        this->matrix = matrix;
    }
    void run();
};

void Solution::run()
{
    matrix.print();
    int opened = matrix.makeClose();
    std::vector<unsigned long long> currentSuppliers(matrix.rows(), 0);
    std::vector<unsigned long long> currentConsumers(matrix.columns(), 0);
    std::vector<std::pair<int, int>> coords = matrix.getSortedCoords();
    int countOfSelected = 0;
    for (const auto &[row, column] : coords)
    {
        if (currentSuppliers[row] < matrix.supplierAt(row) && currentConsumers[column] < matrix.consumersAt(column))
        {
            long long minimum = std::min(matrix.supplierAt(row) - currentSuppliers[row], matrix.consumersAt(column) - currentConsumers[column]);
            matrix.setCountAt(row, column, minimum);
            currentSuppliers[row] += minimum;
            currentConsumers[column] += minimum;
            countOfSelected++;
            matrix.print();
        }
    }
    if (opened == 1)
    {
        for (int i = 0; i < currentSuppliers.size(); ++i)
        {
            if (currentSuppliers[i] < matrix.supplierAt(i))
            {
                matrix.setCountAt(i, matrix.columns() - 1, matrix.supplierAt(i) - currentSuppliers[i]);
                currentConsumers[matrix.columns() - 1] += matrix.supplierAt(i) - currentSuppliers[i];
                currentSuppliers[i] = matrix.supplierAt(i);
                countOfSelected++;
            }
        }
    }
    if (opened == 2)
    {
        for (int i = 0; i < currentConsumers.size(); ++i)
        {
            if (currentConsumers[i] < matrix.consumersAt(i))
            {
                matrix.setCountAt(matrix.rows() - 1, i, matrix.consumersAt(i) - currentConsumers[i]);
                currentSuppliers[matrix.rows() - 1] += matrix.consumersAt(i) - currentConsumers[i];
                currentConsumers[i] = matrix.consumersAt(i);
                countOfSelected++;
            }
        }
        matrix.print();
    }
    if (countOfSelected < matrix.rows() + matrix.columns() - 1)
    {
        int ct = matrix.rows() + matrix.columns() - 1 - countOfSelected;
        std::cout << "The plan is degenerate, it is necessary to add " << ct << " variables to the basis in:" << std::endl;
        for (const auto &[row, column] : coords)
        {
            if (matrix.countAt(row, column) == -1)
            {
                matrix.setCountAt(row, column, 0);
                std::cout << "\033[33m(A" << row + 1 << "; B" << column + 1 << ")\033[0m" << std::endl;
                ct--;
                if (ct == 0)
                {
                    break;
                }
            }
        }
        matrix.print();
    }
    matrix.printDistribution();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "./main <filename>" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::ifstream in(argv[1]);
    Matrix matrix(in);
    Solution solution(matrix);
    solution.run();
    return EXIT_SUCCESS;
}