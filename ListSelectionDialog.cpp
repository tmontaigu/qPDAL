#include "ListSelectionDialog.h"

ListSelectionDialog::ListSelectionDialog(const std::vector<QString>& names, QWidget *parent) : QDialog(parent)
{
	setWindowTitle("Choose dimensions");

	createListWidget(names);
	createOtherWidgets();
	createLayout();
	createConnections();
}

void ListSelectionDialog::createListWidget(const std::vector<QString>& names) {
	m_widget = new QListWidget;

	for (const QString& name : names) {
		m_widget->addItem(name);
	}

	QListWidgetItem* item = 0;
	for (int i(0); i < m_widget->count(); ++i) {
		item = m_widget->item(i);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

		if (item->text() == "X" || item->text() == "Y" || item->text() == "Z") {
			item->setCheckState(Qt::Checked);
		}
		else {
			item->setCheckState(Qt::Unchecked);
		}
	}
}

void ListSelectionDialog::createOtherWidgets() {
	m_view_box = new QGroupBox(tr("Required components"));
	m_button_box = new QDialogButtonBox;
	m_close_button = m_button_box->addButton(QDialogButtonBox::Ok);
}

void ListSelectionDialog::createLayout() {
	QVBoxLayout* view_layout = new QVBoxLayout;
	view_layout->addWidget(m_widget);
	m_view_box->setLayout(view_layout);

	QHBoxLayout* horizontal_layout = new QHBoxLayout;
	horizontal_layout->addWidget(m_button_box);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_view_box);
	mainLayout->addLayout(horizontal_layout);

	setLayout(mainLayout);
}

void ListSelectionDialog::createConnections() const {
	QObject::connect(m_widget, SIGNAL(itemChanged(QListWidgetItem*)),
					 this, SLOT(highlightChecked(QListWidgetItem*)));
	QObject::connect(m_close_button, SIGNAL(clicked()), this, SLOT(close()));
}

void ListSelectionDialog::highlightChecked(QListWidgetItem *item) {
	if (item->checkState() == Qt::Checked)
		item->setBackgroundColor(QColor("#ffffb2"));
	else
		item->setBackgroundColor(QColor("#ffffff"));
}


std::vector<QString> ListSelectionDialog::checkedItems() {
	std::vector<QString> checked_items;

	for (int i(0); i < m_widget->count(); ++i) {
		QListWidgetItem *item = m_widget->item(i);
		if (item->checkState() == Qt::Checked) {
			checked_items.push_back(item->text());
		}
	}
	return checked_items;
}