#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
#include <iostream>

using namespace std;

template <class T>
class Vector
{
	public:
		//Vector(const Vector& other);
		Vector(bool mustBeUnique = false) : mustBeUnique(mustBeUnique)
		{

		}
		Vector operator+(const Vector& b)
		{
			for (unsigned int i=0; i<b.base.size(); i++)
				if(!uniqueCheck(b.base[i]))
					base.push_back(b.base[i]);
		}

		int getIndex(T element)
		{
			for (unsigned int i=0; i<base.size(); i++)
				if (base[i] == element)
					return i;
			return -1;
		}
		inline bool contains(T element) {return getIndex(element) > -1;}
		bool push_back(T element)
		{
			if (uniqueCheck(element))
				return false;
			base.push_back(element);
			return true;
		}
		bool push_front(T element)
		{
			if (uniqueCheck(element))
				return false;
			base.push_front(element);
			return true;
		}
		inline void clear() {base.clear();}
		bool remove(T element, bool all = true)
		{
			int index = contains(element);
			if(index == -1)
				return false;
			while(index != -1)
			{
				base.erase(base.begin() + index);
				if (!all)
					break;
				index = contains(element);
			}
			return true;
		}
		bool add(T element, int index = -1)
		{
			if (index < 0 || (unsigned)index > base.size())
				index = base.size();
			cout << "index: " << index << endl;
			if (uniqueCheck(element))
				return false;
			base.insert(base.begin() + index, element);
			return true;
		}

	private:
		std::vector<T> base;
		bool mustBeUnique;
		bool uniqueCheck(T element)
		{
			return mustBeUnique && contains(element);
		}
};

#endif // VECTOR_H
