make

mynetwork=(Switzerland Paris Berlin London/gtfs Rome Athens/gtfs)   # Networks
i=1     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
w=1     # Walk (1: yes, 0: no)
r=2     # Queries from File: 1, Random Queries: 2

a=`./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a 3 -w "$w" | tail -5`
b=`./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a 3 -w "$w" | tail -5`
c=`./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a 3 -w "$w" | tail -5`
d=`./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a 3 -w "$w" | tail -5`
e=`./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a 3 -w "$w" | tail -5`
f=`./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a 3 -w "$w" | tail -5`

printf "$a\n$b\n$c\n$d\n$e\n$f" > output.txt
