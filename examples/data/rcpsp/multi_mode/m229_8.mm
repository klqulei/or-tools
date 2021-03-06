************************************************************************
file with basedata            : cm229_.bas
initial value random generator: 1087976535
************************************************************************
projects                      :  1
jobs (incl. supersource/sink ):  18
horizon                       :  127
RESOURCES
  - renewable                 :  2   R
  - nonrenewable              :  2   N
  - doubly constrained        :  0   D
************************************************************************
PROJECT INFORMATION:
pronr.  #jobs rel.date duedate tardcost  MPM-Time
    1     16      0       34        4       34
************************************************************************
PRECEDENCE RELATIONS:
jobnr.    #modes  #successors   successors
   1        1          3           2   3   4
   2        2          1           7
   3        2          3           5   6   8
   4        2          1          14
   5        2          2           9  11
   6        2          3           7  12  13
   7        2          2           9  11
   8        2          3           9  10  11
   9        2          2          15  16
  10        2          2          12  13
  11        2          1          17
  12        2          3          15  16  17
  13        2          1          14
  14        2          3          15  16  17
  15        2          1          18
  16        2          1          18
  17        2          1          18
  18        1          0        
************************************************************************
REQUESTS/DURATIONS:
jobnr. mode duration  R 1  R 2  N 1  N 2
------------------------------------------------------------------------
  1      1     0       0    0    0    0
  2      1     4      10    6    0    8
         2     9       7    6    0    8
  3      1     5       6   10    4    0
         2     8       4   10    0    8
  4      1     1       2    4    7    0
         2     5       1    4    0    9
  5      1     4       3    4    0    8
         2    10       3    2    7    0
  6      1    10       6    2   10    0
         2    10       6    2    0    4
  7      1     1       7    9    3    0
         2     6       7    8    0    5
  8      1     2       7    9    0    6
         2     7       3    6    0    4
  9      1     4       5    6    0    3
         2     9       5    3    0    2
 10      1     3       5    3    0    6
         2     7       3    2    4    0
 11      1     1       7    3    0    7
         2     3       5    2    8    0
 12      1     6       2    7    0    2
         2     8       2    3    0    2
 13      1     8      10    7    6    0
         2     9      10    3    3    0
 14      1     5       6    6    6    0
         2    10       2    2    0    5
 15      1     3       4    8    9    0
         2    10       2    4    9    0
 16      1     6       3   10    0    6
         2    10       1    8    0    1
 17      1     5       6    6    0    4
         2     6       5    3    6    0
 18      1     0       0    0    0    0
************************************************************************
RESOURCEAVAILABILITIES:
  R 1  R 2  N 1  N 2
   14   14   70   81
************************************************************************
