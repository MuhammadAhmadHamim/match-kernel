#include <iostream>
#include "Player.h"
#include "Match.h"
#include "MatchmakingSystem.h"
using namespace std;

int main(){

    MatchmakingSystem system;

    // Round 1 — Add players
    cout << "=== ADDING PLAYERS ===\n";
    system.addPlayer("Ace",    Rank::Gold,     SubRank::II);
    system.addPlayer("Zoro",   Rank::Gold,     SubRank::II);
    system.addPlayer("Levi",   Rank::Gold,     SubRank::III);
    system.addPlayer("Mikasa", Rank::Silver,   SubRank::I);
    system.addPlayer("Eren",   Rank::Platinum, SubRank::II);
    system.addPlayer("Luffy",  Rank::Bronze,   SubRank::III);

    // Show queue before matching
    system.displayQueues();

    // Run matchmaking — round 1
    system.runMatchmaking();

    // Show queue after round 1
    system.displayQueues();

    // Show matches formed
    system.displayMatches();

    // Round 2 — Add more players to match the lonely ones
    cout << "=== ADDING MORE PLAYERS ===\n";
    system.addPlayer("Gojo",   Rank::Silver,   SubRank::II);
    system.addPlayer("Naruto", Rank::Platinum, SubRank::I);

    system.displayQueues();

    // Run matchmaking — round 2
    system.runMatchmaking();

    system.displayQueues();
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