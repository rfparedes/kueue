/*
                kueue - keep track of your SR queue
         (C) 2011 - 2012 Stefan Bogner <sbogner@suse.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the

    Free Software Foundation, Inc.
    59 Temple Place - Suite 330
    Boston, MA  02111-1307, USA

    Have a lot of fun :-)

*/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "config.h"
#include "ui/browsers/qmonbrowser.h"
#include "ui/browsers/unitybrowser.h"
#include "ui/browsers/statsbrowser.h"
#include "ui/browsers/queuebrowser.h"
#include "ui/browsers/subownerbrowser.h"
#include "ui/kueuewindow.h"
#include "ui/download/downloadmanager.h"
#include "ui/statusbar.h"
#include "ui/browsers/search/webviewsearch.h"
#include "ui/browsers/search/browsersearch.h"
#include "ui/busywidget.h"

#include <QTabWidget>
#include <QToolButton>

class DownloadManager;
class KueueWindow;
class TabButton;
class TabBar;

class TabWidget : public QTabWidget
{
    Q_OBJECT

    public:
        static TabWidget& tw();
        static void destroy();
        
        static void openInUnity( const QString& id )
        {
            TabWidget::tw().openInUnityImp( id );
        }
        
        static void newUnityWithSR( const QString& id )
        {
            TabWidget::tw().addUnityBrowserWithSR( id );
        }
        
        static void removeUnityTab( int id )
        {
            TabWidget::tw().removeUnityBrowser( id );
        }
        
    private:
        static TabWidget* instance;
        TabWidget(QWidget *parent = 0L);
        ~TabWidget();
        
        int mTabs;
        int mGrabbedWidget;
        bool mSubVisible;
        TabBar* mBar;
        TabButton* mMenuButton;
        QWidget* mPersonalTab;
        QWidget* mSubownerTab;
        QWidget* mMonitorTab;
        QWidget* mStatsTab;
        UnityWidget* mUnityTab;
        QueueBrowser* mQueueBrowser;
    	SubownerBrowser* mSubownerBrowser;
        QMonBrowser* mQmonBrowser;
        StatsBrowser* mStatsBrowser;
        UnityBrowser* mUnityBrowser;
        KueueWindow* mKueueWindow;
        QList<UnityWidget*> mUnityWidgetList;
        QMap<int, UnityBrowser*> mUnityBrowserMap; 
        
        StatusBar* mStatusBar;
        WebViewSearch* webViewSearch( int );
        BrowserSearch* browserSearch( int );
        QMenu* kueueMainMenu();
        QAction* mActionQuit;
        QAction* mActionConfig;
        QAction* mActionShowSR;
        QAction* mActionShowCR;
        QAction* mActionSortUpdate;
        QAction* mActionSortAge;
        QAction* mActionAwaitingCustomer;
        QAction* mActionAwaitingSupport;
        QAction* mActionOthers;
        
        QAction* mActionSubShowSR;
        QAction* mActionSubShowCR;
        QAction* mActionSubSortUpdate;
        QAction* mActionSubSortAge;
        QAction* mActionSubAwaitingCustomer;
        QAction* mActionSubAwaitingSupport;
        QAction* mActionSubOthers;
        QAction* mActionSubCloseSrTables;
        QAction* mActionSubExpandSrTables;
        
        QAction* mActionExpandSrTables;
        QAction* mActionCloseSrTables;
        QAction* mActionAbout;
        QAction* mActionBug;
        QAction* mActionFeature;
        QAction* mActionHelp;
        QAction* mActionWebsite;
        QAction* mActionAddUnityTab;
        QAction* mActionNSA;
        QAction* mActionClone;
        BusyWidget* mBusyWidget;
        
    public slots:
        void setTabsPosition();
        int addUnityBrowser( int = 0 );
        void addUnityBrowserWithSR( QString = QString::Null(), int = 0 );
        void removeUnityBrowser( int );
        void rebuildMaps();
        void showMonitorTab( bool );
        void showStatsTab( bool );
        void showPersonalTab( bool );
        void showSubownerTab( bool );
        void showUnityTab( bool );
        void showSrInUnityBrowser( int, const QString& );
        void switchToMainUnityTab();
        void switchToTab( int );
        void updateUiData();
        void openClipboardInUnity( int );
        void openInUnityImp( const QString& );
        void updateQueueBrowser( const QString& = "bla" );
        void updateSubownerBrowser( const QString& = "bla" );
        void updateQmonBrowser( const QString& = "bla" );
        void updateStatsBrowser( const QString& = "bla" );
        void refreshTabs();
        
    private slots:
        void tabMiddleClicked( int, QPoint );
        void tabRightClicked( int, QPoint );
        void setMenus();
        void unityTabMenu( int tab, const QPoint& p );
        void closeActionTriggered();
        void clipboardActionTriggered();
        void closeAllOtherActionTriggered();
        void permClipboardActionTriggered();
        void setShowSR( bool );
        void setShowCR( bool );
        void setSortAge( bool );
        void setSortUpdate( bool );
        void setShowAwaitingCustomer( bool );
        void setShowAwaitingSupport( bool );
        void setShowStatusOthers( bool );
        void expandAllTables();
        void closeAllTables();
        void subSetShowSR( bool );
        void subSetShowCR( bool );
        void subSetSortAge( bool );
        void subSetSortUpdate( bool );
        void subSetShowAwaitingCustomer( bool );
        void subSetShowAwaitingSupport( bool );
        void subSetShowStatusOthers( bool );
        //void subExpandAllTables();
        //void subCloseAllTables();
        void aboutDialog();
        void reportBug();
        void suggestFeature();
        void openHelp();
        void helpClosed();
        void openWebsite();
        void showSearch();
        void makeNsaReport();

    signals:
        void unityTabMiddleClicked( int );
        void openConfig();
};

class TabBar : public QTabBar
{
    Q_OBJECT

    public:
        explicit TabBar( QObject* parent = 0L );
        ~TabBar();
        
        int selectTab( const QPoint& ) const;
        
    private:
        void mousePressEvent ( QMouseEvent * event );

    signals:
        void tabRightClicked( int, QPoint );
        void tabMiddleClicked( int, QPoint );
};

class TabButton : public QToolButton
{
    Q_OBJECT
    
    public: 
        explicit TabButton( QWidget* parent = 0L );
        ~TabButton();
     
    protected:
        void mousePressEvent( QMouseEvent* );
        
    signals:
        void middleClicked();
};

#endif
