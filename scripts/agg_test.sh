# SERVER MUST BE OPEN
# CLEARS SALES AND CREATES NEW ONES
# EXPECTED RESULT IS A FILE WITH -10000 FOR ID 1
>VENDAS;
sh scripts/addstock.sh 1 1 10000 | ./cv;
sh scripts/addstock.sh 10000 1 -1 | ./cv;
echo "a" | ./ma;
