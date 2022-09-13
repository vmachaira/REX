make

####################### Athens
mynetwork=(Switzerland Paris Berlin London Rome Athens)   # Networks
i=5     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
w=0     # Walk (1: yes, 0: no)
r=1     # Queries from File: 1, Random Queries: 2
alg=3
days=7
walkduration=0;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=300;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=600;


####################### Rome
mynetwork=(Switzerland Paris Berlin London Rome Athens)   # Networks
i=4     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
w=0     # Walk (1: yes, 0: no)
r=1     # Queries from File: 1, Random Queries: 2
alg=3
days=2
walkduration=0;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=300;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=600;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"


####################### London
mynetwork=(Switzerland Paris Berlin London Rome Athens)   # Networks
i=3     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
w=0     # Walk (1: yes, 0: no)
r=1     # Queries from File: 1, Random Queries: 2
alg=3
days=2
walkduration=0;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=300;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=600;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"


####################### Berlin
mynetwork=(Switzerland Paris Berlin London Rome Athens)   # Networks
i=2     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
w=0     # Walk (1: yes, 0: no)
r=1     # Queries from File: 1, Random Queries: 2
alg=3
days=2
walkduration=0;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=300;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=600;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"



####################### Switzerland
mynetwork=(Switzerland Paris Berlin London Rome Athens)   # Networks
i=0     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
w=0     # Walk (1: yes, 0: no)
r=1     # Queries from File: 1, Random Queries: 2
alg=3
days=2
walkduration=0;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=300;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

w=1
walkduration=600;


./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
