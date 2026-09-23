#ifndef JETPACKTEMPLATE_H_
#define JETPACKTEMPLATE_H_

#include "system/lang/String.h"

namespace JetpackTemplate {

inline bool isJetpack(uint32 serverObjectCRC) {
	return serverObjectCRC == STRING_HASHCODE("object/mobile/vehicle/jetpack.iff") ||
			serverObjectCRC == STRING_HASHCODE("object/mobile/vehicle/tcg_hk47_jetpack.iff") ||
			serverObjectCRC == STRING_HASHCODE("object/mobile/vehicle/tcg_merr_sonn_jt12_jetpack.iff");
}

} // namespace JetpackTemplate

#endif /* JETPACKTEMPLATE_H_ */
