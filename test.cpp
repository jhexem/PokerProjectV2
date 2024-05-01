#include "pokerClasses.hpp"

int main() {

   Range testRange1 = Range();
   Range testRange2 = Range();

   testRange1.add("22+,A2s+,K2s+,Q7s+,J7s+,T7s+,96s+,86s+,75s+,64s+,53s+,43s,A2o+,K5o+,Q8o+,J8o+,T8o+,98o,87o,76o,65o", 1.0);
   testRange2.add("22+, ATs+, KTs+, QTs+, J9s+, T8s+, 98s, 87s, 76s, 65s, 54s, ATo+, KTo+, QTo+, JTo, 72o", 1.0);

   testRange1.print();
   testRange2.print();

   Range testRange3 = testRange1 + testRange2;
   Range testRange4 = testRange3 - testRange1;

   testRange3.print();
   testRange4.print();

   return 0;
}