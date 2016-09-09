#ifndef IRunable_hpp
#define IRunable_hpp

class IRunable
{
public:
	virtual void init() = 0;
	virtual void run() = 0;
	virtual void clean() = 0;
};

#endif // IRunable_hpp