#ifndef VECTOR_H
#define VECTOR_H
#include <vector>

template <class T>
class Vector
{
	public:
		static Vector fromStdVector(std::vector<T> vect, bool mustBeUnique = false)
		{
			Vector result(mustBeUnique);
			result.base = vect;
			return result;
		}

		Vector(const Vector& other) : base(other.base), mustBeUnique(other.mustBeUnique) {}
		Vector(bool mustBeUnique = false) : mustBeUnique(mustBeUnique) {}
		Vector operator+(const Vector& b)
		{
			Vector result(*this);
			for (unsigned int i=0; i<b.base.size(); i++)
				if(!uniqueCheck(b.base[i]))
					result.base.push_back(b.base[i]);
			return result;
		}
		Vector operator==(const Vector& b)
		{
			return base == b.base;
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
		bool removeElement(T element, bool all = true)
		{
			int index = getIndex(element);
			if(index == -1)
				return false;
			while(index != -1)
			{
				base.erase(base.begin() + index);
				if (!all)
					break;
				index = getIndex(element);
			}
			return true;
		}
		bool remove(unsigned int index)
		{
			if (index >= base.size())
				return false;
			base.erase(base.begin() + index);
			return true;
		}
		bool add(T element, int index = -1)
		{
			if (index < 0 || (unsigned)index > base.size())
				index = base.size();
			if (uniqueCheck(element))
				return false;
			base.insert(base.begin() + index, element);
			return true;
		}
		inline long size() {return base.size();}
		inline std::vector<T> toStdVector() {return base;}
		inline T get(unsigned int index) {return base[index];}
		inline T getBack() {return base.back();}

	private:
		std::vector<T> base;
		bool mustBeUnique;
		bool uniqueCheck(T element)
		{
			return mustBeUnique && contains(element);
		}
};

#endif // VECTOR_H
