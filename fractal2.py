#! /usr/bin/env python

import sys
from bisect import bisect
from itertools import accumulate

def normalize(polyline):
    start = polyline[0]
    for i in range(len(polyline)):
        polyline[i] -= start
    scale = polyline[-1]
    for i in range(len(polyline)):
        polyline[i] /= scale
    return (start, scale)

def point_at(x, polyline, depth):
    transforms = [polyline[i] - polyline[i-1] for i in range(1, len(polyline))]
    distance_along = list(accumulate(map(abs, transforms)))
    total_distance = distance_along[-1]
    for i in range(len(distance_along)):
        distance_along[i] /= total_distance
    z, t = 0, 1
    for i in range(depth):
        j = bisect(distance_along, x)
        z += polyline[j] * t
        if j == len(distance_along):
            return z
        if j > 0:
            x -= distance_along[j-1]
        x /= abs(transforms[j]) / total_distance
        t *= transforms[j]
    return z + x*t

def main():
    num_cases = int(input())
    for i in range(num_cases):
        num_points = int(input())
        polyline = [complex(*map(int, sys.stdin.readline().split())) for j in range(num_points)]
        depth = int(input())
        x = float(input())
        start, scale = normalize(polyline)
        z = point_at(x, polyline, depth)
        z = start + scale*z
        print(f"{z.real} {z.imag}")

if __name__ == "__main__":
    main()
