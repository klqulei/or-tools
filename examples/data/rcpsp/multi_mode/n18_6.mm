************************************************************************
file with basedata            : cn18_.bas
initial value random generator: 1604507927
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  145
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  1   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       18        2       18
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        3          3           5   9  15
   3        3          3           5  13  15
   4        3          3           6   7   8
   5        3          1           8
   6        3          2          11  13
   7        3          3          10  12  13
   8        3          2          12  14
   9        3          2          16  17
  10        3          2          14  15
  11        3          2          12  14
  12        3          1          16
  13        3          2          16  17
  14        3          1          17
  15        3          1          18
  16        3          1          18
  17        3          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1
------------------------------------------------------------------------
  1      1     0       0    0    0
  2      1     6       4    8    5
         2     9       4    8    0
         3    10       3    8    3
  3      1     4       5    6    6
         2     4       5    8    0
         3     9       4    5    0
  4      1     2       7    4    0
         2     8       6    4    4
         3    10       3    4    3
  5      1     1      10    4    5
         2     5      10    4    0
         3    10      10    3    4
  6      1     4       5    5    0
         2     4       4    6    0
         3     7       2    3    0
  7      1     2       6    9    8
         2     7       4    6    0
         3     7       3    7    0
  8      1     5       2    3    8
         2     7       2    3    0
         3     8       2    2    6
  9      1     2       3    3    6
         2     8       3    2    5
         3     8       3    3    0
 10      1     3       4    8    4
         2     3       5   10    0
         3    10       1    5    4
 11      1     1       8    7    0
         2     4       7    5    9
         3     8       7    3    0
 12      1     5       7    5    0
         2     9       7    4    0
         3     9       6    2    8
 13      1     5       9    2    9
         2     7       8    2    0
         3    10       6    2    0
 14      1     5      10    6    5
         2     7       7    6    0
         3    10       5    6    5
 15      1     7       2    9    0
         2     9       2    8    6
         3    10       1    5    0
 16      1     1       4    6    7
         2     9       3    5    4
         3     9       2    6    0
 17      1     1       6    5    7
         2     8       5    4    0
         3    10       1    4    7
 18      1     0       0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1
   35   31   24
************************************************************************
