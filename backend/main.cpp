#include <iostream>
#include "Player.h"
#include "Match.h"
#include "MatchmakingSystem.h"
using namespace std;

int main(){

    MatchmakingSystem system;

    // Luffy and Levi — 3 ranks apart
    // Luffy is Bronze(0), Levi is Gold(2)
    // After 6 cycles radius = 2, they should match
    system.addPlayer("Luffy", Rank::Bronze, SubRank::III);
    system.addPlayer("Levi",  Rank::Gold,   SubRank::I);

    cout << "--- Running until expansion kicks in ---\n";

    for(int round = 1; round <= 7; round++){
        cout << "========== ROUND " << round << " ==========\n";
        system.runMatchmaking();
    }

    // Second scenario — mix of ranks, some need expansion
    MatchmakingSystem system2;

    system2.addPlayer("Ace",    Rank::Gold,     SubRank::II);
    system2.addPlayer("Mikasa", Rank::Platinum, SubRank::I);
    system2.addPlayer("Eren",   Rank::Silver,   SubRank::III);

    cout << "\n--- Mixed rank scenario ---\n";
    for(int round = 1; round <= 5; round++){
        cout << "=== ROUND " << round << " ===\n";
        system2.runMatchmaking();
    }
    system2.displayMatches();

    system.displayMatches();

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