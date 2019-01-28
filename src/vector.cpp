#include "vector.h"

template <class T>
Vector<T>::Vector(bool mustBeUnique) : mustBeUnique(mustBeUnique)
{

}

//Vector::Vector<T>(const Vector<T>& other)

template <class T>
int Vector<T>::getIndex(T element)
{
	for (unsigned int i=0; i<base.size(); i++)
		if (base[i] == element)
			return i;
	return -1;
}

template <class T>
bool Vector<T>::remove(T element, bool all)
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

template <class T>
Vector<T> Vector<T>::operator+(const Vector<T>& b)
{
	for (unsigned int i=0; i<b.base.size(); i++)
		if(!uniqueCheck(b.base[i]))
			base.push_back(b.base[i]);
}

template <class T>
bool Vector<T>::push_front(T element)
{
	if (uniqueCheck(element))
		return false;
	base.push_front(element);
	return true;
}

template <class T>
bool Vector<T>::push_back(T element)
{
	if (uniqueCheck(element))
		return false;
	base.push_back(element);
	return true;
}

template <class T>
bool Vector<T>::add(T element, int index)
{
	if (index == -1)
		index = base.end();
	if (uniqueCheck(element))
		return false;
	base.insert(base.begin() + index, element);
	return true;
}

template <class T>
bool Vector<T>::uniqueCheck(T element)
{
	return mustBeUnique && contains(element);
}

