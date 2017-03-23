#include <bits/stdc++.h>
#include "veb.cpp"

using namespace std;


struct request {
	unsigned long long value, tp;
};


mt19937_64 rnd;


unsigned long long getRnd(unsigned long long mod) {
	return rnd() % mod;
}

set < unsigned long long > has;

request gen(unsigned long long MAX_VALUE) {
	request ret;

	ret.tp = getRnd(6);

	if (ret.tp == 1) {
		if (has.empty()) {
			ret.tp = 0;
			ret.value = getRnd(MAX_VALUE);
		} 
		else {
			if (getRnd(4) == 0) {
				ret.value = getRnd(MAX_VALUE);
				if (has.find(ret.value) != has.end()) has.erase(ret.value);
			}
			else {
				auto it = has.upper_bound(getRnd(MAX_VALUE));
				if (it == has.end()) --it;
				ret.value = *it;
				has.erase(it);
			}
		}
	}
	else {
		ret.value = getRnd(MAX_VALUE);		
	}
	
	if (ret.tp == 0) {
		has.insert(ret.value);
	}

	return ret; 
}

struct Test {
	unsigned long long MAX_VALUE;
	size_t length;

	void scan() {
		cin >> MAX_VALUE >> length;
	}

	bool test() {
		has.clear();
		clock_t t = clock();
		rnd = mt19937_64(time(0));
		vector < request > reqs (length);

		for (size_t i = 0; i < length; ++i) {
			reqs[i] = gen(MAX_VALUE);
		}

		cerr << "Generation takes " << clock() - t << " microseconds" << endl;

		t = clock();

		vector < unsigned long long > ans1, ans2;

		set < unsigned long long > testSet;

		for (size_t i = 0; i < length; ++i) {
			if (reqs[i].tp == 0) testSet.insert(reqs[i].value);
			if (reqs[i].tp == 1) testSet.erase(reqs[i].value);
			if (reqs[i].tp == 2) {
				auto it = testSet.upper_bound(reqs[i].value);
				if (it == testSet.end()) ans1.push_back(NO);
				else ans1.push_back(*it);
			}
			if (reqs[i].tp == 3) {
				if (testSet.empty()) {
					ans1.push_back(NO);
					continue;
				}
				set<unsigned long long>::iterator it = testSet.lower_bound(reqs[i].value);
				if (it == testSet.end()) {
					--it;
				}
				else {
					while (it != testSet.begin() && *it >= reqs[i].value) --it;
				}
				if (*it >= reqs[i].value) ans1.push_back(-1);
				else ans1.push_back(*it);
			}
			if (reqs[i].tp == 4) {
				if (testSet.empty()) ans1.push_back(-1);
				else ans1.push_back(*(testSet.begin()));
			}
			if (reqs[i].tp == 5) {
				if (testSet.empty()) ans1.push_back(-1);
				else {
					auto it = testSet.end();
					--it;
					ans1.push_back(*it);
				}
			}
		}

		
		clock_t dist1 = clock() - t;
		cerr << "Set takes " << dist1 << " microseconds" << endl;
		
		t = clock();

		VEBTree<64ull> tree;

		for (size_t i = 0; i < length; ++i) {
			if (reqs[i].tp == 0) tree.add(reqs[i].value);
			if (reqs[i].tp == 1) tree.remove(reqs[i].value);
			if (reqs[i].tp == 2) ans2.push_back(tree.next(reqs[i].value));
			if (reqs[i].tp == 3) ans2.push_back(tree.prev(reqs[i].value));
			if (reqs[i].tp == 4) ans2.push_back(tree.getMin());
			if (reqs[i].tp == 5) ans2.push_back(tree.getMax());
		}

		clock_t dist2 = clock() - t;
		cerr << "VEBTree takes " << dist2 << " microseconds" << endl;
		double d1 = dist1;
		double d2 = dist2;
		cerr << "VEBTree faster " << d1 / d2 << " times" << endl;
		cerr << endl;

		if (false) {
			filebuf bf;

			
			bf.open("badTest", ios::out);
			ostream bad (&bf);
			bad << MAX_VALUE << " " << length << endl;
			for (size_t	i = 0; i < length; ++i) {
				bad << reqs[i].tp << " " << reqs[i].value << endl;
			}

			bad << endl;

			for (auto x : ans1) {
				bad << x << " ";
			}
			bad << endl;

			for (auto x : ans2) {
				bad << x << " ";
			}
			bad << endl;
			bf.close();
			assert(false);
		}
		return true;

	}
};


int main() {
	freopen("tests", "r", stdin);
	size_t testNumber;
	Test test;
	cin >> testNumber;
	for (size_t i = 0; i < testNumber; ++i) {
		test.scan();
		test.test();
	}

	return 0;
}