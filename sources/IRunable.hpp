#ifndef IRunable_hpp
#define IRunable_hpp

class IRunable
{
public:
	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void Clean() = 0;
};

#endif // IRunable_hpp