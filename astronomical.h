
#include <math.h>
#include <time.h>


//This was snagged from github, I forgot the source.......

class SunSet {
public:
	SunSet();
	SunSet(double, double, int);
	~SunSet();

	void setPosition(double lat, double lon, int tz);
	void setTZOffset(int);
	double setCurrentDate(int y, int m, int d);
	double calcSunriseUTC();
	double calcSunsetUTC();
	double calcSunrise();
	double calcSunset();
	int moonPhase(int);
	//int moonPhase();

private:
	double degToRad(double);
	double radToDeg(double);
	double calcMeanObliquityOfEcliptic(double);
	double calcGeomMeanLongSun(double);
	double calcObliquityCorrection(double);
	double calcEccentricityEarthOrbit(double);
	double calcGeomMeanAnomalySun(double);
	double calcEquationOfTime(double);
	double calcTimeJulianCent(double);
	double calcSunTrueLong(double);
	double calcSunApparentLong(double);
	double calcSunDeclination(double);
	double calcHourAngleSunrise(double, double);
	double calcHourAngleSunset(double, double);
	double calcJD(int, int, int);
	double calcJDFromJulianCent(double);
	double calcSunEqOfCenter(double);

	double latitude;
	double longitude;
	double julianDate;
	int m_year;
	int m_month;
	int m_day;
	int tzOffset;
};
