# SERVER MUST BE OPEN
# EXPECTED FINAL OUTPUT IS 100000
>STOCKS;
>VENDAS;
(sh scripts/addstock.sh 1 1 100000 | ./cv);
(
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv) &
(sh scripts/addstock.sh 10000 1 -1 | ./cv)
);
./pp 3 | wc -l
