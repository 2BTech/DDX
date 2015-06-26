#include "module.h"

Module::Module(const QString *def, QObject *parent) : QObject(parent)
{
	
}

Module::~Module()
{
	
}

// TODO:  Read this guide in order to get best efficiency out of a repeatedly-changing QString.  Specifically, try reserving ahead of time.
