# SERVER MUST BE OPEN IN THE START OF THE SCRIPT
# SINCE IT REMOVES OLD 'ARTIGOS' FILE
# CLEARS ALL FILES
# TESTS THE CACHE COLLISIONS FOR BIG AMOUNTS OF ITEM ID'S
# EXPECTED OUTPUT IS 2000 lines with different id's on 'VENDAS'
>ARTIGOS
>STRINGS
>STOCKS;
>VENDAS;
echo 'Initializing items'
for ((i = 1; i <= 2000; i+=1))
do
    printf 'i agg_test %d\n' $i | ./ma > /dev/null
done
echo 'Please open the server now and press enter'
read;

for ((i = 1; i <= 2000; i+=1))
do
    echo $i
    sh scripts/addstock.sh 1 $i  1 | ./cv > /dev/null
    sh scripts/addstock.sh 1 $i -1 | ./cv > /dev/null
done
