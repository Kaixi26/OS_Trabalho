# SERVER MUST BE OPEN
# CLEARS SALES AND CREATES NEW ONES
# EXPECTED RESULT IS A FILE WITH -10000 FOR ID's 1-5
>VENDAS;
sh scripts/addstock.sh 1 1 10000 | ./cv;
sh scripts/addstock.sh 1 2 10000 | ./cv;
sh scripts/addstock.sh 1 3 10000 | ./cv;
sh scripts/addstock.sh 1 4 10000 | ./cv;
sh scripts/addstock.sh 1 5 10000 | ./cv;
sh scripts/addstock.sh 10000 1 -1 | ./cv;
sh scripts/addstock.sh 10000 2 -1 | ./cv;
sh scripts/addstock.sh 10000 3 -1 | ./cv;
sh scripts/addstock.sh 10000 4 -1 | ./cv;
sh scripts/addstock.sh 10000 5 -1 | ./cv;
echo "a" | ./ma;
