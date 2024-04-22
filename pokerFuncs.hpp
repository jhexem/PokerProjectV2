#ifndef POKER_FUNCS_HPP
#define POKER_FUNCS_HPP

#include "pokerClasses.hpp"

double populateTree(Node* node, Dealer &dealer) {
   if (node->children.empty()) {
      std::vector<int> flop;
      flop.assign(node->cards.begin(), node->cards.begin() + 3);

      dealer.dealFlop(flop);
      dealer.dealTurnOrRiver(node->cards[3]);
      dealer.dealTurnOrRiver(node->cards[4]);
      std::vector<double> playerWins = dealer.rankHands();
      dealer.resetEverythingPostflop();
      node->winProb = playerWins[0];
      return playerWins[0];
   }
   double wins = 0.0;
   for (Node* child : node->children) {
      wins += populateTree(child, dealer);
   }
   node->winProb = wins / node->children.size();
   return node->winProb;
}

void populateSubtree(Node* tree, Dealer dealer, double &probSum, std::mutex &probSumMutex, int numThreads, int id) {
   double partialProbSum = 0.0;
   int numNodes = tree->children.size();
   int start = (numNodes / numThreads) * id;
   int end = (id == numThreads - 1) ? numNodes : (numNodes / numThreads) * (id + 1);
   for (int j=start; j<end; j++) {
      partialProbSum += populateTree(tree->children[j], dealer);
   }
   probSumMutex.lock();
   probSum += partialProbSum;
   probSumMutex.unlock();
}

void parallelPopulateTree(Node* tree, Dealer &dealer, int numThreads) {
   double probSum = 0.0;
   std::vector<std::thread> threads(numThreads);
   std::mutex probSumMutex;

   for (int i=0; i<numThreads; i++) {
      threads[i] = std::thread(populateSubtree, tree, dealer, std::ref(probSum), std::ref(probSumMutex), numThreads, i);
   }

   for (int i=0; i<numThreads; i++) {
      threads[i].join();
   }

   tree->winProb = probSum / 17296.0;
}

Node* createPreFlopTree(Dealer &dealer, std::vector<std::string> deadCards, int numThreads) {
   std::vector<int> deadCardInts;
   for (int i=0; i<dealer.numPlayers*2; i++) {
      deadCardInts.push_back(dealer.getInt(deadCards[i]));
   }

   Node* tree = new Node(deadCardInts);

   std::vector<std::vector<int>> allFlops = dealer.getAllPossibleFlops();
   int numFlops = allFlops.size();

   for (int i=0; i<numFlops; i++) {
      Node* flopNode = new Node(allFlops[i]);
      tree->addChild(flopNode);

      std::vector<int> allTurns = dealer.getAllPossibleTurnsOrRivers(allFlops[i]);
      int numTurns = allTurns.size();

      for (int j=0; j<numTurns; j++) {
         std::vector<int> flopTurn = {allFlops[i][0], allFlops[i][1], allFlops[i][2], allTurns[j]};

         Node* turnNode = new Node(flopTurn);
         flopNode->addChild(turnNode);

         std::vector<int> allRivers = dealer.getAllPossibleTurnsOrRivers(flopTurn);
         int numRivers = allRivers.size();
         for (int k=0; k<numRivers; k++) {
            std::vector<int> flopTurnRiver = {allFlops[i][0], allFlops[i][1], allFlops[i][2], allTurns[j], allRivers[k]};

            Node* riverNode = new Node(flopTurnRiver);
            turnNode->addChild(riverNode);
         }
      }
   }
   parallelPopulateTree(tree, dealer, numThreads);
   return tree;
}

Node* createFlopTree(Dealer &dealer, std::vector<std::string> deadCards, int numThreads, std::vector<std::string> flop) {
   std::vector<int> deadCardInts;
   for (int i=0; i<dealer.numPlayers*2; i++) {
      deadCardInts.push_back(dealer.getInt(deadCards[i]));
   }
   std::vector<int> intFlop;
   for (int i=0; i<3; i++) {
      intFlop.push_back(dealer.getInt(flop[i]));
   }

   Node* tree = new Node(deadCardInts);

   std::vector<int> allTurns = dealer.getAllPossibleTurnsOrRivers(intFlop);
   int numTurns = allTurns.size();

   for (int j=0; j<numTurns; j++) {
      std::vector<int> flopTurn = {intFlop[0], intFlop[1], intFlop[2], allTurns[j]};

      Node* turnNode = new Node(flopTurn);
      tree->addChild(turnNode);

      std::vector<int> allRivers = dealer.getAllPossibleTurnsOrRivers(flopTurn);
      int numRivers = allRivers.size();
      for (int k=0; k<numRivers; k++) {
         std::vector<int> flopTurnRiver = {intFlop[0], intFlop[1], intFlop[2], allTurns[j], allRivers[k]};

         Node* riverNode = new Node(flopTurnRiver);
         turnNode->addChild(riverNode);
      }
   }
   parallelPopulateTree(tree, dealer, numThreads);
   return tree;
}

std::vector<int> convertBoard(Dealer &dealer, std::vector<std::string> boardStr) {
   std::vector<int> board;
   int boardSize = boardStr.size();
   for (int i=0; i<boardSize; i++) {
      board.push_back(dealer.getInt(boardStr[i]));
   }
   return board;
}

Node* findFlopBoard(Node* tree, std::vector<int> board) {
   int numKids = tree->children.size();
   std::vector<int> newBoard;
   for (int i=0; i<3; i++) {
      newBoard.push_back(board[i]);
   }
   std::sort(newBoard.begin(), newBoard.end(), std::greater<int>());
   bool checkIfMatch;
   for (int i=0; i<numKids; i++) {
      checkIfMatch = true;
      for (int j=0; j<3; j++) {
         if (newBoard[j] != tree->children[i]->cards[j]) {
            checkIfMatch = false;
            break;
         }
      }
      if (checkIfMatch) {
         return tree->children[i];
      }
   }
   std::cout << "Could not find the flop board for some reason." << std::endl;
   return tree;
}

Node* findTurnBoard(Node* tree, int turn) {
   int numKids = tree->children.size();
   for (int i=0; i<numKids; i++) {
      if (turn == tree->children[i]->cards[3]) {
         return tree->children[i];
      }
   }
   std::cout << "Could not find the turn board for some reason." << std::endl;
   return tree;
}

Node* findRiverBoard(Node* tree, int river) {
   int numKids = tree->children.size();
   for (int i=0; i<numKids; i++) {
      if (river == tree->children[i]->cards[4]) {
         return tree->children[i];
      }
   }
   std::cout << "Could not find the turn board for some reason." << std::endl;
   return tree;
}

void deleteTree(Node* root) {
   if (root == nullptr) {
      return;
   }
   for (Node* child : root->children) {
      deleteTree(child);
   }
   delete root;
}

#endif