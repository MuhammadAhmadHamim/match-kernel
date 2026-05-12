#include <iostream>
#include "Player.h"
#include "Match.h"
#include "MatchmakingSystem.h"
using namespace std;

int main(){

    MatchmakingSystem system;

    system.addPlayer("Ace",   Rank::Gold,     SubRank::II);
    system.addPlayer("Zoro",  Rank::Gold,     SubRank::II);
    system.addPlayer("Levi",  Rank::Platinum, SubRank::I);
    system.addPlayer("Luffy", Rank::Bronze,   SubRank::III);

    cout << "\n--- Queue State JSON ---\n";
    cout << system.getQueueStateJson() << "\n";

    system.runMatchmaking();

    cout << "\n--- Matches JSON ---\n";
    cout << system.getMatchesJson() << "\n";

    cout << "\n--- Stats JSON ---\n";
    cout << system.getStatsJson() << "\n";

    cout << "\n--- Single Player JSON ---\n";
    Player p(42, "TestPlayer", Rank::Diamond, SubRank::I);
    cout << JsonHelper::playerToJson(p) << "\n";

    cout << "\n--- Error Response ---\n";
    cout << JsonHelper::errorResponse("Player name cannot be empty") << "\n";

    cout << "\n--- Success Response ---\n";
    cout << JsonHelper::successResponse("message", "\"Player added\"") << "\n";

    return 0;
}
/*
#include <iostream>
#include"httplib.h"
#include "Player.h"
#include "Match.h"

using namespace std;

int main(){
	httplib::Server svr;
	
	svr.Get("/ping", [](const httplib::Request&, httplib::Response& res){
		
		res.set_content("pong", "text/plain");
	});
	
	cout<<"MatchKernel server running on http://localhost:8080\n";
	
	svr.listen("localhost",8080);
	
	return 0;
}
*/