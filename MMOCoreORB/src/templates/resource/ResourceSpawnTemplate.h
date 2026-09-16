/*
 * SharedResourceContainerObjectTemplate.h
 *
 *  Created on: 06/05/2010
 *      Author: kyle
 */

#ifndef RESOURCESPAWNTEMPLATE_H_
#define RESOURCESPAWNTEMPLATE_H_

#include "templates/SharedObjectTemplate.h"

class ResourceSpawnTemplate : public SharedObjectTemplate {
private:

public:
	ResourceSpawnTemplate() {

	}

	~ResourceSpawnTemplate() {

	}

	void readObject(IffStream* iffStream) override {
		uint32 nextType = iffStream->getNextFormType();

		// Resource spawns use a container's client ancestry but remain server
		// scene objects rather than tangible resource containers.
		if (nextType == 'RCCT' || nextType == 'STOT') {
			readInheritedTemplate(iffStream, nextType);
			return;
		}

		SharedObjectTemplate::readObject(iffStream);
	}

	void readObject(LuaObject* templateData) override {
		SharedObjectTemplate::readObject(templateData);

	}
};

#endif /* RESOURCESPAWNTEMPLATE_H_ */
