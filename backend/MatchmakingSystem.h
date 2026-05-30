#ifndef MATCHMAKINGSYSTEM_H
#define MATCHMAKINGSYSTEM_H

#include<iostream>
#include<vector>
#include<climits>
#include <fstream>
#include <sstream>

#include"Player.h"
#include"Match.h"
#include"JsonHelper.h"
#include"Utils.h"
#include"queue.h"

class MatchmakingSystem{
private:
    
    std::vector<CustomQueue<Player>> rankQueues;
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

            CustomQueue<Player> temp = rankQueues[i];
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

        // Phase 1 — Same rank matching with sub-rank priority
        for(int i = 0; i < totalRanks; i++) {
            if(rankQueues[i].size() < 2) continue;

            // Convert queue to vector for flexible access
            std::vector<Player> bucket;
            while(!rankQueues[i].empty()) {
                bucket.push_back(rankQueues[i].front());
                rankQueues[i].pop();
            }

            std::vector<bool> matched(bucket.size(), false);

            for(int j = 0; j < (int)bucket.size(); j++) {
                if(matched[j]) continue;

                int bestIndex = -1;
                int bestDistance = INT_MAX;

                // Search all unmatched players after j
                for(int k = j + 1; k < (int)bucket.size(); k++) {
                    if(matched[k]) continue;

                    int distance = abs(static_cast<int>(bucket[j].subRank) -
                                        static_cast<int>(bucket[k].subRank));
                    if(distance < bestDistance) {
                        bestDistance = distance;
                        bestIndex = k;
                    }
                }

                // If a valid match was found
                if(bestIndex != -1) {
                    matched[j] = true;
                    matched[bestIndex] = true;

                    Match m(++matchIdCounter, bucket[j], bucket[bestIndex]);
                    formedMatches.push_back(m);
                    matchesThisRound++;

                    std::cout << "MATCHED (same-rank, sub-priority): ";
                    m.display();
                }
            }

            // Push unmatched players back into queue
            for(int j = 0; j < (int)bucket.size(); j++) {
                if(!matched[j]) {
                    rankQueues[i].push(bucket[j]);
                }
            }
        }

        // Phase 2 - Increment waiting cycle for unmatched players
        for(int i = 0; i < totalRanks; i++){
            if(!rankQueues[i].empty()){
                // Again, copy trick to update waitCycles
                CustomQueue<Player> temp;
                while(!rankQueues[i].empty()){
                    Player p = rankQueues[i].front();
                    rankQueues[i].pop();
                    p.waitCycles++;
                    temp.push(p);
                }
                rankQueues[i] = temp;
            }
        }

        // Phase 3 — Expansion matching
        // For each rank bucket, check if front player can expand
        for(int i = 0; i < totalRanks; i++){
            if(rankQueues[i].empty()) continue;

            Player p1 = rankQueues[i].front();
            int radius = p1.expansionRadius();
            if(radius == 0) continue;

            // Search adjacent buckets within radius
            bool matched = false;
            for(int offset = 1; offset <= radius && !matched; offset++){
                // Check bucket above
                int above = i + offset;
                if(above < totalRanks && !rankQueues[above].empty()){
                    rankQueues[i].pop();
                    Player p2 = rankQueues[above].front();
                    rankQueues[above].pop();
                    Match m(++matchIdCounter, p1, p2);
                    formedMatches.push_back(m);
                    matchesThisRound++;
                    matched = true;
                    std::cout << "Matched (EXPANDED):";
                    m.display();
                }

                // Check bucket below
                int below = i - offset;
                if(!matched && below >= 0 && !rankQueues[below].empty()){
                    rankQueues[i].pop();
                    Player p2 = rankQueues[below].front();
                    rankQueues[below].pop();
                    Match m(++matchIdCounter, p1, p2);
                    formedMatches.push_back(m);
                    matchesThisRound++;
                    matched = true;
                    std::cout << "Matched (EXPANDED):";
                    m.display();
                }
            }
        }

        std::cout << "\n--- Unmatched players waiting ---\n";
        bool anyWaiting = false;
        for(int i = 0; i < totalRanks; i++){
            if(!rankQueues[i].empty()){
                anyWaiting = true;
                CustomQueue<Player> temp = rankQueues[i];
                while(!temp.empty()){
                    Player p = temp.front(); temp.pop();
                    std::cout << p.userName << " waits... (cycles: "
                              << p.waitCycles << ", expansion: "
                              << p.expansionRadius() << ")\n";
                }
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

    std:: string getQueueStateJson() const {
        std::string rankNames[] = {
            "Bronze", "Silver", "Gold", "Platinum", "Diamond"
        };

        std::string result = "{";
        for(int i = 0; i < totalRanks; i++){
            result += "\"" + rankNames[i] + "\":";
            result += JsonHelper::queueToJson(rankQueues[i]);
            if(i < totalRanks - 1) result += ",";
        }
        result += "}";
        return result;
    }

    // Returns all formed matches as JSON
    std::string getMatchesJson() const {
        return JsonHelper::matchesToJson(formedMatches);
    }

    // Return system stats as JSON
    std::string getStatsJson() const {
        return "{"
                "\"totalPlayers\":" + std::to_string(getTotalPlayers()) + ","
                "\"totalMatches\":" + std::to_string(getTotalMatches()) + 
        "}";
    }

    // Return number of players loaded from file 
    int loadPlayersFromCSV(const std::string& csvContent) {
        int count = 0;
        std::istringstream stream(csvContent);
        std::string line;
    
        getline(stream, line); // skip header
    
        while(getline(stream, line)) {
            if(line.empty()) continue;
            if(!line.empty() && line.back() == '\r')
                line.pop_back();
    
            std::stringstream ss(line);
            std::string username, rankStr, srStr;
    
            getline(ss, username,    ',');
            getline(ss, rankStr, ',');
            getline(ss, srStr,   ',');
    
            if(username.empty() || rankStr.empty() || srStr.empty())
                continue;
    
            Rank r     = Utils::parseRank(rankStr);
            SubRank sr = Utils::parseSubRank(srStr);
            addPlayer(username, r, sr);
            count++;
        }
        return count;
    }
};

#endif