
#include <QDebug>
#include "c.h"

QObject *findRoot(QObject *o)
{
    QObject *root = o;
    for( ; ; ) {
	qDebug() << root->metaObject()->className();
	QObject *p = root->parent();
	if(!p) break;
	root = p;
    }
    qDebug() << "root = " << root->metaObject()->className();
    return root;
}



void MyClass::print()
{
    QObject *root = findRoot(this);
    MyClass *o = root->findChild<MyClass *>("MyClass123");

    qDebug() << (o == this) << o;
}
