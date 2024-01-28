#ifndef INTERVAL_H
#define INTERVAL_H

class Interval {
public:
  double min, max;
  Interval() : min(+infinity), max(-infinity) {}
  Interval(double _min, double _max) : min(_min), max(_max) {}

  bool contains(double x) const { return min <= x && x <= max; }
  bool surrounds(double x) const { return min < x && x < max; }
  static const Interval empty, universe;
};

const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);

#endif // !INTERVAL_H
