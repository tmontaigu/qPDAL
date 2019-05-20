#ifndef CLOUDCOMPAREPROJECTS_CCPDALWRITERS_H
#define CLOUDCOMPAREPROJECTS_CCPDALWRITERS_H

#include <pdal/Writer.hpp>
#include <pdal/Streamable.hpp>


#include <ccPointCloud.h>

static const pdal::PluginInfo s_CloudStreamWriterInfo
		{
				"writers.ccPointCloudStreamWriter",
				"Get the result of a PDAL pipeline back into a ccPointCloud",
				""
		};


class ccPointCloudStreamWriter : public pdal::Writer, public pdal::Streamable
{
public:
	ccPointCloudStreamWriter(ccPointCloud *mCloud) : m_cloud(mCloud)
	{}

	ccPointCloudStreamWriter() = default;

	std::string getName() const override
	{
		return "readers.ccPointCloudStreamWriter";
	}

private:
	bool processOne(pdal::PointRef &point) override
	{
		if (!m_cloud) {
			return  false;
		}
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

CREATE_STATIC_STAGE(ccPointCloudStreamWriter, s_CloudStreamWriterInfo);

#endif //CLOUDCOMPAREPROJECTS_CCPDALWRITERS_H
