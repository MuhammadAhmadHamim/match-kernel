#ifndef UTILS_H
#define UTILS_H

#include<iostream>

#include"Player.h"

using namespace std;

class Utils{
public:

    // Parse rank string to enum
    static Rank parseRank(const string& r) {
        if(r == "Bronze")   return Rank::Bronze;
        if(r == "Silver")   return Rank::Silver;
        if(r == "Gold")     return Rank::Gold;
        if(r == "Platinum") return Rank::Platinum;
        if(r == "Diamond")  return Rank::Diamond;
        return Rank::Bronze; // default fallback
    }

    // Parse subrank string to enum
    static SubRank parseSubRank(const string& sr) {
        if(sr == "I")   return SubRank::I;
        if(sr == "II")  return SubRank::II;
        if(sr == "III") return SubRank::III;
        return SubRank::I; // default fallback
    }
};

#endif