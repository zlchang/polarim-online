#ifndef HCUT_H
#define HCUT_H

#define LARGE_VALUE	(1.0E50)

class HCut {
private:
    float fMin;
    float fMax;
public:
//	Inline functions
    HCut(double min, double max) {
	fMin = min;
	fMax = max;
    };
    void Set(double min, double max) {
	fMin = min;
	fMax = max;
    };
    double Min(double min) {
	fMin = min;
	return fMin;
    };
    double Max(double max) {
	fMax = max;
	return fMax;
    };
    double Min(void) {
	return fMin;
    };
    double Max(void) {
	return fMax;
    };
    void Get(double *min, double *max) {
	*min = fMin;
	*max = fMax;
    };
    int Cut(double value) {
	return (value > fMin && value < fMax);
    };
//	Other functions
    void Draw(int type = 0);
    void Print(const char *name);
};

#endif /* HCUT_H */
