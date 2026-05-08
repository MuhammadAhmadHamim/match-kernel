#include <iostream>
#include "Player.h"
#include "Match.h"
#include "MatchmakingSystem.h"
using namespace std;

int main(){

    MatchmakingSystem system;

    // Add a mix of players
    system.addPlayer("Ace",    Rank::Gold,     SubRank::II);
    system.addPlayer("Zoro",   Rank::Gold,     SubRank::II);
    system.addPlayer("Levi",   Rank::Gold,     SubRank::III);
    system.addPlayer("Mikasa", Rank::Silver,   SubRank::I);
    system.addPlayer("Eren",   Rank::Platinum, SubRank::II);
    system.addPlayer("Gojo",   Rank::Diamond,  SubRank::I);
    system.addPlayer("Luffy",  Rank::Bronze,   SubRank::III);

    // Display queue state
    system.displayQueues();

    // Check stats
    cout << "Total players waiting: " << system.getTotalPlayers() << "\n";
    cout << "Total matches formed: "  << system.getTotalMatches() << "\n";

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