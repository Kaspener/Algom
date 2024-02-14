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
    int del = std::gcd(other.numerator, denominator);
    int del2 = std::gcd(numerator, other.denominator);
    return Fraction{numerator/del2 * other.numerator/del, denominator/del * other.denominator/del2};
}

Fraction &Fraction::operator*=(const Fraction &other)
{
    int del = std::gcd(other.numerator, denominator);
    int del2 = std::gcd(numerator, other.denominator);
    *this = Fraction{numerator/del2 * other.numerator/del, denominator/del * other.denominator/del2};
    return *this;
}

Fraction &Fraction::operator*=(const int &other)
{   
    int del = std::gcd(other, denominator);
    Fraction tmp = Fraction{numerator * other / del, denominator / del};
    *this = tmp;
    return *this;
}

Fraction Fraction::operator/(const Fraction &other) const
{
    if (numerator == 0) return Fraction{0};
    int del = std::gcd(other.denominator, denominator);
    int del2 = std::gcd(other.numerator, numerator);
    return Fraction{numerator/del2 * other.denominator/del, denominator/del * other.numerator/del2};
}

Fraction &Fraction::operator/=(const Fraction &other)
{
    if (numerator == 0) *this = Fraction{0};
    int del = std::gcd(other.denominator, denominator);
    int del2 = std::gcd(other.numerator, numerator);
    *this = Fraction{numerator/del2 * other.denominator/del, denominator/del * other.numerator/del2};
    return *this;
}

Fraction &Fraction::operator/=(const int &other)
{   
    int del = std::gcd(other, numerator);
    Fraction tmp = Fraction{numerator / del, denominator * other / del};
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
