#ifndef JSONHELPER_H
#define JSONHELPER_H

#include<string>
#include<vector>

#include"queue.h"
#include"Player.h"
#include"Match.h"

class JsonHelper {
public:

    // Serialize a single player to JSON
    static std::string playerToJson(const Player& p){
        return "{"
            "\"id\":" + std::to_string(p.playerId) + ","
            "\"username\":\"" + p.userName + "\","
            "\"rank\":\"" + p.ranktoString() + "\","
            "\"subrank\":\"" + p.subRanktoString() + "\","
            "\"waitCycles\":" + std::to_string(p.waitCycles) + ","
            "\"expansionRadius\":" + std::to_string(p.expansionRadius()) +
        "}";
    }

    // Serialize a single match to JSON
    static std::string matchToJson(const Match& m) {
        return "{"
            "\"matchId\":" + std::to_string(m.matchId) + ","
            "\"player1\":" + playerToJson(m.player1) + ","
            "\"player2\":" + playerToJson(m.player2) + ","
            "\"isCrossRank\":" + (m.isCrossRank ? "true" : "false") +
        "}";
    }

    //Serialize a vector of matches to JSON array
    static std::string matchesToJson(const std::vector<Match>& matches){
        std::string result = "[";
        for(int i = 0; i < (int) matches.size(); i++){
            result += matchToJson(matches[i]);
            
            if(i < (int) matches.size() - 1){
                result += ",";
            }
        }
        result +="]";
        return result;
    }

    // Serialize a single rank bucket into JSON array
    static std::string queueToJson(CustomQueue<Player> q){
        // Takes queue by value (copy) so original is untouched
        std::string result = "[";
        bool first = true;
        while(!q.empty()){
            if(!first) result += ",";
            result += playerToJson(q.front());
            q.pop();
            first = false;
        }
        result += "]";
        return result;
    }
    
    // Wrap any JSON content with a status field
    static std::string successResponse(const std::string& key,
                                       const std::string& value){
        return "{\"status\":\"success\",\"" + key + "\":" + value + "}";
    }
    
    static std::string errorResponse(const std::string& message){
        return "{\"status\":\"error\",\"message\":\"" + message + "\"}";
    }
};
#endif