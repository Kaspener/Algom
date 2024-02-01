#include "fraction.hpp"
#include <fstream>
#include <string>
#include <vector>

class Matrix
{
private:
    int n, m;
    std::vector<std::vector<Fraction>> matrix;
    void setMaxElement(int i, int j);
    bool checkZero(int i, int j);
    bool noSolutions();

public:
    Matrix(std::ifstream &in);

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
    Fraction mmax = matrix[starti][j];
    for (int i = starti + 1; i < n; ++i)
    {
        if (matrix[i][j].numerator != 0 && Fraction::abs(matrix[i][j]) > mmax)
        {
            mmax = matrix[i][j];
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
                if (matrix[i][m - 1].numerator != 0){
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
                if (flag) std::cout << "0";
                std::cout << std::endl;
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
    return 0;
}
