from sortedcontainers import SortedList

def _get_lower(a):
    if isinstance(a, int):
        return a
    else:
        return a.lower

class IntervalList:
    def __init__(self, intervals):
        self.intervals = SortedList([], key=_get_lower)
        for interval in intervals:
            self.unite(interval)

    def unite(self, interval):
        """
        Inserts the given interval into the list of interval.
        Takes care to merge any adjacent intervals.
        """
        first, last = self._intersect_continuous(interval)
        if first != last:
            interval.lower = min(interval.lower, self.intervals[first].lower)
            interval.upper = max(interval.upper, self.intervals[last - 1].upper)
        del self.intervals[first:last]
        self.intervals.add(interval)

    def _intersect_continuous(self, interval):
        """
        Finds all intervals whose intersection with interval is not empty;
        also includes the two intervals the the most left and right if their
        bounds are directly adjacent to the interval. For example, if intervals
        contains:
          [..., [1, 3), [6, 8), ...]
        and this function is called with [3, 6), it includes the two intervals.
        """
        first = self.intervals.bisect_left(interval)
        last = first
        while first > 0 and \
              self.intervals[first - 1].upper >= interval.lower:
            first -= 1
        while last < len(self.intervals) and \
              self.intervals[last].lower <= interval.upper:
            last += 1
        return first, last

    def _intersect(self, interval):
        """
        Finds all intervals whose intersection with interval is not empty.
        """
        first = self.intervals.bisect_left(interval)
        last = first
        while first > 0 and \
              self.intervals[first - 1].upper > interval.lower:
            first -= 1
        while last < len(self.intervals) and \
              self.intervals[last].lower < interval.upper:
            last += 1
        return first, last

    def subtract(self, interval):
        """
        Removes the given interval from all intervals.
        Takes care to remove empty intervals.
        """
        first, last = self._intersect(interval)
        if last - first == 1:
            new_intervals = self.intervals[first].subtract(interval)
            del self.intervals[first]
            self.intervals.update(new_intervals)
        elif last - first != 0:
            if self.intervals[first].lower != interval.lower:
                self.intervals[first].upper = interval.lower
                if self.intervals[first].length > 0:
                    first = min(first + 1, len(self.intervals))
            if self.intervals[last - 1].upper != interval.upper:
                self.intervals[last - 1].lower = interval.upper
                if self.intervals[last - 1].length > 0:
                    last = max(last - 1, 0)
            del self.intervals[first:last]

    def find(self, value):
        """
        Returns the interval that contains the given value.
        """
        index = self.intervals.bisect_left(value)
        if index < len(self.intervals) and self.intervals[index].lower == value:
            return self.intervals[index]
        if index > 0 and self.intervals[index - 1].contains(value):
            return self.intervals[index - 1]
        return None

    def contains(self, interval):
        """
        Returns true if the list of intervals has a non-empty intersection with
        the given interval.
        """
        first, last = self._intersect(interval)
        return first != last

    def __iter__(self):
        return self.intervals.__iter__()

    def __len__(self):
        return self.intervals.__len__()