#include <iostream>
#include <deque>
using namespace std;


class A {
	int n;
public:
	A(int _n) : n(_n) {
		cout << n << "»ý¼ºµÊ" << endl;
	}
	~A() {
		cout << n << "¼Ò¸êµÊ" << endl;
	}
	void prn() {
		cout << n << endl;
	}
};


class A_PTR {
	A* a;
public:
	A_PTR(A *_a) : a(_a){}

	~A_PTR() {
		delete a;
	}

	A* operator -> () {
		return a;
	}

	A operator * () {
		return *a;
	}
};

void main() {
	deque<auto_ptr<A>> d;
	//auto_ptr<A> a(new A(3));
	deque<A_PTR> ad;
	ad.push_back(new A(3));
	//d.push_back(new A(3));
	A_PTR da = new A(5);
	da->prn();
}


