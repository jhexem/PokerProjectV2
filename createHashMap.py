import csv
import itertools
from operator import itemgetter

values = "AKQJT98765432"

royalFlushes = ["fAKQJT"]
straightFlushes = ["fKQJT9", "fQJT98", "fJT987", "fT9876", "f98765", "f87654", "f76543", "f65432", "fA5432"]

straights = ["AKQJT", "KQJT9", "QJT98", "JT987", "T9876", "98765", "87654", "76543", "65432", "A5432"]

quads = []
for i in range(13):
   for j in range(13):
      if i < j:
         quadStr = values[i] * 4 + values[j]
         quads.append(quadStr)
      elif i > j:
         quadStr = values[j] + values[i] * 4
         quads.append(quadStr)

fullHouses = []
for i in range(13):
   for j in range(13):
      if i < j:
         fhStr = values[i] * 3 + values[j] * 2
         fullHouses.append(fhStr)
      elif i > j:
         fhStr = values[j] * 2 + values[i] * 3
         fullHouses.append(fhStr)
         
trips = []
intTrips = []
for i in range(13):
   for j in range(i+1, 13):
      for k in range(j+1, 13):
         L1 = [i, i, i, j, k]
         L2 = [i, j, j, j, k]
         L3 = [i, j, k, k, k]
         intTrips.append(L1)
         intTrips.append(L2)
         intTrips.append(L3)
         
intTrips = sorted(intTrips, key=itemgetter(2))
for intlist in intTrips:
   tripStr = values[intlist[0]] + values[intlist[1]] + values[intlist[2]] + values[intlist[3]] + values[intlist[4]]
   trips.append(tripStr)

twoPairs = []
intTwoPairs = []
for i in range(13):
   for j in range(i+1, 13):
         newL = [i, i, j, j]
         intTwoPairs.append(newL)

for intList in intTwoPairs:
   for i in range(13):
      if i != intList[0] and i != intList[2]:
         if i < intList[0]:
            tpStr = values[i] + values[intList[0]] + values[intList[1]] + values[intList[2]] + values[intList[3]]
            twoPairs.append(tpStr)
         elif i > intList[0] and i < intList[2]:
            tpStr = values[intList[0]] + values[intList[1]] + values[i] + values[intList[2]] + values[intList[3]]
            twoPairs.append(tpStr)
         elif i > intList[2]:
            tpStr = values[intList[0]] + values[intList[1]] + values[intList[2]] + values[intList[3]] + values[i]
            twoPairs.append(tpStr)
            
pairs = []
intPairs = []
for i in range(13):
   intPairs.append(i)
   
for num in intPairs:
   for i in range(13):
      for j in range(i+1, 13):
         for k in range(j+1, 13):
            if i != num and j != num and k != num:
               if num < i:
                  pairStr = values[num] + values[num] + values[i] + values[j] + values[k]
                  pairs.append(pairStr)
               elif num > i and num < j:
                  pairStr = values[i] + values[num] + values[num] + values[j] + values[k]
                  pairs.append(pairStr)
               elif num > j and num < k:
                  pairStr = values[i] + values[j] + values[num] + values[num] + values[k]
                  pairs.append(pairStr)
               elif num > k:
                  pairStr = values[i] + values[j] + values[k] + values[num] + values[num]
                  pairs.append(pairStr)

highCards = []
for i in range(13):
   for j in range(i+1, 13):
      for k in range(j+1, 13):
         for l in range(k+1, 13):
            for m in range(l+1, 13):
               highStr = values[i] + values[j] + values[k] + values[l] + values[m]
               if highStr not in straights:
                  highCards.append(highStr)

flushes = []
for i in range(13):
   for j in range(i+1, 13):
      for k in range(j+1, 13):
         for l in range(k+1, 13):
            for m in range(l+1, 13):
               flushStr = values[i] + values[j] + values[k] + values[l] + values[m]
               if flushStr not in straights:
                  flushes.append("f" + flushStr)


filename = "hashMap.csv"

with open(filename, "w") as csvfile:
   writer = csv.writer(csvfile)
   
   i = 1
   
   writer.writerow([royalFlushes[0], i])
   i += 1
   
   for hand in straightFlushes:
      writer.writerow([hand, i])
      i += 1
   
   for hand in quads:
      writer.writerow([hand, i])
      i += 1
      
   for hand in fullHouses:
      writer.writerow([hand, i])
      i += 1
      
   for hand in flushes:
      writer.writerow([hand, i])
      i += 1
      
   for hand in straights:
      writer.writerow([hand, i])
      i += 1
      
   for hand in trips:
      writer.writerow([hand, i])
      i += 1
      
   for hand in twoPairs:
      writer.writerow([hand, i])
      i += 1
      
   for hand in pairs:
      writer.writerow([hand, i])
      i += 1
      
   for hand in highCards:
      writer.writerow([hand, i])
      i += 1