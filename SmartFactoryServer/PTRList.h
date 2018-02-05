#pragma once


#define LISTMAXSIZE 255

#define HS_ERROR_LIST_IS_FULL -100
#define HS_ERROR_LIST_OUTOFRANGE -101
typedef unsigned int index_t;




//일단 Template 컨테이너 클래스이지만
//소켓에 최적화 되있으므로 소켓이외의 것이 들어가면 
//미정의 동작 나올지도 모름,,,,,,
//
template <typename T>
class PTRList {
	T* list[LISTMAXSIZE];
	index_t size;


	bool outOfRange(index_t index) 
	{
		if (index >= size || index < 0)
			return true;
		else
			return false;
	}

	void replace(index_t index)
	{

	}

public:



	PTRList() : size(0) {
		for (int i = 0; i < LISTMAXSIZE; i++) 
			list[i] = nullptr;
	}

	index_t getLength() 
	{
		return size;
	}


	//리스트의 가장 뒤쪽에 element를 추가하는 메서드
	int push(T* sock) 
	{
		if (size == 255) {
			return HS_ERROR_LIST_IS_FULL;
		}

		list[size] = sock;
		size++;
		return 1;
	}


	//element를 리스트에서 제거함과 동시에 delete하는 메서드
	int remove(index_t index) 
	{
		if (outOfRange(index)) {
			return HS_ERROR_LIST_OUTOFRANGE;
		}

		if (index == size - 1) {
			size--;
			T* returnT = list[index];
			list[index] = nullptr;
			delete returnT;
			return 1;
		}
		else {
			T* returnT = list[index];
			list[index] = list[size - 1];
			list[size - 1] = nullptr;
			size--;
			delete returnT;
			return 1;
		}
	}

	//element를 리스트에서 제거하지만 delete하진 않는 메서드.
	T* poach(index_t index) 
	{
		if (outOfRange(index)) {
			return nullptr;
		}

		if (index == size - 1) {
			size--;
			T* returnT = list[index];
			list[index] = nullptr;
			return returnT;
		}
		else {
			T* returnT = list[index];
			list[index] = list[size - 1];
			list[size - 1] = nullptr;
			size--;
			return returnT;
		}
	}

	T* get(index_t index)
	{
		if (outOfRange(index))
			return nullptr;
		else
			return list[index];
	}

	T* operator [](index_t index) {
		return get(index);
	}
};



