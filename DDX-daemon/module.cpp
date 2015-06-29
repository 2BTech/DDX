#include "module.h"

void Module::skip() {
	// TODO: Set all "c" members of the members of newColumn to empty strings (do not make new strings)
}



Module::Module(const QString *model, QObject *parent) : QObject(parent)
{
	// TODO
}

Module::~Module()
{
	// TODO
}


void Module::process() {
	// TODO:  Make default implementation be an error about not reimplemented
}


bool Module::handleReconfigure() {
	// TODO:  Make default implementation be an error about not reimplemented
}


const DataDef* Module::reconfigure() {
	// TODO:  Start by erasing newColumns and setting outputColumns to inputColumns
	return 0;
}
