#include "fraction.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

class Matrix
{
private:
    int n, m;
    int matrix_size = 0;
    bool noSolution = false;
    std::vector<std::vector<Fraction>> matrix;

    void setMaxElement(int i, int j);
    bool checkZero(int i, int j);
    bool noSolutions();
    void printOneBasis(const std::vector<int> &basis) const;
    std::vector<std::vector<int>> basisPositions();

public:
    Matrix(std::ifstream &in);
    void Basises();
    int getMatrixSize() const;
    void printMatrix() const;
    void JordanGauss();
};

Matrix::Matrix(std::ifstream &in)
{
    in >> n >> m;
    matrix.resize(n, std::vector<Fraction>(m));
    for (auto &it : matrix)
    {
        for (auto &num : it)
        {
            in >> num;
        }
    }
}

void Matrix::printMatrix() const
{
    for (const auto &row : matrix)
    {
        for (const auto &el : row)
        {
            std::cout << el << " ";
        }
        std::cout << std::endl;
    }
}

bool Matrix::checkZero(int i, int j)
{
    while (i < n)
    {
        if (matrix[i][j].numerator != 0)
            return false;
        ++i;
    }
    return true;
}

void Matrix::setMaxElement(int starti, int j)
{
    int swapPos = -1;
    Fraction mmax = Fraction::abs(matrix[starti][j]);
    for (int i = starti + 1; i < n; ++i)
    {
        if (Fraction::abs(matrix[i][j]) > mmax)
        {
            mmax = Fraction::abs(matrix[i][j]);
            swapPos = i;
        }
    }
    if (swapPos != -1)
    {
        for (int i = 0; i < m; ++i)
        {
            std::swap(matrix[starti][i], matrix[swapPos][i]);
        }
    }
}

bool Matrix::noSolutions()
{
    for (int i = n - 1; i > -1; --i)
    {
        if (matrix[i][m - 1].numerator != 0)
        {
            bool isZero = true;
            for (int j = 0; j < m - 1; ++j)
            {
                if (matrix[i][j].numerator != 0)
                {
                    isZero = false;
                    break;
                }
            }
            if (isZero)
                return true;
        }
    }
    return false;
}

int Matrix::getMatrixSize() const
{
    return matrix_size;
}

std::vector<std::vector<int>> Matrix::basisPositions()
{
    auto nextSet = [](std::vector<int> &a, int n, int m)
    {
        int k = m;
        for (int i = k - 1; i >= 0; --i)
            if (a[i] < n - k + i + 1)
            {
                ++a[i];
                for (int j = i + 1; j < k; ++j)
                    a[j] = a[j - 1] + 1;
                return true;
            }
        return false;
    };

    std::vector<std::vector<int>> vec;
    if (m - 1 >= matrix_size)
    {
        std::vector<int> tmp(matrix_size);
        for (int i = 0; i < matrix_size; ++i)
        {
            tmp[i] = i;
        }
        vec.push_back(tmp);
        while (nextSet(tmp, m - 2, matrix_size))
            vec.push_back(tmp);
    }
    return vec;
}

void Matrix::printOneBasis(const std::vector<int> &basis) const
{
    std::cout << "{ ";
    for (int i = 0; i < basis.size() - 1; ++i)
    {
        std::cout << "x" << basis[i] + 1 << "; ";
    }
    std::cout << "x" << basis.back() + 1 << " }   ";
}

void Matrix::Basises()
{
    if (noSolution)
    {
        std::cout << "The matrix has no solutions, so you cannot use the search for basic solutions" << std::endl;
        return;
    }
    std::vector<std::vector<int>> basis = basisPositions();
    std::cout << "SIZE = " << basis.size() << std::endl;
    std::vector<std::vector<Fraction>> startVec(matrix_size, std::vector<Fraction>(m));
    for (auto &basisItem : basis)
    {
        std::vector<std::vector<int>> isUsed(matrix_size, std::vector<int>());
        for (int i = 0; i < matrix_size; ++i)
        {
            for (int j = 0; j < m; ++j)
            {
                startVec[i][j] = matrix[i][j];
            }
        }
        bool flag = true;
        for (int row = 0; row < matrix_size; ++row)
        {
            for (auto &it : basisItem)
            {
                if (startVec[row][it].numerator != 0)
                {
                    isUsed[row].push_back(it);
                }
            }
            if (isUsed[row].size() == 0)
            {
                flag = false;
                break;
            }
            if (isUsed[row].size() == 1)
            {
                for (int i = 0; i < row; ++i)
                {
                    auto it = std::find(isUsed[i].begin(), isUsed[i].end(), isUsed[row][0]);
                    if (it != isUsed[i].end())
                        isUsed[i].erase(std::find(isUsed[i].begin(), isUsed[i].end(), isUsed[row][0]));
                    if (isUsed[i].size() == 0) {
                        flag = false;
                        break;
                    }
                }
            }
        }
        if (!flag)
        {
            printOneBasis(basisItem);
            std::cout << "NO BASIS" << std::endl;
            continue;
        }
        for (int row = 0; row < matrix_size; ++row)
        {
            if (isUsed[row].size() > 1)
            {
                int num = isUsed[row][0];
                isUsed[row].clear();
                isUsed[row].push_back(num);
                for (int i = row + 1; i < matrix_size; ++i)
                {
                    auto it = std::find(isUsed[i].begin(), isUsed[i].end(), num);
                    if (it != isUsed[i].end())
                        isUsed[i].erase(std::find(isUsed[i].begin(), isUsed[i].end(), num));
                }
            }
        }
        for (int row = 0; row < matrix_size; ++row)
        {
            Fraction del = startVec[row][isUsed[row][0]];
            for (int i = 0; i < m; ++i)
            {
                if (del.numerator != 0)
                    startVec[row][i] /= del;
                else{
                    flag = false;
                    break;
                }
            }
            for (int i = row + 1; i < matrix_size; ++i)
            {
                for (int j = isUsed[row][0] + 1; j < m; ++j)
                {
                    startVec[i][j] -= startVec[row][j] * startVec[i][isUsed[row][0]];
                }
                for (int j = isUsed[row][0] - 1; j > -1; --j)
                {
                    startVec[i][j] -= startVec[row][j] * startVec[i][isUsed[row][0]];
                }
                startVec[i][isUsed[row][0]].numerator = 0;
                startVec[i][isUsed[row][0]].denominator = 1;
            }
            for (int i = row - 1; i > -1; --i)
            {
                for (int j = isUsed[row][0] + 1; j < m; ++j)
                {
                    startVec[i][j] -= startVec[row][j] * startVec[i][isUsed[row][0]];
                }
                for (int j = isUsed[row][0] - 1; j > -1; --j)
                {
                    startVec[i][j] -= startVec[row][j] * startVec[i][isUsed[row][0]];
                }
                startVec[i][isUsed[row][0]].numerator = 0;
                startVec[i][isUsed[row][0]].denominator = 1;
            }
        }
        if (!flag)
        {
            printOneBasis(basisItem);
            std::cout << "NO BASIS" << std::endl;
            continue;
        }
        std::vector<Fraction> res(m - 1, 0);
        for (auto &it : basisItem)
        {
            for (int i = 0; i < matrix_size; ++i)
            {
                if (isUsed[i][0] == it)
                {
                    res[it] = startVec[i][m - 1];
                    break;
                }
            }
        }
        printOneBasis(basisItem);
        std::cout << "{ ";
        for (int i = 0; i < m - 2; ++i)
        {
            std::cout << res[i] << "; ";
        }
        std::cout << res.back() << " }" << std::endl;
    }
}

void Matrix::JordanGauss()
{
    int row = 0;
    int column = 0;

    std::cout << "Start matrix:" << std::endl;
    printMatrix();
    std::cout << std::endl;
    for (; row < n && column < m - 1; ++row, ++column)
    {
        while (checkZero(row, column) && column < m - 1)
        {
            column++;
            std::cout << "Column " << column << " only zero!" << std::endl;
        }
        if (column == m - 1)
        {
            break;
        }
        setMaxElement(row, column);
        std::cout << "Set max element at " << row << " " << column << std::endl;
        printMatrix();
        std::cout << std::endl;
        for (int i = column + 1; i < m; ++i)
        {
            matrix[row][i] /= matrix[row][column];
        }
        matrix[row][column].numerator = 1;
        matrix[row][column].denominator = 1;
        std::cout << "Change line " << row << std::endl;
        printMatrix();
        std::cout << std::endl;
        for (int i = row + 1; i < n; ++i)
        {
            for (int j = column + 1; j < m; ++j)
            {
                matrix[i][j] -= matrix[row][j] * matrix[i][column];
            }
            matrix[i][column].numerator = 0;
            matrix[i][column].denominator = 1;
        }
        for (int i = row - 1; i > -1; --i)
        {
            for (int j = column + 1; j < m; ++j)
            {
                matrix[i][j] -= matrix[row][j] * matrix[i][column];
            }
            matrix[i][column].numerator = 0;
            matrix[i][column].denominator = 1;
        }
        std::cout << "Zeroing a column " << column << std::endl;
        printMatrix();
        std::cout << std::endl;
    }

    if (noSolutions())
    {
        std::cout << "The system has no solutions" << std::endl;
        noSolution = true;
        return;
    }
    else
    {
        std::cout << "Solution:" << std::endl;
        for (int i = 0, j = 0; i < n && j < m - 1; ++i, ++j)
        {
            if (matrix[i][j] == Fraction{1})
            {
                bool flag = true;
                if (matrix[i][m - 1].numerator != 0)
                {
                    std::cout << "x" << j + 1 << " = " << matrix[i][m - 1] << " ";
                    flag = false;
                }
                else
                    std::cout << "x" << j + 1 << " = ";
                for (int tmp = j + 1; tmp < m - 1; ++tmp)
                {
                    if (matrix[i][tmp].numerator != 0)
                    {
                        flag = false;
                        std::cout << (matrix[i][tmp].numerator < 0 ? "+ " : "- ");
                        std::cout << Fraction::abs(matrix[i][tmp]) << "(x" << tmp + 1 << ") ";
                    }
                }
                if (flag)
                    std::cout << "0";
                std::cout << std::endl;
                matrix_size++;
            }
            else
            {
                i--;
            }
        }
    }
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
    matrix.JordanGauss();
    std::cout << std::endl;
    matrix.Basises();
    return 0;
}