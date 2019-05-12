(sh scripts/addstock.sh 10 1 100000 | ./cv &
 sh scripts/addstock.sh 10 2 500000 | ./cv &
 sh scripts/addstock.sh 10 3 800000 | ./cv &
 sh scripts/addstock.sh 1 1 1 | ./cv);

 (sh scripts/addstock.sh 10 1 -100000 | ./cv &
 sh scripts/addstock.sh  10 2 -500000 | ./cv &
 sh scripts/addstock.sh  10 3 -800000 | ./cv) &
