#ifndef SKILLSURRENDERDISPLAY_H_
#define SKILLSURRENDERDISPLAY_H_

#include "engine/engine.h"

#include <algorithm>
#include <cstring>
#include <vector>

class SkillSurrenderDisplay {
public:
	struct Entry {
		String skillName;
		String displayName;
	};

	// Sort a separate list of labels: surrender plans retain dependency order.
	static void sort(std::vector<Entry>& entries) {
		std::sort(entries.begin(), entries.end(), [](const Entry& left, const Entry& right) {
			const String leftLabel = left.displayName.toLowerCase();
			const String rightLabel = right.displayName.toLowerCase();
			const int labelOrder = std::strcmp(leftLabel.toCharArray(), rightLabel.toCharArray());
			if (labelOrder != 0)
				return labelOrder < 0;
			return std::strcmp(left.skillName.toCharArray(), right.skillName.toCharArray()) < 0;
		});
	}
};

#endif /* SKILLSURRENDERDISPLAY_H_ */
