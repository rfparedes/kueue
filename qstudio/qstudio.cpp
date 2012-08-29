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

#include "qstudio.h"

#include <QDebug>
#include <QObject>
#include <QtXml>
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QDateTime>

QStudio::QStudio( const QString& server, const QString& user, const QString& key, bool debug )
{
    qDebug() << "[QSTUDIO] Constructing";

    mServer = server;
    mUser = user;
    mApiKey = key;
    mDebug = debug;
    
    mNAM = new QNetworkAccessManager( this );
    
    connect( mNAM, SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ), 
             this, SLOT( authenticate( QNetworkReply*, QAuthenticator* ) ) );
}

QStudio::~QStudio()
{
    qDebug() << "[QSTUDIO] Destroying";
}

void QStudio::log( const QString& f, const QString& x )
{
    if ( mDebug )
    {
        QFile file( QDesktopServices::storageLocation( QDesktopServices::DataLocation ) + "/studio-xml.log" );
        
        if ( !file.open( QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text ) )
        {
            qDebug() << "[QSTUDIO] Logfile not open";
            return;
        }
        
        QString d = "[" + QDateTime::currentDateTime().toString( "MM/dd hh:mm:ss" ) + "]";
        QTextStream out(&file);
        
        out << "\n\n";
        out << "-----" + d + "-----------------------------------------------------------------------------------------";
        out << "\n\n";
        out << f;
        out << "\n\n";
        out << x;
        
        file.close();
    }
}

QString QStudio::getRequest( const QString& req )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + mServer + "/api/v2" + req ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream" ) );
    QNetworkReply *reply = mNAM->get( request );
    reply->ignoreSslErrors(); 
    
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    reply->deleteLater();
    
    return result;
}

QString QStudio::putRequest( const QString& req, const QByteArray& data )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + mServer + "/api/v2" + req ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream" ) );
    QNetworkReply *reply = mNAM->put( request, data );
    
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    reply->deleteLater();
    
    return result;
}

QString QStudio::postRequest( const QString& req, const QByteArray& data )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + mServer + "/api/v2" + req ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream" ) );    
    QNetworkReply *reply = mNAM->post( request, data );
    
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    reply->deleteLater();
    
    return result;
}

QString QStudio::postFile( const QString& req, const QString& fn )
{
    QEventLoop loop;
    QString result;
    
    QHttpMultiPart* mp = new QHttpMultiPart( QHttpMultiPart::FormDataType );
    QHttpPart fp;
    QString h = "form-data; name=\"file\"; filename=\"" + QFileInfo( fn ).fileName() + "\"\n";

    fp.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream" ) );
    fp.setHeader( QNetworkRequest::ContentDispositionHeader, QVariant( h ) );
    
    QFile* file = new QFile( fn );
    file->open( QIODevice::ReadOnly );
    fp.setBodyDevice( file );
    file->setParent( mp );

    mp->append( fp );
    
    QNetworkRequest request( QUrl( "http://" + mServer + "/api/v2" + req ) );
        
    QNetworkReply* reply = mNAM->post( request, mp );
    
    mp->setParent( reply ); 
    
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    reply->deleteLater();
    
    return result;
}

QString QStudio::putFile( const QString& req, const QString& fn )
{
    QEventLoop loop;
    QString result;
    
    QHttpMultiPart* mp = new QHttpMultiPart( QHttpMultiPart::FormDataType );
    QHttpPart fp;
    QString h = "form-data; name=\"file\"; filename=\"" + QFileInfo( fn ).fileName() + "\"\n";

    fp.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream" ) );
    fp.setHeader( QNetworkRequest::ContentDispositionHeader, QVariant( h ) );
    
    QFile* file = new QFile( fn );
    file->open( QIODevice::ReadOnly );
    fp.setBodyDevice( file );
    file->setParent( mp );

    mp->append( fp );
    
    QNetworkRequest request( QUrl( "http://" + mServer + "/api/v2" + req ) );
        
    QNetworkReply* reply = mNAM->put( request, mp );
    
    mp->setParent( reply ); 
    
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
    
    result = reply->readAll();
    reply->deleteLater();
    
    return result;
}

QString QStudio::deleteRequest( const QString& req )
{
    QEventLoop loop;
    QString result;
    
    QNetworkRequest request( QUrl( "http://" + mServer + "/api/v2" + req ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream" ) );
    QNetworkReply *reply = mNAM->deleteResource( request );
     
    QObject::connect( reply, SIGNAL( finished() ), 
                      &loop, SLOT( quit() ) );
     
    connect( reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             this, SLOT( networkError( QNetworkReply::NetworkError ) ) );
                                    
    loop.exec();
       
    result = reply->readAll();
    reply->deleteLater();
    
    return result;
}

void QStudio::authenticate( QNetworkReply* reply, QAuthenticator* auth )
{
    auth->setUser( mUser );
    auth->setPassword( mApiKey );
}

void QStudio::networkError( QNetworkReply::NetworkError error )
{
    QNetworkReply* r = qobject_cast< QNetworkReply* >( sender() );
    
    qDebug() << "[QSTUDIO] Network Error" << r->url() << r->errorString();
}

QList<TemplateSet> QStudio::getTemplates()
{
    QString xml = getRequest( "/user/template_sets" );
    QList<TemplateSet> list;
    
    QDomDocument doc;
    doc.setContent( xml );
    
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        TemplateSet ts;
        
        if ( e.tagName() == "template_set" )
        {
            QDomNode no = n.firstChild();
        
            while ( !no.isNull() )
            {
                QDomElement te = no.toElement();
                  
                if ( te.tagName() == "template" )
                {
                    Template t;
                    QDomNode tn = no.firstChild();
                
                    while ( !tn.isNull() )
                    {
                        QDomElement ele = tn.toElement();
                        
                        if ( ele.tagName() == "appliance_id" )
                        {
                            t.id = ele.text().toInt();
                        }            
                        else if ( ele.tagName() == "name" )
                        {
                            t.name = ele.text();
                        }            
                        else if ( ele.tagName() == "description" )
                        {
                            t.description = ele.text();
                        }            
                        else if ( ele.tagName() == "basesystem" )
                        {
                            t.basesystem = ele.text();
                        }            
                        
                        tn = tn.nextSibling();
                    }

                    ts.templates.append( t );
                }
                else if ( te.tagName() == "name" )
                {
                    ts.name = te.text();
                }
                else if ( te.tagName() == "description" )
                {
                    ts.description = te.text();
                }
            
                no = no.nextSibling();
            }

            n = n.nextSibling();
        }

        list.append( ts );
    }
    
    log( "getTemplates", xml );
    
    return list;
}

Appliance QStudio::cloneAppliance( int id, const QString& name, const QString& arch )
{
    QByteArray empty;
    QString xml = postRequest( "/user/appliances?clone_from=" + QString::number( id ) + "&name=" + name + "&arch=" + arch, empty );
    
    Appliance app;
    
    QDomDocument doc;
    doc.setContent( xml );
    
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "id" )
        {
            app.id = e.text().toInt();
        }
        else if ( e.tagName() == "name" )
        {
            app.name = e.text();
        }
        else if ( e.tagName() == "arch" )
        {
            app.arch = e.text();
        }
        else if ( e.tagName() == "type" )
        {
            app.type = e.text();
        }
        else if ( e.tagName() == "last_edited" )
        {
            app.last_edited = e.text();
        }
        else if ( e.tagName() == "estimated_raw_size" )
        {
            app.estimated_raw_size = e.text();
        }
        else if ( e.tagName() == "estimated_compressed_size" )
        {
            app.estimated_compressed_size = e.text();
        }
        else if ( e.tagName() == "edit_url" )
        {
            app.edit_url = e.text();
        }
        else if ( e.tagName() == "basesystem" )
        {
            app.basesystem = e.text();
        }
        else if ( e.tagName() == "uuid" )
        {
            app.uuid = e.text();
        }
        else if ( e.tagName() == "parent" )
        {
            QDomNode tn = n.firstChildElement( "id" );
            app.parent = n.toElement().text().toInt();
        }

        n = n.nextSibling();
    }
    
    log( "cloneAppliance - ID: " + QString::number( id ) + " - NAME: " + name + " - ARCH: " + arch, xml );
    
    return app;
}

RPM QStudio::uploadRPM( const QString& basesystem, const QString& filename )
{
    RPM rpm;
       
    QString xml = postFile( "/user/rpms?base_system=" + basesystem, filename );
    
    QDomDocument doc;
    doc.setContent( xml );
    
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "id" )
        {
            rpm.id = e.text().toInt();
        }
        else if ( e.tagName() == "filename" )
        {
            rpm.filename = e.text();
        }
        else if ( e.tagName() == "size" )
        {
            rpm.size = e.text();
        }
        else if ( e.tagName() == "archive" )
        {
            if ( e.text() == "true" )
            {
                rpm.archive = true;
            }
            else
            {
                rpm.archive = false;
            }
        }
        else if ( e.tagName() == "base_system" )
        {
            rpm.basesystem = e.text();
        }
        
        n = n.nextSibling();
    }
    
    log( "uploadRPM - BASESYSTEM: " + basesystem + " - FILENAME: " + filename, xml );
        
    return rpm;
}

OverlayFile QStudio::addOverlayFile( int appliance, const QString& filename, const QString& path, const QString& owner, const QString& group, const QString& permissions )
{
    OverlayFile of;
       
    QFileInfo info( filename );
    
    QString urltmp;
    
    urltmp += "appliance_id=" + QString::number( appliance ) + "&filename=" + info.fileName() + "&path=" + path;
    
    if ( !owner.isNull() )
    {
        urltmp += "&owner=" + owner;
    }
    
    if ( !group.isNull() )
    {
        urltmp += "&group=" + group;
    }
    
    if ( !permissions.isNull() )
    {
        urltmp += "&permissions=" + permissions;
    }
    
    QString xml = postFile( "/user/files?" + urltmp, filename );
    
    QDomDocument doc;
    doc.setContent( xml );
    
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "id" )
        {
            of.id = e.text().toInt();
        }
        else if ( e.tagName() == "filename" )
        {
            of.filename = e.text();
        }
        else if ( e.tagName() == "path" )
        {
            of.path = e.text();
        }
        else if ( e.tagName() == "enabled" )
        {
            if ( e.text() == "true" )
            {
                of.enabled = true;
            }
            else
            {
                of.enabled = false;
            }
        }
        else if ( e.tagName() == "download_url" )
        {
            of.downloadurl = e.text();
        }
        else if ( e.tagName() == "owner" )
        {
            of.owner = e.text();
        }
        else if ( e.tagName() == "group" )
        {
            of.group = e.text();
        }
        else if ( e.tagName() == "permissions" )
        {
            of.permissions = e.text();
        }
        
        n = n.nextSibling();
    }
    
    log( "addOverlayFile " + path + " - FILENAME: " + filename, xml );
        
    return of;
}

bool QStudio::addPackage( int id, const QString& package )
{
    QByteArray empty;
    QString xml = postRequest( "/user/appliances/" + QString::number( id ) + "/cmd/add_package?name=" + package, empty );
    
    log( "addPackage - ID: " + QString::number( id ) + " NAME: " + package, xml );
    
    QDomDocument doc;
    doc.setContent( xml );
    QDomElement docelement = doc.documentElement();
    
    if ( docelement.tagName() == "success" )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool QStudio::setLogo( int appliance, const QString& file )
{
    QString xml = putFile( "/user/appliances/" + QString::number( appliance ) + "/configuration/logo", file );
    
    if ( xml.contains( "<success>" ) )
    {
        return true;
    }
    
    return false;
}

int QStudio::startApplianceBuild( int id )
{
    QByteArray empty;
    
    // Building type "vmx" here, as build status doesn't seem to work reliably for XEN.
    
    QString xml = postRequest( "/user/running_builds?appliance_id=" + QString::number( id ) + "&image_type=vmx", empty );
    
    log( "startApplianceBuild - ID: " + QString::number( id ), xml );
    
    QDomDocument doc;
    doc.setContent( xml );
    QDomElement docelement = doc.documentElement();
    
    if ( docelement.tagName() == "error" )
    {
        return -1;
    }
    else
    {
        QDomNode n = docelement.firstChildElement( "id" );
        return n.toElement().text().toInt();
    }
}

bool QStudio::addUserRepository( int id )
{
    QByteArray empty;
    QString xml = postRequest( "/user/appliances/" + QString::number( id )  + "/cmd/add_user_repository", empty );
    
    log( "addUserRepository - ID: " + QString::number( id ), xml );
    
    if ( xml.contains( "success" ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

BuildStatus QStudio::getBuildStatus( int id )
{
    QString xml = getRequest( "/user/running_builds/" + QString::number( id ) );
    
    BuildStatus bs;
    
    QDomDocument doc;
    doc.setContent( xml );
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "id" )
        {
            bs.id = e.text().toInt();
        }
        else if ( e.tagName() == "state" )
        {
           bs.state = e.text();
        }
        else if ( e.tagName() == "percent" )
        {
           bs.percent = e.text().toInt();
        }
        else if ( e.tagName() == "time_elapsed" )
        {
           bs.time_elapsed = e.text().toInt();
        }
        else if ( e.tagName() == "message" )
        {
           bs.message = e.text();
        }
        
        n = n.nextSibling();
    }
    
    log( "getBuildStatus - ID: " + QString::number( id ), xml );
    
    return bs;
}

Testdrive QStudio::getTestdrive( int build )
{
    QByteArray empty;
    QString xml = postRequest( "/user/testdrives?build_id=" + QString::number( build ), empty );
   
    QDomDocument doc;
    doc.setContent( xml );
    
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    Testdrive td;
    
    log( "getTestdrive - ID: " + QString::number( build ), xml );
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "url" )
        {
            td.url = e.text();
        }
        else if ( e.tagName() == "id" )
        {
            td.id = e.text().toInt();
        }
        else if ( e.tagName() == "server" )
        {
            QDomNode no = n.firstChild();
        
            while ( !no.isNull() )
            {
                QDomElement te = no.toElement();
                  
                if ( te.tagName() == "vnc" )
                {
                    QDomNode tn = no.firstChild();
                
                    while ( !tn.isNull() )
                    {
                        QDomElement ele = tn.toElement();
                        
                        if ( ele.tagName() == "host" )
                        {
                            if ( ele.text() == "localhost" )
                            {
                                td.vnchost = mServer;
                            }
                            else
                            {
                                td.vnchost = ele.text();
                            }
                        }            
                        else if ( ele.tagName() == "port" )
                        {
                            td.vncport = ele.text();
                        }            
                        else if ( ele.tagName() == "password" )
                        {
                            td.vncpassword = ele.text();
                        }            
                        
                        tn = tn.nextSibling();
                    }
                }
            
                no = no.nextSibling();
            }
        }
        
        n = n.nextSibling();
    }

    return td;
}

QList< UserTestDrive > QStudio::getUserTestdrives()
{
    QString xml = getRequest( "/user/testdrives" );
    
    QList<UserTestDrive> tdl;
    
    QDomDocument doc;
    doc.setContent( xml );
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "testdrive" )
        {
            UserTestDrive t;
            QDomNode no = n.firstChild();
        
            while ( !no.isNull() )
            {
                QDomElement te = no.toElement();
                  
                if ( te.tagName() == "id" )
                {
                    t.testdriveid = te.text().toInt();
                }
                else if ( te.tagName() == "build_id" )
                {
                    t.buildid = te.text().toInt();
                }
                else if ( te.tagName() == "state" )
                {
                    t.state = te.text();
                }
            
                no = no.nextSibling();
            }
            
            tdl.append( t );
            n = n.nextSibling();
        }
    }
    
    log( "getUserTestdrives ", xml );
    
    return tdl;
}

QString QStudio::getDownloadUrlForBuild( int build )
{
    QString xml = getRequest( "/user/builds/" + QString::number( build ) );
    QString url;
    
    log( "getDownloadUrlForBuild ", xml );
    
    QDomDocument doc;
    doc.setContent( xml );
    QDomElement docelement = doc.documentElement();
    
    QDomNode n = docelement.firstChild();
    
    while ( !n.isNull() )
    {
        QDomElement e = n.toElement();
        
        if ( e.tagName() == "download_url" )
        {
            url = e.toElement().text();
        }
        
        n = n.nextSibling();
    }
    
    if ( url.startsWith( "/download" ) )
    {
        url = "http://" + mServer + url;
    }
        
    return url;
}

bool QStudio::deleteBuild( int id )
{
    QString xml = deleteRequest( "/user/builds/" + QString::number( id ) );
    log( "deleteBuild ", xml );
    
    if ( xml.contains( "<success" ) )
    {
        return true;
    }
    
    return false;
}

bool QStudio::addBuildScript( int appliance, const QString& script )
{
    QString xml = getRequest( "/user/appliances/" + QString::number( appliance ) + "/configuration" );
    QDomDocument doc;
    doc.setContent( xml );
    qDebug() << doc.toString();
    QByteArray data;
    QTextStream stream( &data );
    stream << script;
    
    QDomElement root = doc.documentElement();
    qDebug() << "DOCELE" << root.toElement().tagName();
    
    QDomElement e = root.firstChildElement( "scripts" ).firstChildElement( "build" );
    
    QDomElement sc = e.firstChildElement( "script" );
    
    
    QDomElement newele = doc.createElement( "script" );
    QDomCDATASection cdata = doc.createCDATASection( script );
    newele.appendChild( cdata );
    
    e.replaceChild( newele, sc );
    
    QString endxml = putRequest( "/user/appliances/" + QString::number( appliance ) + "/configuration", doc.toByteArray() );
    qDebug() << endxml;
    return true;
    
}


#include "qstudio.moc"
