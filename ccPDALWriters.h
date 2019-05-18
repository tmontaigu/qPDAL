#ifndef CLOUDCOMPAREPROJECTS_CCPDALWRITERS_H
#define CLOUDCOMPAREPROJECTS_CCPDALWRITERS_H

#include <pdal/Writer.hpp>
#include <pdal/Streamable.hpp>


#include <ccPointCloud.h>



class ccPointCloudStreamWriter : public pdal::Writer, public pdal::Streamable {
public:
	ccPointCloudStreamWriter(ccPointCloud *mCloud) : m_cloud(mCloud)
	{}

	std::string getName() const override
	{
		return "readers.ccPointCloudStreamWriter";
	}

private:
	bool processOne(pdal::PointRef &point) override
	{
		CCVector3 p(
				point.getFieldAs<PointCoordinateType>(pdal::Dimension::Id::X),
				point.getFieldAs<PointCoordinateType>(pdal::Dimension::Id::Y),
				point.getFieldAs<PointCoordinateType>(pdal::Dimension::Id::Z)
		);
		m_cloud->addPoint(p);
		return true;
	}

private:
	ccPointCloud *m_cloud;
};


#endif //CLOUDCOMPAREPROJECTS_CCPDALWRITERS_H
