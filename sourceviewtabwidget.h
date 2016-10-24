#ifndef SOURCEVIEWTABWIDGET_H
#define SOURCEVIEWTABWIDGET_H

#include <QObject>

class SourceViewTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit SourceViewTabWidget(QWidget *parent = 0);

signals:

public slots:
    void onCloseAll();
    void onCloseAllButThis();
    void onCloseCurrent();
    void onTabCloseRequested(int index);

private slots:

    void onCustomContextMenuRequested(const QPoint &pos);
    void onCurrentChanged(int index);
private:
};

#endif // SOURCEVIEWTABWIDGET_H
