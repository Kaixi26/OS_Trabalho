# expected result:
# when piped into ./ma
# on empty files produces a STRINGS file with 8 names

for ((x = 1; x < 9; x+=1));
do
    printf 'i 0123456789 %d\n' $x
done

echo 'n 1 9876543210'            
echo 'n 2 9876543210'
