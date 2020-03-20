#include"IDW.h"


IDW::IDW(std::vector<QPoint> origin_points_, std::vector<QPoint> target_points)
{
	p_points_ = origin_points_;
	q_points_ = target_points;
	int i = 0;
	while(i<origin_points_.size())
	{
		T.push_back(computeMatrix(i++));
	}
}

IDW::~IDW()
{

}

Eigen::Matrix2d IDW::computeMatrix(int i)
{
	double sigm;
	Eigen::MatrixXd A(2,2);
	A.setZero();
	Eigen::VectorXd x(2),y(2), Bx(2),By(2);                //solve Ax=b
	Bx.setZero();
	By.setZero();
	for (size_t j = 0; j < p_points_.size(); j++)
	{
		if (j != i)
		{
			sigm=1 / distSquare(p_points_[j], p_points_[i]);

			A(0,0) += sigm *(p_points_[j].x() - p_points_[i].x()) * (p_points_[j].x() - p_points_[i].x());
			A(0,1) += sigm * (p_points_[j].x() - p_points_[i].x()) * (p_points_[j].y() - p_points_[i].y());
			Bx(0) += sigm * (p_points_[j].x() - p_points_[i].x()) * (q_points_[j].x() - q_points_[i].x());

			A(1, 1) += sigm * (p_points_[j].y() - p_points_[i].y()) * (p_points_[j].y() - p_points_[i].y());
			A(1, 0) += sigm * (p_points_[j].y() - p_points_[i].y()) * (p_points_[j].x() - p_points_[i].x());
			Bx(1) += sigm * (p_points_[j].y() - p_points_[i].y()) * (q_points_[j].x() - q_points_[i].x());

			By(0) += sigm * (p_points_[j].x() - p_points_[i].x()) * (q_points_[j].y() - q_points_[i].y());
			By(1) += sigm * (p_points_[j].y() - p_points_[i].y()) * (q_points_[j].y() - q_points_[i].y());
		}//if j==i skip
	}
	x = A.colPivHouseholderQr().solve(Bx);
	y = A.colPivHouseholderQr().solve(By);
	Eigen::Matrix2d Tr;

	Tr(0, 0) = x(0);
	Tr(0, 1) = x(1);
	Tr(1, 0) = y(0);
	Tr(1, 1) = y(1);
	return Tr;
}

double IDW::distSquare(QPoint p, QPoint q)
{
	double dist =double((p.x() - q.x()) * (p.x() - q.x()) + (p.y() - q.y()) * (p.y() - q.y()));
	return dist;
}

double IDW::sigma(int i, QPoint p)
{
	double as;
	as = 1 / distSquare(p, p_points_[i]);
	return(as);
}

double IDW::sumsigma(QPoint p)
{
	double sum=0;
	int i = 0;
	while(i<p_points_.size())
	{
		sum += sigma(i++, p);
	}
	return sum;
}

double IDW::weight(int i, QPoint p)
{
	return(sigma(i, p) / sumsigma(p));
}

double IDW::localApprox(int i, QPoint p)
{
	double x;
	x = q_points_[i].x() + (T[i](0, 0)) * (p.x() - p_points_[i].x()) + (T[i](0, 1)) * (p.y() - p_points_[i].y());
	return x;
}

double IDW::localApproy(int i, QPoint p)
{
	double y;
	y = q_points_[i].y() + (T[i](1, 0)) * (p.x() - p_points_[i].x()) + (T[i](1, 1)) * (p.y() - p_points_[i].y());
	return y;
}

double IDW::mainfunctx(QPoint input_point)
{
	double x = 0;
	for(size_t i=0 ;i< p_points_.size();i++)
	{
		x += weight(i, input_point) * localApprox(i, input_point);
	}
	return x;
}

double IDW::mainfuncty(QPoint input_point)
{
	double y = 0;
	for (size_t i = 0; i < p_points_.size(); i++)
	{
		y += weight(i, input_point) * localApproy(i, input_point);
	}
	return y;
}

QPoint IDW::Mainfunction(QPoint input_point)
{
	QPoint output_point;
	double xsum = (mainfunctx(input_point));
	double ysum = (mainfuncty(input_point));
	return QPoint(int(xsum), int(ysum));
}