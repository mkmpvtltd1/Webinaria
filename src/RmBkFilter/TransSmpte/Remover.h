#pragma once

union color_s
{
	struct
	{
        byte r:8;
        byte g:8;
        byte b:8;
        byte a:8;
	};
	int RGBA;
};

struct color_HSV
{
        int H;
        int S;
        int V;
};

struct point
{
    int x;
    int y;
};

// This class provide changing background color of bitmap to transparent
class CRemover
{
public:
	// Default constructor
	CRemover(void);

	// Virtual destructor
	virtual ~CRemover(void);

	// Clear specified DIB
	static void ClearBack(char * bits, int width, int height);

protected:
	// Return hue of specification RGB color
	static int GetHValue(int ColorRGB);

	// Return saturation of specification RGB color
	static int GetSValue(int ColorRGB);

	// Return value of specification RGB color
	static int GetVValue(int ColorRGB);

	// Difference from two hue
	static int CRemover::HDistance(int H1, int H2);

	// Grow need region
	static int RegionGrow(int w, int h, int * mb, int * m, int is, int js, int c, color_HSV hsvc, int tn, int tmax);
};
