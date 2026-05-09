#include <iostream>
#include "Player.h"
#include "Match.h"
#include "MatchmakingSystem.h"
using namespace std;

int main(){

    MatchmakingSystem system;

    // Add one lonely player with nobody to match with
    // This forces them to wait multiple rounds
    system.addPlayer("Luffy", Rank::Bronze, SubRank::III);
    system.addPlayer("Levi",  Rank::Gold,   SubRank::I);

    // Manual verification of expansionRadius math
    cout << "\n--- MANUAL EXPANSION MATH CHECK ---\n";
    Player testPlayer(999, "TestPlayer", Rank::Gold, SubRank::II);

    int cycleCheckpoints[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 15};
    for(int cycles : cycleCheckpoints){
        testPlayer.waitCycles = cycles;
        cout << "waitCycles: " << cycles
             << " -> expansionRadius: "
             << testPlayer.expansionRadius() << "\n";
    }

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