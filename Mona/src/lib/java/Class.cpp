// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

//#include <gcj/javaprims.h>
#include <java/lang/Class.h>
#include <gcj/cni.h>
#include <gcj/field.h>
#include <sms_gc/sms_gc.h>
#ifdef MONA
#include <monapi.h>
#else
#include <string.h>
#endif

void
java::lang::Class::initializeClass ()
{
	for (int i = 0; i < static_field_count; i++) {
		if (fields[i].isRef()) {
			*(void**)fields[i].u.addr = NULL;
			sms_gc_register(fields[i].u.addr);
		}
	}
	for (int i = 0; i < method_count; i++) {
		if (strcmp(methods[i].name->data, "<clinit>") == 0) {
			(*(void(*)())methods[i].ncode)();
			break;
		}
	}
	for (int i = 0; i < constants.size; i++) {
		if (constants.tags[i] == 8) {
			void** p = (void**)constants.data + i;
			sms_gc_register(p);
			*p = JvNewStringLatin1(((_Jv_Utf8Const*)*p)->data);
		}
	}
	state = JV_STATE_DONE;
}

