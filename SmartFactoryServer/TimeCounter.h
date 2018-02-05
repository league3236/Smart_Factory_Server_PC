#include <time.h>

class Timer {
	int threshold;
	time_t lastTime;
	time_t currentTime;
	void reflesh() {
		lastTime = currentTime;
		time(&currentTime);
	}

	void count() {
		time(&currentTime);
	}

public:

	explicit Timer(int _threshold):threshold(_threshold) {
		time(&currentTime);
		reflesh();
	}
	
	void start() {
		reflesh();
	}
	
	bool timeOver() {
		count();
		int v = currentTime - lastTime;
		//printf("V: %d,thres : %d\n", v,threshold);
		if (v > threshold)
			return true;
		else 
			return false;
	}



};








