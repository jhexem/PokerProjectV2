# Heads Up No Limit Texas Hold'em Poker Simulator
## Overview
This project is a simulator for the Heads Up No Limit Texas Hold'em variant of poker. The program facilitates games between two human players, managing the intricacies of gameplay and betting rounds. It is designed to handle the full scope of a poker game, including card dealing, betting, and hand evaluation at showdown.
## Features
- Gameplay Mechanics: Supports all four betting rounds typical to Texas Hold'em. Deals community cards in the prescribed sequence and evaluates the best five-card poker hand at showdown.
- Player and Pot Management: Tracks each player's chip stack and the total pot amount. Displays the current bet required to match the opposing player's wager.
- End of Game Detection: Identifies the conclusion of betting rounds and detects when a player has depleted their chip stack.
- Win Probability Calculation: Includes an optional advanced feature to calculate the exact win probabilities for each player at various stages of the game. This is achieved by constructing an n-ary tree of all potential future community card combinations, with win probabilities calculated and stored at each node.
## Betting Configurations
- Discrete and Continuous Betting: The simulator can toggle between discrete fixed betting sizes and a continuous betting range.
Probability Tree: To activate win probability calculations, compile the executable with the PROBABILITY macro defined.
- Continuous Betting: Enable this feature by defining the NORMALBETTING macro at compile time.
## Compilation Instructions
Both the PROBABILITY and NORMALBETTING features can be activated individually or in conjunction. To include them in your build, use the following macros during compilation: "-D PROBABILITY" or "-D NORMALBETTING". I used c++-14 when compiling this project as well. Compile your program with the desired macros to customize the functionality as needed.
