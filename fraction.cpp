#include "fraction.hpp"
#include <numeric>
#include <string>

Fraction::Fraction(long long n, long long d)
{
    if (d == 0)
    {
        std::cerr << "Denominator may not be 0." << std::endl;
        exit(0);
    }
    if (n == 0)
    {
        numerator = 0;
        denominator = 1;
    }
    else
    {
        numerator = n;
        denominator = d;
        calculateGcd();
    }
}

void Fraction::calculateGcd()
{
    int sign = 1;
    if (numerator < 0)
    {
        sign *= -1;
        numerator *= -1;
    }
    if (denominator < 0)
    {
        sign *= -1;
        denominator *= -1;
    }

    long long tmp = std::gcd(numerator, denominator);
    numerator = numerator / tmp * sign;
    denominator = denominator / tmp;
}

Fraction Fraction::operator+(const Fraction &other) const
{
    return Fraction{numerator * other.denominator + other.numerator * denominator, denominator * other.denominator};
}

Fraction &Fraction::operator+=(const Fraction &other)
{
    Fraction tmp = Fraction{numerator * other.denominator + other.numerator * denominator, denominator * other.denominator};
    *this = tmp;
    return *this;
}

Fraction Fraction::operator-(const Fraction &other) const
{
    return Fraction{numerator * other.denominator + other.numerator * denominator, denominator * other.denominator};
}

Fraction Fraction::operator-() const{
    return Fraction{-numerator, denominator};
}

Fraction &Fraction::operator-=(const Fraction &other)
{
    Fraction tmp = Fraction{numerator * other.denominator - other.numerator * denominator, denominator * other.denominator};
    *this = tmp;
    return *this;
}

Fraction Fraction::operator*(const Fraction &other) const
{
    return Fraction{numerator * other.numerator, denominator * other.denominator};
}

Fraction &Fraction::operator*=(const Fraction &other)
{
    Fraction tmp = Fraction{numerator * other.numerator, denominator * other.denominator};
    *this = tmp;
    return *this;
}

Fraction &Fraction::operator*=(const int &other)
{
    Fraction tmp = Fraction{numerator * other, denominator};
    *this = tmp;
    return *this;
}

Fraction Fraction::operator/(const Fraction &other) const
{
    return Fraction{numerator * other.denominator, denominator * other.numerator};
}

Fraction &Fraction::operator/=(const Fraction &other)
{
    Fraction tmp = Fraction{numerator * other.denominator, denominator * other.numerator};
    *this = tmp;
    return *this;
}

bool Fraction::operator==(const Fraction &other) const
{
    return numerator == other.numerator && denominator == other.denominator;
}

bool Fraction::operator<(const Fraction &other) const
{
    return numerator*other.denominator<denominator*other.numerator;
}

bool Fraction::operator>(const Fraction &other) const
{
    return numerator*other.denominator>denominator*other.numerator;
}

std::ostream &operator<<(std::ostream &os, const Fraction &dt)
{
    if (dt.denominator != 1)
        os << dt.numerator << '/' << dt.denominator;
    else
        os << dt.numerator;
    return os;
}

std::istream &operator>>(std::istream &is, Fraction &dt)
{
    std::string str;
    Fraction tmp;
    is >> str;
    int pos = str.find('/');
    if (pos == std::string::npos)
    {
        dt.numerator = std::stoll(str);
    }
    else
    {
        dt.numerator = std::stoll(str.substr(0, pos));
        dt.denominator = std::stoll(str.substr(pos + 1));
        dt.calculateGcd();
    }
    return is;
}

Fraction Fraction::abs(const Fraction &x)
{
    Fraction tmp = x;
    if (tmp.numerator < 0)
        tmp.numerator = -tmp.numerator;
    return tmp;
}
