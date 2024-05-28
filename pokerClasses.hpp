#ifndef POKER_CLASSES_HPP
#define POKER_CLASSES_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <stack>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <iomanip>

class Deck {
   private:
      char values[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
      char suits[4] = {'c', 'd', 'h', 's'};
      std::stack<int> cards; // value is card / 4 and suit is card % 4 (cdhs)
      int numCards;

   public:
      Deck() {
         numCards = 52;
         for (int i=0; i<52; i++) {
            cards.push(i);
         }
         shuffle();
      }

      Deck(std::vector<std::string> deadStrs) {
         std::vector<int> deadCards;
         int numDeadCards = deadStrs.size();
         numCards = 52 - numDeadCards;
         for (int i=0; i<numDeadCards; i++) {
            deadCards.push_back(getInt(deadStrs[i]));
         }
         for (int i=0; i<52; i++) {
            bool isDeadCard = false;
            for (int j=0; j<numDeadCards; j++) {
               if (deadCards[j] == i) {
                  isDeadCard = true;
               }
            }
            if (!isDeadCard) {
               cards.push(i);
            }
         }
         //shuffle();
      }

      Deck(std::vector<int> deadInts) {
         std::vector<int> deadCards;
         int numDeadCards = deadInts.size();
         numCards = 52 - numDeadCards;
         for (int i=0; i<numDeadCards; i++) {
            deadCards.push_back(deadInts[i]);
         }
         for (int i=0; i<52; i++) {
            bool isDeadCard = false;
            for (int j=0; j<numDeadCards; j++) {
               if (deadCards[j] == i) {
                  isDeadCard = true;
               }
            }
            if (!isDeadCard) {
               cards.push(i);
            }
         }
         //shuffle();
      }

      int getNumCards() {
         return numCards;
      }

      int dealCard() {
         if (cards.empty()) {
            std::cout << "No more cards in the deck." << std::endl;
            return -1;
         }
         int card = cards.top();
         cards.pop();
         numCards--;
         return card;
      }

      std::string getCard(int card) {
         std::string value = std::string(1, values[card / 4]);
         std::string suit = std::string(1, suits[card % 4]);
         return value + suit;
      }

      void remove(std::vector<std::string> rmCardsStrs) {
         std::vector<int> rmCards;
         int numRmCards = rmCardsStrs.size();
         for (int i=0; i<numRmCards; i++) {
            rmCards.push_back(getInt(rmCardsStrs[i]));
         }
         std::vector<int> temp;
         while (!cards.empty()) {
            bool isMatch = false;
            for (int i=0; i<numRmCards; i++) {
               if (cards.top() == rmCards[i]) {isMatch = true;}
            }
            if (!isMatch) {
               temp.push_back(cards.top());
               cards.pop();
            }
         }
         while (!temp.empty()) {
            cards.push(temp.back());
            temp.pop_back();
         }
         shuffle();
      }

      void remove(std::vector<int> rmCards) {
         std::vector<int> temp;
         int numRmCards = rmCards.size();
         while (!cards.empty()) {
            bool isMatch = false;
            for (int i=0; i<numRmCards; i++) {
               if (cards.top() == rmCards[i]) {isMatch = true;}
            }
            if (!isMatch) {
               temp.push_back(cards.top());
               cards.pop();
            }
         }
         while (!temp.empty()) {
            cards.push(temp.back());
            temp.pop_back();
         }
         shuffle();
      }

      int getInt(std::string card) {
         char value = card[0];
         char suit = card[1];
         int ans = 0;
         for (int i=0; i<13; i++) {
            if (value == values[i]) {
               ans = i * 4;
            }
         }
         for (int i=0; i<4; i++) {
            if (suit == suits[i]) {
               ans += i;
            }
         }
         return ans;
      }

      void shuffle() {
         std::vector<int> temp;
         while (!cards.empty()) {
            temp.push_back(cards.top());
            cards.pop();
         }
         unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
         std::default_random_engine engine(seed);
         std::shuffle(temp.begin(), temp.end(), engine);
         while (!temp.empty()) {
            cards.push(temp.back());
            temp.pop_back();
         }
      }
};

class Player {
   private:
      char values[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
      char suits[4] = {'c', 'd', 'h', 's'};

   public:
      int name;
      std::vector<int> hand;
      float stack;
      std::vector<int> fullHand;
      std::vector<std::vector<int>> fiveCardHands;
      std::vector<std::string> keys;
      std::unordered_map<std::string, int> handRanks;

      Player(int input, float startingStack) {
         name = input;
         stack = startingStack;

         std::ifstream file("hashMap.csv");
         if (!file.is_open()) {
            std::cerr << "Could not open the file" << std::endl;
         } else {
            std::string line;
            while (std::getline(file, line)) {
               std::istringstream ss(line);
               std::string key, value;
               
               // Assuming each line in the CSV is of the form "key,value"
               if (std::getline(ss, key, ',') && std::getline(ss, value)) {
                     handRanks[key] = std::stoi(value);
               }
            }
            file.close();
         }
      }

      void takeCard(int card) {
         int size = hand.size();
         hand.push_back(card);
         fullHand.push_back(card);
      }

      void takeCard(std::string cardStr) {
         int card = getInt(cardStr);
         hand.push_back(card);
         fullHand.push_back(card);
      }

      int getInt(std::string card) {
         char value = card[0];
         char suit = card[1];
         int ans = 0;
         for (int i=0; i<13; i++) {
            if (value == values[i]) {
               ans = i * 4;
            }
         }
         for (int i=0; i<4; i++) {
            if (suit == suits[i]) {
               ans += i;
            }
         }
         return ans;
      }

      bool check72Rule() {
         if (!((hand[0] == 0) or (hand[0] == 1) or (hand[0] == 2) or (hand[0] == 3) or (hand[1] == 0) or (hand[1] == 1) or (hand[1] == 2) or (hand[1] == 3))) {
            return false;
         }
         if (!((hand[0] == 20) or (hand[0] == 21) or (hand[0] == 22) or (hand[0] == 23) or (hand[1] == 20) or (hand[1] == 21) or (hand[1] == 22) or (hand[1] == 23))) {
            return false;
         }
         if (hand[0] % 4 == hand[1] % 4) {
            return false;
         }
         return true;
      }

      void resetHand() {
         hand.clear();
         fullHand.clear();
         fiveCardHands.clear();
      }

      std::vector<std::string> getHand() {
         std::string card1 = getCard(hand[0]);
         std::string card2 = getCard(hand[1]);
         std::vector<std::string> cards{card1, card2};
         return cards;
      }

      std::string getCard(int card) {
         std::string value = std::string(1, values[card / 4]);
         std::string suit = std::string(1, suits[card % 4]);
         return value + suit;
      }

      void printHand() {
         std::vector<std::string> cards = getHand();
         std::cout << "Player " << name << " has " << cards[0] << " " << cards[1] << std::endl;
      }

      void getFiveCardHands() {
         fiveCardHands.clear();
         int numCards = fullHand.size();
         std::vector<int> newHand;

         std::sort(fullHand.begin(), fullHand.end(), std::greater<int>());

         switch(numCards) {
            case 5:
               fiveCardHands.push_back(fullHand);
               break;
            case 6:
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[3], fullHand[4]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[3], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[3], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[2], fullHand[3], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[1], fullHand[2], fullHand[3], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               break;
            case 7:
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[3], fullHand[4]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[3], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[3], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[4], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[2], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[3], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[3], fullHand[4], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[3], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[1], fullHand[4], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[2], fullHand[3], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[2], fullHand[3], fullHand[4], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[2], fullHand[3], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[2], fullHand[4], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[0], fullHand[3], fullHand[4], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[1], fullHand[2], fullHand[3], fullHand[4], fullHand[5]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[1], fullHand[2], fullHand[3], fullHand[4], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[1], fullHand[2], fullHand[3], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[1], fullHand[2], fullHand[4], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[1], fullHand[3], fullHand[4], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               newHand = {fullHand[2], fullHand[3], fullHand[4], fullHand[5], fullHand[6]};
               fiveCardHands.push_back(newHand);
               break;
            default:
               std::cout << "Not enough cards to create five card hands." << std::endl;
         }
      }

      bool checkFlush(std::vector<int> fiveCardHand) {
         int suit = fiveCardHand[0] % 4;
         for (int i=1; i<5; i++) {
            if (fiveCardHand[i] % 4 != suit) {
               return false;
            }
         }
         return true;
      }

      std::string getKey(std::vector<int> fiveCardHand) {
         char card1 = values[fiveCardHand[0] / 4];
         char card2 = values[fiveCardHand[1] / 4];
         char card3 = values[fiveCardHand[2] / 4];
         char card4 = values[fiveCardHand[3] / 4];
         char card5 = values[fiveCardHand[4] / 4];
         std::string key = std::string(1, card1) + std::string(1, card2) + std::string(1, card3) + std::string(1, card4) + std::string(1, card5);
         bool flush = checkFlush(fiveCardHand);
         if (flush){
            key = "f" + key;
         }
         return key;
      }

      void getAllKeys() {
         keys.clear();
         getFiveCardHands();
         int size = fiveCardHands.size();
         for (int i=0; i<size; i++) {
            keys.push_back(getKey(fiveCardHands[i]));
         }
      }

      int rankPlayerHand() {
         getAllKeys();
         int size = keys.size();
         int minRank = 10000;
         int rank;
         for (int i=0; i<size; i++) {
            rank = handRanks[keys[i]];
            if (minRank > rank) {
               minRank = rank;
            }
         }
         return minRank;
      }

      int getPlayerHandType() {
         int rank = rankPlayerHand();
         if (rank == 1) {
            return 9;  //royal flush
         } else if (rank > 1 && rank <= 10) {
            return 8;  //straight flush
         } else if (rank > 10 && rank <= 166) {
            return 7;  //four of a kind
         } else if (rank > 166 && rank <= 322) {
            return 6;  //full house
         } else if (rank > 322 && rank <= 1599) {
            return 5;  //flush
         } else if (rank > 1599 && rank <= 1609) {
            return 4;  //straight
         } else if (rank > 1609 && rank <= 2467) {
            return 3;  //three of a kind
         } else if (rank > 2467 && rank <= 3325) {
            return 2;  //two pair
         } else if (rank > 3325 && rank <= 6185) {
            return 1;  //pair
         } else {
            return 0;  //high card
         }
      }
};

class Dealer {
   private:
         char values[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
         char suits[4] = {'c', 'd', 'h', 's'};
         
   public:
      Deck deck;
      std::vector<Player> players;
      int numPlayers;
      float pot;
      bool p1Button;
      float smallBlind;
      float bigBlind;
      std::vector<int> board;
      std::unordered_map<std::string, int> handRanks;
      std::vector<int> deadCards;
      bool play72Rule;
      float bounty72Rule;

      Dealer(std::vector<std::string> deadCardStr) {
         int numDeadCards = deadCardStr.size();
         for (int i=0; i<numDeadCards; i++) {
            deadCards.push_back(getInt(deadCardStr[i]));
         }
         Deck newDeck(deadCards);
         deck = newDeck;

         numPlayers = numDeadCards / 2;
         std::vector<Player> newPlayers;
         for (int i=0; i<numPlayers; i++) {
            Player newPlayer(i+1, 100.0);
            newPlayers.push_back(newPlayer);
            newPlayer.takeCard(deadCardStr[2*i]);
            newPlayer.takeCard(deadCardStr[2*i + 1]);
            newPlayers.push_back(newPlayer);
         }
         players = newPlayers;
         pot = 0.0;
         p1Button = false;
         smallBlind = 0.5;
         bigBlind = 1.0;
      }

      Dealer(std::vector<std::string> deadCardStr, float startingStack) {
         int numDeadCards = deadCardStr.size();
         for (int i=0; i<numDeadCards; i++) {
            deadCards.push_back(getInt(deadCardStr[i]));
         }
         Deck newDeck(deadCards);
         deck = newDeck;

         numPlayers = numDeadCards / 2;
         std::vector<Player> newPlayers;
         for (int i=0; i<numPlayers; i++) {
            Player newPlayer(i+1, startingStack);
            newPlayers.push_back(newPlayer);
            newPlayer.takeCard(deadCardStr[2*i]);
            newPlayer.takeCard(deadCardStr[2*i + 1]);
            newPlayers.push_back(newPlayer);
         }
         players = newPlayers;
         pot = 0.0;
         p1Button = false;
      }

      Dealer(float p1StartingStack, float p2StartingStack, float SB, float BB, bool play72, float bounty72) {
         numPlayers = 2;
         Deck deck;
         std::vector<Player> newPlayers;
         Player player1(1, p1StartingStack);
         newPlayers.push_back(player1);
         Player player2(2, p2StartingStack);
         newPlayers.push_back(player2);
         players = newPlayers;
         pot = 0.0;
         smallBlind = SB;
         bigBlind = BB;
         p1Button = false;
         play72Rule = play72;
         bounty72Rule = bounty72;
      }

      Dealer() {
         numPlayers = 2;
         Deck deck;
         std::vector<Player> newPlayers;
         Player player1(1, 0.0);
         newPlayers.push_back(player1);
         Player player2(2, 0.0);
         newPlayers.push_back(player2);
         players = newPlayers;
      }

      int getInt(std::string card) {
         char value = card[0];
         char suit = card[1];
         int ans = 0;
         for (int i=0; i<13; i++) {
            if (value == values[i]) {
               ans = i * 4;
            }
         }
         for (int i=0; i<4; i++) {
            if (suit == suits[i]) {
               ans += i;
            }
         }
         return ans;
      }

      std::string getCard(int card) {
            std::string value = std::string(1, values[card / 4]);
            std::string suit = std::string(1, suits[card % 4]);
            return value + suit;
      }

      void dealHands() {
         for (int i=0; i<numPlayers; i++) {
            int newCard1 = deck.dealCard();
            int newCard2 = deck.dealCard();
            players[i].takeCard(newCard1);
            players[i].takeCard(newCard2);
            deadCards.push_back(newCard1);
            deadCards.push_back(newCard2);
         }
      }

      void dealHands(std::vector<std::string> hands) {
         if (hands.size() != 4) {std::cout << "Invalid number of cards for player hands." << std::endl;}
         else {
            deck.remove(hands);
            players[0].takeCard(getInt(hands[0]));
            players[0].takeCard(getInt(hands[1]));
            players[1].takeCard(getInt(hands[2]));
            players[1].takeCard(getInt(hands[3]));
         }
      }

      void dealHands(std::vector<int> hands) {
         if (hands.size() != 4) {std::cout << "Invalid number of cards for player hands." << std::endl;}
         else {
            deck.remove(hands);
            players[0].takeCard(hands[0]);
            players[0].takeCard(hands[1]);
            players[1].takeCard(hands[2]);
            players[1].takeCard(hands[3]);
         }
      }

      std::vector<std::string> getAllHands() {
         std::vector<std::string> hands;
         for (int i=0; i<numPlayers; i++) {
            std::vector<std::string> hand = players[i].getHand();
            hands.push_back(hand[0]);
            hands.push_back(hand[1]);
         }
         return hands;
      }

      void showAllHands() {
         for (int i=0; i<numPlayers; i++) {
            players[i].printHand();
         }
      }

      void showStacks() {
         if (players[0].stack < 1e-4) {
            players[0].stack = 0.0;
         } else if (players[1].stack < 1e-4) {
            players[1].stack = 0.0;
         }
         std::cout << "Player 1's Stack: " << players[0].stack << std::endl;
         std::cout << "Player 2's Stack: " << players[1].stack << std::endl;
      }

      std::string getPlayerDecision(Player player) {
         std::string decision;
         std::cout << "Player " << player.name << " please make your decision (f = fold, c = check/call, b = bet/raise): ";
         std::getline(std::cin, decision);
         std::cout << std::endl;
         return decision;
      }

      std::string getFixedPlayerDecision(Player player, std::vector<float> validBetSizes, std::vector<float> validRaiseSizes, bool isBet) {
         std::string decision;
         if (isBet) {
            std::cout << "Player " << player.name << " make your decision (f = fold, c = check/call, b1 = bet " << validBetSizes[0] << ", b2 = bet " << validBetSizes[1] << ", b3 = bet " << validBetSizes[2] << "): ";
         } else {
            std::cout << "Player " << player.name << " make your decision (f = fold, c = check/call, b1 = raise to " << validRaiseSizes[0] << ", b2 = raise to " << validRaiseSizes[1] << ", b3 = raise to " << validRaiseSizes[2] << "): ";
         }
         std::getline(std::cin, decision);
         std::cout << std::endl;
         return decision;
      }

      std::vector<float> getValidFixedSizes(std::vector<float> validSizes, float minBet, float p1Stack, float p2Stack) {
         float maxBet = std::min(p1Stack, p2Stack);
         if (maxBet < minBet) {
            validSizes[0] = maxBet;
            validSizes[1] = maxBet;
            validSizes[2] = maxBet;
         } else {
            for (int i=0; i<3; i++) {
               if (validSizes[i] <= minBet) {
                  validSizes[i] = minBet;
               } else if (validSizes[i] >= maxBet) {
                  validSizes[i] = maxBet;
               }
            }
         }
         return validSizes;
      }

      std::string getAllInPlayerDecision(Player player) {
         std::string decision;
         std::cout << "Player " << player.name << " please make your decision (f = fold, c = call): ";
         std::getline(std::cin, decision);
         std::cout << std::endl;
         return decision;
      }

      int startPreFlopBettingRound() {
         std::vector<std::string> playerHands = getAllHands();

         int checkWin; //if checkWin = player.name, then that player won. If checkWin = 0, then both players matched each other's bet. If checkWin = -1 then one or both players are all in.
         float amountToCall = 0.0;
         bool betMatched = false;
         float totalBetAmountP1 = 0.0;
         float totalBetAmountP2 = 0.0;
         float p1StartingStack = players[0].stack;
         float p2StartingStack = players[1].stack;
         bool isAllIn = false;
         float remainingStack;

         if (p1Button) {
            if (players[0].stack <= smallBlind or players[1].stack <= smallBlind) {
               isAllIn = true;

               if (players[0].stack <= players[1].stack) {
                  remainingStack = players[0].stack;
               } else {
                  remainingStack = players[1].stack;
               }
               totalBetAmountP1 = totalBetAmountP1 + remainingStack;
               players[0].stack = players[0].stack - remainingStack;
               pot = pot + remainingStack;

               totalBetAmountP2 = totalBetAmountP2 + remainingStack;
               players[1].stack = players[1].stack - remainingStack;
               pot = pot + remainingStack;

               betMatched = true;
            } else if (players[1].stack <= bigBlind) {
               isAllIn = true;
               remainingStack = players[1].stack;

               totalBetAmountP1 = totalBetAmountP1 + smallBlind;
               players[0].stack = players[0].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP2 = totalBetAmountP2 + remainingStack;
               players[1].stack = players[1].stack - remainingStack;
               pot = pot + remainingStack;
               amountToCall = remainingStack - smallBlind;
            } else {
               totalBetAmountP1 = totalBetAmountP1 + smallBlind;
               players[0].stack = players[0].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP2 = totalBetAmountP2 + bigBlind;
               players[1].stack = players[1].stack - bigBlind;
               pot = pot + bigBlind;
               amountToCall = bigBlind - smallBlind;
            }

            while (not betMatched) {
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     p1Decision = getPlayerDecision(players[0]);
                  }
                  if (p1Decision == "f") {
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        break;
                     } else if (amountToCall == bigBlind - smallBlind) {
                        std::cout << "Player 1 limped" << std::endl << std::endl;
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        amountToCall = 0.0;
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        bool p1raiseInvalid = true;
                        while (p1raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p1raiseAbsolute;
                           std::cout << std::endl;
                           p1raise = p1raiseAbsolute - totalBetAmountP1;
                           if ((p1raiseAbsolute >= totalBetAmountP2 + amountToCall or p1raiseAbsolute == p2StartingStack) and p1raiseAbsolute <= p2StartingStack) {
                              if (p1raiseAbsolute == p2StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1raise;
                              std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1raise;
                              pot = pot + p1raise;
                              amountToCall = p1raise - amountToCall;
                              p1raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p1betAbsolute;
                        bool p1betInvalid = true;
                        while (p1betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p1betAbsolute;
                           std::cout << std::endl;
                           float p1bet = p1betAbsolute - totalBetAmountP1;
                           if (p1bet >= bigBlind and p1betAbsolute <= p2StartingStack) {
                              if (p1betAbsolute == p2StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1bet;
                              std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1bet;
                              pot = pot + p1bet;
                              amountToCall = p1bet;
                              p1betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << p2StartingStack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     p2Decision = getPlayerDecision(players[1]);
                  }
                  if (p2Decision == "f") {
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        bool p2raiseInvalid = true;
                        while (p2raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p2raiseAbsolute;
                           std::cout << std::endl;
                           p2raise = p2raiseAbsolute - totalBetAmountP2;
                           if ((p2raiseAbsolute >= totalBetAmountP1 + amountToCall or p2raiseAbsolute == p1StartingStack) and p2raiseAbsolute <= p1StartingStack) {
                              if (p2raiseAbsolute == p1StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2raise;
                              std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2raise;
                              pot = pot + p2raise;
                              amountToCall = p2raise - amountToCall;
                              p2raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p2betAbsolute;
                        bool p2betInvalid = true;
                        while (p2betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p2betAbsolute;
                           std::cout << std::endl;
                           float p2bet = p2betAbsolute - totalBetAmountP2;
                           if (p2bet >= bigBlind and p2betAbsolute <= p1StartingStack) {
                              if (p2betAbsolute == p1StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2bet;
                              std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2bet;
                              pot = pot + p2bet;
                              amountToCall = p2bet;
                              p2betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << p1StartingStack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         } else {
            if (players[0].stack <= smallBlind or players[1].stack <= smallBlind) {
               isAllIn = true;

               if (players[0].stack <= players[1].stack) {
                  remainingStack = players[0].stack;
               } else {
                  remainingStack = players[1].stack;
               }
               totalBetAmountP1 = totalBetAmountP1 + remainingStack;
               players[0].stack = players[0].stack - remainingStack;
               pot = pot + remainingStack;

               totalBetAmountP2 = totalBetAmountP2 + remainingStack;
               players[1].stack = players[1].stack - remainingStack;
               pot = pot + remainingStack;

               betMatched = true;
            } else if (players[0].stack <= bigBlind) {
               isAllIn = true;
               remainingStack = players[0].stack;

               totalBetAmountP2 = totalBetAmountP2 + smallBlind;
               players[1].stack = players[1].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP1 = totalBetAmountP1 + remainingStack;
               players[0].stack = players[0].stack - remainingStack;
               pot = pot + remainingStack;
               amountToCall = remainingStack - smallBlind;
            } else {
               totalBetAmountP2 = totalBetAmountP2 + smallBlind;
               players[1].stack = players[1].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP1 = totalBetAmountP1 + bigBlind;
               players[0].stack = players[0].stack - bigBlind;
               pot = pot + bigBlind;
               amountToCall = bigBlind - smallBlind;
            }

            while (not betMatched) {
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     p2Decision = getPlayerDecision(players[1]);
                  }
                  if (p2Decision == "f") {
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        break;
                     } else if (amountToCall == bigBlind - smallBlind) {
                        std::cout << "Player 2 limped" << std::endl << std::endl;
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        amountToCall = 0.0;
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        bool p2raiseInvalid = true;
                        while (p2raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p2raiseAbsolute;
                           std::cout << std::endl;
                           p2raise = p2raiseAbsolute - totalBetAmountP2;
                           if ((p2raiseAbsolute >= totalBetAmountP1 + amountToCall or p2raiseAbsolute == p1StartingStack) and p2raiseAbsolute <= p1StartingStack) {
                              if (p2raiseAbsolute == p1StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2raise;
                              std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2raise;
                              pot = pot + p2raise;
                              amountToCall = p2raise - amountToCall;
                              p2raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p2betAbsolute;
                        bool p2betInvalid = true;
                        while (p2betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p2betAbsolute;
                           std::cout << std::endl;
                           float p2bet = p2betAbsolute - totalBetAmountP2;
                           if (p2bet >= bigBlind and p2bet <= p1StartingStack) {
                              if (p2betAbsolute == p1StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2bet;
                              std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2bet;
                              pot = pot + p2bet;
                              amountToCall = p2bet;
                              p2betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << p1StartingStack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     p1Decision = getPlayerDecision(players[0]);
                  }
                  if (p1Decision == "f") {
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        bool p1raiseInvalid = true;
                        while (p1raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p1raiseAbsolute;
                           std::cout << std::endl;
                           p1raise = p1raiseAbsolute - totalBetAmountP1;
                           if ((p1raiseAbsolute >= totalBetAmountP2 + amountToCall or p1raiseAbsolute == p2StartingStack) and p1raiseAbsolute <= p2StartingStack) {
                              if (p1raiseAbsolute == p2StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1raise;
                              std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1raise;
                              pot = pot + p1raise;
                              amountToCall = p1raise - amountToCall;
                              p1raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p1betAbsolute;
                        bool p1betInvalid = true;
                        while (p1betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p1betAbsolute;
                           std::cout << std::endl;
                           float p1bet = p1betAbsolute - totalBetAmountP1;
                           if (p1bet >= bigBlind and p1bet <= p2StartingStack) {
                              if (p1betAbsolute == p2StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1bet;
                              std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1bet;
                              pot = pot + p1bet;
                              amountToCall = p1bet;
                              p1betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << p2StartingStack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         }
      }

      int startPostFlopBettingRound() {
         std::vector<std::string> playerHands = getAllHands();

         int checkWin; //If checkWin = player.name, then that player won. If checkWin = 0, then both players matched each other's bet. If checkWin = -1 then one or both players are all in.
         float amountToCall = 0.0;
         bool betMatched = false;
         float totalBetAmountP1 = 0.0;
         float totalBetAmountP2 = 0.0;
         float p1StartingStack = players[0].stack;
         float p2StartingStack = players[1].stack;
         bool isAllIn = false;

         if (not p1Button) {
            while (not betMatched) {
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     p1Decision = getPlayerDecision(players[0]);
                  }
                  if (p1Decision == "f") {
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        bool p1raiseInvalid = true;
                        while (p1raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p1raiseAbsolute;
                           std::cout << std::endl;
                           p1raise = p1raiseAbsolute - totalBetAmountP1;
                           if ((p1raiseAbsolute >= totalBetAmountP2 + amountToCall or p1raiseAbsolute == p2StartingStack) and p1raiseAbsolute <= p2StartingStack) {
                              if (p1raiseAbsolute == p2StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1raise;
                              std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1raise;
                              pot = pot + p1raise;
                              amountToCall = p1raise - amountToCall;
                              p1raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p1bet;
                        bool p1betInvalid = true;
                        while (p1betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p1bet;
                           std::cout << std::endl;
                           if (p1bet >= bigBlind and p1bet <= players[1].stack) {
                              if (p1bet == players[1].stack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1bet;
                              std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1bet;
                              pot = pot + p1bet;
                              amountToCall = p1bet;
                              p1betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << players[1].stack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     p2Decision = getPlayerDecision(players[1]);
                  }
                  if (p2Decision == "f") {
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        bool p2raiseInvalid = true;
                        while (p2raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p2raiseAbsolute;
                           std::cout << std::endl;
                           p2raise = p2raiseAbsolute - totalBetAmountP2;
                           if ((p2raiseAbsolute >= totalBetAmountP1 + amountToCall or p2raiseAbsolute == p1StartingStack) and p2raiseAbsolute <= p1StartingStack) {
                              if (p2raiseAbsolute == p1StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2raise;
                              std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2raise;
                              pot = pot + p2raise;
                              amountToCall = p2raise - amountToCall;
                              p2raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p2bet;
                        bool p2betInvalid = true;
                        while (p2betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p2bet;
                           std::cout << std::endl;
                           if (p2bet >= bigBlind and p2bet <= players[0].stack) {
                              if (p2bet == players[0].stack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2bet;
                              std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2bet;
                              pot = pot + p2bet;
                              amountToCall = p2bet;
                              p2betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << players[0].stack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         } else {
            while (not betMatched) {
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     p2Decision = getPlayerDecision(players[1]);
                  }
                  if (p2Decision == "f") {
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        bool p2raiseInvalid = true;
                        while (p2raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p2raiseAbsolute;
                           std::cout << std::endl;
                           p2raise = p2raiseAbsolute - totalBetAmountP2;
                           if ((p2raiseAbsolute >= totalBetAmountP1 + amountToCall or p2raiseAbsolute == p1StartingStack) and p2raiseAbsolute <= p1StartingStack) {
                              if (p2raiseAbsolute == p1StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2raise;
                              std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2raise;
                              pot = pot + p2raise;
                              amountToCall = p2raise - amountToCall;
                              p2raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p2bet;
                        bool p2betInvalid = true;
                        while (p2betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p2bet;
                           std::cout << std::endl;
                           if (p2bet >= bigBlind and p2bet <= players[0].stack) {
                              if (p2bet == players[0].stack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP2 = totalBetAmountP2 + p2bet;
                              std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                              players[1].stack = players[1].stack - p2bet;
                              pot = pot + p2bet;
                              amountToCall = p2bet;
                              p2betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << players[0].stack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     p1Decision = getPlayerDecision(players[0]);
                  }
                  if (p1Decision == "f") {
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        bool p1raiseInvalid = true;
                        while (p1raiseInvalid) {
                           std::cout << "Raise to: ";
                           std::cin >> p1raiseAbsolute;
                           std::cout << std::endl;
                           p1raise = p1raiseAbsolute - totalBetAmountP1;
                           if ((p1raiseAbsolute >= totalBetAmountP2 + amountToCall or p1raiseAbsolute == p2StartingStack) and p1raiseAbsolute <= p2StartingStack) {
                              if (p1raiseAbsolute == p2StartingStack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1raise;
                              std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1raise;
                              pot = pot + p1raise;
                              amountToCall = p1raise - amountToCall;
                              p1raiseInvalid = false;
                           } else {
                              std::cout << "Please enter a valid raise." << std::endl << std::endl;
                           }
                        }
                        break;
                     } else {
                        float p1bet;
                        bool p1betInvalid = true;
                        while (p1betInvalid) {
                           std::cout << "Bet amount: ";
                           std::cin >> p1bet;
                           std::cout << std::endl;
                           if (p1bet >= bigBlind and p1bet <= players[1].stack) {
                              if (p1bet == players[1].stack) {
                                 isAllIn = true;
                              }
                              totalBetAmountP1 = totalBetAmountP1 + p1bet;
                              std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                              players[0].stack = players[0].stack - p1bet;
                              pot = pot + p1bet;
                              amountToCall = p1bet;
                              p1betInvalid = false;
                           } else {
                              std::cout << "Please enter a bet that is between " << bigBlind << " and " << players[1].stack << "." << std::endl;
                           }
                        }  
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         }
      }

      int startFixedPreFlopBettingRound(const float* betSizes, const float* raiseSizes) {
         std::vector<std::string> playerHands = getAllHands();
         std::ostringstream oss;
         oss << "Player 1 has " << playerHands[0] << " " << playerHands[1] << std::endl << "Player 2 has " << playerHands[2] << " " << playerHands[3] << std::endl << "Player 1 stack is " << std::fixed << std::setprecision(2) << players[0].stack << std::endl << "Player 2 stack is " << std::fixed << std::setprecision(2) << players[1].stack << std::endl;
         std::string data = oss.str();
         std::ofstream outFile("gameLogs.txt", std::ios::app);
         if (!outFile) {
            std::cerr << "Error opening file for writing!" << std::endl;
         }
         outFile << data;
         outFile.close();

         int checkWin; //If checkWin = player.name, then that player won. If checkWin = 0, then both players matched each other's bet. If checkWin = -1 then one or both players are all in.
         float amountToCall = 0.0;
         bool betMatched = false;
         float totalBetAmountP1 = 0.0;
         float totalBetAmountP2 = 0.0;
         float p1StartingStack = players[0].stack;
         float p2StartingStack = players[1].stack;
         bool isAllIn = false;
         float remainingStack;

         if (p1Button) {
            if (players[0].stack <= smallBlind or players[1].stack <= smallBlind) {
               isAllIn = true;

               if (players[0].stack <= players[1].stack) {
                  remainingStack = players[0].stack;
               } else {
                  remainingStack = players[1].stack;
               }
               totalBetAmountP1 = totalBetAmountP1 + remainingStack;
               players[0].stack = players[0].stack - remainingStack;
               pot = pot + remainingStack;

               totalBetAmountP2 = totalBetAmountP2 + remainingStack;
               players[1].stack = players[1].stack - remainingStack;
               pot = pot + remainingStack;

               betMatched = true;
            } else if (players[1].stack <= bigBlind) {
               isAllIn = true;
               remainingStack = players[1].stack;

               totalBetAmountP1 = totalBetAmountP1 + smallBlind;
               players[0].stack = players[0].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP2 = totalBetAmountP2 + remainingStack;
               players[1].stack = players[1].stack - remainingStack;
               pot = pot + remainingStack;
               amountToCall = remainingStack - smallBlind;
            } else {
               totalBetAmountP1 = totalBetAmountP1 + smallBlind;
               players[0].stack = players[0].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP2 = totalBetAmountP2 + bigBlind;
               players[1].stack = players[1].stack - bigBlind;
               pot = pot + bigBlind;
               amountToCall = bigBlind - smallBlind;
            }
            if (players[0].stack + smallBlind < bigBlind) {
               isAllIn = true;
               amountToCall = players[0].stack;
            }

            while (not betMatched) {
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4)) and (pot != smallBlind + bigBlind)) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP2 * raiseSizes[0], totalBetAmountP2 * raiseSizes[1], totalBetAmountP2 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP2, p1StartingStack, p2StartingStack);
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     p1Decision = getFixedPlayerDecision(players[0], validBetSizes, validRaiseSizes, false);
                  }
                  if (p1Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 1 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        break;
                     } else if (amountToCall == bigBlind - smallBlind) {
                        std::cout << "Player 1 limped" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 limped" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        totalBetAmountP1 = totalBetAmountP2;
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        amountToCall = 0.0;
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 called " << std::fixed << std::setprecision(2) << totalBetAmountP2 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b1" and not isAllIn) {
                     float p1raise;
                     float p1raiseAbsolute;
                     p1raiseAbsolute = validRaiseSizes[0];
                     p1raise = p1raiseAbsolute - totalBetAmountP1;
                     totalBetAmountP1 = totalBetAmountP1 + p1raise;
                     std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[0].stack = players[0].stack - p1raise;
                     pot = pot + p1raise;
                     amountToCall = p1raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p1Decision == "b2" and not isAllIn) {
                     float p1raise;
                     float p1raiseAbsolute;
                     p1raiseAbsolute = validRaiseSizes[1];
                     p1raise = p1raiseAbsolute - totalBetAmountP1;
                     totalBetAmountP1 = totalBetAmountP1 + p1raise;
                     std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[0].stack = players[0].stack - p1raise;
                     pot = pot + p1raise;
                     amountToCall = p1raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p1Decision == "b3" and not isAllIn) {
                     float p1raise;
                     float p1raiseAbsolute;
                     p1raiseAbsolute = validRaiseSizes[2];
                     p1raise = p1raiseAbsolute - totalBetAmountP1;
                     totalBetAmountP1 = totalBetAmountP1 + p1raise;
                     std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[0].stack = players[0].stack - p1raise;
                     pot = pot + p1raise;
                     amountToCall = p1raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4)) and (pot != smallBlind + bigBlind)) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP1 * raiseSizes[0], totalBetAmountP1 * raiseSizes[1], totalBetAmountP1 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP1, p1StartingStack, p2StartingStack);
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     p2Decision = getFixedPlayerDecision(players[1], validBetSizes, validRaiseSizes, false);
                  }
                  if (p2Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 2 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 called " << std::fixed << std::setprecision(2) << totalBetAmountP1 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b1" and not isAllIn) {
                     float p2raise;
                     float p2raiseAbsolute;
                     p2raiseAbsolute = validRaiseSizes[0];
                     p2raise = p2raiseAbsolute - totalBetAmountP2;
                     totalBetAmountP2 = totalBetAmountP2 + p2raise;
                     std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[1].stack = players[1].stack - p2raise;
                     pot = pot + p2raise;
                     amountToCall = p2raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p2Decision == "b2" and not isAllIn) {
                     float p2raise;
                     float p2raiseAbsolute;
                     p2raiseAbsolute = validRaiseSizes[1];
                     p2raise = p2raiseAbsolute - totalBetAmountP2;
                     totalBetAmountP2 = totalBetAmountP2 + p2raise;
                     std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[1].stack = players[1].stack - p2raise;
                     pot = pot + p2raise;
                     amountToCall = p2raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p2Decision == "b3" and not isAllIn) {
                     float p2raise;
                     float p2raiseAbsolute;
                     p2raiseAbsolute = validRaiseSizes[2];
                     p2raise = p2raiseAbsolute - totalBetAmountP2;
                     totalBetAmountP2 = totalBetAmountP2 + p2raise;
                     std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[1].stack = players[1].stack - p2raise;
                     pot = pot + p2raise;
                     amountToCall = p2raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         } else {
            if (players[0].stack <= smallBlind or players[1].stack <= smallBlind) {
               isAllIn = true;

               if (players[0].stack <= players[1].stack) {
                  remainingStack = players[0].stack;
               } else {
                  remainingStack = players[1].stack;
               }
               totalBetAmountP1 = totalBetAmountP1 + remainingStack;
               players[0].stack = players[0].stack - remainingStack;
               pot = pot + remainingStack;

               totalBetAmountP2 = totalBetAmountP2 + remainingStack;
               players[1].stack = players[1].stack - remainingStack;
               pot = pot + remainingStack;

               betMatched = true;
            } else if (players[0].stack <= bigBlind) {
               isAllIn = true;
               remainingStack = players[0].stack;

               totalBetAmountP2 = totalBetAmountP2 + smallBlind;
               players[1].stack = players[1].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP1 = totalBetAmountP1 + remainingStack;
               players[0].stack = players[0].stack - remainingStack;
               pot = pot + remainingStack;
               amountToCall = remainingStack - smallBlind;
            } else {
               totalBetAmountP2 = totalBetAmountP2 + smallBlind;
               players[1].stack = players[1].stack - smallBlind;
               pot = pot + smallBlind;

               totalBetAmountP1 = totalBetAmountP1 + bigBlind;
               players[0].stack = players[0].stack - bigBlind;
               pot = pot + bigBlind;
               amountToCall = bigBlind - smallBlind;
            }
            if (players[1].stack + smallBlind < bigBlind) {
               isAllIn = true;
               amountToCall = players[1].stack;
            }

            while (not betMatched) {
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4)) and (pot != smallBlind + bigBlind)) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP1 * raiseSizes[0], totalBetAmountP1 * raiseSizes[1], totalBetAmountP1 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP1, p1StartingStack, p2StartingStack);
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     p2Decision = getFixedPlayerDecision(players[1], validBetSizes, validRaiseSizes, false);
                  }
                  if (p2Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 2 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        break;
                     } else if (amountToCall == bigBlind - smallBlind) {
                        std::cout << "Player 2 limped" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 limped" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        totalBetAmountP2 = totalBetAmountP1;
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        amountToCall = 0.0;
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 called " << std::fixed << std::setprecision(2) << totalBetAmountP1 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b1" and not isAllIn) {
                     float p2raise;
                     float p2raiseAbsolute;
                     p2raiseAbsolute = validRaiseSizes[0];
                     p2raise = p2raiseAbsolute - totalBetAmountP2;
                     totalBetAmountP2 = totalBetAmountP2 + p2raise;
                     std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[1].stack = players[1].stack - p2raise;
                     pot = pot + p2raise;
                     amountToCall = p2raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p2Decision == "b2" and not isAllIn) {
                     float p2raise;
                     float p2raiseAbsolute;
                     p2raiseAbsolute = validRaiseSizes[1];
                     p2raise = p2raiseAbsolute - totalBetAmountP2;
                     totalBetAmountP2 = totalBetAmountP2 + p2raise;
                     std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[1].stack = players[1].stack - p2raise;
                     pot = pot + p2raise;
                     amountToCall = p2raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p2Decision == "b3" and not isAllIn) {
                     float p2raise;
                     float p2raiseAbsolute;
                     p2raiseAbsolute = validRaiseSizes[2];
                     p2raise = p2raiseAbsolute - totalBetAmountP2;
                     totalBetAmountP2 = totalBetAmountP2 + p2raise;
                     std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[1].stack = players[1].stack - p2raise;
                     pot = pot + p2raise;
                     amountToCall = p2raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4)) and (pot != smallBlind + bigBlind)) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP2 * raiseSizes[0], totalBetAmountP2 * raiseSizes[1], totalBetAmountP2 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP2, p1StartingStack, p2StartingStack);
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     p1Decision = getFixedPlayerDecision(players[0], validBetSizes, validRaiseSizes, false);
                  }
                  if (p1Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 1 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 called " << std::fixed << std::setprecision(2) << totalBetAmountP2 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b1" and not isAllIn) {
                     float p1raise;
                     float p1raiseAbsolute;
                     p1raiseAbsolute = validRaiseSizes[0];
                     p1raise = p1raiseAbsolute - totalBetAmountP1;
                     totalBetAmountP1 = totalBetAmountP1 + p1raise;
                     std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[0].stack = players[0].stack - p1raise;
                     pot = pot + p1raise;
                     amountToCall = p1raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p1Decision == "b2" and not isAllIn) {
                     float p1raise;
                     float p1raiseAbsolute;
                     p1raiseAbsolute = validRaiseSizes[1];
                     p1raise = p1raiseAbsolute - totalBetAmountP1;
                     totalBetAmountP1 = totalBetAmountP1 + p1raise;
                     std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[0].stack = players[0].stack - p1raise;
                     pot = pot + p1raise;
                     amountToCall = p1raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else if (p1Decision == "b3" and not isAllIn) {
                     float p1raise;
                     float p1raiseAbsolute;
                     p1raiseAbsolute = validRaiseSizes[2];
                     p1raise = p1raiseAbsolute - totalBetAmountP1;
                     totalBetAmountP1 = totalBetAmountP1 + p1raise;
                     std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                     std::ostringstream oss;
                     oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     players[0].stack = players[0].stack - p1raise;
                     pot = pot + p1raise;
                     amountToCall = p1raise - amountToCall;
                     if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                        isAllIn = true;
                     }
                     break;
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         }
      }

      int startFixedPostFlopBettingRound(const float* betSizes, const float* raiseSizes) {
         std::vector<std::string> playerHands = getAllHands();
         std::ostringstream oss;
         oss << "The board is " << getBoard() << std::endl;
         std::string data = oss.str();
         std::ofstream outFile("gameLogs.txt", std::ios::app);
         if (!outFile) {
            std::cerr << "Error opening file for writing!" << std::endl;
            return 1;
         }
         outFile << data;
         outFile.close();

         int checkWin; //If checkWin = player.name, then that player won. If checkWin = 0, then both players matched each other's bet. If checkWin = -1 then one or both players are all in.
         float amountToCall = 0.0;
         bool betMatched = false;
         float totalBetAmountP1 = 0.0;
         float totalBetAmountP2 = 0.0;
         float p1StartingStack = players[0].stack;
         float p2StartingStack = players[1].stack;
         bool isAllIn = false;

         if (not p1Button) {
            while (not betMatched) {
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4))) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP2 * raiseSizes[0], totalBetAmountP2 * raiseSizes[1], totalBetAmountP2 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP2, p1StartingStack, p2StartingStack);
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     if (amountToCall > 0) {
                        p1Decision = getFixedPlayerDecision(players[0], validBetSizes, validRaiseSizes, false);
                     } else {
                        p1Decision = getFixedPlayerDecision(players[0], validBetSizes, validRaiseSizes, true);
                     }
                  }
                  if (p1Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 1 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 called " << std::fixed << std::setprecision(2) << totalBetAmountP2 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b1" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        p1raiseAbsolute = validRaiseSizes[0];
                        p1raise = p1raiseAbsolute - totalBetAmountP1;
                        totalBetAmountP1 = totalBetAmountP1 + p1raise;
                        std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1raise;
                        pot = pot + p1raise;
                        amountToCall = p1raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p1bet;
                        p1bet = validBetSizes[0];
                        totalBetAmountP1 = totalBetAmountP1 + p1bet;
                        std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 bet " << std::fixed << std::setprecision(2) << p1bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1bet;
                        pot = pot + p1bet;
                        amountToCall = p1bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p1Decision == "b2" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        p1raiseAbsolute = validRaiseSizes[1];
                        p1raise = p1raiseAbsolute - totalBetAmountP1;
                        totalBetAmountP1 = totalBetAmountP1 + p1raise;
                        std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 rasied to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1raise;
                        pot = pot + p1raise;
                        amountToCall = p1raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p1bet;
                        p1bet = validBetSizes[1];
                        totalBetAmountP1 = totalBetAmountP1 + p1bet;
                        std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 bet " << std::fixed << std::setprecision(2) << p1bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1bet;
                        pot = pot + p1bet;
                        amountToCall = p1bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p1Decision == "b3" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        p1raiseAbsolute = validRaiseSizes[2];
                        p1raise = p1raiseAbsolute - totalBetAmountP1;
                        totalBetAmountP1 = totalBetAmountP1 + p1raise;
                        std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1raise;
                        pot = pot + p1raise;
                        amountToCall = p1raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p1bet;
                        p1bet = validBetSizes[2];
                        totalBetAmountP1 = totalBetAmountP1 + p1bet;
                        std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 bet " << std::fixed << std::setprecision(2) << p1bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1bet;
                        pot = pot + p1bet;
                        amountToCall = p1bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4))) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP1 * raiseSizes[0], totalBetAmountP1 * raiseSizes[1], totalBetAmountP1 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP1, p1StartingStack, p2StartingStack);
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     if (amountToCall > 0) {
                        p2Decision = getFixedPlayerDecision(players[1], validBetSizes, validRaiseSizes, false);
                     } else {
                        p2Decision = getFixedPlayerDecision(players[1], validBetSizes, validRaiseSizes, true);
                     }
                  }
                  if (p2Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 2 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 called " << std::fixed << std::setprecision(2) << totalBetAmountP1 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b1" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        p2raiseAbsolute = validRaiseSizes[0];
                        p2raise = p2raiseAbsolute - totalBetAmountP2;
                        totalBetAmountP2 = totalBetAmountP2 + p2raise;
                        std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2raise;
                        pot = pot + p2raise;
                        amountToCall = p2raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p2bet;
                        p2bet = validBetSizes[0];
                        totalBetAmountP2 = totalBetAmountP2 + p2bet;
                        std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 bet " << std::fixed << std::setprecision(2) << p2bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2bet;
                        pot = pot + p2bet;
                        amountToCall = p2bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p2Decision == "b2" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        p2raiseAbsolute = validRaiseSizes[1];
                        p2raise = p2raiseAbsolute - totalBetAmountP2;
                        totalBetAmountP2 = totalBetAmountP2 + p2raise;
                        std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2raise;
                        pot = pot + p2raise;
                        amountToCall = p2raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p2bet;
                        p2bet = validBetSizes[1];
                        totalBetAmountP2 = totalBetAmountP2 + p2bet;
                        std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 bet " << std::fixed << std::setprecision(2) << p2bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2bet;
                        pot = pot + p2bet;
                        amountToCall = p2bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p2Decision == "b3" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        p2raiseAbsolute = validRaiseSizes[2];
                        p2raise = p2raiseAbsolute - totalBetAmountP2;
                        totalBetAmountP2 = totalBetAmountP2 + p2raise;
                        std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2raise;
                        pot = pot + p2raise;
                        amountToCall = p2raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p2bet;
                        p2bet = validBetSizes[2];
                        totalBetAmountP2 = totalBetAmountP2 + p2bet;
                        std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 bet " << std::fixed << std::setprecision(2) << p2bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2bet;
                        pot = pot + p2bet;
                        amountToCall = p2bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         } else {
            while (not betMatched) {
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4))) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 2's Hand:  " << playerHands[2] << " " << playerHands[3] << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP1 * raiseSizes[0], totalBetAmountP1 * raiseSizes[1], totalBetAmountP1 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP1, p1StartingStack, p2StartingStack);
                  std::string p2Decision;
                  if (isAllIn) {
                     p2Decision = getAllInPlayerDecision(players[1]);
                  } else {
                     if (amountToCall > 0) {
                        p2Decision = getFixedPlayerDecision(players[1], validBetSizes, validRaiseSizes, false);
                     } else {
                        p2Decision = getFixedPlayerDecision(players[1], validBetSizes, validRaiseSizes, true);
                     }
                  }
                  if (p2Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 2 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[0].name;
                     return checkWin;
                  } else if (p2Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 2 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        break;
                     } else {
                        std::cout << "Player 2 called " << totalBetAmountP1 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 called " << std::fixed << std::setprecision(2) << totalBetAmountP1 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p2Decision == "b1" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        p2raiseAbsolute = validRaiseSizes[0];
                        p2raise = p2raiseAbsolute - totalBetAmountP2;
                        totalBetAmountP2 = totalBetAmountP2 + p2raise;
                        std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2raise;
                        pot = pot + p2raise;
                        amountToCall = p2raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p2bet;
                        p2bet = validBetSizes[0];
                        totalBetAmountP2 = totalBetAmountP2 + p2bet;
                        std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 bet " << std::fixed << std::setprecision(2) << p2bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2bet;
                        pot = pot + p2bet;
                        amountToCall = p2bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p2Decision == "b2" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        p2raiseAbsolute = validRaiseSizes[1];
                        p2raise = p2raiseAbsolute - totalBetAmountP2;
                        totalBetAmountP2 = totalBetAmountP2 + p2raise;
                        std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2raise;
                        pot = pot + p2raise;
                        amountToCall = p2raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p2bet;
                        p2bet = validBetSizes[1];
                        totalBetAmountP2 = totalBetAmountP2 + p2bet;
                        std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 bet " << std::fixed << std::setprecision(2) << p2bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2bet;
                        pot = pot + p2bet;
                        amountToCall = p2bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p2Decision == "b3" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p2raise;
                        float p2raiseAbsolute;
                        p2raiseAbsolute = validRaiseSizes[2];
                        p2raise = p2raiseAbsolute - totalBetAmountP2;
                        totalBetAmountP2 = totalBetAmountP2 + p2raise;
                        std::cout << "Player 2 raised to " << p2raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 raised to " << std::fixed << std::setprecision(2) << p2raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2raise;
                        pot = pot + p2raise;
                        amountToCall = p2raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p2bet;
                        p2bet = validBetSizes[2];
                        totalBetAmountP2 = totalBetAmountP2 + p2bet;
                        std::cout << "Player 2 bet " << p2bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 2 bet " << std::fixed << std::setprecision(2) << p2bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[1].stack = players[1].stack - p2bet;
                        pot = pot + p2bet;
                        amountToCall = p2bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
               if (betMatched) {
                  break;
               }
               while (true) {
                  if (((players[0].stack - amountToCall <= 1e-4) and (players[0].stack - amountToCall >= -1e-4)) or ((players[1].stack - amountToCall <= 1e-4) and (players[1].stack - amountToCall >= -1e-4))) {
                     isAllIn = true;
                  }
                  std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
                  showBoard();
                  std::cout << std::endl << "Player 1's Hand: " << playerHands[0] << " " << playerHands[1] << std::endl << std::endl;
                  std::cout << "Pot: " << pot << std::endl;
                  std::cout << "Amount to call: " << amountToCall << std::endl;
                  std::cout << std::endl;
                  showStacks();
                  std::cout << std::endl;
                  std::vector<float> validBetSizes = {pot * betSizes[0], pot * betSizes[1], pot * betSizes[2]};
                  std::vector<float> validRaiseSizes = {totalBetAmountP2 * raiseSizes[0], totalBetAmountP2 * raiseSizes[1], totalBetAmountP2 * raiseSizes[2]};
                  validBetSizes = getValidFixedSizes(validBetSizes, bigBlind, p1StartingStack, p2StartingStack);
                  validRaiseSizes = getValidFixedSizes(validRaiseSizes, amountToCall + totalBetAmountP2, p1StartingStack, p2StartingStack);
                  std::string p1Decision;
                  if (isAllIn) {
                     p1Decision = getAllInPlayerDecision(players[0]);
                  } else {
                     if (amountToCall > 0) {
                        p1Decision = getFixedPlayerDecision(players[0], validBetSizes, validRaiseSizes, false);
                     } else {
                        p1Decision = getFixedPlayerDecision(players[0], validBetSizes, validRaiseSizes, true);
                     }
                  }
                  if (p1Decision == "f") {
                     std::ostringstream oss;
                     oss << "Player 1 folded" << std::endl;
                     std::string data = oss.str();
                     std::ofstream outFile("gameLogs.txt", std::ios::app);
                     if (!outFile) {
                        std::cerr << "Error opening file for writing!" << std::endl;
                     }
                     outFile << data;
                     outFile.close();
                     checkWin = players[1].name;
                     return checkWin;
                  } else if (p1Decision == "c") {
                     if (amountToCall == 0.0) {
                        std::cout << "Player 1 checked" << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 checked" << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        betMatched = true;
                        break;
                     } else {
                        std::cout << "Player 1 called " << totalBetAmountP2 << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 called " << std::fixed << std::setprecision(2) << totalBetAmountP2 << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - amountToCall;
                        pot = pot + amountToCall;
                        betMatched = true;
                        break;
                     }
                  } else if (p1Decision == "b1" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        p1raiseAbsolute = validRaiseSizes[0];
                        p1raise = p1raiseAbsolute - totalBetAmountP1;
                        totalBetAmountP1 = totalBetAmountP1 + p1raise;
                        std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1raise;
                        pot = pot + p1raise;
                        amountToCall = p1raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p1bet;
                        p1bet = validBetSizes[0];
                        totalBetAmountP1 = totalBetAmountP1 + p1bet;
                        std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 bet " << std::fixed << std::setprecision(2) << p1bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1bet;
                        pot = pot + p1bet;
                        amountToCall = p1bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p1Decision == "b2" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        p1raiseAbsolute = validRaiseSizes[1];
                        p1raise = p1raiseAbsolute - totalBetAmountP1;
                        totalBetAmountP1 = totalBetAmountP1 + p1raise;
                        std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1raise;
                        pot = pot + p1raise;
                        amountToCall = p1raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p1bet;
                        p1bet = validBetSizes[1];
                        totalBetAmountP1 = totalBetAmountP1 + p1bet;
                        std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 bet " << std::fixed << std::setprecision(2) << p1bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1bet;
                        pot = pot + p1bet;
                        amountToCall = p1bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else if (p1Decision == "b3" and not isAllIn) {
                     if (amountToCall > 0) {
                        float p1raise;
                        float p1raiseAbsolute;
                        p1raiseAbsolute = validRaiseSizes[2];
                        p1raise = p1raiseAbsolute - totalBetAmountP1;
                        totalBetAmountP1 = totalBetAmountP1 + p1raise;
                        std::cout << "Player 1 raised to " << p1raiseAbsolute << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 raised to " << std::fixed << std::setprecision(2) << p1raiseAbsolute << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1raise;
                        pot = pot + p1raise;
                        amountToCall = p1raise - amountToCall;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     } else {
                        float p1bet;
                        p1bet = validBetSizes[2];
                        totalBetAmountP1 = totalBetAmountP1 + p1bet;
                        std::cout << "Player 1 bet " << p1bet << std::endl << std::endl;
                        std::ostringstream oss;
                        oss << "Player 1 bet " << std::fixed << std::setprecision(2) << p1bet << std::endl;
                        std::string data = oss.str();
                        std::ofstream outFile("gameLogs.txt", std::ios::app);
                        if (!outFile) {
                           std::cerr << "Error opening file for writing!" << std::endl;
                        }
                        outFile << data;
                        outFile.close();
                        players[0].stack = players[0].stack - p1bet;
                        pot = pot + p1bet;
                        amountToCall = p1bet;
                        if (((players[0].stack <= 1e-4) and (players[0].stack >= -1e-4)) or ((players[1].stack <= 1e-4) and (players[1].stack >= -1e-4))) {
                           isAllIn = true;
                        }
                        break;
                     }
                  } else {
                     std::cout << "Please enter a valid decision." << std::endl << std::endl;
                  }
               }
            }
            if (isAllIn) {
               return -1;
            } else {
               return 0;
            }
         }
      }

      void declareWinner(int checkWin, float winProb, bool isPostFlop) {
         std::cout << "------------------------" << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
         if (checkWin == 1) {
            if (players[0].check72Rule() and isPostFlop and play72Rule) {
               float bounty = bounty72Rule * bigBlind;
               if (bounty >= players[1].stack + 1e-4) {
                  bounty = players[1].stack;
               }
               std::cout << "Player 1 wins the hand with 72o! Player 1 collects a bounty of " << bounty << " from Player 2!" << std::endl << std::endl;
               players[0].stack = players[0].stack + pot + bounty;
               players[1].stack = players[1].stack - bounty;
               pot = 0.0;
               showStacks();
               std::cout << std::endl;
            } else {
               std::cout << "Player 1 wins the hand!" << std::endl << std::endl;
               std::ostringstream oss;
               oss << "Player 1 wins the hand" << std::endl << std::endl;
               std::string data = oss.str();
               std::ofstream outFile("gameLogs.txt", std::ios::app);
               if (!outFile) {
                  std::cerr << "Error opening file for writing!" << std::endl;
               }
               outFile << data;
               outFile.close();
               players[0].stack = players[0].stack + pot;
               pot = 0.0;
               showStacks();
               std::cout << std::endl;
            }
         } else if (checkWin == 2) {
            if (players[1].check72Rule() and isPostFlop and play72Rule) {
               float bounty = bounty72Rule * bigBlind;
               if (bounty >= players[0].stack + 1e-4) {
                  bounty = players[0].stack;
               }
               std::cout << "Player 2 wins the hand with 72o! Player 2 collects a bounty of " << bounty << " from Player 1!" << std::endl << std::endl;
               players[1].stack = players[1].stack + pot + bounty;
               players[0].stack = players[0].stack - bounty;
               pot = 0.0;
               showStacks();
               std::cout << std::endl;
            } else {
               std::cout << "Player 2 wins the hand!" << std::endl << std::endl;
               std::ostringstream oss;
               oss << "Player 2 wins the hand" << std::endl << std::endl;
               std::string data = oss.str();
               std::ofstream outFile("gameLogs.txt", std::ios::app);
               if (!outFile) {
                  std::cerr << "Error opening file for writing!" << std::endl;
               }
               outFile << data;
               outFile.close();
               players[1].stack = players[1].stack + pot;
               pot = 0.0;
               showStacks();
               std::cout << std::endl;
            }
         } else {
            if (winProb == 1.0) {
               if (players[0].check72Rule() and isPostFlop and play72Rule) {
                  float bounty = bounty72Rule * bigBlind;
                  if (bounty >= players[1].stack + 1e-4) {
                     bounty = players[1].stack;
                  }
                  std::cout << "Player 1 wins the hand with 72o! Player 1 collects a bounty of " << bounty << " from Player 2!" << std::endl << std::endl;
                  players[0].stack = players[0].stack + pot + bounty;
                  players[1].stack = players[1].stack - bounty;
                  pot = 0.0;
                  showStacks();
                  std::cout << std::endl;
               } else {
                  std::cout << "Player 1 wins the hand!" << std::endl << std::endl;
                  std::ostringstream oss;
                  oss << "Player 1 wins the hand" << std::endl << std::endl;
                  std::string data = oss.str();
                  std::ofstream outFile("gameLogs.txt", std::ios::app);
                  if (!outFile) {
                     std::cerr << "Error opening file for writing!" << std::endl;
                  }
                  outFile << data;
                  outFile.close();
                  players[0].stack = players[0].stack + pot;
                  pot = 0.0;
                  showStacks();
                  std::cout << std::endl;
               }
            } else if (winProb == 0.0) {
               if (players[1].check72Rule() and isPostFlop and play72Rule) {
                  float bounty = bounty72Rule * bigBlind;
                  if (bounty >= players[0].stack + 1e-4) {
                     bounty = players[0].stack;
                  }
                  std::cout << "Player 2 wins the hand with 72o! Player 2 collects a bounty of " << bounty << " from Player 1!" << std::endl << std::endl;
                  players[1].stack = players[1].stack + pot + bounty;
                  players[0].stack = players[0].stack - bounty;
                  pot = 0.0;
                  showStacks();
                  std::cout << std::endl;
               } else {
                  std::cout << "Player 2 wins the hand!" << std::endl << std::endl;
                  std::ostringstream oss;
                  oss << "Player 2 wins the hand" << std::endl << std::endl;
                  std::string data = oss.str();
                  std::ofstream outFile("gameLogs.txt", std::ios::app);
                  if (!outFile) {
                     std::cerr << "Error opening file for writing!" << std::endl;
                  }
                  outFile << data;
                  outFile.close();
                  players[1].stack = players[1].stack + pot;
                  pot = 0.0;
                  showStacks();
                  std::cout << std::endl;
               }
            } else {
               std::cout << "Players chop!" << std::endl << std::endl;
               std::ostringstream oss;
               oss << "Players chop" << std::endl << std::endl;
               std::string data = oss.str();
               std::ofstream outFile("gameLogs.txt", std::ios::app);
               if (!outFile) {
                  std::cerr << "Error opening file for writing!" << std::endl;
               }
               outFile << data;
               outFile.close();
               players[0].stack = players[0].stack + pot * 0.5;
               players[1].stack = players[1].stack + pot * 0.5;
               pot = 0.0;
               showStacks();
               std::cout << std::endl;
            }
         }
      }

      void dealFlop() {
         board.push_back(deck.dealCard());
         board.push_back(deck.dealCard());
         board.push_back(deck.dealCard());
         for (int i=0; i<numPlayers; i++) {
            players[i].fullHand.push_back(board[0]);
            players[i].fullHand.push_back(board[1]);
            players[i].fullHand.push_back(board[2]);
         }
      }

      void dealFlop(std::vector<std::string> customFlop) {
         if (customFlop.empty()) {
            dealFlop();
         } else {
            board.push_back(getInt(customFlop[0]));
            board.push_back(getInt(customFlop[1]));
            board.push_back(getInt(customFlop[2]));
            for (int i=0; i<numPlayers; i++) {
               players[i].fullHand.push_back(board[0]);
               players[i].fullHand.push_back(board[1]);
               players[i].fullHand.push_back(board[2]);
            }
         }
      }

      void dealFlop(std::vector<int> customFlop) {
         if (customFlop.empty()) {
            dealFlop();
         } else {
            board.push_back(customFlop[0]);
            board.push_back(customFlop[1]);
            board.push_back(customFlop[2]);
            for (int i=0; i<numPlayers; i++) {
               players[i].fullHand.push_back(board[0]);
               players[i].fullHand.push_back(board[1]);
               players[i].fullHand.push_back(board[2]);
            }
         }
      }

      void dealTurnOrRiver() {
         int newCard = deck.dealCard();
         board.push_back(newCard);
         for (int i=0; i<numPlayers; i++) {
            players[i].fullHand.push_back(newCard);
         }
      }

      void dealTurnOrRiver(std::string customCard) {
         if (customCard.empty()) {
            dealTurnOrRiver();
         } else {
            int newCard = getInt(customCard);
            board.push_back(newCard);
            for (int i=0; i<numPlayers; i++) {
               players[i].fullHand.push_back(newCard);
            }
         }
      }

      void dealTurnOrRiver(int customCard) {
         int newCard = customCard;
         board.push_back(newCard);
         for (int i=0; i<numPlayers; i++) {
            players[i].fullHand.push_back(newCard);
         }
      }

      std::vector<std::vector<int>> getAllPossibleFlops() {
         std::vector<std::vector<int>> allFlops;
         std::vector<int> cardVec;

         Deck newDeck(deadCards);
         deck = newDeck;

         int numCards = deck.getNumCards();
         while (deck.getNumCards() > 0) {
            cardVec.push_back(deck.dealCard());
         }

         Deck newDeck2(deadCards);
         deck = newDeck2;
         //deck.shuffle();

         for (int i=0; i<numCards-2; ++i) {
            for (int j=i+1; j<numCards-1; ++j) {
               for (int k=j+1; k<numCards; ++k) {
                  std::vector<int> flop = {cardVec[i], cardVec[j], cardVec[k]};
                  allFlops.push_back(flop);
               }
            }
         }
         return allFlops;
      }

      std::vector<int> getAllPossibleTurnsOrRivers(std::vector<int> boardCards) {
         std::vector<int> allDeadCards;
         for (int i=0; i<numPlayers*2; i++) {
            allDeadCards.push_back(deadCards[i]);
         }
         int numBoardCards = boardCards.size();
         for (int i=0; i<numBoardCards; i++) {
            allDeadCards.push_back(boardCards[i]);
         }

         int numDeadCards = allDeadCards.size();
         std::vector<int> allTurnsOrRivers;

         for (int i=51; i>=0; i--) {
            bool isDeadCard = false;
            for (int j=0; j<numDeadCards; j++) {
               if (allDeadCards[j] == i) {
                  isDeadCard = true;
               }
            }
            if (!isDeadCard) {
               allTurnsOrRivers.push_back(i);
            }
         }

         return allTurnsOrRivers;
      }

      void showBoard() {
         int size = board.size();
         if (size > 0) {
            std::cout << "Board: ";
            for (int i=0; i<size; i++) {
               std::cout << getCard(board[i]) << " ";
            }
            std::cout << std::endl;
         }
      }

      std::string getBoard() {
         int size = board.size();
         std::string currentBoard;
         if (size > 0) {
            for (int i=0; i<size; i++) {
               currentBoard = currentBoard + getCard(board[i]) + " ";
            }
         }
         return currentBoard;
      }

      void resetEverythingPostflop() {
         board.clear();
         for (int i=0; i<numPlayers; i++) {
            players[i].resetHand();
            players[i].takeCard(deadCards[2*i]);
            players[i].takeCard(deadCards[2*i + 1]);
         }
         Deck newDeck(deadCards);
         deck = newDeck;
      }

      void resetEverything() {
         board.clear();
         for (int i=0; i<numPlayers; i++) {
            players[i].resetHand();
         }
         Deck newDeck;
         deck = newDeck;
         p1Button = not p1Button;
      }

      std::vector<float> rankHands() {
         std::vector<float> playerWins;
         for (int i=0; i<numPlayers; i++) {
            playerWins.push_back(0);
         }
         std::vector<int> ranks;
         int numPlayers = players.size();
         for (int i=0; i<numPlayers; i++) {
            ranks.push_back(players[i].rankPlayerHand());
         }

         int bestHand = 10000;
         std::vector<int> winners;
         for (int i=0; i<numPlayers; i++) {
            if (ranks[i] < bestHand) {
               bestHand = ranks[i];
               winners.clear();
               winners.push_back(i);
            } else if (ranks[i] == bestHand) {
               winners.push_back(i);
            }
         }
         float size = (float)winners.size();
         for (int i=0; i<size; i++) {
            playerWins[winners[i]] += 1 / size;
         }
         return playerWins;
      }
};

class Node {
   public:
      std::vector<int> cards;
      float winProb;
      std::vector<Node*> children;

      Node(std::vector<int> inputCards) : cards(inputCards) {}

      void addChild(Node* child) {
         children.push_back(child);
      }
};

class RandomGenerator {
   private:
      std::random_device rd;
      std::mt19937 gen;
      std::uniform_real_distribution<float> dis;

   public:
      RandomGenerator() : gen(rd()), dis(0.0, 1.0) {}
      float rnum() {return dis(gen);}
      void reseed() {gen.seed(rd());}
};

class Range {
   private:
      static RandomGenerator rng;

      char values[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
      char suits[4] = {'c', 'd', 'h', 's'};

      Dealer dealer;

      int totalCombos = 1326;
      int triangleNumbers[12] = {0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66};

      float pairs[78] = {0.0};  //2D array of all pairs stored in row major ordering
      //value orders: 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A
      //suit orders: cd, ch, cs, dh, ds, hs
      float nonpairs[1248] = {0.0};  //3D triangular wedge shaped array of all nonpaired hands stored in row major ordering
      //value orders: 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A
      //suit orders: cc, dd, hh, ss, cd, ch, cs, dc, dh, ds, hc, hd, hs, sc, sd, sh

   public:
      Range() {}

      Range(std::string strand) {add(strand, 1.0);}

      Range operator-(const Range& other) const {
         Range newRange = Range();
         for (int i=0; i<78; i++) {
            if (pairs[i] < other.pairs[i]) {
               newRange.pairs[i] = 0.0;
            } else {
               newRange.pairs[i] = pairs[i] - other.pairs[i];
            }
         }
         for (int i=0; i<1248; i++) {
            if (nonpairs[i] < other.nonpairs[i]) {
               newRange.nonpairs[i] = 0.0;
            } else {
               newRange.nonpairs[i] = nonpairs[i] - other.nonpairs[i];
            }
         }
         return newRange;
      }

      Range operator+(const Range& other) const {
         Range newRange = Range();
         for (int i=0; i<78; i++) {
            if (pairs[i] + other.pairs[i] > 1) {
               newRange.pairs[i] = 1.0;
            } else {
               newRange.pairs[i] = pairs[i] + other.pairs[i];
            }
         }
         for (int i=0; i<1248; i++) {
            if (nonpairs[i] + other.nonpairs[i] > 1) {
               newRange.nonpairs[i] = 1.0;
            } else {
               newRange.nonpairs[i] = nonpairs[i] + other.nonpairs[i];
            }
         }
         return newRange;
      }

      /*Range operator%(const Range& other) const {

      }*/

      /*std::vector<int> getRandomHand() {
         rng.reseed();
         float r = rng.rnum();
      }*/

      void add(int card1, int card2, float weight) {
         int suitIndex;
         if (checkPair(card1, card2)) {
            suitIndex = getPairSuitIndex(card1, card2);
            int valueIndex = card1 / 4;
            pairs[suitIndex * 13 + valueIndex] = weight;
         } else {
            suitIndex = getNonpairSuitIndex(card1, card2);
            int valueIndex1 = std::max(card1 / 4, card2 / 4);
            int valueIndex2 = std::min(card1 / 4, card2 / 4);
            nonpairs[suitIndex * 78 + triangleNumbers[valueIndex1-1] + valueIndex2] = weight;
         }
      }

      void remove(int card1, int card2) {
         int suitIndex;
         if (checkPair(card1, card2)) {
            suitIndex = getPairSuitIndex(card1, card2);
            int valueIndex = card1 / 4;
            pairs[suitIndex * 13 + valueIndex] = 0.0;
         } else {
            suitIndex = getNonpairSuitIndex(card1, card2);
            int valueIndex1 = std::max(card1 / 4, card2 / 4);
            int valueIndex2 = std::min(card1 / 4, card2 / 4);
            nonpairs[suitIndex * 78 + triangleNumbers[valueIndex1-1] + valueIndex2] = 0.0;
         }
      }

      float getWeight(int card1, int card2) {
         int suitIndex;
         if (checkPair(card1, card2)) {
            suitIndex = getPairSuitIndex(card1, card2);
            int valueIndex = card1 / 4;
            return pairs[suitIndex * 13 + valueIndex];
         }
         suitIndex = getNonpairSuitIndex(card1, card2);
         int valueIndex1 = std::max(card1 / 4, card2 / 4);
         int valueIndex2 = std::min(card1 / 4, card2 / 4);
         return nonpairs[suitIndex * 78 + triangleNumbers[valueIndex1-1] + valueIndex2];
      }

      bool isInRange(int card1, int card2) {
         int suitIndex;
         if (checkPair(card1, card2)) {
            suitIndex = getPairSuitIndex(card1, card2);
            int valueIndex = card1 / 4;
            if (pairs[suitIndex * 13 + valueIndex] > 0.0) {return true;}
            return false;
         }
         suitIndex = getNonpairSuitIndex(card1, card2);
         int valueIndex1 = std::max(card1 / 4, card2 / 4);
         int valueIndex2 = std::min(card1 / 4, card2 / 4);
         if (nonpairs[suitIndex * 78 + triangleNumbers[valueIndex1-1] + valueIndex2] > 0.0) {return true;}
         return false;
      }

      int countCombos() {
         int numCombos = 0;
         for (int i=0; i<78; i++) {
            if (pairs[i] > 0.0) {
               numCombos += 1;
            }
         }
         for (int i=0; i<1248; i++) {
            if (nonpairs[i] > 0.0) {
               numCombos += 1;
            }
         }
         return numCombos;
      }

      bool checkPair(int card1, int card2) {
         if (card1 / 4 == card2 / 4) {return true;}
         return false;
      }

      int getPairSuitIndex(int card1, int card2) {
         int suit1 = card1 % 4;
         int suit2 = card2 % 4;

         if ((suit1 == 0 or suit1 == 1) and (suit2 == 0 or suit2 == 1)) {return 0;}
         if ((suit1 == 0 or suit1 == 2) and (suit2 == 0 or suit2 == 2)) {return 1;}
         if ((suit1 == 0 or suit1 == 3) and (suit2 == 0 or suit2 == 3)) {return 2;}
         if ((suit1 == 1 or suit1 == 2) and (suit2 == 1 or suit2 == 2)) {return 3;}
         if ((suit1 == 1 or suit1 == 3) and (suit2 == 1 or suit2 == 3)) {return 4;}
         if ((suit1 == 2 or suit1 == 3) and (suit2 == 2 or suit2 == 3)) {return 5;}

         std::cout << "Cannot find suit index for paired hand" << std::endl;
         return 6;
      }

      int getNonpairSuitIndex(int card1, int card2) {
         int suit1 = card1 % 4;
         int suit2 = card2 % 4;

         if (suit1 == suit2) {return suit1;}

         if (suit1 == 0) {
            if (suit2 == 1) {return 4;}
            if (suit2 == 2) {return 5;}
            if (suit2 == 3) {return 6;}
         }
         if (suit1 == 1) {
            if (suit2 == 0) {return 7;}
            if (suit2 == 2) {return 8;}
            if (suit2 == 3) {return 9;}
         }
         if (suit1 == 2) {
            if (suit2 == 0) {return 10;}
            if (suit2 == 1) {return 11;}
            if (suit2 == 3) {return 12;}
         }
         if (suit1 == 3) {
            if (suit2 == 0) {return 13;}
            if (suit2 == 1) {return 14;}
            if (suit2 == 2) {return 15;}
         }
         std::cout << "Cannot find suit index for nonpaired hand" << std::endl;
         return 16;
      }

      void add(std::string hands, float weight) {
         int value1;
         int value2;
         int value3;
         int value4;
         int valueIndex1;
         int valueIndex2;
         int valueIndex3;
         int valueIndex4;

         hands.erase(std::remove(hands.begin(), hands.end(), ' '), hands.end());

         std::vector<std::string> tokens;
         std::istringstream ss(hands);
         std::string token;

         while (getline(ss, token, ',')) {
            tokens.push_back(token);
         }

         int numTokens = tokens.size();

         for (int iter=0; iter<numTokens; iter++) {
            int length = tokens[iter].length();
            if (length < 2) {
               std::cout << "Please enter a valid hand to add." << std::endl;
            } else if (length == 2) {
               for (int i=0; i<13; i++) {
                  if (tokens[iter][0] == values[i]) {value1 = i;}
                  if (tokens[iter][1] == values[i]) {value2 = i;}
               }
               if (value1 == value2) {
                  for (int i=0; i<6; i++) {
                     pairs[i * 13 + value1] = weight;
                  }
               } else {
                  for (int i=0; i<16; i++) {
                     valueIndex1 = std::max(value1, value2);
                     valueIndex2 = std::min(value1, value2);
                     nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + valueIndex2] = weight;
                  }
               }
            } else if (length == 3) {
               for (int i=0; i<13; i++) {
                  if (tokens[iter][0] == values[i]) {value1 = i;}
                  if (tokens[iter][1] == values[i]) {value2 = i;}
               }
               valueIndex1 = std::max(value1, value2);
               valueIndex2 = std::min(value1, value2);
               if (value1 == value2 and tokens[iter][2] == '+') {
                  for (int j=value1; j<13; j++) {
                     for (int i=0; i<6; i++) {
                        pairs[i * 13 + j] = weight;
                     }
                  }
               } else if (value1 == value2) {
                  std::cout << "Please enter a valid hand to add." << std::endl;
               } else if (tokens[iter][2] == 's') {
                  for (int i=0; i<4; i++) {
                     nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + valueIndex2] = weight;
                  }
               } else if (tokens[iter][2] == 'o') {
                  for (int i=4; i<16; i++) {
                     nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + valueIndex2] = weight;
                  }
               } else if (tokens[iter][2] == '+') {
                  for (int j=valueIndex2; j<valueIndex1; j++) {
                     for (int i=0; i<16; i++) {
                        nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + j] = weight;
                     }
                  }
               } else {
                  std::cout << "Please enter a valid hand to add." << std::endl;
               }
            } else if (length == 4) {
               for (int i=0; i<13; i++) {
                  if (tokens[iter][0] == values[i]) {value1 = i;}
                  if (tokens[iter][1] == values[i]) {value2 = i;}
               }
               valueIndex1 = std::max(value1, value2);
               valueIndex2 = std::min(value1, value2);
               if (valueIndex1 != valueIndex2 and tokens[iter][2] == 's' and tokens[iter][3] == '+') {
                  if (valueIndex1 - valueIndex2 == 1) {
                     for (int i=valueIndex1; i<13; i++) {
                        for (int j=0; j<4; j++) {
                           nonpairs[j * 78 + triangleNumbers[i-1] + i-1] = weight;
                        }
                     }
                  } else {
                     for (int j=valueIndex2; j<valueIndex1; j++) {
                        for (int i=0; i<4; i++) {
                           nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + j] = weight;
                        }
                     }
                  }
               } else if (value1 != value2 and tokens[iter][2] == 'o' and tokens[iter][3] == '+') {
                  if (valueIndex1 - valueIndex2 == 1) {
                     for (int i=valueIndex1; i<13; i++) {
                        for (int j=4; j<16; j++) {
                           nonpairs[j * 78 + triangleNumbers[i-1] + i-1] = weight;
                        }
                     }
                  } else {
                     for (int j=valueIndex2; j<valueIndex1; j++) {
                        for (int i=4; i<16; i++) {
                           nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + j] = weight;
                        }
                     }
                  }
               } else {
                  std::cout << "Please enter a valid hand to add." << std::endl;
               }
            } else if (length == 5) {
               for (int i=0; i<13; i++) {
                  if (tokens[iter][0] == values[i]) {value1 = i;}
                  if (tokens[iter][1] == values[i]) {value2 = i;}
                  if (tokens[iter][3] == values[i]) {value3 = i;}
                  if (tokens[iter][4] == values[i]) {value4 = i;}
               }
               valueIndex1 = std::max(value1, value2);
               valueIndex2 = std::min(value1, value2);
               valueIndex3 = std::max(value3, value4);
               valueIndex4 = std::min(value3, value4);
               if ((tokens[iter][2] != '-') or (valueIndex1 < valueIndex3) or (valueIndex1 == valueIndex3 and valueIndex2 <= valueIndex4)) {
                  std::cout << "Please enter a valid hand to add." << std::endl;
               } else if (valueIndex1 - valueIndex2 == valueIndex3 - valueIndex4) {
                  if (valueIndex1 == valueIndex2) {
                     for (int j=0; j<=valueIndex1-valueIndex3; j++) {
                        for (int i=0; i<6; i++) {
                           pairs[i * 13 + valueIndex3+j] = weight;
                        }
                     }
                  } else if (valueIndex1 != valueIndex2) {
                     if (valueIndex1 != valueIndex3) {
                        for (int j=0; j<=valueIndex1-valueIndex3; j++) {
                           for (int i=0; i<16; i++) {
                              nonpairs[i * 78 + triangleNumbers[valueIndex3+j-1] + valueIndex4+j] = weight;
                           }
                        }
                     } else {
                        for (int j=0; j<=valueIndex2-valueIndex4; j++) {
                           for (int i=0; i<16; i++) {
                              nonpairs[i * 78 + triangleNumbers[valueIndex3-1] + valueIndex4+j] = weight;
                           }
                        }
                     }
                  }
               } else if (valueIndex1 == valueIndex3 and valueIndex1 != valueIndex2) {
                  for (int j=valueIndex4; j<=valueIndex2; j++) {
                     for (int i=0; i<16; i++) {
                        nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + j] = weight;
                     }
                  }
               } else {
                  std::cout << "Please enter a valid hand to add." << std::endl;
               }
            } else if (length == 7) {
               for (int i=0; i<13; i++) {
                  if (tokens[iter][0] == values[i]) {value1 = i;}
                  if (tokens[iter][1] == values[i]) {value2 = i;}
                  if (tokens[iter][4] == values[i]) {value3 = i;}
                  if (tokens[iter][5] == values[i]) {value4 = i;}
               }
               valueIndex1 = std::max(value1, value2);
               valueIndex2 = std::min(value1, value2);
               valueIndex3 = std::max(value3, value4);
               valueIndex4 = std::min(value3, value4);
               if (tokens[iter][3] != '-' or valueIndex1 < valueIndex3 or tokens[iter][2] != tokens[iter][6] or (not (tokens[iter][2] == 's' or tokens[iter][2] == 'o')) or valueIndex1 == valueIndex2) {
                  std::cout << "Please enter a valid hand to add." << std::endl;
               } else if (tokens[iter][2] == 's') {
                  if (valueIndex1 - valueIndex2 == valueIndex3 - valueIndex4) {
                     if (valueIndex1 != valueIndex3) {
                        for (int j=0; j<=valueIndex1-valueIndex3; j++) {
                           for (int i=0; i<4; i++) {
                              nonpairs[i * 78 + triangleNumbers[valueIndex3+j-1] + valueIndex4+j] = weight;
                           }
                        }
                     } else {
                        for (int j=0; j<=valueIndex2-valueIndex4; j++) {
                           for (int i=0; i<4; i++) {
                              nonpairs[i * 78 + triangleNumbers[valueIndex3-1] + valueIndex4+j] = weight;
                           }
                        }
                     }
                  } else if (valueIndex1 == valueIndex3 and valueIndex2 != valueIndex4) {
                     for (int j=valueIndex4; j<=valueIndex2; j++) {
                        for (int i=0; i<4; i++) {
                           nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + j] = weight;
                        }
                     }
                  } else {
                     std::cout << "Please enter a valid hand to add." << std::endl;
                  }
               } else if (tokens[iter][2] == 'o') {
                  if (valueIndex1 - valueIndex2 == valueIndex3 - valueIndex4) {
                     if (valueIndex1 != valueIndex3) {
                        for (int j=0; j<=valueIndex1-valueIndex3; j++) {
                           for (int i=4; i<16; i++) {
                              nonpairs[i * 78 + triangleNumbers[valueIndex3+j-1] + valueIndex4+j] = weight;
                           }
                        }
                     } else {
                        for (int j=0; j<=valueIndex2-valueIndex4; j++) {
                           for (int i=4; i<16; i++) {
                              nonpairs[i * 78 + triangleNumbers[valueIndex3-1] + valueIndex4+j] = weight;
                           }
                        }
                     }
                  } else if (valueIndex1 == valueIndex3 and valueIndex1 != valueIndex2) {
                     for (int j=valueIndex4; j<=valueIndex2; j++) {
                        for (int i=4; i<16; i++) {
                           nonpairs[i * 78 + triangleNumbers[valueIndex1-1] + j] = weight;
                        }
                     }
                  } else {
                     std::cout << "Please enter a valid hand to add." << std::endl;
                  }
               } else {
                  std::cout << "Please enter a valid hand to add." << std::endl;
               }
            } else {
               std::cout << "Please enter a valid hand to add." << std::endl;
            }
         }
      }

      void print() {
         float grid[169] = {0.0};
         float offsuit = 0.0;
         float suited = 0.0;

         for (int i=0; i<13; i++) {  //add pairs
            for (int j=0; j<6; j++) {
               if (pairs[j * 13 + i] > 10e-4) {
                  grid[i * 13 + i] += (1.0/6.0) * pairs[j * 13 + i];
               }
            }
         }
         for (int i=1; i<13; i++) {  //add nonpairs
            for (int j=0; j<i; j++) {
               offsuit = 0.0;
               suited = 0.0;
               for (int k=0; k<16; k++) {
                  if (nonpairs[k * 78 + triangleNumbers[i-1] + j] > 10e-4) {
                     if (k<4) {
                        suited += nonpairs[k * 78 + triangleNumbers[i-1] + j];
                     } else {
                        offsuit += nonpairs[k * 78 + triangleNumbers[i-1] + j];
                     }
                  }
               }
               grid[j * 13 + i] += offsuit / 12.0;
               grid[i * 13 + j] += suited / 4.0;
            }
         }
         std::cout << "   " << values[12] << "    " << values[11] << "    "  << values[10] << "    "  << values[9] << "    "  << values[8] << "    "  << values[7] << "    "  << values[6] << "    "  << values[5] << "    "  << values[4] << "    "  << values[3] << "    "  << values[2] << "    "  << values[1] << "    "  << values[0] << std::endl;
         for (int i=12; i>=0; i--) {  //display hand chart grid
            std::cout << values[i] << " ";
            for (int j=12; j>=0; j--) {
               std::cout << std::fixed << std::setprecision(2) << grid[i * 13 + j] << " ";
            }
            std::cout << std::endl;
         }
         std::cout << std::endl;
      }

      std::string getCard(int card) {
         std::string value = std::string(1, values[card / 4]);
         std::string suit = std::string(1, suits[card % 4]);
         return value + suit;
      }

      int getInt(std::string card) {
         char value = card[0];
         char suit = card[1];
         int ans = 0;
         for (int i=0; i<13; i++) {
            if (value == values[i]) {
               ans = i * 4;
            }
         }
         for (int i=0; i<4; i++) {
            if (suit == suits[i]) {
               ans += i;
            }
         }
         return ans;
      }
};

#endif