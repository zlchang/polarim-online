#include <stdio.h>
#include <TLine.h>
#include "HCut.h"

void HCut::Draw(int type)
{
    TLine l;
    l.SetLineColor(2);
    l.SetLineWidth(2);
    switch (type) {
    case 0:	// X-cuts
	l.DrawLine(fMin, -LARGE_VALUE, fMin, LARGE_VALUE);
	l.DrawLine(fMax, -LARGE_VALUE, fMax, LARGE_VALUE);
	break;
    case 1:	// Y-cuts
	l.DrawLine(-LARGE_VALUE, fMin, LARGE_VALUE,  fMin);
	l.DrawLine(-LARGE_VALUE, fMax, LARGE_VALUE,  fMax);
	break;
    }
}

void HCut::Print(const char *name) 
{
    printf("%f < %s < %f\n", fMin, name, fMax);
}
