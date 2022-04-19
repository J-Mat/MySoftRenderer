#pragma once
#include <iostream>
#include<fstream>
#include <string>
#include "math_ext.h"
using namespace Math;
using namespace std;

#define DEBUG_INFO(...) printf(__VA_ARGS__)
#define DEBUG_VALUE(value) cout  << value << endl;
#define DEBUG_POS2(pos)    cout  << pos.x << " " << pos.y << endl;
#define DEBUG_POS3(pos)	   cout  << pos.x << " " << pos.y << " " << pos.z << endl;
#define DEBUG_POS4(pos)	   cout  << pos.x << " " << pos.y << " " << pos.z << " " << pos.w << endl;

#define OPENFILE  ofstream ofs;
#define OUTFOMAT  ofs.open("test.txt", ios::out);
#define CLOSEFILE ofs.close();

class Log
{
public:
	Log()
	{
		m_bUseFile = false;
		ofs.open("C:\\Users\\zjmn4182\\Desktop\\aaa\\01.txt", ios::out);
	};
	~Log() {};
	static Log* GetInstance()
	{
		if (s_pInstance == nullptr)  //判断是否第一次调用
		{
			s_pInstance = new Log();
		}
		return s_pInstance;
	}
	void OutString(string str)
	{
		if (m_bUseFile)
			ofs << str;
		else
			cout << str;
	}

	void OutMat(mat4 mat)
	{
		if (m_bUseFile)
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					ofs << mat[i][j] << " ";
				}
				ofs << "\n";
			}
		else
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					cout << mat[i][j] << " ";
				}
				cout << "\n";
			}
	}


	template <typename T>
	void OutValue(T str)
	{
		if (m_bUseFile)
			ofs << str << endl;
		else
			cout << str << endl;
	}

	void Out_Pos2(vec2 pos)
	{
		if (m_bUseFile)
			ofs << pos.x << " " << pos.y << endl;
		else
			cout << pos.x << " " << pos.y << endl;
	}

	void Out_Pos3(vec3 pos)
	{
		if (m_bUseFile)	
			ofs << pos.x << " " << pos.y << " " << pos.z << endl;
		else
			cout << pos.x << " " << pos.y << " " << pos.z << endl;

	}

	void Out_Pos4(vec4 pos)
	{
		if (m_bUseFile)	
			ofs << pos.x << " " << pos.y << " " << pos.z << " " << pos.w << endl;
		else
			cout << pos.x << " " << pos.y << " " << pos.z << " " << pos.w << endl;
	}
	
	void CloseFile()
	{
		if (m_bUseFile)
		{
			ofs.close();
		}
	}

private:
	static Log* s_pInstance;
	bool m_bUseFile = true;
	ofstream ofs;
};