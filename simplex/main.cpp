#include "fraction.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

class Matrix
{
private:
    int rows, columns;
    int countOfVariables;
    bool noSolution = false;
    std::vector<std::vector<Fraction>> matrix;
    std::vector<std::string> symbols;
    std::vector<Fraction> Z;
    std::vector<int> basises;
    std::vector<int> free;
    std::vector<Fraction> CO;
    bool checkNegativElements();
    void setMaxElement(int i, int j);
    bool checkNegativ(int i, int j);
    bool noSolutions();
    bool noSupportingSolutions();
    void removeZeroVectors();
    bool makeCanon();
    void calcBasisesIndex();
    void GaussStep(const int &row, const int &pos);
    bool SimplexStep();
    std::pair<int, int> negativZPos();
    bool checkNegativZ();
    void SimpexSolution();

public:
    Matrix(std::ifstream &in);
    bool isMax;
    void printMatrix() const;
    void printStart(const std::string &max) const;
    void printSimplex(const int &row = -1, const int &column = -1);
    bool JordanGauss();
    void Simplex();
};

Matrix::Matrix(std::ifstream &in)
{
    in >> rows >> columns;
    countOfVariables = columns;
    columns++;
    matrix.resize(rows, std::vector<Fraction>(columns));
    symbols.resize(rows);
    Z.resize(columns);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns - 1; ++j)
        {
            in >> matrix[i][j];
        }
        in >> symbols[i];
        in >> matrix[i][columns - 1];
    }
    for (int i = 0; i < columns - 1; ++i)
    {
        in >> Z[i];
    }
    Z[columns - 1] = Fraction(0);
    std::string temp;
    in >> temp;
    if (temp == "max")
        isMax = true;
    else
        isMax = false;
}

void Matrix::printMatrix() const
{
    int spacing = 9;

    auto printBorder = [](int m_columns, int spacing) -> void
    {
        for (int i = 0; i < m_columns; ++i)
        {
            std::cout << "+" << std::string((spacing - 1), '-');
        }
        std::cout << "+" << std::endl;
    };
    printBorder(columns, spacing);
    for (int i = 0; i < rows; ++i)
    {
        std::cout << "|";
        for (int j = 0; j < columns - 1; ++j)
        {
            std::cout << std::setw(spacing - 1) << std::right << matrix[i][j] << "|";
        }
        std::cout << "\033[33m" << std::setw(spacing - 1) << std::right << matrix[i][columns - 1] << "\033[0m|";
        std::cout << std::endl;
        printBorder(columns, spacing);
    }
    std::cout << std::endl;
}

void Matrix::printStart(const std::string &max) const
{
    for (int i = 0; i < rows; ++i)
    {
        bool isFirst = true;
        for (int j = 0; j < columns - 1; ++j)
        {
            if (matrix[i][j].numerator != 0)
            {
                if (matrix[i][j].numerator > -1 && !isFirst)
                {
                    std::cout << "+";
                }
                std::cout << matrix[i][j] << "(x" << j + 1 << ")";
                isFirst = false;
            }
        }
        std::cout << " " << symbols[i] << " ";
        std::cout << matrix[i][columns - 1] << std::endl;
    }
    std::cout << "Z = ";
    bool isFirst = true;
    for (int i = 0; i < columns; ++i)
    {
        if (Z[i].numerator != 0)
        {
            if (Z[i].numerator > -1 && !isFirst)
            {
                std::cout << "+";
            }
            std::cout << Z[i] << "(x" << i + 1 << ")";
            isFirst = false;
        }
    }
    std::cout << " -> " << max << std::endl;
}

void Matrix::printSimplex(const int &row, const int &column)
{
    int spacing = 9;
    int columnsCt = columns + 1;
    if (column != -1)
        columnsCt++;
    auto printBorder = [](int m_columns, int spacing) -> void
    {
        for (int i = 0; i < m_columns; ++i)
        {
            std::cout << "+" << std::string((spacing - 1), '-');
        }
        std::cout << "+" << std::endl;
    };
    printBorder(columnsCt, spacing);
    int half = (spacing - 2) / 2;
    std::cout << "|";
    std::cout << std::string(half, ' ') << "\033[33mBV\033[0m" << std::string(spacing - 3 - half, ' ') << "|";
    std::cout << std::string(half + 1, ' ') << "\033[33m1\033[0m" << std::string(spacing - 3 - half, ' ') << "|";
    for (int i = 0; i < columns - 1; ++i)
    {
        std::cout << std::string(half, ' ') << "\033[33mx" << i + 1 << "\033[0m" << std::string(spacing - 3 - half, ' ') << "|";
    }
    if (column != -1)
        std::cout << std::string(half, ' ') << "\033[33mCO\033[0m" << std::string(spacing - 3 - half, ' ') << "|";
    std::cout << std::endl;
    printBorder(columnsCt, spacing);
    for (int i = 0; i < rows; ++i)
    {
        std::cout << "|";
        std::cout << std::string(half, ' ') << "\033[33mx" << basises[i] + 1 << "\033[0m" << std::string(spacing - 3 - half, ' ') << "|";
        std::cout << "\033[32m" << std::setw(spacing - 1) << std::right << matrix[i][columns - 1] << "\033[0m|";
        for (int j = 0; j < columns - 1; ++j)
        {
            if (i == row && j == column)
                std::cout << "\033[31m" << std::setw(spacing - 1) << std::right << matrix[i][j] << "\033[0m|";
            else
                std::cout << std::setw(spacing - 1) << std::right << matrix[i][j] << "|";
        }
        if (column != -1)
        {
            if (CO[i].numerator > 0)
            {
                if (i == row)
                    std::cout << "\033[31m" << std::setw(spacing - 1) << std::right << CO[i] << "\033[0m|";
                else
                    std::cout << std::setw(spacing - 1) << std::right << CO[i] << "|";
            }
            else
                std::cout << std::setw(spacing - 1) << std::right << "- " << "|";
        }
        std::cout << std::endl;
        printBorder(columnsCt, spacing);
    }
    std::cout << "|" << std::string(half + 1, ' ') << "\033[33mZ\033[0m" << std::string(spacing - 3 - half, ' ') << "|";
    std::cout << "\033[32m" << std::setw(spacing - 1) << std::right << Z[columns - 1] << "\033[0m|";
    for (int i = 0; i < columns - 1; ++i)
    {
        if (i == column)
            std::cout << "\033[31m" << std::setw(spacing - 1) << std::right << Z[i] << "\033[0m|";
        else
            std::cout << std::setw(spacing - 1) << std::right << Z[i] << "|";
    }
    if (column != -1)
        std::cout << std::string(spacing - 1, ' ') << "|";
    std::cout << std::endl;
    printBorder(columnsCt, spacing);
    std::cout << std::endl;
}

bool Matrix::checkNegativElements()
{
    bool flag = false;
    for (auto &it : matrix)
    {
        if (it.back().numerator < 0)
        {
            flag = true;
            for (auto &i : it)
            {
                i = -i;
            }
        }
    }
    return flag;
}

bool Matrix::checkNegativ(int i, int j)
{
    while (i < rows)
    {
        if (matrix[i][j].numerator > 0)
            return false;
        ++i;
    }
    return true;
}

void Matrix::setMaxElement(int starti, int j)
{
    int swapPos = -1;
    Fraction mmin;
    if (matrix[starti][j].numerator > 0)
        mmin = matrix[starti].back() / matrix[starti][j];
    else
    {
        for (int i = starti + 1; i < rows; ++i)
        {
            if (matrix[i][j].numerator > 0)
            {
                mmin = matrix[i].back() / matrix[i][j];
                swapPos = i;
                break;
            }
        }
    }
    for (int i = starti + 1; i < rows; ++i)
    {
        if (matrix[i][j].numerator > 0)
        {
            if (mmin > matrix[i].back() / matrix[i][j])
            {
                mmin = matrix[i].back() / matrix[i][j];
                swapPos = i;
            }
        }
    }
    if (swapPos != -1)
    {
        for (int i = 0; i < columns; ++i)
        {
            std::swap(matrix[starti][i], matrix[swapPos][i]);
        }
    }
}

bool Matrix::noSolutions()
{
    for (int i = rows - 1; i > -1; --i)
    {
        if (matrix[i][columns - 1].numerator != 0)
        {
            bool isZero = true;
            for (int j = 0; j < columns - 1; ++j)
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

bool Matrix::noSupportingSolutions()
{
    bool flag = false;
    for (auto &it : matrix)
    {
        if (it.back().numerator < 0)
        {
            flag = true;
            for (int i = 0; i < columns - 1; ++i)
            {
                if (it[i].numerator < 0)
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
                return true;
        }
    }
    return flag;
}

void Matrix::removeZeroVectors()
{
    int rowsCt = 0;
    for (auto &it : matrix)
    {
        if (std::all_of(it.begin(), it.end(), [](const Fraction &val)
                        { return val.numerator == 0; }))
        {
            rowsCt++;
        }
    }
    for (int i = 0; i < rowsCt; ++i)
    {
        matrix.pop_back();
    }
    rows -= rowsCt;
}

void Matrix::GaussStep(const int &row, const int &pos)
{
    Fraction del = matrix[row][pos];
    for (int i = 0; i < columns; ++i)
    {
        matrix[row][i] /= del;
    }
    for (int i = row + 1; i < rows; ++i)
    {
        for (int j = pos + 1; j < columns; ++j)
        {
            matrix[i][j] -= matrix[row][j] * matrix[i][pos];
        }
        for (int j = pos - 1; j > -1; --j)
        {
            matrix[i][j] -= matrix[row][j] * matrix[i][pos];
        }
        matrix[i][pos].numerator = 0;
        matrix[i][pos].denominator = 1;
    }
    for (int j = pos + 1; j < columns; ++j)
    {
        Z[j] -= matrix[row][j] * Z[pos];
    }
    for (int j = pos - 1; j > -1; --j)
    {
        Z[j] -= matrix[row][j] * Z[pos];
    }
    Z[pos].numerator = 0;
    Z[pos].denominator = 1;
    for (int i = row - 1; i > -1; --i)
    {
        for (int j = pos + 1; j < columns; ++j)
        {
            matrix[i][j] -= matrix[row][j] * matrix[i][pos];
        }
        for (int j = pos - 1; j > -1; --j)
        {
            matrix[i][j] -= matrix[row][j] * matrix[i][pos];
        }
        matrix[i][pos].numerator = 0;
        matrix[i][pos].denominator = 1;
    }
}

bool Matrix::JordanGauss()
{
    int row = 0;
    int column = 0;
    int ct = 0;
    for (; row < rows && column < columns - 1; ++row, ++column)
    {
        while (checkNegativ(row, column) && column < columns - 1)
        {
            column++;
        }
        if (column == columns - 1)
        {
            break;
        }
        setMaxElement(row, column);
        GaussStep(row, column);
        printMatrix();
        ct++;
    }
    removeZeroVectors();
    bool flag = true;
    while (ct < rows && flag)
    {
        int row = ct;
        int column = 0;
        flag = false;
        for (; row < rows && column < columns - 1; ++row, ++column)
        {
            while (checkNegativ(row, column) && column < columns - 1)
            {
                column++;
            }
            if (column == columns - 1)
            {
                break;
            }
            flag = true;
            setMaxElement(row, column);
            GaussStep(row, column);
            printMatrix();
            ct++;
        }
    }
    if (!flag) return false;
    flag = true;
    while (flag)
    {
        flag = false;
        for (int row = 0; row < rows; ++row)
        {
            if (matrix[row].back().numerator < 0)
            {
                flag = true;
                bool isOkey = false;
                for (int i = 0; i < columns - 1; ++i)
                {
                    if (matrix[row][i].numerator < 0)
                    {
                        isOkey = true;
                        break;
                    }
                }
                if (!isOkey)
                    return false;
                matrix[row][columns - 1] = -matrix[row][columns - 1];
                int pos = -1;
                Fraction mmin = Fraction(-1);
                for (int j = 0; j < columns - 1; ++j)
                {
                    matrix[row][j] = -matrix[row][j];
                    if (matrix[row][j].numerator > 0)
                    {
                        if (mmin.numerator == -1)
                        {
                            mmin = matrix[row].back() / matrix[row][j];
                            pos = j;
                        }
                        else
                        {
                            if (mmin > matrix[row].back() / matrix[row][j])
                            {
                                mmin = matrix[row].back() / matrix[row][j];
                                pos = j;
                            }
                        }
                    }
                }
                if (pos == -1)
                {
                    flag = false;
                    break;
                }
                else
                {
                    GaussStep(row, pos);
                }
                printMatrix();
                break;
            }
        }
    }
    return true;
}

bool Matrix::makeCanon()
{
    bool flag = false;
    for (int i = 0; i < rows; ++i)
    {
        if (symbols[i] == "<=")
        {
            for (int j = 0; j < rows; ++j)
            {
                matrix[j].push_back(matrix[j].back());
                matrix[j][columns - 1] = Fraction(0);
            }
            Z.push_back(Fraction(0));
            columns++;
            matrix[i][columns - 2] = Fraction(1);
            symbols[i] = "=";
            flag = true;
        }
        else if (symbols[i] == ">=")
        {
            for (int j = 0; j < rows; ++j)
            {
                matrix[j].push_back(matrix[j].back());
                matrix[j][columns - 1] = Fraction(0);
            }
            Z.push_back(Fraction(0));
            columns++;
            matrix[i][columns - 2] = Fraction(-1);
            symbols[i] = "=";
            flag = true;
        }
    }
    if (!isMax)
    {
        for (int i = 0; i < columns; ++i)
        {
            Z[i] = -Z[i];
        }
        flag = true;
    }
    return flag;
}

void Matrix::calcBasisesIndex()
{
    basises.resize(rows);
    for (int j = 0; j < columns - 1; ++j)
    {
        bool flag = true;
        int count1 = 0;
        int pos = 0;
        for (int i = 0; i < rows; i++)
        {
            if (matrix[i][j] == Fraction(1))
            {
                count1++;
                pos = i;
            }
            else if (matrix[i][j].numerator != 0)
            {
                free.push_back(j);
                flag = false;
                break;
            }
        }
        if (flag && count1 == 1)
        {
            basises[pos] = j;
        }
    }
}

std::pair<int, int> Matrix::negativZPos()
{
    std::pair<int, int> pos = {-1, -1};
    Fraction mmax = Fraction(0);
    for (int i = 0; i < columns - 1; ++i)
    {
        if (Z[i].numerator < mmax.numerator)
        {
            bool flag = false;
            for (int j = 0; j < rows; j++)
            {
                if (matrix[j][i].numerator > 0)
                {
                    flag = true;
                    break;
                }
            }
            if (flag)
            {
                mmax = Z[i];
                pos.second = i;
            }
        }
    }
    mmax = Fraction(-1);
    for (int i = 0; i < rows; ++i)
    {
        if (matrix[i][pos.second].numerator > 0)
        {
            CO[i] = matrix[i].back() / matrix[i][pos.second];
            if (mmax.numerator == -1 || mmax > CO[i])
            {
                mmax = CO[i];
                pos.first = i;
            }
        }
        else
        {
            CO[i] = Fraction(-1);
        }
    }
    return pos;
}

bool Matrix::checkNegativZ()
{
    for (int i = 0; i < columns - 1; ++i)
    {
        if (Z[i].numerator < 0)
            return true;
    }
    return false;
}

bool Matrix::SimplexStep()
{
    std::pair<int, int> pos = negativZPos();
    if (pos.second == -1)
        return false;
    printSimplex(pos.first, pos.second);
    GaussStep(pos.first, pos.second);
    free.erase(std::find(free.begin(), free.end(), pos.second));
    free.push_back(basises[pos.first]);
    basises[pos.first] = pos.second;
    printSimplex();
    return true;
}

void Matrix::SimpexSolution()
{
    std::vector<Fraction> solution(countOfVariables, 0);
    for (int i = 0; i < rows; ++i)
    {
        solution[basises[i]] = matrix[i].back();
    }
    int hasMoreSolutonsPos = -1;
    for (auto &it : free)
    {
        if (Z[it].numerator == 0)
        {
            for (int i = 0; i < rows; ++i)
            {
                if (matrix[i][it].numerator > 0)
                {
                    hasMoreSolutonsPos = it;
                    break;
                }
            }
        }
    }
    if (hasMoreSolutonsPos > -1)
    {
        std::cout << "The solution is optimal, but not the only one, another solution:" << std::endl;
        Fraction mmax = Fraction(-1);
        int pos = -1;
        for (int i = 0; i < rows; ++i)
        {
            if (matrix[i][hasMoreSolutonsPos].numerator > 0)
            {
                CO[i] = matrix[i].back() / matrix[i][hasMoreSolutonsPos];
                if (mmax.numerator == -1 || mmax > CO[i])
                {
                    mmax = CO[i];
                    pos = i;
                }
            }
            else
            {
                CO[i] = Fraction(-1);
            }
        }
        printSimplex(pos, hasMoreSolutonsPos);
        GaussStep(pos, hasMoreSolutonsPos);
        free.erase(std::find(free.begin(), free.end(), hasMoreSolutonsPos));
        free.push_back(basises[pos]);
        basises[pos] = hasMoreSolutonsPos;
        printSimplex();
        std::vector<Fraction> solution2(countOfVariables, 0);
        for (int i = 0; i < rows; ++i)
        {
            solution2[basises[i]] = matrix[i].back();
        }
        std::cout << "Z(" << solution[0];
        for (int i = 1; i < solution.size(); ++i)
        {
            std::cout << "; " << solution[i];
        }
        std::cout << ") and Z(" << solution2[0];
        for (int i = 1; i < solution2.size(); ++i)
        {
            std::cout << "; " << solution2[i];
        }
        std::cout << ")" << std::endl;
        std::cout << "\033[34mANSWERD:\033[0m" << std::endl;
        if (isMax)
        {
            std::cout << "Z max = Z(";
        }
        else
        {
            Z.back() = -Z.back();
            std::cout << "Z min = -Z max = Z(";
        }
        std::cout << solution[0] << (solution[0].numerator >= 0 ? "-" : "+") << Fraction::abs(solution[0]) << "a" << (solution2[0].numerator >= 0 ? "+" : "-") << Fraction::abs(solution2[0]) << "a";
        for (int i = 1; i < solution.size(); ++i)
        {
            std::cout << "; " << solution[i] << (solution[i].numerator >= 0 ? "-" : "+") << Fraction::abs(solution[i]) << "a" << (solution2[i].numerator >= 0 ? "+" : "-") << Fraction::abs(solution2[i]) << "a";
        }
        Fraction temp = solution2[0] - solution[0];
        std::cout << ") = Z(" << solution[0] << (temp.numerator >= 0 ? "+" : "") << temp << "a";
        for (int i = 1; i < solution.size(); ++i)
        {
            temp = solution2[i] - solution[i];
            std::cout << "; " << solution[i] << (temp.numerator >= 0 ? "+" : "") << temp << "a";
        }
        std::cout << ") = " << Z.back() << "\t 0 <= a <= 1" << std::endl;
    }
    else
    {
        std::cout << "\033[34mANSWERD:\033[0m" << std::endl;
        if (isMax)
        {
            std::cout << "Z max = Z(" << solution[0];
        }
        else
        {
            Z.back() = -Z.back();
            std::cout << "Z min = -Z max = Z(" << solution[0];
        }
        for (int i = 1; i < solution.size(); ++i)
        {
            std::cout << "; " << solution[i];
        }
        std::cout << ") = " << Z.back() << std::endl;
    }
}

void Matrix::Simplex()
{
    std::cout << std::endl
              << "The initial task of linear programming:" << std::endl;
    printStart(isMax ? "max" : "min");
    bool flag = true;
    for (int i = 0; i < columns; ++i)
    {
        if (Z[i].numerator != 0)
        {
            flag = false;
            break;
        }
    }
    if (flag)
    {
        std::cout << "Z " << (isMax ? "max" : "min") << " = 0" << std ::endl;
        return;
    }
    std::cout << std::endl;
    if (makeCanon())
    {
        std::cout << "Transition to the canonical form:" << std::endl;
        printStart("max");
        std::cout << std::endl;
    }
    std::cout << "Matrix before JordanGauss:" << std::endl;
    printMatrix();
    if (checkNegativElements())
    {
        std::cout << "Changed all the values on the right side to non-negative" << std::endl;
        printMatrix();
    }
    for (int i = 0; i < columns - 1; ++i)
    {
        Z[i] = -Z[i];
    }
    bool isGauss = JordanGauss();
    std::cout << "Matrix after JordanGauss:" << std::endl;
    printMatrix();
    if (noSolutions() || noSupportingSolutions() || !isGauss)
    {
        std::cout << "\033[31mThe system has no supporting solutions\033[0m" << std::endl;
        noSolution = true;
        return;
    }
    calcBasisesIndex();
    CO.resize(rows, Fraction(-1));
    std::cout << "Start Simplex:" << std::endl;
    printSimplex();
    while (SimplexStep())
        ;
    if (checkNegativZ())
    {
        std::cout << "\033[31mThe space of acceptable solutions is unlimited. There is no solution.\033[0m" << std::endl;
        noSolution = true;
        return;
    }
    SimpexSolution();
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
    matrix.Simplex();
    return 0;
}
