#ifndef LUASHARED_H
#define LUASHARED_H

class CLuaShared
{
	template<typename t>
	t get(int index)
	{
		return ((*(t*)this) + index);
	}

public:



};

#endif // LUASHARED_H