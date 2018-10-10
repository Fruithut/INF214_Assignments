 # Oblig 2

## Question 1

### "bank1.cpp" invariant

balance >= 0

### "bank2.cpp" invariant

balance >= 0 AND (queueNumber >= nextInLine OR queueNumber != nextInLine)
(this invariant holds if there are less threads than the max value of the integer type from negative to positive)

## Question 2

### Termination

Does this program always terminate assuming a weakly fair scheduler?

We know that for a program to terminate under a weakly fair sheduler a critical condition must become true and stay true for the rest of the execution of the program.

In this program the condition in the while loop is (x != y), and the statements inside are:

  - if (x > y)  x = x - y;
  - if (x < y)  y = y - x;

This means that the values of x and y are heading towards eachother, and if x > y && x < y, then x must be equal to y, and hence the condition of the while loop is broken and the program terminates.

This all works out if we assume that the inputs are positive, if we have negative inputs however this program
will not work, take (-10, 30) as an example. The program will find that 30 > -10, and it will then try to decrease
30 with y = 30 - (-10), which is in fact increasing the value. This input will throw the program into and endless loop.

### Proof of "gcd"

Prove that if the program terminates, it does so in a state where x=gcd(n,m):

We know that the following properties hold for gcd:

 1. x > y => gcd(x, y) = gcd(x - y, y)
 2. gcd(x, x) = x
 3. gcd(x, y) = gcd(y, x)

To prove the termination state: ...
Needs:

- reasoning why every Hoare-triple in the program holds.
- for all pairs of consequtive statements, reasoning why the post-condition of the previous statement implies the precondition of the next.
- To get you started, consider using gcd(x,y)=gcd(n,m) as the loop invariant

LOOP INVARIANT {I: gcd(x,y) = gcd(n,m)}

    ```cs

    int gcd(int n, int m) {
        {n > 0 ∧ m > 0}
        A<int> x = n; 
        A<int> y = m;
        {x = n ^ y = n}

        {gcd(x,y) = gcd(n,m)}

        while (x != y) {

          CO ([&]{ ATO 

            {gcd(x,y) = gcd(n,m)}

            if (x > y)  x = x - y;

            {gcd(x,y) = gcd(n,m)}

          MIC; },

          
          [&]{ ATO 

            {gcd(x,y) = gcd(n,m)}

            if (x < y)  y = y - x; 

            {gcd(x,y) = gcd(n,m)}

          MIC; });

        }
        
        {gcd(x,y) = gcd(n,m)}
        

        {x = gcd(n, m)}
        return x;
    }

    ```

