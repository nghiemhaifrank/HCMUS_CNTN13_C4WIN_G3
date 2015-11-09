#include "MObject.h"

struct MPath
{
	vector<int> mList;
	WCHAR * mDir;
	MPath():mList(vector<int>()), mDir(NULL){
	}

	bool operator == (const vector<int> & path){
		if (mList.size() != path.size())
			return false;
		int n = mList.size();
		for(int i = 0; i < n; i++)
			if (mList[i] != path[i])
				return false;
		return true;
	}
};

class MData{
public:
	const static POINT PERR;
	const static int L = 40;
	const static int l = 30;
	const static int N = 3;
private:
	POINT mLTPoint;
	POINT mPrevMapP;
	MObject * mCurObj;
	vector<int> mCurPath;
	vector<MPath *> mPaths;
	static MData * instance;
	MData(){
		mLTPoint.x = 0; 
		mLTPoint.y = 0;
		mCurPath = vector<int>();
		MPath * path = new MPath();
		path->mList.push_back(0);
		path->mList.push_back(1);
		path->mList.push_back(2);
		path->mList.push_back(5);
		path->mDir = new WCHAR[255];
		wsprintf(path->mDir, L"calc.exe");
		mPaths.push_back(path);
	}
public:
	static MData * getInstance(){
		if (!instance)
			instance = new MData();
		return instance;
	}

	static void destroy(){
		if (instance)
			delete instance;
	}

	~MData(){
		int n = mPaths.size();
		for (int i = 0; i < n; i++)
			delete mPaths[i];
	}

	MObject * getObj(){
		return mCurObj;
	}

	void setObj(MObject * obj){
		if (mCurObj)
		{
			delete mCurObj;
			mCurPath.clear();
		}
		mCurObj = obj;
	}

	void addMapPoint(POINT p){
		int d = p.y*N + p.x;
		int n = mCurPath.size();
		if (n != 0 && d == mCurPath[n-1])
			return;
		mCurPath.push_back(d);
	}

	bool isMapPoint(POINT p){
		if (p.x < 0 || p.y < 0 || p.x >= N || p.y >= N)
			return false;
		return true;
	}

	POINT trans2MapP(POINT p){
		int LL = L + 2*l;
		int X = (p.x - mLTPoint.x)/LL;
		int Y = (p.y - mLTPoint.y)/LL;
		POINT MP = {X, Y};
		if (!isMapPoint(MP))
			return PERR;
		if (p.x < X*LL + l || p.y < Y*LL 
			|| p.x >= X*LL + l + L
			|| p.y >= Y*LL + l + L)
			return PERR;
		return MP;
	}

	WCHAR * findSymbol(){
		int n = mPaths.size();
		for (int i = 0; i < n; i++)
			if ((*mPaths[i]) == mCurPath)
				return mPaths[i]->mDir;
		return NULL;
	}

	bool addCurPath(WCHAR * dir){
		if (findSymbol())
			return false;
		if (mCurPath.size() < 3)
			return false;
		MPath * path = new MPath();
		path->mDir = dir;
		path->mList = mCurPath;
		mPaths.push_back(path);
		return true;
	}

	int getNPaths(){
		return mPaths.size();
	}

	WCHAR * getPreviewPath(int pos){
		if (pos < 0 || pos >= mPaths.size())
			return NULL;
		int n = mPaths[pos]->mList.size()*2;
		WCHAR * buff = new WCHAR[n];
		for (int i = 0; i < n-1; i++)
			if (i%2)
				buff[i] = L'-';
			else
				buff[i] = mPaths[pos]->mList[i/2] + L'0';
		buff[n-1] = L'\0';
		return buff;
	}

	WCHAR * getDir(int pos){
		if (pos < 0 || pos >= mPaths.size())
			return NULL;
		return mPaths[pos]->mDir;
	}

	void delPath(int pos){
		if (pos < 0 || pos >= mPaths.size())
			return;
		mPaths.erase(mPaths.begin() + pos);
	}

	void SavePathToFile(char * file){
	}

	void LoadPathFromFile(char * file){
	}
};

MData * MData::instance = NULL;
const POINT MData::PERR = {-1, -1};