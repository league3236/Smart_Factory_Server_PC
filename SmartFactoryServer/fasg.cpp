#include <iostream>
#include <deque>
#include "HSProtocol.h"
class A {
	int n;
public:
	A(int _n) : n(_n) {
		std::cout << n << "»ý¼ºµÊ" << std::endl;
	}
	~A() {
		std::cout << n << "¼Ò¸êµÊ" << std::endl;
	}
};
class B {
	int n[10];
public :
	B() {
		for (int i = 0; i < 10; i++) {
			n[i] = i + 1;
		}

	}

	int operator [] (int index) {
		return n[index];
	}

};
using namespace std;
typedef auto_ptr<A> AP;
void main() {
	


}
