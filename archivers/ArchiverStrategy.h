/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2010 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

/*! \file ArchiverStrategy.h */

#ifndef __ARCHIVER_STRATEGY
#define __ARCHIVER_STRATEGY

#include <QString>
#include <QStringList>
#include <QByteArray>

#include "ArchiversConfiguration.h"

class QFile;
class FileSignature;

	/**
	 * @brief Base class that provides configuration information and commandline parameters for external archivers.
	 */
    class FileSignature
    {
    public:
        FileSignature(unsigned int offset, const char *pattern, unsigned int len);
        FileSignature();
        FileSignature(const FileSignature &sig);
        ~FileSignature();

        bool matches(QFile *file) const;
        FileSignature& operator =(const FileSignature &sig);

    private:
        unsigned int offset;
        QByteArray pattern;
    };

    class ArchiverStrategy
    {
    public:
        ArchiverStrategy(const QString &name, const FileSignature &sig);
        virtual ~ArchiverStrategy();

		/**
		 * @brief Return true if specified archive file can be opened with this archiver.
		 *
		 * @param f archive file
		 *
		 * @return true if f can be extracted
		 */
        virtual bool canOpen(QFile *f) const;

		/**
		 * @brief Return true if specified archive file can be opened with this archiver.
		 *
		 * @param archive filename
		 *
		 * @return true if file can be extracted
		 */
        virtual bool canOpen(const QString &filename) const;

        QStringList getExtractArguments(const QString &filename) const;
        QStringList getExtractArguments() const;
        QStringList getListArguments(const QString &filename) const;
        QStringList getListArguments() const;
        QStringList getExtensions() const;

		/**
		 * @brief Return true if this archiver is supported.
		 *
		 * @return true if archiver is supported
		 */
        bool isSupported() const;

		/**
		 * @brief Fills command line template with given arguments. The returned list is ready for use for archiver process invocation. Special argument @@F is replaced with filename.
		 *
		 * @param args arguments list
		 * @param filename archive filename
		 *
		 * @return archiver executable invocation parameters
		 */
        static QStringList fillTemplateArguments(const QStringList &args, const QString &filename);

		/**
		 * @brief Performs configuration of this archiver. Detects if this archiver is supported and sets internal parameters accordingly.
		 */
        virtual void configure() = 0;

		/**
		 * @brief Provides hints related to configuration and availability of this archiver.
		 *
		 * @return hints
		 */
        virtual QList<ArchiverHint> getHints() const;

        operator ArchiverStatus() const;

    protected:
        void setFileMagic(const FileSignature &sig);
        void setExtractArguments(const QString &command);
        void setListArguments(const QString &command);
        void setSupported(bool f=true);
        void setExecutables(const QString &exec1, const QString &exec2=QString::null);
        void addExtension(const QString &ext);

    private:
        FileSignature signature;
        QString name;
        bool supported;
        QStringList executables;
        QStringList extensions;
        QStringList extractArgs;
        QStringList listArgs;
    };
    
    class AceArchiverStrategy: public ArchiverStrategy
    {
    public:
        AceArchiverStrategy();
        virtual ~AceArchiverStrategy();

        virtual void configure();
    };
    
    class P7zipArchiverStrategy: public ArchiverStrategy
    {
    public:
        P7zipArchiverStrategy();
        virtual ~P7zipArchiverStrategy();

        virtual void configure();
    };

    class RarArchiverStrategy: public ArchiverStrategy
    {
    public:
        RarArchiverStrategy();
        virtual ~RarArchiverStrategy();

        virtual void configure();
        virtual QList<ArchiverHint> getHints() const;

    private:
        bool nonfree_unrar;
    };
    
    class TarArchiverStrategy: public ArchiverStrategy
    {
    public:
        TarArchiverStrategy();
        virtual ~TarArchiverStrategy();

        virtual void configure();
    };
    
    class Tarbz2ArchiverStrategy: public ArchiverStrategy
    {
    public:
        Tarbz2ArchiverStrategy();
        virtual ~Tarbz2ArchiverStrategy();

        virtual void configure();
    };

    class TargzArchiverStrategy: public ArchiverStrategy
    {
    public:
        TargzArchiverStrategy();
        virtual ~TargzArchiverStrategy();

        virtual void configure();
    };
    
    class ZipArchiverStrategy: public ArchiverStrategy
    {
    public:
        ZipArchiverStrategy();
        virtual ~ZipArchiverStrategy();

        virtual void configure();
    };

    #endif
