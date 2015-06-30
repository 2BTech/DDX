#include "module.h"

void Module::init(QStringList settings) {
	alert("init() not reimplemented!");
}

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

void Module::cleanup() {
	
}


Module::Module(const QString *model, QObject *parent) : QObject(parent)
{
	// TODO
	// Set name & pathname
	// Call initializer
}


Module::~Module()
{
	delete outputColumns;
	for (int i = 0; i < newColumns->size(); i++)
		delete newColumns->at(i);
	delete newColumns;
}


void Module::reconfigure() {
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

QString* Module::insertColumn(QString name, int index) {
	if (findColumn(name)) return 0;
	Column *c = new Column(name, this);
	newColumns->append(c);
	outputColumns->insert(index, c);
	return c->buffer();
}

void Module::removeColumn(const Column *c) {
	// TODO:  Test whether this even works with pointers
	outputColumns->removeAll((Column*) c);
	if (c->p == this)
		newColumns->removeAll((Column*) c);
}
