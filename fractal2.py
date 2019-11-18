from sys import stdin
from math import *
from collections import namedtuple
from itertools import accumulate

def isclose(a, b, tol=1e-12):
    """Overrides Python's implementation of 'isclose' to exclusively use
    absolute tolerance."""
    return abs(a - b) < tol

# Can also be represented as the sum of vectors.
Point = namedtuple("Point", ['x', 'y'])

class Vector:
    """Two-dimensional vector.

    Can be defined in Cartesian or polar form.
    """

    @staticmethod
    def _to_polar(x, y):
        """Given a vector's Cartesian coordinates, returns a tuple
        containing its polar coordinates."""
        return (hypot(x, y), atan2(y, x))

    @staticmethod
    def _to_cartesian(r, theta):
        """Given a vector's polar coordinates, returns a tuple
        containing its Cartesian coordinates."""
        return (r*cos(theta), r*sin(theta))

    @staticmethod
    def connecting_vector(origin, destination):
        """Returns the vector that connects an origin point to a
        destination point."""
        return Vector(*destination) - Vector(*origin)

    def __init__(self, a, b, polar=False):
        """Initialize self. See help(type(self)) for accurate signature."""
        if polar:
            self.r = a
            self.theta = b
            self.x, self.y = self._to_cartesian(a, b)
        else:
            self.x = a
            self.y = b
            self.r, self.theta = self._to_polar(a, b)

    def __abs__(self):
        """Return abs(self)."""
        return self.r

    def __add__(self, value):
        """Return self + value."""
        return Vector(self.x + value.x, self.y + value.y)

    def __sub__(self, value):
        """Return self - value."""
        return Vector(self.x - value.x, self.y - value.y)

    def __neg__(self):
        """Return -self."""
        return Vector(-self.x, -self.y)

    def translate(self, point):
        """Returns the point that this vector connects an initial point
        to."""
        return Point(point[0] + self.x, point[1] + self.y)

    def as_cartesian(self):
        """Returns a tuple containing the Cartesian coordinates of this
        vector."""
        return (self.x, self.y)

    def as_polar(self):
        """Returns a tuple containing the polar coordinates of this
        vector."""
        return (self.r, self.theta)

def polyline(points):
    """Returns the defining function for a polyline fractal."""
    start_vector = Vector.connecting_vector(points[0], points[-1])
    def f(V):
        x = V.r / start_vector.r
        phi = V.theta - start_vector.theta
        result = []
        for i in range(len(points) - 1):
            A = Vector.connecting_vector(points[i], points[i+1])
            result.append(Vector(x*A.r, A.theta + phi, True))
        return result
    return f

def interval_search(x, seq, lo, hi):
    """Binary search algorithm implementation for continuous intervals."""
    if hi - lo == 1:
        return (lo, hi)
    else:
        mid = (lo + hi) // 2
        if seq[lo] <= x <= seq[mid]:
            return interval_search(x, seq, lo, mid)
        else:
            return interval_search(x, seq, mid, hi)

def containing_interval(nums, x):
    """Uses a binary search algorithm to determine, given a point set,
    the smallest closed interval in the set to which a number belongs.

    Let 'i' be the index of an element in an extended point set that
    consists of the original point set with '-inf' as the first element
    and 'inf' as the last. Let the point set itself be denoted as 'a'.
    If a[i] < x < a[i+1], (i-1, i) is returned. If x == a[i] or
    x == a[i+1], (i-1,) or (i,) is returned, respectively.
    """
    pset = [-inf] + list(nums) + [inf]
    interval = interval_search(x, pset, 0, len(pset) - 1)
    if isclose(x, pset[interval[0]]):
        return (interval[0] - 1,)
    elif isclose(x, pset[interval[1]]):
        return (interval[1] - 1,)
    else:
        return (interval[0] - 1, interval[1] - 1)

def magnitude_ratios(f):
    """Returns a list containing the ratios of the cumulative magnitudes
    of the vectors composing a polyline fractal to the sum of all of
    their magnitudes."""
    test_vector = Vector(1, 0)
    ratios = [0] + list(accumulate(V.r for V in f(test_vector)))
    for i in range(len(ratios)):
        ratios[i] /= ratios[-1]
    return ratios

def endpoint(points, depth, fraction):
    """Returns the point reached after traversal of a given fraction of
    a polyline fractal."""
    f = polyline(points)
    start, end = points[0], points[-1]
    ratios = magnitude_ratios(f)
    for i in range(depth):
        vectors = f(Vector.connecting_vector(start, end))
        indices = containing_interval(ratios, fraction)
        offset = sum(vectors[:indices[0]], Vector(0, 0))
        if len(indices) == 1:
            return offset.translate(start)
        else:
            a, b = indices
            start = offset.translate(start)
            end = vectors[a].translate(start)
            fraction -= ratios[a]
            fraction /= ratios[b] - ratios[a]
    V = Vector.connecting_vector(start, end)
    return Vector(V.r * fraction, V.theta, True).translate(start)

def main():
    """Main function."""
    test_cases = int(stdin.readline())
    for i in range(test_cases):
        point_count = int(stdin.readline())
        points = []
        for j in range(point_count):
            x, y = map(int, stdin.readline().split())
            points.append(Point(x, y))
        depth = int(stdin.readline())
        fraction = float(stdin.readline())
        dest = endpoint(points, depth, fraction)
        print(dest.x, dest.y)

if __name__ == "__main__":
    main()
