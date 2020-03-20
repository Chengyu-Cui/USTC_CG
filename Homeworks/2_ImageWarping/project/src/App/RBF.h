#pragma once

#ifndef RBF_H
#define RBF_H

#include <QPoint>
#include <vector>
#include<Eigen/Dense>
#include<math.h>

using namespace Eigen;

class RBF
{
public:
	RBF(std::vector<QPoint> origin_points_, std::vector<QPoint> target_points);
	~RBF();

public:
	QPoint Mainfunction(QPoint input_point);

private:
	void computeCoefficient();                          //set Matrix Ax
	void computeMindistance();
	double distSquare(QPoint p, QPoint q);                  //compute distance

private:
	std::vector<QPoint> p_points_;                       //origin points p
	std::vector<QPoint> q_points_;                       //movement target q
	std::vector<double> xcoef;
	std::vector<double> ycoef;
	std::vector<double> mindist;
	double miu;
};

#endif