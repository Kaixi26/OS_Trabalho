# SERVER MUST BE OPEN
# EXPECTED FINAL OUTPUT IS 100000
>STOCKS;
>VENDAS;
sh scripts/addstock.sh 100000 1 1 | ./cv;
sh scripts/addstock.sh 100000 1 -1 | ./cv;
./pp 3 | wc -l
