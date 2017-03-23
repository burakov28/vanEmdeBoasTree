#include "veb.h"
#include <unordered_map>
#include <cassert>
#include <limits>


template <unsigned int S>
class VEBTree : AbstractVEBTree<S> {
public:
	VEBTree() : localMin(NO), localMax(NO), children(), aux(nullptr) {}

	virtual ~VEBTree() {								//Virtual destructor for tree
		for (auto p : children) {
			delete p.second;
		}
		if (aux != nullptr) delete aux;
	}

	bool isEmpty() const { return localMin == NO; }

	void add(unsigned long long x) {
		correctnessOfInput(x);							//Assert if x is too big
		if (isEmpty()) {								//Insertion in empty tree
			localMin = localMax = x;
			return;
		}

		if (x == localMin || x == localMax) return;     //Already inserted

		if (localMin == localMax) {  					//Exactly one element in tree (and not equal to x)
			if (x < localMin) localMin = x;
			else localMax = x;
			return;
		}

		//Tree contains at least two distinct elements

		if (x < localMin) std::swap(x, localMin);		//If x is a new minimum or maximum
		if (x > localMax) std::swap(x, localMax);

		//After that x couldn't affect on localMin or localMax

		if (aux == nullptr) aux = new VEBTree<HALF>();  //Optimization for leafs

		unsigned long long hb = getHighBits(x);
		if (children.find(hb) == children.end()) {		//If tree for first half of bits doesn't exist
			aux->add(hb);
			children[hb] = new VEBTree<HALF>();
		}
		children[hb]->add(getLowBits(x));				//Adding to a child-tree
	}

	void remove(unsigned long long x) {
		correctnessOfInput(x);							
		if (isEmpty()) return;							//Nothing to remove
		if (x < localMin || x > localMax) return;		//

		if (localMin == localMax) {						//Exactly one element in tree
			if (localMin == x) localMin = localMax = NO;//If it's equal to x than remove
			return;
		}

		if (aux == nullptr) {							//Exactly two elements in tree
			if (x == localMax) localMax = localMin;
			if (x == localMin) localMin = localMax;
			return;
		}

		//At least three elements in tree (aux != nullptr)

		if (x == localMin) {							//Replace localMin by minimum from children-trees
			x = localMin = getAllBits(aux->getMin(), children[aux->getMin()]->getMin());
		}
		if (x == localMax) {							//So on...
			x = localMax = getAllBits(aux->getMax(), children[aux->getMax()]->getMax());
		}

		//Element to remove can be only in one of the children

		unsigned long long hb = getHighBits(x);
		if (children.find(hb) == children.end()) return; //Nothing to remove

		children[hb]->remove(getLowBits(x));			//Removing from child
		if (children[hb]->isEmpty()) {					//If child-tree became empty - delete it
			delete children[hb];      					//Code's accuracy, but if it's necessary to speed up - you can erase this code
			children.erase(hb);		   					//

			aux->remove(hb);
			if (aux->isEmpty()) {	   					//
				delete aux;
				aux = nullptr;
			}
		}
	}

	unsigned long long next(unsigned long long x) const {
		if (isEmpty() || x >= localMax) return NO;		//Next doesn't exist

		//x < localMax, so localMax may be the answer

		if (x < localMin) return localMin;				//If less than localMin then localMin is the next

		// x >= localMin, so next is in child-tree or localMax

			//No children 	  //maximum in children is too small
		if (aux == nullptr || getAllBits(aux->getMax(), children[aux->getMax()]->getMax()) <= x) return localMax; //In this case next is localMax

		unsigned long long ans, hb = getHighBits(x);
		if (children.find(hb) == children.end() || (ans = children[hb]->next(getLowBits(x))) == NO) { //Answer is minimum in children[aux->next(hb)]
			unsigned long long nexthb = aux->next(hb);
			return getAllBits(nexthb, children[nexthb]->getMin());
		}
		else return getAllBits(hb, ans);								//Answer is next in children[hb]		 
	}

	//So on...
	unsigned long long prev(unsigned long long x) const {
		if (isEmpty() || x <= localMin) return NO;

		if (localMax < x) return localMax;

		if (aux == nullptr || getAllBits(aux->getMin(), children[aux->getMin()]->getMin()) >= x) return localMin;

		unsigned long long ans, hb = getHighBits(x);
		if (children.find(hb) == children.end() || (ans = children[hb]->prev(getLowBits(x))) == NO) {
			unsigned long long prevhb = aux->prev(hb);
			return getAllBits(prevhb, children[prevhb]->getMax()); 
		}
		else return getAllBits(hb, ans);
	}

	unsigned long long getMin() const { return localMin; }

	unsigned long long getMax() const { return localMax; }

private:
	static const unsigned int HALF = S >> 1;
	unsigned long long localMin, localMax;
	mutable std::unordered_map < unsigned long long, VEBTree<HALF>* > children;
	VEBTree<HALF> * aux;

	//Return greatest half of number
	inline unsigned long long getHighBits(unsigned long long x) const {
		return (x >> HALF);
	}

	//Return lowest half of number
	inline unsigned long long getLowBits(unsigned long long x) const {
		return x & ((1ull << HALF) - 1);
	}

	//Return number - merge of high as greatest half ans low as lowest one
	inline unsigned long long getAllBits(unsigned long long high, unsigned long long low) const {
		return (high << (HALF)) | low;
	}

	//Check that x from supported range
	inline void correctnessOfInput(unsigned long long x) const {
		if (S == 1) assert(x < 2);
		else assert(((x >> HALF) >> HALF) == 0);
	}
}; 