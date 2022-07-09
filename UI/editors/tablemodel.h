#include <QAbstractTableModel>
#include <QObject>
#include <QList>

class TableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	TableModel(QObject *parent);
	TableModel(QList< QPair<QString, QString> > pairs, QObject *parent);

	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool insertRows(int position, int rows, const QModelIndex &index);
	bool removeRows(int position, int rows, const QModelIndex &index);
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	bool appendRow(const QPair<QString,QString>& row);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QList< QPair<QString, QString> > getList();

private:
	QList< QPair<QString, QString> > listOfPairs;
};