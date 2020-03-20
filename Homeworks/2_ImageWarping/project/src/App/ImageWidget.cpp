#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	draw_status_ = false;
}


ImageWidget::~ImageWidget(void)
{
	delete ptr_image_;
	delete ptr_image_backup_;
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_); 

	painter.end();
	if (!origin_points_.empty()&&origin_points_.size()== target_points_.size())
	{
		QPainter pp;
		pp.begin(this);
		for (size_t i = 0; i < origin_points_.size(); i++)
		{
			QPen origin_pen_;
			QPen target_pen_;
			QPen link_line_;
			origin_pen_.setWidth(6);
			target_pen_.setWidth(6);
			link_line_.setWidth(3);
			origin_pen_.setColor(Qt::yellow);
			target_pen_.setColor(Qt::yellow);
			link_line_.setColor(Qt::white);
			pp.setPen(origin_pen_);
			pp.drawPoint(origin_points_[i]);
			pp.setPen(target_pen_);
			pp.drawPoint(target_points_[i]);
			pp.setPen(link_line_);
			pp.drawLine(origin_points_[i], target_points_[i]);
		}
		if (draw_status_)
		{
			pp.drawLine(start_point_, end_point_);
		}
		pp.end();
	}
}

void ImageWidget::Open()
{
	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(ptr_image_) = *(ptr_image_backup_);
	origin_points_.erase(origin_points_.begin(), origin_points_.end());
	target_points_.erase(target_points_.begin(), target_points_.end());
	update();
}

void ImageWidget::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button())
	{
		draw_status_ = true;
		start_point_ = end_point_ = event->pos();
		update();
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* event)
{
	end_point_ = event->pos();
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (draw_status_)
	{
		origin_points_.push_back(start_point_);
		target_points_.push_back(end_point_);
		draw_status_ = false;
		update();
	}
	update();
}

 void ImageWidget::start_IDW()
{
	IDW idw(origin_points_,target_points_);
	//initiate class idw
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();
	int** fill_status_ = new int* [width];
	int i, j;
	for (i = 0; i < width; i++)
	{
		fill_status_[i] = new int[height]();
	}
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			fill_status_[i][j] = 0;
		}
	}
	for (i=0; i <width; i++)
	{
		for (j=0; j < height; j++)
		{
			QRgb color = image_tmp.pixel(i, j);
			//QPoint p(i,j);
			QPoint p = idw.Mainfunction(QPoint(i, j));
			if (p.x() >= 0 && p.x() < width && p.y() >= 0 && p.y() < height)
			{
				if (!fill_status_[p.x()][p.y()])
				{
					ptr_image_->setPixel(p.x(), p.y(), color);
					fill_status_[p.x()][p.y()] = 1;
				}
			}
		}
	}
	update();
}

void ImageWidget::start_RBF()
{
	RBF rbf(origin_points_, target_points_);
	//initiate class idw
	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();
	int** fill_status_ = new int* [width];
	int i, j;
	for (i = 0; i < width; i++)
	{
		fill_status_[i] = new int[height]();
	}
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			fill_status_[i][j] = 0;
		}
	}
	for (i=0; i < width; i++)
	{
		for (j=0; j < height; j++)
		{
			QRgb color = image_tmp.pixel(i, j);
			QPoint p=rbf.Mainfunction(QPoint(i,j));
			if (p.x() >= 0 && p.x() < width && p.y() >= 0 && p.y() < height)
			{
				if (!fill_status_[p.x()][p.y()])
				{
					ptr_image_->setPixel(p.x(), p.y(), color);
					fill_status_[p.x()][p.y()] = 1;
				}
			}
		}
	}
	update();
}