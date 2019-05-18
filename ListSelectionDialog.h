//
// Created by t.montaigu on 5/18/2019.
//

#ifndef CLOUDCOMPAREPROJECTS_LISTSELECTIONDIALOG_H
#define CLOUDCOMPAREPROJECTS_LISTSELECTIONDIALOG_H
#include <QDialogButtonBox>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

class ListSelectionDialog : public QDialog
{
Q_OBJECT

public:
	explicit ListSelectionDialog(const std::vector<QString>& names, QWidget *parent = nullptr);
	std::vector<QString> checkedItems();
public slots:
	void highlightChecked(QListWidgetItem* item);

private:
	QListWidget * m_widget{};
	QDialogButtonBox* m_button_box{};
	QGroupBox* m_view_box{};
	QPushButton* m_save_button{};
	QPushButton* m_close_button{};

	void createListWidget(const std::vector<QString>& names);
	void createOtherWidgets();
	void createLayout();
	void createConnections() const;
};
#endif //CLOUDCOMPAREPROJECTS_LISTSELECTIONDIALOG_H
