#include<iostream>
#include"httplib.h"
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
