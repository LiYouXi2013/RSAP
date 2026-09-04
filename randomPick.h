#include <vector>
#include <random>
#include <string>

using namespace std;

static mt19937 gen(0);

struct Person {
	string name = "";
	int weight;
};

vector<int> buildPool(const vector<Person>& lst) {
	vector<int> pool;
	for (size_t i = 0; i < lst.size(); ++i) {
		for (int j = 0; j < lst[i].weight; ++j) {
			pool.push_back(i);
		}
	}
	return pool;
}

int randomPick(const vector<int>& pool) {
	std::uniform_int_distribution<> dist(0, (int)pool.size() - 1);
	return pool[dist(gen)];
}

typedef vector<Person> personVec;
typedef vector<int> poolVec;
