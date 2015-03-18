
#include <windows.h>
#include "Remover.h"

// Default constructor
CRemover::CRemover(void)
{
}

// Virtual destructor
CRemover::~CRemover(void)
{
}

//Clear specified DIB
void CRemover::ClearBack(char * bits, int width, int height)
{
	int W = width,
		H = height;

	color_s BGColor;
	BGColor.RGBA = 0x00000000;

	color_s * m = new color_s[W*H];
	color_s * mt = new color_s[W*H];
	color_s * mr = new color_s[W*H];
	color_s * mi = new color_s[W*H];

	memcpy(m, bits, W*H*4);

	int c = 0, x;

	// Minimal threashold
	int TMin = 30,
	// Maximum threashold
		TMax = 100;
	// Neighbor threashold
	int TN = 45;

	int Hc,S,V;

	int pt;

	bool R = 1;

	color_HSV CMin;

	CMin.H = GetHValue(m[(H - 10)*W + 10].RGBA);
	CMin.S = GetSValue(m[(H - 10)*W + 10].RGBA);
	CMin.V = GetVValue(m[(H - 10)*W + 10].RGBA);

	//----------- Color Seading -------------------
	int i;
	for( i = 0; i < H; i++)
		for(int j = 0; j < W; j++)
		{
			x = i * W + j;
			pt = m[x].RGBA;
			Hc = GetHValue(pt);
			S = GetSValue(pt);
			V = GetVValue(pt);
			if(R)
				mr[x].RGBA = 0;
			else
				mr[x].RGBA = 255;

			if(HDistance(Hc,CMin.H) < TMin)
				if(abs(S - CMin.S) < TMin)
					if(abs(V - CMin.V) < TMin)
					{
						if(R)
							mr[x].RGBA = 255;
						else
							mr[x].RGBA = 0;
					}
		}
	//-------- End Color Seading -----------

	//----------- Fast Color Region Growing -------------------
	int CCount = 2;
	bool Siloette = 0;
	for(i = 0; i < H; i++)
		for(int j = 0; j < W; j++)
		{
			if(mr[i * W + j].RGBA == 0)
				mr[i * W + j].RGBA = 0;
			else 
				mr[i * W + j].RGBA = 1;
		}

	int l, lmax = 0, nmax;

	for(i = 1;i < H - 1; i++)
		for(int j = 1; j < W - 1; j++)
			if(mr[i*W+j].RGBA==1)
			{
				l = RegionGrow(W, H, (int*)mr, (int*)m, i, j, CCount, CMin, TN, TMax);
				if(l > lmax)
				{
					lmax = l;
					nmax = CCount;
				}
				CCount++;
			}

	Siloette=0;
	//----------- End Fast Color Region Growing -------------------

	//----------- Anti-Aliasing -------------------
	//Filtration
	for(i = 1; i < H - 1; i++)
		for(int j = 1; j < W - 1; j++)
		{
			l = 0;
			for(int i1 = i - 1; i1 <= i + 1; i1++)
				for(int j1 = j - 1; j1 <= j + 1; j1++)
					if(mr[i1 * W + j1].RGBA == 0)
						l++;
			if(l > 5)
				mi[i * W + j].RGBA = -1;
			else
				mi[i * W + j].RGBA = 0;
	}

	for(i = 1; i < H - 1; i++)
		for(int j = 1; j < W - 1; j++)
			mr[i * W + j].RGBA = mt[i * W + j].RGBA = mi[i * W + j].RGBA;

	//Horisontal 
	for(int j = 1; j < W - 1; j++)
	{
		for(i = 1;i < H - 1; i++)
		{
			if(mt[i * W + j].RGBA == -1)
			{
				if(mt[(i - 1) * W + j].RGBA == 0)
				{
					if(mt[i * W + j - 1].RGBA != -1)
						mt[i * W + j].RGBA = mt[i * W + (j - 1)].RGBA + 1;
					else
						mt[i * W + j].RGBA = 1;
				}
			}
		}
		for(i = H - 2; i > 1; i--)
		{
			if(mt[i * W + j].RGBA == -1)
			{
				if(mt[(i + 1) * W + j].RGBA == 0)
				{
					if(mt[i * W + j - 1].RGBA != -1)
						mt[i * W + j].RGBA = mt[i * W + (j - 1)].RGBA + 1;
					else
						mt[i * W + j].RGBA = 1;
				}
			}
		}
	}

	c = 1;
	int cl;
	for(i = H - 2; i > 1; i--)
		for(int j = W - 2; j > 1; j--)
		{
			if(mt[i * W + j].RGBA > 0)
				if(mt[i * W + j + 1].RGBA == 0)
				{
					c = mt[i * W + j].RGBA + 1;
					while(mt[i * W + j].RGBA > 0 && j > 1)
					{
						mi[i * W + j].RGBA = (255 - mt[i * W +j].RGBA * 255 / c);
						j--;
					}
			}

			if(mt[i * W + j].RGBA > 0)
				if(mt[i * W + j + 1].RGBA == -1)
				{
					c = mt[i * W + j].RGBA + 1;
					while(mt[i * W + j].RGBA > 0 && j > 1)
					{
						mi[i * W + j].RGBA = (mt[i * W + j].RGBA * 255 / c);
						j--;
					}
				}

			if(mt[i * W + j].RGBA == -1)
				mi[i * W + j].RGBA = 255;
			if(mt[i * W + j].RGBA == 0)
				mi[i * W + j].RGBA = 0;
	}

	//Vertical
	for(i = 1;i < H - 1; i++)
	{
		int j;
		for(j = 1; j < W - 1; j++)
		{
			if(mr[i * W + j].RGBA == -1)
			{
				if(mr[i * W + j - 1].RGBA == 0)
				{
					if(mr[(i - 1) * W + j].RGBA != -1)
						mr[i * W + j].RGBA = mr[(i - 1) * W + j].RGBA + 1;
					else
						mr[i * W + j].RGBA = 1;
				}
			}
		}
		for(j = W - 2; j > 1; j--)
		{
			if(mr[i * W + j].RGBA == -1)
			{
				if(mr[i * W + j + 1].RGBA == 0)
				{
					if(mr[(i - 1) * W + j].RGBA != -1)
						mr[i * W + j].RGBA = mr[(i - 1) * W + j].RGBA + 1;
					else
						mr[i * W + j].RGBA = 1;
				}
			}
		}
	}

	int j;

	for(j = W - 2; j > 1; j--)
		for(int i = H - 2; i > 1; i--)
		{
			if(mr[i * W + j].RGBA > 0)
				if(mr[(i + 1) * W + j].RGBA == 0)
				{
					c = mr[i * W + j].RGBA + 1;
					while(mr[i * W + j].RGBA > 0 && i > 1)
					{
						cl = (255 - mr[i * W + j].RGBA * 255 / c);
						if(mi[i * W + j].RGBA > cl)
							mi[i * W + j].RGBA = cl;
						i--;
					}
				}
			if(mr[i * W + j].RGBA > 0)
				if(mr[(i + 1) * W + j].RGBA == -1)
				{
					c = mr[i * W + j].RGBA + 1;
					while(mr[i * W + j].RGBA > 0 && i > 1)
					{
						cl = (mr[i * W + j].RGBA * 255 / c);
						if(mi[i * W + j].RGBA > cl)
							mi[i * W + j].RGBA = cl;
						i--;
					}
				}
	}
	//----------- End Anti-Aliasing -------------------

	//----------- Applying Mask -------------------
	for(i = 0; i < H; i++)
		for(int j = 0; j < W; j++)
		{
			l = mi[i * W + j].RGBA;
			if(l > 255) 
				l = 255;
			m[i * W + j].a = l;
		}
    //----------- End Applying Mask -------------------

	// Clear borders
	/*for(i = 0; i < H; i++)
	{
		m[i * W ] = BGColor;
		m[(i + 1) * W - 1] = BGColor;
	}
	for(j = 0; j < W; j++)
	{
		m[j] = BGColor;
		m[(H - 1) * W + j] = BGColor;
	}*/

	memcpy(bits, m, W*H*4);
}

// Return hue of specification RGB color
int CRemover::GetHValue(int ColorRGB)
{
	int H = 180;

	int r = GetRValue(ColorRGB);
	int g = GetGValue(ColorRGB);
	int b = GetBValue(ColorRGB);

	if(r > g)
	{
		if(g > b)		//rgb
		{     
			if(r != b)
				H = (g - b) * 60 / (r - b);
		} 
		else if(r > b)	//rbg
		{
			if(r != g)
				H = 360 - (b - g) * 60 / (r - g);
		} else			//brg
		{
			if(g != b)
				H = 240 + (r - g) * 60 / (b - g);
		}
	}
	else
	{					//g > r
		if(r > b)		//grb
		{   
			if(g != b)
				H = 120 - (r - b) * 60 / (g - b);
		} 
		else if(g > b)	//gbr
		{
			if(g != r)
			H = 120 + (b - r) * 60 / (g - r);
		} 
		else			//bgr
		{          
			if(r != b)
			H = 240 - (g - r) * 60 / (b - r);
		}
	}
	
	return H;
}

// Return saturation of specification RGB color
int CRemover::GetSValue(int ColorRGB)
{
        int S = 0;

        int r = GetRValue(ColorRGB);
        int g = GetGValue(ColorRGB);
        int b = GetBValue(ColorRGB);

        if(r > g)
		{
            if(g > b)		//rgb
			{
                S = 100 - (int)(100 * (float)b / (float)r);
            }
			else if(r > b)	//rbg
			{   
                S = 100 - (int)(100 * (float)g / (float)r);
            }
			else			//brg
			{          
                S = 100 - (int)(100 * (float)g / (float)b);
            }
        }
        else				//g>r
		{         
            if(r > b)		//grb
			{
                S = 100 - (int)(100 * (float)b / (float)g);
            } 
			else if(g > b)	//gbr
			{   
				S = 100 - (int)(100 * (float)r / (float)g);
            } 
			else			//bgr
			{
				S = 100 - (int)(100 * (float)r / (float)b);
            }
        }
        return S;
}

// Return value of specification RGB color
int CRemover::GetVValue(int ColorRGB)
{
        int V = 0;

        int r = GetRValue(ColorRGB);
        int g = GetGValue(ColorRGB);
        int b = GetBValue(ColorRGB);

        if(r > g)
		{
            if(g>b)			//rgb
			{     
				V = 100 * r / 255;
            } 
			else if(r > b)	//rbg
			{
				V = 100 * r / 255;
            } 
			else			//brg
			{
				V = 100 * b / 255;
            }
        }
        else				//g>r
		{         
            if(r > b)		//grb
			{   
				V = 100 * g / 255;
            } 
			else if(g > b)	//gbr
			{
				V = 100 * g / 255;
            }
			else			//bgr
			{
				V = 100 * b / 255;
            }
        }
        return V;
}

// Difference from two hue
int CRemover::HDistance(int H1, int H2)
{
    if( abs(H2 - H1) > 180)
		return 360 - abs(H2 - H1);
    else 
		return abs(H2 - H1);
}

// Grow need region
int CRemover::RegionGrow(int w, int h, int * mb, int * m, int is, int js, int c, color_HSV hsvc, int tn, int tmax)
{
    point * P = new point[h * w];
    int PCount = 1,
		PCurrent = 0;
    int e, emax;
    mb[is * w + js] = c;
    P[0].y = is;
	P[0].x = js;
   
	color_HSV hsvp, hsvo;

    while(PCurrent < PCount)
	{
        hsvp.H = GetHValue(m[(P[PCurrent].y) * w + P[PCurrent].x]);
        hsvp.S = GetSValue(m[(P[PCurrent].y) * w + P[PCurrent].x]);
        hsvp.V = GetVValue(m[(P[PCurrent].y) * w + P[PCurrent].x]);

		if((P[PCurrent].y > 0) && (P[PCurrent].y < h - 1) && (P[PCurrent].x > 0) && (P[PCurrent].x < w-1))
		{
			for(int i = P[PCurrent].y - 1; i <= P[PCurrent].y + 1; i++)
				for(int j = P[PCurrent].x - 1; j <= P[PCurrent].x + 1; j++)
				{
					hsvo.H = GetHValue(m[i * w + j]);
					hsvo.S = GetSValue(m[i * w + j]);
					hsvo.V = GetVValue(m[i * w + j]);
					emax =	abs(hsvp.H - hsvc.H) + 
							abs(hsvp.S - hsvc.S) +
							abs(hsvp.V - hsvc.V);
					e = abs(hsvp.H - hsvo.H) +
						abs(hsvp.S - hsvo.S) +
						abs(hsvp.V - hsvo.V);
					if((mb[i * w + j] == 1) || (e < tn) && (emax < tmax) && (mb[i * w + j] == 0))
					{
							mb[i * w + j] = c;
							P[PCount].y = i;
							P[PCount].x = j;
							PCount++;
					}
				}
		}
		PCurrent++;
    }
    delete P;
    return PCount;
}