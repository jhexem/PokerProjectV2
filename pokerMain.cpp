#include "pokerFuncs.hpp"

int main(int argc, char* argv[]) {

   if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << " numThreads" << std::endl;
      return 1;
   }

   const int numThreads = std::atoi(argv[1]);

   bool play72Rule = false; //true means 72 rule is turned on, false means it is turned off
   double bounty72Rule = 10.0; //bounty in big blinds for winning a hand with 72o

   double p1StartingStack = 100.0;
   double p2StartingStack = 100.0;

   double smallBlind = 0.5;
   double bigBlind = 1.0;

   const double raiseSizes[] = {2.0, 3.0, 5.0}; //relative to opponent's bet/raise size
   const double betSizes[] = {0.33, 0.75, 1.3}; //relative to the pot

   Dealer dealer(p1StartingStack, p2StartingStack, smallBlind, bigBlind, play72Rule, bounty72Rule);
   bool playAgain = true;

   while (playAgain) {
      dealer.resetEverything();
      dealer.dealHands();
      std::vector<std::string> playerHands = dealer.getAllHands();

      #ifdef PROBABILITY
      std::cout << std::endl << "Setting up the game..." << std::endl;
      Node* tree = createPreFlopTree(dealer, playerHands, numThreads);
      #endif

      dealer.deck.shuffle();
      std::cout << std::endl;

      #ifdef PROBABILITY
      std::cout << "Player 1's preflop win percentage is: " << tree->winProb * 100 << "%" << std::endl << std::endl;
      #endif
      
      #ifndef NORMALBETTING
      int checkWin = dealer.startFixedPreFlopBettingRound(betSizes, raiseSizes);
      #endif
      #ifdef NORMALBETTING
      int checkWin = dealer.startPreFlopBettingRound();
      #endif

      if (checkWin == 1 or checkWin == 2) {
         dealer.declareWinner(checkWin, 0.0, false);
      } else if (checkWin == -1) {
         dealer.dealFlop();

         #ifdef PROBABILITY
         Node* flopNode = findFlopBoard(tree, dealer.board);
         std::cout << "Player 1's flop win percentage is: " << flopNode->winProb * 100 << "%" << std::endl << std::endl;
         #endif

         dealer.dealTurnOrRiver();

         #ifdef PROBABILITY
         Node* turnNode = findTurnBoard(flopNode, dealer.board[3]);
         std::cout << "Player 1's turn win percentage is: " << turnNode->winProb * 100 << "%" << std::endl << std::endl;
         #endif
         
         dealer.dealTurnOrRiver();

         #ifdef PROBABILITY
         Node* riverNode = findRiverBoard(turnNode, dealer.board[4]);
         std::cout << "Player 1's river win percentage is: " << riverNode->winProb * 100 << "%" << std::endl << std::endl;
         
         double p1winProb = riverNode->winProb;
         
         #else
         std::vector<double> winners = dealer.rankHands();
         double p1winProb;
         if (winners[0] > winners[1]) {
            p1winProb = 1.0;
         } else if (winners[0] < winners[1]) {
            p1winProb = 0.0;
         } else {
            p1winProb = 0.5;
         }
         #endif

         dealer.declareWinner(checkWin, p1winProb, true);

         dealer.showBoard();
         std::cout << std::endl;
         dealer.showAllHands();
         std::cout << std::endl;

      } else {

         dealer.dealFlop();

         #ifdef PROBABILITY
         Node* flopNode = findFlopBoard(tree, dealer.board);
         std::cout << "Player 1's flop win percentage is: " << flopNode->winProb * 100 << "%" << std::endl << std::endl;
         #endif

         #ifndef NORMALBETTING
         checkWin = dealer.startFixedPostFlopBettingRound(betSizes, raiseSizes);
         #endif
         #ifdef NORMALBETTING
         checkWin = dealer.startPostFlopBettingRound();
         #endif

         if (checkWin == 1 or checkWin == 2) {
            dealer.declareWinner(checkWin, 0.0, true);
         } else if (checkWin == -1) {
            dealer.dealTurnOrRiver();

            #ifdef PROBABILITY
            Node* turnNode = findTurnBoard(flopNode, dealer.board[3]);
            std::cout << "Player 1's turn win percentage is: " << turnNode->winProb * 100 << "%" << std::endl << std::endl;
            #endif

            dealer.dealTurnOrRiver();

            #ifdef PROBABILITY
            Node* riverNode = findRiverBoard(turnNode, dealer.board[4]);
            std::cout << "Player 1's river win percentage is: " << riverNode->winProb * 100 << "%" << std::endl << std::endl;

            double p1winProb = riverNode->winProb;
            
            #else
            std::vector<double> winners = dealer.rankHands();
            double p1winProb;
            if (winners[0] > winners[1]) {
               p1winProb = 1.0;
            } else if (winners[0] < winners[1]) {
               p1winProb = 0.0;
            } else {
               p1winProb = 0.5;
            }
            #endif

            dealer.declareWinner(checkWin, p1winProb, true);

            dealer.showBoard();
            std::cout << std::endl;
            dealer.showAllHands();
            std::cout << std::endl;
            
         } else {
            dealer.dealTurnOrRiver();

            #ifdef PROBABILITY
            Node* turnNode = findTurnBoard(flopNode, dealer.board[3]);
            std::cout << "Player 1's turn win percentage is: " << turnNode->winProb * 100 << "%" << std::endl << std::endl;
            #endif

            #ifndef NORMALBETTING
            checkWin = dealer.startFixedPostFlopBettingRound(betSizes, raiseSizes);
            #endif
            #ifdef NORMALBETTING
            checkWin = dealer.startPostFlopBettingRound();
            #endif

            if (checkWin == 1 or checkWin == 2) {
               dealer.declareWinner(checkWin, 0.0, true);
            } else if (checkWin == -1) {
               dealer.dealTurnOrRiver();

               #ifdef PROBABILITY
               Node* riverNode = findRiverBoard(turnNode, dealer.board[4]);
               std::cout << "Player 1's river win percentage is: " << riverNode->winProb * 100 << "%" << std::endl << std::endl;
               
               double p1winProb = riverNode->winProb;
         
               #else
               std::vector<double> winners = dealer.rankHands();
               double p1winProb;
               if (winners[0] > winners[1]) {
                  p1winProb = 1.0;
               } else if (winners[0] < winners[1]) {
                  p1winProb = 0.0;
               } else {
                  p1winProb = 0.5;
               }
               #endif

               dealer.declareWinner(checkWin, p1winProb, true);

               dealer.showBoard();
               std::cout << std::endl;
               dealer.showAllHands();
               std::cout << std::endl;
               
            } else {
               dealer.dealTurnOrRiver();

               #ifdef PROBABILITY
               Node* riverNode = findRiverBoard(turnNode, dealer.board[4]);
               std::cout << "Player 1's river win percentage is: " << riverNode->winProb * 100 << "%" << std::endl << std::endl;
               #endif

               #ifndef NORMALBETTING
               checkWin = dealer.startFixedPostFlopBettingRound(betSizes, raiseSizes);
               #endif
               #ifdef NORMALBETTING
               checkWin = dealer.startPostFlopBettingRound();
               #endif

               #ifdef PROBABILITY
               double p1winProb = riverNode->winProb;

               #else
               std::vector<double> winners = dealer.rankHands();
               double p1winProb;
               if (winners[0] > winners[1]) {
                  p1winProb = 1.0;
               } else if (winners[0] < winners[1]) {
                  p1winProb = 0.0;
               } else {
                  p1winProb = 0.5;
               }
               #endif

               dealer.declareWinner(checkWin, p1winProb, true);

               dealer.showBoard();
               std::cout << std::endl;
               dealer.showAllHands();
               std::cout << std::endl;
            }
         }
         #ifdef PROBABILITY
         deleteTree(tree);
         #endif
      }

      if (dealer.players[0].stack <= 1e-10) {
         std::cout << "Player 2 wins the game! Thanks for playing!" << std::endl << std::endl;
         playAgain = false;
         break;
      } else if (dealer.players[1].stack <= 1e-10) {
         std::cout << "Player 1 wins the game! Thanks for playing!" << std::endl << std::endl;
         playAgain = false;
         break;
      } else {
         std::string oneMoreHand;
         while (true) {
            std::cout << "Would you like to play another hand? (y = yes, n = no): ";
            std::cin >> oneMoreHand;
            std::cout << std::endl;
            if (oneMoreHand == "y") {
               break;
            } else if (oneMoreHand == "n") {
               std::cout << "Thanks for playing! The final stack sizes are: " << std::endl << std::endl;
               dealer.showStacks();
               std::cout << std::endl;
               playAgain = false;
               break;
            } else {
               std::cout << "Please type a valid input." << std::endl << std::endl;
            }
         }
      }
   }
   
   return 0;
}

//calculate stats like VPIP, pot odds, implied odds, PFR, cbet%, etc. to the HUD