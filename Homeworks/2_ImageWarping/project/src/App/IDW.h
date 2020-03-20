#pragma once

#ifndef IDW_H
#define IDW_H

#include <QPoint>
#include <vector>
#include<Eigen/Dense>
#include<math.h>

using namespace Eigen;

class IDW
{
public:
	IDW(std::vector<QPoint> origin_points_, std::vector<QPoint> target_points);
	~IDW();
	double weight(int i, QPoint p);                    //comute weight
	double localApprox(int i, QPoint p);               //compute x component of local approximation
	double localApproy(int i, QPoint p);               //compute y component of local approximation
	double mainfunctx(QPoint input_point);             //connector for ImageWidget.cpp:x component
	double mainfuncty(QPoint input_point);             //connector for ImageWidget.cpp:y component

public:
	QPoint Mainfunction(QPoint input_point);

private:
	Eigen::Matrix2d computeMatrix(int i);              //set Matrix T
	double distSquare(QPoint p, QPoint q);             //compute distance
	double sigma(int i, QPoint p);                     //compute sigma
	double sumsigma(QPoint p);                         //compute the some of all sigma
	
private:
	std::vector<QPoint> p_points_;                   //origin points p
	std::vector<QPoint> q_points_;                   //movement target q
	std::vector<Eigen::Matrix2d> T;      //gredient of loacal approximation
	int R;
	double miu;
};

#endif 