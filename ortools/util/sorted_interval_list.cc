// Copyright 2010-2017 Google
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ortools/util/sorted_interval_list.h"

#include <algorithm>

#include "ortools/base/logging.h"
#include "ortools/base/stringprintf.h"
#include "ortools/util/saturated_arithmetic.h"

namespace operations_research {

std::string ClosedInterval::DebugString() const {
  if (start == end) return StringPrintf("[%" GG_LL_FORMAT "d]", start);
  return StringPrintf("[%" GG_LL_FORMAT "d,%" GG_LL_FORMAT "d]", start, end);
}

std::string IntervalsAsString(const std::vector<ClosedInterval>& intervals) {
  std::string result;
  for (ClosedInterval interval : intervals) {
    result += interval.DebugString();
  }
  return result;
}

std::ostream& operator<<(std::ostream& out, const ClosedInterval& interval) {
  return out << interval.DebugString();
}

std::ostream& operator<<(std::ostream& out,
                         const std::vector<ClosedInterval>& intervals) {
  return out << IntervalsAsString(intervals);
}

std::vector<ClosedInterval> SortedDisjointIntervalsFromValues(
    std::vector<int64> values) {
  std::sort(values.begin(), values.end());
  std::vector<ClosedInterval> result;
  for (const int64 v : values) {
    if (result.empty() || v > result.back().end + 1) {
      result.push_back({v, v});
    } else {
      result.back().end = v;
    }
  }
  return result;
}

bool IntervalsAreSortedAndDisjoint(
    const std::vector<ClosedInterval>& intervals) {
  if (intervals.empty()) return true;
  int64 previous_end;
  bool is_first_interval = true;
  for (const ClosedInterval interval : intervals) {
    if (interval.start > interval.end) return false;
    if (!is_first_interval) {
      // First test make sure that previous_end + 1 will not overflow.
      if (interval.start <= previous_end) return false;
      if (interval.start <= previous_end + 1) return false;
    }
    is_first_interval = false;
    previous_end = interval.end;
  }
  return true;
}

bool SortedDisjointIntervalsContain(gtl::Span<ClosedInterval> intervals,
                                    int64 value) {
  for (const ClosedInterval& interval : intervals) {
    if (interval.start <= value && interval.end >= value) return true;
  }
  return false;
}

std::vector<ClosedInterval> IntersectionOfSortedDisjointIntervals(
    const std::vector<ClosedInterval>& a,
    const std::vector<ClosedInterval>& b) {
  DCHECK(IntervalsAreSortedAndDisjoint(a));
  DCHECK(IntervalsAreSortedAndDisjoint(b));
  std::vector<ClosedInterval> result;
  for (int i = 0, j = 0; i < a.size() && j < b.size();) {
    const ClosedInterval intersection{std::max(a[i].start, b[j].start),
                                      std::min(a[i].end, b[j].end)};
    if (intersection.start > intersection.end) {
      // Intersection is empty, we advance past the first interval of the two.
      if (a[i].start < b[j].start) {
        i++;
      } else {
        j++;
      }
    } else {
      // Intersection is non-empty, we add it to the result and advance past
      // the first interval to finish.
      result.push_back(intersection);
      if (a[i].end < b[j].end) {
        i++;
      } else {
        j++;
      }
    }
  }
  return result;
}

std::vector<ClosedInterval> ComplementOfSortedDisjointIntervals(
    const std::vector<ClosedInterval>& intervals) {
  DCHECK(IntervalsAreSortedAndDisjoint(intervals));
  std::vector<ClosedInterval> result;
  int64 next_start = kint64min;
  for (const ClosedInterval& interval : intervals) {
    if (interval.start != kint64min) {
      result.push_back({next_start, interval.start - 1});
    }
    if (interval.end == kint64max) return result;
    next_start = interval.end + 1;
  }
  result.push_back({next_start, kint64max});
  return result;
}

std::vector<ClosedInterval> NegationOfSortedDisjointIntervals(
    std::vector<ClosedInterval> intervals) {
  DCHECK(IntervalsAreSortedAndDisjoint(intervals));
  if (intervals.empty()) return intervals;
  std::reverse(intervals.begin(), intervals.end());
  if (intervals.back().end == kint64min) intervals.pop_back();  // corner-case
  for (ClosedInterval& ref : intervals) {
    std::swap(ref.start, ref.end);
    ref.start = ref.start == kint64min ? kint64max : -ref.start;
    ref.end = ref.end == kint64min ? kint64max : -ref.end;
  }
  return intervals;
}

namespace {

// Transforms a sorted list of intervals in a sorted DISJOINT list for which
// IntervalsAreSortedAndDisjoint() would return true.
void UnionOfSortedIntervals(std::vector<ClosedInterval>* intervals) {
  DCHECK(std::is_sorted(intervals->begin(), intervals->end()));
  int new_size = 0;
  for (const ClosedInterval& i : *intervals) {
    if (new_size > 0 && i.start <= CapAdd((*intervals)[new_size - 1].end, 1)) {
      (*intervals)[new_size - 1].end =
          std::max(i.end, (*intervals)[new_size - 1].end);
    } else {
      (*intervals)[new_size++] = i;
    }
  }
  intervals->resize(new_size);
  DCHECK(IntervalsAreSortedAndDisjoint(*intervals)) << *intervals;
}

}  // namespace

std::vector<ClosedInterval> UnionOfSortedDisjointIntervals(
    const std::vector<ClosedInterval>& a,
    const std::vector<ClosedInterval>& b) {
  DCHECK(IntervalsAreSortedAndDisjoint(a));
  DCHECK(IntervalsAreSortedAndDisjoint(b));
  std::vector<ClosedInterval> result(a.size() + b.size());
  std::merge(a.begin(), a.end(), b.begin(), b.end(), result.begin());
  UnionOfSortedIntervals(&result);
  return result;
}

std::vector<ClosedInterval> AdditionOfSortedDisjointIntervals(
    const std::vector<ClosedInterval>& a,
    const std::vector<ClosedInterval>& b) {
  DCHECK(IntervalsAreSortedAndDisjoint(a));
  DCHECK(IntervalsAreSortedAndDisjoint(b));
  std::vector<ClosedInterval> result;
  for (const ClosedInterval& i : a) {
    for (const ClosedInterval& j : b) {
      result.push_back({CapAdd(i.start, j.start), CapAdd(i.end, j.end)});
    }
  }
  std::sort(result.begin(), result.end());
  UnionOfSortedIntervals(&result);
  return result;
}

std::vector<ClosedInterval> MultiplicationOfSortedDisjointIntervals(
    std::vector<ClosedInterval> intervals, int64 coeff) {
  DCHECK(IntervalsAreSortedAndDisjoint(intervals));
  const int64 abs_coeff = std::abs(coeff);
  for (ClosedInterval& i : intervals) {
    i.start = CapProd(i.start, abs_coeff);
    i.end = CapProd(i.end, abs_coeff);
  }
  UnionOfSortedIntervals(&intervals);
  return coeff > 0 ? intervals : NegationOfSortedDisjointIntervals(intervals);
}

std::vector<ClosedInterval> PreciseMultiplicationOfSortedDisjointIntervals(
    std::vector<ClosedInterval> intervals, int64 coeff, bool* success) {
  DCHECK(IntervalsAreSortedAndDisjoint(intervals));
  *success = true;
  if (intervals.empty() || coeff == 0) return {};
  const int64 abs_coeff = std::abs(coeff);
  if (abs_coeff != 1) {
    if (CapSub(intervals.back().end, intervals.front().start) <= 1000) {
      std::vector<int64> individual_values;
      for (ClosedInterval& i : intervals) {
        for (int v = i.start; v <= i.end; ++v) {
          individual_values.push_back(CapProd(v, abs_coeff));
        }
      }
      intervals = SortedDisjointIntervalsFromValues(individual_values);
    } else {
      *success = false;
      return {};
    }
  }
  return coeff > 0 ? intervals : NegationOfSortedDisjointIntervals(intervals);
}

int64 CeilRatio(int64 value, int64 positive_coeff) {
  DCHECK_GT(positive_coeff, 0);
  const int64 result = value / positive_coeff;
  const int64 adjust = static_cast<int64>(result * positive_coeff < value);
  return result + adjust;
}

int64 FloorRatio(int64 value, int64 positive_coeff) {
  DCHECK_GT(positive_coeff, 0);
  const int64 result = value / positive_coeff;
  const int64 adjust = static_cast<int64>(result * positive_coeff > value);
  return result - adjust;
}

std::vector<ClosedInterval> InverseMultiplicationOfSortedDisjointIntervals(
    std::vector<ClosedInterval> intervals, int64 coeff) {
  DCHECK(IntervalsAreSortedAndDisjoint(intervals));
  if (coeff == 0) {
    return SortedDisjointIntervalsContain(intervals, 0)
               ? std::vector<ClosedInterval>({{kint64min, kint64max}})
               : std::vector<ClosedInterval>();
  }
  int new_size = 0;
  const int64 abs_coeff = std::abs(coeff);
  for (const ClosedInterval& i : intervals) {
    const int64 start = CeilRatio(i.start, abs_coeff);
    const int64 end = FloorRatio(i.end, abs_coeff);
    if (start > end) continue;
    if (new_size > 0 && start == intervals[new_size - 1].end + 1) {
      intervals[new_size - 1].end = end;
    } else {
      intervals[new_size++] = {start, end};
    }
  }
  intervals.resize(new_size);
  return coeff > 0 ? intervals : NegationOfSortedDisjointIntervals(intervals);
}

std::vector<ClosedInterval> DivisionOfSortedDisjointIntervals(
    std::vector<ClosedInterval> intervals, int64 coeff) {
  CHECK_NE(coeff, 0);
  DCHECK(IntervalsAreSortedAndDisjoint(intervals));
  const int64 abs_coeff = std::abs(coeff);
  for (ClosedInterval& i : intervals) {
    i.start = i.start / abs_coeff;
    i.end = i.end / abs_coeff;
  }
  UnionOfSortedIntervals(&intervals);
  return coeff > 0 ? intervals : NegationOfSortedDisjointIntervals(intervals);
}

SortedDisjointIntervalList::SortedDisjointIntervalList() {}

SortedDisjointIntervalList::SortedDisjointIntervalList(
    const std::vector<int64>& starts, const std::vector<int64>& ends) {
  InsertIntervals(starts, ends);
}

SortedDisjointIntervalList::SortedDisjointIntervalList(
    const std::vector<int>& starts, const std::vector<int>& ends) {
  InsertIntervals(starts, ends);
}

SortedDisjointIntervalList::SortedDisjointIntervalList(
    const std::vector<ClosedInterval>& vector_representation) {
  for (ClosedInterval interval : vector_representation) {
    InsertInterval(interval.start, interval.end);
  }
}

SortedDisjointIntervalList
SortedDisjointIntervalList::BuildComplementOnInterval(int64 start, int64 end) {
  SortedDisjointIntervalList interval_list;
  int64 next_start = start;
  for (auto it = FirstIntervalGreaterOrEqual(start); it != this->end(); ++it) {
    const ClosedInterval& interval = *it;
    const int64 next_end = CapSub(interval.start, 1);
    if (next_end > end) break;
    if (next_start <= next_end) {
      interval_list.InsertInterval(next_start, next_end);
    }
    next_start = CapAdd(interval.end, 1);
  }
  if (next_start <= end) {
    interval_list.InsertInterval(next_start, end);
  }
  return interval_list;
}

SortedDisjointIntervalList::Iterator SortedDisjointIntervalList::InsertInterval(
    int64 start, int64 end) {
  // start > end could mean an empty interval, but we prefer to LOG(DFATAL)
  // anyway. Really, the user should never give us that.
  if (start > end) {
    LOG(DFATAL) << "Invalid interval: " << ClosedInterval({start, end});
    return intervals_.end();
  }

  auto result = intervals_.insert({start, end});
  if (!result.second) return result.first;  // Duplicate: exit immediately.

  // TODO(user): tune the algorithm below if it proves to be a bottleneck.
  // For example, one could try to avoid an insertion if it's not needed
  // (when the interval merges with a single existing interval or is fully
  // contained by one).

  // Iterate over the previous iterators whose end is after (or almost at) our
  // start. After this, "it1" will point to the first interval that needs to be
  // merged with the current interval (possibly pointing to the current interval
  // itself, if no "earlier" interval should be merged).
  auto it1 = result.first;
  if (start == kint64min) {  // Catch underflows
    it1 = intervals_.begin();
  } else {
    const int64 before_start = start - 1;
    while (it1 != intervals_.begin()) {
      auto prev_it = it1;
      --prev_it;
      if (prev_it->end < before_start) break;
      it1 = prev_it;
    }
  }

  // Ditto, on the other side: "it2" will point to the interval *after* the last
  // one that should be merged with the current interval.
  auto it2 = result.first;
  if (end == kint64max) {
    it2 = intervals_.end();
  } else {
    const int64 after_end = end + 1;
    do {
      ++it2;
    } while (it2 != intervals_.end() && it2->start <= after_end);
  }

  // [it1..it2) is the range (inclusive on it1, exclusive on it2) of intervals
  // that should be merged together. We'll set it3 = it2-1 and erase [it1..it3)
  // and set *it3 to the merged interval.
  auto it3 = it2;
  it3--;
  if (it1 == it3) return it3;  // Nothing was merged.
  const int64 new_start = std::min(it1->start, start);
  const int64 new_end = std::max(it3->end, end);
  auto it = intervals_.erase(it1, it3);
  // HACK(user): set iterators point to *const* values. Which is expected,
  // because if one alters a set element's value, then it collapses the set
  // property! But in this very special case, we know that we can just overwrite
  // it->start, so we do it.
  const_cast<ClosedInterval*>(&(*it))->start = new_start;
  const_cast<ClosedInterval*>(&(*it))->end = new_end;
  return it;
}

SortedDisjointIntervalList::Iterator SortedDisjointIntervalList::GrowRightByOne(
    int64 value, int64* newly_covered) {
  auto it = intervals_.upper_bound({value, kint64max});
  auto it_prev = it;

  // No interval containing or adjacent to "value" on the left (i.e. below).
  if (it != begin()) {
    --it_prev;
  }
  if (it == begin() || ((value != kint64min) && it_prev->end < value - 1)) {
    *newly_covered = value;
    if (it == end() || it->start != value + 1) {
      // No interval adjacent to "value" on the right: insert a singleton.
      return intervals_.insert(it, {value, value});
    } else {
      // There is an interval adjacent to "value" on the right. Extend it by
      // one. Note that we already know that there won't be a merge with another
      // interval on the left, since there were no interval adjacent to "value"
      // on the left.
      DCHECK_EQ(it->start, value + 1);
      const_cast<ClosedInterval*>(&(*it))->start = value;
      return it;
    }
  }

  // At this point, "it_prev" points to an interval containing or adjacent to
  // "value" on the left: grow it by one, and if it now touches the next
  // interval, merge with it.
  CHECK_NE(kint64max, it_prev->end) << "Cannot grow right by one: the interval "
                                       "that would grow already ends at "
                                       "kint64max";
  *newly_covered = it_prev->end + 1;
  if (it != end() && it_prev->end + 2 == it->start) {
    // We need to merge it_prev with 'it'.
    const_cast<ClosedInterval*>(&(*it_prev))->end = it->end;
    intervals_.erase(it);
  } else {
    const_cast<ClosedInterval*>(&(*it_prev))->end = it_prev->end + 1;
  }
  return it_prev;
}

template <class T>
void SortedDisjointIntervalList::InsertAll(const std::vector<T>& starts,
                                           const std::vector<T>& ends) {
  CHECK_EQ(starts.size(), ends.size());
  for (int i = 0; i < starts.size(); ++i) InsertInterval(starts[i], ends[i]);
}

void SortedDisjointIntervalList::InsertIntervals(
    const std::vector<int64>& starts, const std::vector<int64>& ends) {
  InsertAll(starts, ends);
}

void SortedDisjointIntervalList::InsertIntervals(const std::vector<int>& starts,
                                                 const std::vector<int>& ends) {
  // TODO(user): treat kint32min and kint32max as their kint64 variants.
  InsertAll(starts, ends);
}

SortedDisjointIntervalList::Iterator
SortedDisjointIntervalList::FirstIntervalGreaterOrEqual(int64 value) const {
  const auto it = intervals_.upper_bound({value, kint64max});
  if (it == begin()) return it;
  auto it_prev = it;
  it_prev--;
  DCHECK_LE(it_prev->start, value);
  return it_prev->end >= value ? it_prev : it;
}

SortedDisjointIntervalList::Iterator
SortedDisjointIntervalList::LastIntervalLessOrEqual(int64 value) const {
  const auto it = intervals_.upper_bound({value, kint64max});
  if (it == begin()) return end();
  auto it_prev = it;
  it_prev--;
  return it_prev;
}

std::string SortedDisjointIntervalList::DebugString() const {
  std::string str;
  for (const ClosedInterval& interval : intervals_) {
    str += interval.DebugString();
  }
  return str;
}

}  // namespace operations_research
