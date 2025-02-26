#pragma once

#include <QObject>
#include <QPoint>

class EventFilterObject : public QObject
{
public:
	EventFilterObject(QObject* parent = nullptr);
	~EventFilterObject();

protected:
	bool eventFilter(QObject *watched,QEvent* ev)override;

private:
	QPoint m_pressPos;
};
