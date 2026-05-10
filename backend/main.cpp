#include <iostream>
#include "Player.h"
#include "Match.h"
#include "MatchmakingSystem.h"
using namespace std;

int main(){

    // Test 1 — Sub-rank priority
    // Gold II should match with Gold II
    // Gold I should match with Gold III (closest available)
    MatchmakingSystem system;

    cout << "=== TEST 1: Sub-rank priority ===\n";
    system.addPlayer("Ace",   Rank::Gold, SubRank::II);
    system.addPlayer("Zoro",  Rank::Gold, SubRank::I);
    system.addPlayer("Luffy", Rank::Gold, SubRank::II);
    system.addPlayer("Nami",  Rank::Gold, SubRank::III);

    system.displayQueues();
    system.runMatchmaking();
    system.displayMatches();

    // Test 2 — Mixed scenario with expansion still working
    cout << "\n=== TEST 2: Sub-rank + expansion together ===\n";
    MatchmakingSystem system2;

    system2.addPlayer("Levi",   Rank::Gold,   SubRank::II);
    system2.addPlayer("Mikasa", Rank::Silver, SubRank::I);

    for(int round = 1; round <= 4; round++){
        cout << "=== ROUND " << round << " ===\n";
        system2.runMatchmaking();
    }
    system2.displayMatches();

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