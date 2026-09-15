#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

struct Person;

vector<Person> readInt(const string& t){
	auto l = t.begin();
	while (l != t.end() && std::isspace(*l)) ++l;
	auto r = t.end();
	do {
		--r;
	} while (std::distance(l, r) > 0 && std::isspace(*r));
	string s = string(l, r + 1);
	
	std::vector<Person> res;
	std::stringstream ss(s);
	std::string item;
	string t1;
	string t2;
	
	while (std::getline(ss, item, ',')) {
		cout<<"item:"<<item<<endl;
		stringstream t3(item);
		getline(t3,t1,'-');
		getline(t3,t2,'-');

		int val;
		try {
			val = std::stoi(t2);
		} catch (...) {
			continue;
		}

		res.push_back({t1,val});
	}

	if(res.size()==0)printf("nmd");
	
	return res;
}

std::string join(const std::vector<Person>& v) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) s += ',';
        s += v[i].name;
		s += '-';
		s += to_string(v[i].weight);

    }
    return s;
}