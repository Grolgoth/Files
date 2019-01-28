#ifndef VECTOR_H
#define VECTOR_H
#include <vector>

template <class T>
class Vector
{
	public:
		//Vector(const Vector& other);
		Vector(bool mustBeUnique = false);
		Vector operator+(const Vector& b);

		int getIndex(T element);
		inline bool contains(T element) {return getIndex(element) > -1;}
		bool push_back(T element);
		bool push_front(T element);
		inline void clear() {base.clear();}
		bool remove(T element, bool all = true);
		bool add(T element, int index = -1);

	private:
		std::vector<T> base;
		bool mustBeUnique;
		bool uniqueCheck(T element);
};

#endif // VECTOR_H
