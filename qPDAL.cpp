//##########################################################################
//#                                                                        #
//#                CLOUDCOMPARE PLUGIN: qPDAL                      #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 of the License.               #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#                             COPYRIGHT: XXX                             #
//#                                                                        #
//##########################################################################

// First:
//	Replace all occurrences of 'qPDAL' by your own plugin class name in this file.
//	This includes the resource path to info.json in the constructor.

// Second:
//	Open qPDAL.qrc, change the "prefix" and the icon filename for your plugin.
//	Change the name of the file to <yourPluginName>.qrc

// Third:
//	Open the info.json file and fill in the information about the plugin.
//	 "type" should be one of: "Standard", "GL", or "I/O" (required)
//	 "name" is the name of the plugin (required)
//	 "icon" is the Qt resource path to the plugin's icon (from the .qrc file)
//	 "description" is used as a tootip if the plugin has actions and is displayed in the plugin dialog
//	 "authors", "maintainers", and "references" show up in the plugin dialog as well

#include <map>

#include <QtGui>
#include <QEventLoop>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

#include <pdal/filters/RangeFilter.hpp>
#include <pdal/PluginManager.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/PipelineManager.hpp>

#include <json/json.h>

#include "qPDAL.h"
#include "ccPDALReaders.h"
#include "ccPDALWriters.h"
#include "ListSelectionDialog.h"


bool startsWith(const std::string &str, const std::string &startStr)
{
	if (str.size() < startStr.size())
	{
		return false;
	}
	return str.substr(0, startStr.size()) == startStr;
}


std::vector<QString> askForSelection(const std::vector<QString> &choices)
{
	QEventLoop loop;
	ListSelectionDialog dm(choices);
	dm.show();
	QObject::connect(&dm, &ListSelectionDialog::finished, &loop, &QEventLoop::quit);
	loop.exec();
	return dm.checkedItems();
}

// Default constructor:
//	- pass the Qt resource path to the info.json file (from <yourPluginName>.qrc file) 
//  - constructor should mainly be used to initialize actions and other members
qPDAL::qPDAL(QObject *parent)
		: QObject(parent), ccStdPluginInterface(":/CC/plugin/qPDAL/info.json"), m_action(nullptr)
{
}

// This method should enable or disable your plugin actions
// depending on the currently selected entities ('selectedEntities').
void qPDAL::onNewSelection(const ccHObject::Container &selectedEntities)
{
	if (m_action == nullptr)
	{
		return;
	}

	// If you need to check for a specific type of object, you can use the methods
	// in ccHObjectCaster.h or loop and check the objects' classIDs like this:
	//
	//	for ( ccHObject *object : selectedEntities )
	//	{
	//		if ( object->getClassID() == CC_TYPES::VIEWPORT_2D_OBJECT )
	//		{
	//			// ... do something with the viewports
	//		}
	//	}

	// For example - only enable our action if something is selected.
	//m_action->setEnabled(!selectedEntities.empty());
	m_action->setEnabled(true);
}

// This method returns all the 'actions' your plugin can perform.
// getActions() will be called only once, when plugin is loaded.
QList<QAction *> qPDAL::getActions()
{
	// default action (if it has not been already created, this is the moment to do it)
	if (!m_action)
	{
		// Here we use the default plugin name, description, and icon,
		// but each action should have its own.
		m_action = new QAction("PDAL filter", this);
		m_action->setToolTip("Apply a PDAL filter to the selected cloud");
		m_action->setIcon(getIcon());

		// Connect appropriate signal
		connect(m_action, &QAction::triggered, this, &qPDAL::doAction);
	}

	if (!m_executePipeline)
	{
		// Here we use the default plugin name, description, and icon,
		// but each action should have its own.
		m_executePipeline = new QAction("Execute Pipeline", this);
		m_executePipeline->setToolTip("PDAL pipeline");
		m_executePipeline->setIcon(getIcon());

		// Connect appropriate signal
		connect(m_executePipeline, &QAction::triggered, this, &qPDAL::executePipeline);
	}

	return {m_action, m_executePipeline};
}

// This is an example of an action's method called when the corresponding action
// is triggered (i.e. the corresponding icon or menu entry is clicked in CC's
// main interface). You can access most of CC's components (database,
// 3D views, console, etc.) via the 'm_app' variable (see the ccMainAppInterface
// class in ccMainAppInterface.h).
void qPDAL::doAction()
{
	if (m_app == nullptr)
	{
		// m_app should have already been initialized by CC when plugin is loaded
		Q_ASSERT(false);

		return;
	}

	if (m_app->getSelectedEntities().empty() || m_app->getSelectedEntities().front()->getClassID() != CC_TYPES::POINT_CLOUD) {
		return;
	}

	// Force plugin loading.
	pdal::StageFactory f(false);

	pdal::StringList allStageNames = pdal::PluginManager<pdal::Stage>::names();
	pdal::StringList filtersNames;
	std::copy_if(
			allStageNames.begin(),
			allStageNames.end(),
			std::back_inserter(filtersNames),
			[](const std::string &stageName)
			{ return startsWith(stageName, "filters."); }
	);

	std::vector<QString> qFiltersNames;
	qFiltersNames.reserve(filtersNames.size());
	std::transform(
			filtersNames.begin(),
			filtersNames.end(),
			std::back_inserter(qFiltersNames),
			[](const std::string &filterName)
			{ return QString::fromStdString(filterName); }
	);

	std::vector<QString> selectedStages = askForSelection(qFiltersNames);
	if (selectedStages.empty())
	{
		m_app->dispToConsole("Select a stage", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}



	std::string selectedStage = selectedStages.front().toStdString();
	m_app->dispToConsole(QString("Stage Selected: %1").arg(selectedStages.front()));

	//std::unique_ptr<pdal::Stage> s = std::unique_ptr<pdal::Stage>(f.createStage(selectedStage));
	pdal::Stage *s = f.createStage(selectedStage);
	if (s == nullptr)
	{
		std::cerr << "Unable to create stage " << selectedStage << "\n";
		return;
	}

	pdal::ProgramArgs args;
	s->addAllArgs(args);

	std::ostringstream ostr;
	args.dump3(ostr);
	std::string json = ostr.str();

	Json::Reader jsonReader;
	Json::Value array;
	jsonReader.parse(json, array);


	std::map<QString, QLineEdit*> optionInputs;

	QWidget window;
	QFormLayout form(&window);

	// Intentionally skip the 3 last options for now
	for (int i = array.size() - 1; i >= 3; --i)
	{
		QString optionName = QString::fromStdString(array[i]["name"].asString());
		m_app->dispToConsole(optionName);
		QLineEdit *optionEdit = new QLineEdit;
		form.addRow(optionName, optionEdit);
		optionInputs[optionName] = optionEdit;
	}


	QPushButton btn("Ok");
	form.addRow(&btn);

	QEventLoop loop;
	window.show();
	QObject::connect(&btn, &QPushButton::clicked, &loop, &QEventLoop::quit);
	loop.exec();

	pdal::Options filterOptions;
	for (const auto& optionInput: optionInputs)
	{
		if (!optionInput.second->text().isEmpty()) {
			filterOptions.add(optionInput.first.toStdString(), optionInput.second->text().toStdString());
			m_app->dispToConsole(QString("%1: \"%2\"").arg(optionInput.first).arg(optionInput.second->text()));
		}
	}

	ccPointCloud *inputCloud = ccHObjectCaster::ToPointCloud(m_app->getSelectedEntities().front());
	if (inputCloud == nullptr) {
		m_app->dispToConsole("FK", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}
	ccPointCloudStreamReader reader(inputCloud);

	s->setInput(reader);

	try
	{
		s->setOptions(filterOptions);
	} catch (const std::exception& e) {
		m_app->dispToConsole(QString::fromUtf8(e.what()), ccMainAppInterface::ERR_CONSOLE_MESSAGE);
		return;
	}

	ccPointCloud *r = new ccPointCloud("lol");
	ccPointCloudStreamWriter writer(r);

	writer.setInput(*s);

	pdal::FixedPointTable t(1000);

	try
	{
		writer.prepare(t);
		writer.execute(t);
	} catch (const std::exception& e) {
		m_app->dispToConsole(QString::fromUtf8(e.what()), ccMainAppInterface::ERR_CONSOLE_MESSAGE);
	}
	m_app->addToDB(r);
}

void qPDAL::executePipeline()
{

}
