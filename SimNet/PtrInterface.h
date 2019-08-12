//Copyright(c), All Rights Reserved.
#pragma once
#ifndef FWK_PTRINTERFACE_H
#define FWK_PTRINTERFACE_H

#include "Types.h"

// forward declaration of class template Ptr
template <class T> class Ptr;

namespace Fwk {

	template <class T>
	class PtrInterface {
	private:
		long unsigned ref_;
	public:
		PtrInterface() : ref_(1) {}
		unsigned long references() const { return ref_; }
		enum Attribute {
			nextAttributeNumber__ = 1
		};
		// support for templates
		inline const PtrInterface * newRef() const;
		inline void deleteRef() const;
		inline void referencesDec(U32 dec) const;
	protected:
		virtual ~PtrInterface() {}

		virtual void onZeroReferences() { delete this; }
		friend class Ptr<T>;
	};

	template<class T> const PtrInterface<T> *
		PtrInterface<T>::newRef() const {
		PtrInterface<T> *me = const_cast<PtrInterface<T> *>(this);
		++me->ref_;
		return this;
	}

	template<class T> void
		PtrInterface<T>::deleteRef() const {
		PtrInterface<T> *me = const_cast<PtrInterface<T> *>(this);
		if (--me->ref_ == 0) me->onZeroReferences();
	}

	template<class T> void
		PtrInterface<T>::referencesDec(U32 dec) const {
		PtrInterface<T> *me = const_cast<PtrInterface<T> *>(this);
		if ((me->ref_ -= dec) == 0) me->onZeroReferences();
	}

}

#endif
