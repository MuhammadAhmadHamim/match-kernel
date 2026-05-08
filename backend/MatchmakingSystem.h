#ifndef MATCHMAKINGSYSTEM_H
#define MATCHMAKINGSYSTEM_H

#include<iostream>
#include<vector>
#include<queue>

#include"Player.h"
#include"Match.h"

class MatchmakingSystem{
private:
    
    std::vector<std::queue<Player>> rankQueues;
    std::vector<Match> formedMatches;
    int totalRanks = 5;
    int playerIdCounter = 0;
    int matchIdCounter = 0;

public:
    
    MatchmakingSystem(){
        rankQueues.resize(totalRanks);
    }

    void addPlayer(std::string username, Rank r, SubRank sr){
        Player p(++playerIdCounter, username, r, sr);
        rankQueues[static_cast<int>(r)].push(p);
        std::cout<<"Player added: ";
        p.display();
    }

    void displayQueues() const {
        std::cout << "\n===== CURRENT QUEUE STATE =====\n";
        std::string rankNames[] = {
            "Bronze", "Silver", "Gold", "Platinum", "Diamond"
        };

        for(int i = 0; i < totalRanks; i++){
            std::cout<< rankNames[i] << ": ";

            if(rankQueues[i].empty()){
                std::cout << "empty\n";
                continue;
            }

            std::queue<Player> temp = rankQueues[i];
            while(!temp.empty()){
                Player p = temp.front();
                temp.pop();
                std::cout << p.userName << "(" << p.subRanktoString() << ") ";
            }
            std::cout << "\n";
        }
        
        std::cout << "================================\n\n";
    }

    void displayMatches() const {
        if(formedMatches.empty()) {
            std::cout << "No matches formed yet.\n";
            return;
        }

        std::cout << "\n===== FORMED MATCHES =====\n";
        for(const Match& m : formedMatches) {
            m.display();
        }
        std::cout << "==========================\n\n";
    }

    int getTotalPlayers() const {
        int count = 0;
        
        for(int i = 0; i < totalRanks; i++){
            count +=rankQueues[i].size();
        }

        return count;
    }

    int getTotalMatches() const {
        return formedMatches.size();
    }
};

#endif