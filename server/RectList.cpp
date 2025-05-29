

// RectList implementation

#include "RectList.h"

bool
operator <(const RECT _X, const RECT _Y)
{
	return _X.top < _Y.top;
}

bool
operator >(const RECT _X, const RECT _Y)
{
	return _X.top > _Y.top;
}

bool
operator ==(const RECT &_X, const RECT &_Y)
{
	return (_X.left == _Y.left) &&
			(_X.right == _Y.right) &&
			(_X.top == _Y.top) &&
			(_X.bottom == _Y.bottom);
}

bool
operator !=(const RECT &_X, const RECT &_Y)
{
	return (_X.left != _Y.left) ||
			(_X.right != _Y.right) ||
			(_X.top != _Y.top) ||
			(_X.bottom != _Y.bottom);
}
