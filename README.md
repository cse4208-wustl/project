# GenAI solution to labs 0 - 3

The `genai` directory of this repository contains code that was built entirely with GenAI. GenAI was given one prompt, as follows:

>write a c++ program that plays a simple game of texas holdem in a terminal. The game is simple because there is no betting: just deal out the cards to the players and to the table, as you would in texas holdem and figure out the winner. Also, I want this program to play simple Pinochle. It's simple because it just deals the cards and figures out the melds. And this program should play Go Fish, allowing the user to pick which deck of cards to use in Go Fish: texas holdem deck, pinochle deck, or Uno deck.

You can compile and run the code from the genai directory with:
```
cd genai
g++ -std=c++17 -Wall -Wextra -o cardgames *.cpp
./cardgames
```

## Step 1 - Data Collection
Copy your lab3 to `lab3` directory of this repository. Add the necessary files to `genai` and `lab3` directories, to support your claims in the Data Collection step of the project.

## Step 3 - Application: Build It Again, With You in the Loop
For this step of the project, point GenAI to the `new_solution` directory of this repository. 
