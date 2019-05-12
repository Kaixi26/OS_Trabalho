# SERVER MUST BE OPEN IN THE START OF THE SCRIPT
# SINCE IT REMOVES OLD 'ARTIGOS' FILE
# CLEARS ALL FILES
# TESTS THE AGGREGATOR FOR BIG AMOUNTS OF ITEM ID'S
# EXPECTED OUTPUT IS 2000 lines with './pp 4'
>ARTIGOS
>STRINGS
>STOCKS;
>VENDAS;
for ((i = 1; i <= 200; i+=1))
do
    printf 'i agg_test %d\n' $i | ./ma
done
echo 'Please open the server now and press enter'
read;

for ((i = 1; i <= 200; i+=1))
do
    echo $i
    sh scripts/addstock.sh 1 $i 100 | ./cv > /dev/null
    sh scripts/addstock.sh 100 $i -1 | ./cv > /dev/null
done

echo "a" | ./ma;
