//
//    FILE: Angle.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.00
// PURPOSE: library for Angle math for Arduino
//     URL: 
//
// Released to the public domain
//

// 0.1.00 - initial version


#include "Angle.h"
/*
Angle::Angle(const double alpha)
{
    // reference - 10 float operations
    double a = alpha;
    d = int(a);
    a = a - d;
    a = a * 60;
    m = int(a);
    a = a - m;
    a = a * 60;
    s = int(a);
    a = a - s;
    a = a * 1000;
    t = round(a);
}*/

Angle::Angle(const double alpha)
{
    // improved 5 float operations (TODO test if this is more precise / faster)
    double a = alpha;
    bool neg = (alpha < 0);
    if (neg) a = -a;
   
    d = int(a);
    a = a - d;
    // unsigned long p = a * 3600000L;   // 3600 000 = 2^7 • 3^2 • 5^5 = 128*28125
    a = a * 128;                         // will only affect exponent - no loss precision
    unsigned long p = round(a * 28125);  // less digits so less loss of significant digits.
    t = p % 1000UL;
    p = (p - t)/1000UL;
    s = p % 60UL;
    m = (p - s)/60UL;
    
    if (neg) d = -d;
}

// PRINTING
size_t Angle::printTo(Print& p) const
{
    size_t n = 0;
    n += p.print(d);
    n += p.print('.');
    if (m < 10) n += p.print('0'); 
    n += p.print(m); 
    n += p.print('\'');
    if (s < 10) n += p.print('0'); 
    n += p.print(s); 
    n += p.print('\"');
    if (t < 100) n += p.print('0'); 
    if (t < 10) n += p.print('0'); 
    n += p.print(t);
    return n;
};

double Angle::toDouble(void)
{
    unsigned long v = t;
    v += s * 1000UL;
    v += m * 60000UL;
    double val = ((1.0 / 28125.0) / 128) * v + d;
    return val;
}


// BASIC MATH

/* Angle Angle::operator = (const Angle &a)
{
    d = a.d;
    m = a.m;
    s = a.s;
    t = a.t;
    return *this;
}
*/

Angle Angle::operator + (const Angle &a)
{
    Angle temp = *this;
    if (sign(temp.d) == sign(a.d))
    {
        temp.d += a.d;
        temp.m += a.m;
        temp.s += a.s;
        temp.t += a.t;
    }
    else 
    {
        temp.d = sign(temp.d)* (abs(temp.d) - abs(a.d));
        temp.m -= a.m;
        temp.s -= a.s;
        temp.t -= a.t;
    }
    temp.normalize();
    return temp;
}

Angle& Angle::operator += (const Angle &a)
{
    Angle temp = *this;
    if (sign(temp.d) == sign(a.d))
    {
        temp.d += a.d;
        temp.m += a.m;
        temp.s += a.s;
        temp.t += a.t;
    }
    else 
    {
        temp.d = sign(temp.d)* (abs(temp.d) - abs(a.d));
        temp.m -= a.m;
        temp.s -= a.s;
        temp.t -= a.t;
    }
    temp.normalize();
    *this = temp;
    return *this;
}

Angle Angle::operator - (const Angle &a)
{
    Angle temp = *this;
    if (sign(temp.d) == sign(a.d))
    {
        temp.d -= a.d;
        temp.m -= a.m;
        temp.s -= a.s;
        temp.t -= a.t;
    }
    else 
    {
        temp.d = sign(temp.d) * (abs(temp.d) + abs(a.d));
        temp.m += a.m;
        temp.s += a.s;
        temp.t += a.t;
    }
    temp.normalize();
    return temp;
}

Angle& Angle::operator -= (const Angle &a)
{
    Angle temp = *this;
    if (sign(temp.d) == sign(a.d))
    {
        temp.d -= a.d;
        temp.m -= a.m;
        temp.s -= a.s;
        temp.t -= a.t;
    }
    else 
    {
        temp.d = sign(temp.d) * (abs(temp.d) + abs(a.d));
        temp.m += a.m;
        temp.s += a.s;
        temp.t += a.t;
    }
    temp.normalize();
    *this = temp;
    return temp;
}

Angle Angle::operator * (const double dd)
{
    return Angle(this->toDouble() * dd);	
}

Angle Angle::operator / (const double dd)
{
    return Angle(this->toDouble() / dd);
}

Angle& Angle::operator *= (const double dd)
{
    double val = this->toDouble() * dd;
    Angle temp(val);
    this->d = temp.d;
    this->m = temp.m;
    this->s = temp.s;
    this->t = temp.t;
    return *this;
}

Angle& Angle::operator /= (const double dd)
{
    double val = this->toDouble() / dd;
    Angle temp(val);
    this->d = temp.d;
    this->m = temp.m;
    this->s = temp.s;
    this->t = temp.t;
    return *this;
}

double Angle::operator / (Angle& a)
{
    double f = this->toDouble();
    double g = a.toDouble();
    return f/g;
}
///////////////////////////////////////////////////////////
//
// PRIVATE
//
void Angle::add(const Angle& a)
{
    d += a.d;
    m += a.m;
    s += a.s;
    t += a.t;
    normalize();
}

void Angle::sub(const Angle& a)
{
    d -= a.d;
    m -= a.m;
    s -= a.s;
    t -= a.t;
    normalize();
}

int Angle::compare(const Angle &a, const Angle &b)
{
    if (a.d > b.d) return 1;
    if (a.d < b.d) return -1;
    if (a.m > b.m) return 1;
    if (a.m < b.m) return -1;
    if (a.s > b.s) return 1;
    if (a.s < b.s) return -1;
    if (a.t > b.t) return 1;
    return 0;
}

void Angle::normalize()
{
    bool neg = (d < 0);
    if (neg) d = -d;
    while (t < 0)     { s--; t += 1000; }
    while (t >= 1000) { s++; t -= 1000; }
    while (s < 0)     { m--; s += 60; }
    while (s >= 60)   { m++; s -= 60; }
    while (m < 0)     { d--; m += 60; }
    while (m >= 60)   { d++; m -= 60; }
    if (neg) d = -d;
}

int Angle::sign(int d) 
{
    return (d < 0?-1:1);
}

// --- END OF FILE ---