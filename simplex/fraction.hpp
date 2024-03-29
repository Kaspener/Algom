#include <iostream>

class Fraction {
    public:
        long long numerator, denominator;

        Fraction() : numerator(0), denominator(1) {}
        Fraction(long long n) : numerator(n), denominator(1) {}
        Fraction(long long n, long long d);

        void calculateGcd();
        static Fraction abs(const Fraction& x);

        operator int() {return numerator/denominator;}
        operator float() {return ((float)numerator)/denominator;}
        operator double() {return ((double)numerator)/denominator;}
        Fraction operator+(const Fraction& other) const;
        Fraction& operator+=(const Fraction& other);
        Fraction operator-(const Fraction& other) const;
        Fraction operator-() const;
        Fraction& operator-=(const Fraction& other);
        Fraction operator*(const Fraction& other) const;
        Fraction& operator*=(const Fraction& other);
        Fraction& operator*=(const int& other);
        Fraction operator/(const Fraction& other) const;
        Fraction& operator/=(const Fraction& other);
        Fraction& operator/=(const int& other);
        bool operator==(const Fraction& other) const;
        bool operator<(const Fraction& other) const;
        bool operator>(const Fraction& other) const;
        

        friend std::ostream& operator<<(std::ostream& os, const Fraction& dt);
        friend std::istream& operator>>(std::istream& is, Fraction& dt);
};