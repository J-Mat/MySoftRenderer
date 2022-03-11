#pragma once
#include <iostream>
using namespace std;

#define DEBUG_INFO(...) printf(__VA_ARGS__)
#define DEBUG_POS2(pos) cout  << pos.x << " " << pos.y << endl;
#define DEBUG_POS3(pos) cout  << pos.x << " " << pos.y << " " << pos.z << endl;
#define DEBUG_POS4(pos) cout  << pos.x << " " << pos.y << " " << pos.z << " " << pos.w << endl;