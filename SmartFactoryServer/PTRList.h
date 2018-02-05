#pragma once


#define LISTMAXSIZE 255

#define HS_ERROR_LIST_IS_FULL -100
#define HS_ERROR_LIST_OUTOFRANGE -101
typedef unsigned int index_t;




//�ϴ� Template �����̳� Ŭ����������
//���Ͽ� ����ȭ �������Ƿ� �����̿��� ���� ���� 
//������ ���� �������� ��,,,,,,
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


	//����Ʈ�� ���� ���ʿ� element�� �߰��ϴ� �޼���
	int push(T* sock) 
	{
		if (size == 255) {
			return HS_ERROR_LIST_IS_FULL;
		}

		list[size] = sock;
		size++;
		return 1;
	}


	//element�� ����Ʈ���� �����԰� ���ÿ� delete�ϴ� �޼���
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

	//element�� ����Ʈ���� ���������� delete���� �ʴ� �޼���.
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



