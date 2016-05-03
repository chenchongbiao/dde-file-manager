#ifndef DTOOLBAR_H
#define DTOOLBAR_H

#include <QFrame>
#include <QStackedWidget>
#include "dstatebutton.h"

class DIconTextButton;
class DCheckableButton;
class DStateButton;
class DSearchBar;
class DTabBar;
class DCrumbWidget;
class FMEvent;
class HistoryStack;
class DHoverButton;

class DToolBar : public QFrame
{
    Q_OBJECT
public:
    explicit DToolBar(QWidget *parent = 0);
    ~DToolBar();
    static const int ButtonHeight;
    void initData();
    void initUI();
    void initAddressToolBar();
    void initContollerToolBar();
    void initConnect();
    void startup();
    DSearchBar * m_searchBar = NULL;

signals:
    void requestIconView();
    void requestListView();

    void requestSwitchLayout();
    void refreshButtonClicked();

public slots:
    void searchBarClicked();
    void searchBarActivated();
    void searchBarDeactivated();
    void searchBarTextEntered();
    void crumbSelected(const FMEvent &e);
    void crumbChanged(const FMEvent &event);
    void searchBarChanged(QString path);
    void backButtonClicked();
    void forwardButtonClicked();
private:
    QFrame* m_addressToolBar;
    DStateButton* m_backButton=NULL;
    DStateButton* m_forwardButton=NULL;
    DStateButton* m_upButton=NULL;
    DStateButton* m_searchButton = NULL;
    DStateButton* m_refreshButton = NULL;

    QFrame* m_contollerToolBar;
    DIconTextButton* m_newFolderButton=NULL;
    DIconTextButton* m_deleteFileButton=NULL;

    QPushButton* m_iconViewButton=NULL;
    QPushButton* m_listViewButton=NULL;
    QPushButton* m_hierarchicalButton = NULL;
    DHoverButton* m_sortingButton=NULL;

    bool m_switchState = false;
    DCrumbWidget * m_crumbWidget = NULL;
    QButtonGroup * m_viewButtonGroup = NULL;
    HistoryStack * m_navStack = NULL;
};

#endif // DTOOLBAR_H
