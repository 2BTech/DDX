#include "module.h"

void Module::handleReconfigure() {
	alert("handleReconfigure() not reimplemented!");
}


void Module::process() {
	alert("process() not reimplemented!");
}


void Module::skip() {
	for (int i = 0; i < newColumns->size(); i++)
		newColumns->at(i)->c = "";
}


Module::Module(const QString *model, QObject *parent) : QObject(parent)
{
	// TODO
	// Set name & pathname
	// Call initializer
}


Module::~Module()
{
	delete accessMap;
	delete outputColumns;
	for (int i = 0; i < newColumns->size(); i++)
		delete newColumns->at(i);
	delete newColumns;
}


void Module::reconfigure() {
	accessMap->clear();
	newColumns->clear();
	*outputColumns = *inputColumns;
	
	handleReconfigure();
}

void Module::alert(QString msg) {
	QString out(*pathName);
	out.append(":").append(name).append(": ");
	out.append(msg);
	emit sendAlert(out);
}

const Column* Module::findColumn(QString name) const {
	for (int i = 0; i < outputColumns->size(); i++)
		if (QString::compare(outputColumns->at(i)->n, name, Qt::CaseInsensitive) == 0)
			return outputColumns->at(i);
	return 0;
}

bool Module::insertColumn(QString name, int index) {
	if (findColumn(name)) return false;
	Column *c = new Column(name, this);
	newColumns->append(c);
	outputColumns->insert(index, c);
	addAccessor(c);
	return true;
}

void Module::removeColumn(const Column *c) {
	// TODO:  Test whether this even works with pointers
	outputColumns->removeAll((Column*) c);
	if (c->p == this)
		newColumns->removeAll((Column*) c);
}

void Module::addAccessor(const Column *c) {
	accessMap->insert(c->n, (QString*) &(c->c));
}

inline bool Module::addAccessor(QString name) {
	const Column *c = findColumn(name);
	if ( ! c) return false;
	addAccessor(c);
	return true;
}
