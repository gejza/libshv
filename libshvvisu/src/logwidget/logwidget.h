#pragma once

#include "../shvvisuglobal.h"

#include "logtablemodelbase.h"

#include <QTableView>

class QAbstractButton;

namespace shv {
namespace visu {

class LogFilterProxyModel;
class LogTableModelRow;
class LogTableModelBase;

namespace Ui {
class LogWidget;
}

class LogWidgetTableView : public QTableView
{
	Q_OBJECT
private:
	typedef QTableView Super;
public:
	LogWidgetTableView(QWidget *parent);

	void copy();
protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
};

class SHVVISU_DECL_EXPORT LogWidget : public QWidget
{
	Q_OBJECT
private:
	typedef QWidget Super;
public:
	explicit LogWidget(QWidget *parent = nullptr);
	~LogWidget();

	Q_SLOT void addLogRow(const LogTableModelRow &row);
	Q_SLOT void scrollToLastRow();

	void clear();
	virtual void setLogTableModel(LogTableModelBase *m);
	LogTableModelBase* logTableModel();

	//Q_SIGNAL void severityTresholdChanged(qf::core::Log::Level lvl);
	//void setSeverityTreshold(qf::core::Log::Level lvl);
protected:
	QAbstractButton* tableMenuButton();
	QTableView* tableView() const;

	//void clearCategoryActions();
	//virtual void addCategoryActions(const QString &caption, const QString &id, qf::core::Log::Level level = qf::core::Log::Level::Invalid);
	//QMap<QString, core::Log::Level> selectedLogCategories() const;
	//virtual void registerLogCategories();

	//virtual void onDockWidgetVisibleChanged(bool visible);
	void onVerticalScrollBarValueChanged();
private:
	//Q_SLOT void onSeverityTresholdChanged(int index);
	Q_SLOT void filterStringChanged(const QString &filter_string);
	Q_SLOT void on_btClearLog_clicked();
	Q_SLOT void on_btResizeColumns_clicked();
protected:
	LogTableModelBase* m_logTableModel = nullptr;
	LogFilterProxyModel* m_filterModel = nullptr;
private:
	Ui::LogWidget *ui;
	//bool m_logCategoriesRegistered = false;

	//QList<QAction*> m_logLevelActions;
	//QList<QMenu*> m_loggingCategoriesMenus;
	bool m_columnsResized = false;
	bool m_isAutoScroll = true;
};

}}

