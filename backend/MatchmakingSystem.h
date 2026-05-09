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

    void runMatchmaking(){
        std::cout << "\n===== RUNNING MATCHMAKING =====\n";
        int matchesThisRound = 0;

        // Phase 1 - Same rank matching 
        for(int i = 0; i < totalRanks; i++){
            while(rankQueues[i].size() >= 2){

                // Take two players from the front of the same Queue 
                Player p1 = rankQueues[i].front(); rankQueues[i].pop();
                Player p2 = rankQueues[i].front(); rankQueues[i].pop();

                // Create a match
                Match m(++matchIdCounter, p1, p2);
                formedMatches.push_back(m);
                matchesThisRound++;
                
                std::cout << "MATCHED: ";
                m.display();
            }
        }

        // Phase 2 - Increment waiting cycle for unmatched players
        std::cout << "\n--- Unmatched players waiting ---\n";
        bool anyWaiting = false;

        for(int i = 0; i < totalRanks; i++){
            if(!rankQueues[i].empty()){
                anyWaiting = true;

                // Again, copy trick to update waitCycles
                std::queue<Player> temp;
                while(!rankQueues[i].empty()){
                    Player p = rankQueues[i].front();
                    rankQueues[i].pop();
                    p.waitCycles++;
                    std::cout << p.userName << " waits... (cycles: "
                              << p.waitCycles << ", expansion: "
                              << p.expansionRadius() << ")\n";
                    temp.push(p);
                }
                rankQueues[i] = temp;
            }
        }

        if(!anyWaiting){
            std::cout<<"All players matched!\n";
        }

        std::cout << "\nMatches this round: " << matchesThisRound << "\n";
        std::cout << "Total matches so far: " << getTotalMatches() << "\n";
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