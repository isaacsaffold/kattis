#! /usr/bin/env python3

from sys import stdin
from bisect import bisect
from math import copysign

def perfect_power(n):
    """Returns the largest integer 'k' for which n == a**k."""
    primes = (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31)
    start = int(n < 0)
    for i in range(start, bisect(primes, n.bit_length())):
        a = copysign(round(abs(n)**(1 / primes[i])), n)
        if a**primes[i] == n:
            return primes[i] * perfect_power(int(a))
    return 1

def main():
    """Main function."""
    for n in iter(stdin.readline, "0\n"):
        print(perfect_power(int(n)))

if __name__ == "__main__":
    main()
