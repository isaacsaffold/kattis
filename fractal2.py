#! /usr/bin/env python

import sys
from bisect import bisect
from itertools import accumulate

def normalize(polyline, num_points):
    start = polyline[0]
    for i in range(num_points):
        polyline[i] -= start
    scale = polyline[-1]
    for i in range(num_points):
        polyline[i] /= scale
    return (start, scale)

def point_at(x, polyline, num_points, depth):
    transforms = [polyline[i] - polyline[i-1] for i in range(1, num_points)]
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
    polyline = []
    num_cases = int(input())
    for i in range(num_cases):
        num_points = int(input())
        n = min(num_points, len(polyline))
        j = 0
        while j < n:
            x, y = sys.stdin.readline().split()
            polyline[j] = complex(int(x), int(y))
            j += 1
        while j < num_points:
            x, y = sys.stdin.readline().split()
            polyline.append(complex(int(x), int(y)))
            j += 1
        depth = int(input())
        x = float(input())
        start, scale = normalize(polyline, num_points)
        z = point_at(x, polyline, num_points, depth)
        z = start + scale*z
        print(f"{z.real} {z.imag}")

if __name__ == "__main__":
    main()
