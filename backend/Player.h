#ifndef PLAYER_H
#define PLAYER_H

#include<iostream>
#include<string>

enum class Rank {
    Bronze = 0,
    Silver = 1,
    Gold = 2,
    Platinum = 3,
    Diamond = 4
};

enum class SubRank{
    I = 1,
    II = 2,
    III = 3
};

struct Player{

    int playerId;
    std::string userName;
    Rank rank;
    SubRank subRank;
    int waitCycles;

    Player(int id, std::string username, Rank r, SubRank sr)
        : playerId(id), userName(username), rank(r), subRank(sr), waitCycles(0){}

    int expansionRadius() const {
        return waitCycles / 3;
    }

    std::string ranktoString() const {
        switch(rank){
            case Rank::Bronze :{
                return "Bronze";
            }
            
            case Rank::Silver :{
                return "Silver";
            }
            
            case Rank::Gold :{
                return "Gold";
            }
            
            case Rank::Platinum :{
                return "Platinum";
            }
            
            case Rank::Diamond :{
                return "Diamond";
            }
            
            default: {
                return "Unknown";
            }
        }
    }

    std::string subRanktoString() const {
        switch(subRank){
            case SubRank::I :{
                return "I";
            }
            
            case SubRank::II :{
                return "II";
            }
            
            case SubRank::III :{
                return "III";
            }
    
            default:{
                return "?";
            }
        }
    }

    void display() const {
        std::cout << "[" << playerId << "] "
                  << userName << " | "
                  << ranktoString() << " "
                  << subRanktoString()
                  << " | Waited: " << waitCycles << " cycles\n";
    }
};

#endif