#include"RBF.h"


RBF::RBF(std::vector<QPoint> origin_points_, std::vector<QPoint> target_points)
{
	miu = 0.5;
	p_points_ = origin_points_;
	q_points_ = target_points;
	computeMindistance();
	computeCoefficient();
}

RBF::~RBF()
{

}

void RBF::computeCoefficient()
{
	Eigen::MatrixXd A(p_points_.size(), p_points_.size());
	Eigen::VectorXd x(p_points_.size());
	Eigen::VectorXd bx(p_points_.size());
	Eigen::VectorXd y(p_points_.size());
	Eigen::VectorXd by(p_points_.size());
	for (size_t i = 0; i < p_points_.size(); i++)
	{
		for (size_t j = 0; j < p_points_.size(); j++)
		{
			A(i, j) = pow((distSquare(p_points_[i], p_points_[j]) + mindist[j]),miu);
		}
		bx(i) = q_points_[i].x() - p_points_[i].x();
		by(i) = q_points_[i].y() - p_points_[i].y();
	}

	x = A.colPivHouseholderQr().solve(bx);
	y = A.colPivHouseholderQr().solve(by);
	for (size_t i = 0; i < p_points_.size(); i++)
	{
		xcoef.push_back(x(i));
		ycoef.push_back(y(i));
	}
}

void RBF::computeMindistance()
{
	double dist;
	if (p_points_.size() <= 1)
	{
		mindist.push_back(0);
	}
	else
	{
		for (size_t i = 0; i < p_points_.size(); i++)
		{
			dist = -1;
			for (size_t j = 0; j < p_points_.size(); j++)
			{
				if (i != j)
				{
					if ((dist > distSquare(p_points_[i], p_points_[j])) || dist == -1)
					{
						dist = distSquare(p_points_[i], p_points_[j]);
					}
				}
			}
			mindist.push_back(dist);
		}
	}
}

double RBF::distSquare(QPoint p, QPoint q)
{
	double dist = (p.x() - q.x()) * (p.x() - q.x()) + (p.y() - q.y()) * (p.y() - q.y());
	return dist;
}

QPoint RBF::Mainfunction(QPoint input_point)
{
	double xsum = 0;
	double ysum = 0;
	for (size_t i = 0; i < p_points_.size(); i++)
	{
		xsum += xcoef[i] * pow((distSquare(input_point, p_points_[i]) + mindist[i]),miu);
		ysum += ycoef[i] * pow((distSquare(input_point, p_points_[i]) + mindist[i]),miu);
	}
	xsum += input_point.x();
	ysum += input_point.y();
	return QPoint(int(xsum), int(ysum));
}

