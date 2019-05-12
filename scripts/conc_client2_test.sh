# SERVER MUST BE OPEN
# ITEMS 1-5 MUST EXIST
# EXPECTED FINAL OUTPUT IS 100000
>STOCKS;
>VENDAS;
( (sh scripts/addstock.sh 20000 1 1 | ./cv) &
(sh scripts/addstock.sh 20000 2 1 | ./cv) &
(sh scripts/addstock.sh 20000 3 1 | ./cv) &
(sh scripts/addstock.sh 20000 4 1 | ./cv) &
(sh scripts/addstock.sh 20000 5 1 | ./cv) );

( (sh scripts/addstock.sh 20000 1 -1 | ./cv) &
(sh scripts/addstock.sh 20000 2 -1 | ./cv) &
(sh scripts/addstock.sh 20000 3 -1 | ./cv) &
(sh scripts/addstock.sh 20000 4 -1 | ./cv) &
(sh scripts/addstock.sh 20000 5 -1 | ./cv) );

./pp 3 | wc -l
