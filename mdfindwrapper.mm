#import <sys/types.h>
#import <unistd.h>
#import <CoreServices/CoreServices.h>
#import <Foundation/Foundation.h>
#import <QtCore/QtCore>
#import <QString>

bool MDFindWrapper(const QString& fileName, QStringList& results)
{
    QString queryString = QString("* == \"%1\"wcd || kMDItemFSName = \"%1\"c").arg(fileName);
    CFStringRef rawQuery = CFStringCreateWithCString(kCFAllocatorDefault,
                                                     queryString.toStdString().c_str(),
                                                     CFStringGetSystemEncoding());
    if (!rawQuery)
    {
        qDebug() << "creating CFString failed";
        return false;
    }

    MDQueryRef queryRef = MDQueryCreate(kCFAllocatorDefault, rawQuery, NULL, NULL);
    if (!queryRef)
    {
        qDebug() << "creating MDQuery failed";

        CFRelease(rawQuery);
        return false;
    }

    // go execute the query
    if (!MDQueryExecute(queryRef, kMDQuerySynchronous))
    {
        qDebug() << "MDQueryExecute failed";

        CFRelease(rawQuery);
        CFRelease(queryRef);
        return false;
    }

    int count = MDQueryGetResultCount(queryRef);
    for (int i = 0; i < count; i++)
    {
        MDItemRef item = (MDItemRef)MDQueryGetResultAtIndex(queryRef, i);
        NSString* name = (NSString*)MDItemCopyAttribute(item, kMDItemPath);
        results.append( QString::fromNSString(name));
        [name autorelease];
    }

    CFRelease(rawQuery);
    CFRelease(queryRef);

    return true;
}
