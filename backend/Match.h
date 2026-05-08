#ifndef MATCH_H
#define MATCH_H

#include<iostream>
#include"Player.h"

struct Match{
    int matchId;
    Player player1;
    Player player2;
    bool isCrossRank;

    Match(int id, Player p1, Player p2): 
        matchId(id), player1(p1), player2(p2), isCrossRank(player1.rank != player2.rank){}

    void display() const {
        std::cout << "=== Match #" << matchId << " ===\n";
        std::cout << player1.userName << " ("
                  << player1.ranktoString() << " "
                  << player1.subRanktoString() << ")";
        std::cout << "  VS  ";
        std::cout << player2.userName << " ("
                  << player2.ranktoString() << " "
                  << player2.subRanktoString() << ")";
        if(isCrossRank)
            std::cout << "  [CROSS-RANK]\n";
        std::cout << "\n";
    }
};

#endif