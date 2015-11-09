#include <iostream>
#include <Windows.h>
#include <vector>
using namespace std;

class MObject{
protected:
	virtual void _adjustDC(HDC hdc){
	}
	virtual void _render(HDC hdc) = 0;
public:
	MObject(){
	}
	virtual void draw(HDC hdc){
		_adjustDC(hdc);
		_render(hdc);
	}

	virtual void doHandleMove(HWND hWnd, POINT p){
	}

	virtual int getNVertex() = 0;
};

class MLine : public MObject{
private:
	POINT mA;
	POINT mB;
	void _render(HDC hdc){
		MoveToEx(hdc, mA.x, mA.y, NULL);
		LineTo(hdc, mB.x, mB.y);
	}
public:
	MLine(POINT A, POINT B):MObject(), mA(A), mB(B){
	}
	
	int getNVertex(){
		return 2;
	}
};

class MFreePen : public MObject{
private:
	vector<POINT> mList;
	void _render(HDC hdc){
	}
public:
	MFreePen(POINT p): MObject(), mList(vector<POINT>()){
		mList.push_back(p);
	}

	void doHandleMove(HWND hWnd, POINT p){
		HDC hdc = GetDC(hWnd);
		MLine(mList[mList.size()-1], p).draw(hdc);
		ReleaseDC(hWnd, hdc);
		mList.push_back(p);
	}

	int getNVertex(){
		return mList.size();
	}
};