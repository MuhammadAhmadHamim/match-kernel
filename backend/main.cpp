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
	
    // Get queue-state
	svr.Get("/queue-state", [](const httplib::Request&,
                                    httplib::Response& res){
		setCORSHeaders(res);
        string json = matchSystem.getQueueStateJson();
        res.set_content(
            JsonHelper::successResponse("queues", json),
            "application/json"
        );
		
	});
    
	svr.listen("localhost",8080);
	return 0;
}