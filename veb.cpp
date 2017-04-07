#include "veb.h"
#include <unordered_map>


template <unsigned int S>
struct VEBTree : AbstractVEBTree<S> {
public:
	VEBTree() : localMin(NO), localMax(NO), aux(nullptr) {}

	virtual ~VEBTree() {
		if (aux != nullptr) delete aux;
		for (auto ptr : clasters) {
			delete ptr.second;
		}
	}

  	void add(unsigned long long x) {
  		if (x == localMin || x == localMax) return;
  		if (localMin == NO) {
  			localMin = localMax = x;
  			return;
  		}
  		if (localMin == localMax) {
  			if (x > localMax) localMax = x;
  			else localMin = x;
  			return;
  		}

  		if (localMin > x) std::swap(x, localMin);
  		if (localMax < x) std::swap(x, localMax);

  		if (aux == nullptr) aux = new VEBTree<S - HALF>();

  		unsigned long long high = getHighBits(x);
  		if (clasters.count(high) == 0) {
  			aux->add(high);
  			clasters[high] = new VEBTree<HALF>();
  		}
  		clasters[high]->add(getLowBits(x)); 
  	}

  	void remove(unsigned long long x) {
  		if (localMin == NO) return;
  		if (localMin == localMax) {
  			if (localMin == x) localMin = localMax = NO;
  			return;
  		}

  		if (clasters.empty()) {
  			if (x == localMin) localMin = localMax;
  			if (x == localMax) localMax = localMin;
  			return;
  		}

  		if (localMin == x) x = localMin = getAllBits(aux->getMin(), clasters[aux->getMin()]->getMin());
  		if (localMax == x) x = localMax = getAllBits(aux->getMax(), clasters[aux->getMax()]->getMax());

  		unsigned long long high = getHighBits(x);
  		if (clasters.count(high) == 0) return;
  		clasters[high]->remove(getLowBits(x));
  		if (clasters[high]->empty()) {
  			delete clasters[high];
  			clasters.erase(high);
  			aux->remove(high);
  		}
  	}

  	unsigned long long next(unsigned long long x) const {
  		if (localMin == NO || localMax <= x) return NO;
  		if (localMin > x) return localMin;
  		if (clasters.empty() || getAllBits(aux->getMax(), clasters[aux->getMax()]->getMax()) <= x) return localMax;

  		unsigned long long high = getHighBits(x);
  		unsigned long long cans;
  		if (clasters.count(high) == 0 || (cans = clasters[high]->next(getLowBits(x))) == NO) {
  			high = aux->next(high);
  			return getAllBits(high, clasters[high]->getMin());
  		}
  		else return getAllBits(high, cans);
  	}

  	unsigned long long prev(unsigned long long x) const {
  		if (localMin == NO || localMin >= x) return NO;
  		if (localMax < x) return localMax;
  		if (clasters.empty() || getAllBits(aux->getMin(), clasters[aux->getMin()]->getMin()) >= x) return localMin;

  		unsigned long long high = getHighBits(x);
  		unsigned long long cans;
  		if (clasters.count(high) == 0 || (cans = clasters[high]->prev(getLowBits(x))) == NO) {
  			high = aux->prev(high);
  			return getAllBits(high, clasters[high]->getMax());
  		}
  		else return getAllBits(high, cans);
  	}
  	
  	unsigned long long getMin() const {
  		return localMin;
  	}

  	unsigned long long getMax() const {
  		return localMax;
  	}

  	bool empty() const {
  		return localMin == NO;
  	}

private:
	static const unsigned int HALF = S >> 1;

	unsigned long long localMin, localMax;
	mutable std::unordered_map<unsigned long long, VEBTree<HALF> *> clasters;
	VEBTree<S - HALF> *aux;

	inline unsigned long long getHighBits(unsigned long long x) const {
		return x >> HALF;
	}

	inline unsigned long long getLowBits(unsigned long long x) const {
		return x & ((1ull << HALF) - 1);
	}

	inline unsigned long long getAllBits(unsigned long long high, unsigned long long low) const {
		return (high << HALF) + low;
	}
};