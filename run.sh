make

# Enabled Networks
Berlin=0
London=0
Rome=0
Switzerland=0
Athens=1

# Other Info
days=1
r=1     # Queries from File: 1, Random Queries: 2
alg=3
w=1     # Walk (1: yes, 0: no)

# Networks
mynetwork=(Switzerland Paris Berlin London Rome Athens)

####################### Berlin
i=2     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)

walkduration=300;

if [ "$Berlin" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi

walkduration=600;


if [ "$Berlin" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi

####################### Athens
i=5     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
walkduration=300;


if [ "$Athens" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi
walkduration=600;


#if [ "$Athens" -eq 1 ]
#then
#   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
#fi

####################### Rome
i=4     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
walkduration=300;


if [ "$Rome" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi
walkduration=600;


if [ "$Rome" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi

####################### London
i=3     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
walkduration=300;


if [ "$London" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi
walkduration=600;


if [ "$London" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi

####################### Paris
i=1     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
walkduration=300;


 #./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

walkduration=600;


 #./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"

####################### Switzerland
i=0     # Select Network (0: Switzerland, 1: Paris, 2: Berlin, 3: London 4: Rome 5: Athens)
walkduration=300;


if [ "$Switzerland" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi

walkduration=600;


if [ "$Switzerland" -eq 1 ]
then
   ./main -f 2 -n "${mynetwork[$i]}" -o "$r" -g 2 -a "$alg" -w "$w" -x "$days" -y "$walkduration"
fi
