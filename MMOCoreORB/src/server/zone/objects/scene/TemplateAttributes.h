#ifndef TEMPLATEATTRIBUTES_H_
#define TEMPLATEATTRIBUTES_H_

#include "conf/ConfigManager.h"
#include "server/zone/packets/scene/AttributeListMessage.h"
#include "templates/SharedObjectTemplate.h"

namespace TemplateAttributes {

inline void append(AttributeListMessage* message, SceneObject* object) {
	if (message == nullptr || object == nullptr || !object->isTangibleObject() ||
			!ConfigManager::instance()->showTangibleTemplate()) {
		return;
	}

	const auto* objectTemplate = object->getObjectTemplate();

	if (objectTemplate == nullptr)
		return;

	const auto& serverTemplate = objectTemplate->getFullTemplateString();
	const auto& clientTemplate = objectTemplate->getClientTemplateFileName();

	if (!serverTemplate.isEmpty())
		message->insertAttribute("Server Template", serverTemplate);

	if (!clientTemplate.isEmpty())
		message->insertAttribute("Shared Template", clientTemplate);
}

} // namespace TemplateAttributes

#endif /* TEMPLATEATTRIBUTES_H_ */
