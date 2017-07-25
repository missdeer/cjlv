#ifndef QUICKWIDGETAPI_H
#define QUICKWIDGETAPI_H

#include <QObject>

class QuickWidgetAPI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int from READ getFrom WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(int to READ getTo WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(int firstValue READ getFirstValue WRITE setFirstValue NOTIFY firstValueChanged)
    Q_PROPERTY(int secondValue READ getSecondValue WRITE setSecondValue NOTIFY secondValueChanged)
public:
    explicit QuickWidgetAPI(QObject* parent = nullptr);
    ~QuickWidgetAPI();

    int getFrom() const;
    void setFrom(int from);

    int getTo() const;
    void setTo(int to);

    int getFirstValue() const;
    void setFirstValue(int v);

    int getSecondValue() const;
    void setSecondValue(int v);
signals:
    void fromChanged();
    void toChanged();
    void firstValueChanged();
    void secondValueChanged();
    void valueChanged();
private:
    int m_firstValue;
    int m_secondValue;
    int m_from;
    int m_to;
};

#endif // QUICKWIDGETAPI_H
