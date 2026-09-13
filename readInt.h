#include <vector>
#include <string>
#include <sstream>

using namespace std;

vector<int> readInt(const string& t){
	auto l = t.begin();
	while (l != t.end() && std::isspace(*l)) ++l;
	auto r = t.end();
	do {
		--r;
	} while (std::distance(l, r) > 0 && std::isspace(*r));
	string s = string(l, r + 1);
	
	
	std::vector<int> res;
	std::stringstream ss(s);
	std::string item;
	
	while (std::getline(ss, item, ',')) {
		try {
			int val = std::stoi(item);
			res.push_back(val);
		} catch (...) {
			continue;
		}
	}
	
	return res;
}

std::string join(const std::vector<int>& v) {
    std::string s;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) s += ',';
        s += std::to_string(v[i]);
    }
    return s;
}