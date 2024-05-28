#include "pokerClasses.hpp"

int main() {

   Range testRange1 = Range();

   testRange1.add("22+,A7s+,K9s+,Q9s+,J9s+,T8s+,97s+,86s+,75s+,64s+,54s,A9o+,KTo+,QTo+,JTo,T9o", 1.0);
   
   testRange1.print();
   std::cout << (static_cast<float>(testRange1.countCombos()) / static_cast<float>(testRange1.totalCombos)) * 100.0 << "%" << std::endl;

   return 0;
}