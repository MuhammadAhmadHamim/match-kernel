#include <iostream>
#include"httplib.h"
#include "Player.h"
#include "Match.h"
#include "MatchmakingSystem.h"
#include "JsonHelper.h"
using namespace std;

// Global system instance — persists across all requests
MatchmakingSystem matchSystem;

// Helper to add CORS headers to every response
void setCORSHeaders(httplib::Response& res){
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods",
                    "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers",
                    "Content-Type");
}

// Parse rank string to enum
Rank parseRank(const string& r) {
    if(r == "Bronze")   return Rank::Bronze;
    if(r == "Silver")   return Rank::Silver;
    if(r == "Gold")     return Rank::Gold;
    if(r == "Platinum") return Rank::Platinum;
    if(r == "Diamond")  return Rank::Diamond;
    return Rank::Bronze; // default fallback
}

// Parse subrank string to enum
SubRank parseSubRank(const string& sr) {
    if(sr == "I")   return SubRank::I;
    if(sr == "II")  return SubRank::II;
    if(sr == "III") return SubRank::III;
    return SubRank::I; // default fallback
}

int main(){
	
    httplib::Server svr;

    // Handle preflight CORS requests
    svr.Options(R"(.*)", [](const httplib::Request&,
                            httplib::Response& res){
        setCORSHeaders(res);
        res.status = 200;
    });
	
    // Get /queue-state
	svr.Get("/queue-state", [](const httplib::Request&,
                                    httplib::Response& res){
        std::cout << "GET /queue-state hit!\n";
        setCORSHeaders(res);
        string json = matchSystem.getQueueStateJson();
        res.set_content(
            JsonHelper::successResponse("queues", json),
            "application/json"
        );
		
	});

    // Get /matches
	svr.Get("/matches", [](const httplib::Request&,
                                    httplib::Response& res){
        std::cout << "GET /matches hit!\n";
        setCORSHeaders(res);
        string json = matchSystem.getMatchesJson();
        res.set_content(
            JsonHelper::successResponse("matches", json),
            "application/json"
        );

    });

    // Get /stats
	svr.Get("/stats", [](const httplib::Request&,
                                    httplib::Response& res){
        std::cout << "GET /stats hit!\n";
        setCORSHeaders(res);
        res.set_content(
            matchSystem.getStatsJson(),
            "application/json"
        );

    });

    // Post /add-player
    svr.Post("/add-player", [](const httplib::Request& req,
                                    httplib::Response& res){
        std::cout << "POST /add-player hit!\n";
        setCORSHeaders(res);
        
        // Parse body manually: "username=Ace&rank=Gold&subrank=II"
        string body = req.body;
        string username, rankStr, subrankStr;

        // Helper lambda to extract value by key
        auto getValue = [&](const string& key) -> string {
        string search = key + "=";
        size_t start = body.find(search);
        if(start == string::npos) return "";
        start += search.length();
        size_t end = body.find('&', start);
        if(end == string::npos) end = body.length();
        return body.substr(start, end - start);
        };

        username   = getValue("username");
        rankStr    = getValue("rank");
        subrankStr = getValue("subrank");

        if(username.empty() || rankStr.empty() || subrankStr.empty()){
            res.set_content(
                JsonHelper::errorResponse("Missing fields"),
                "application/json"
            );
            return;
        }

        Rank r = parseRank(rankStr);
        SubRank sr = parseSubRank(subrankStr);
        matchSystem.addPlayer(username, r, sr);

        res.set_content(
            JsonHelper::successResponse("success", "\"Player " + username + " added to queue\""),
            "application/json"
        );
    });

    // Post /run-matchmaking
    svr.Post("/run-matchmaking", [](const httplib::Request& req,
                                    httplib::Response& res){
        std::cout << "POST /run-matchmaking hit!\n";
        setCORSHeaders(res);

        matchSystem.runMatchmaking();
        string queues = matchSystem.getQueueStateJson();
        string matches = matchSystem.getMatchesJson();
        string result = 
            "{\"status\":\"success\","
            "\"queues\":" + queues + ","
            "\"matches\":" + matches + "}";
        res.set_content(result, "application/json");
    });

	svr.listen("localhost",8080);
	return 0;
}