/*
 * \brief  Generic network address definitions
 * \author Stefan Kalkowski
 * \date   2010-08-20
 */

/*
 * Copyright (C) 2010-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _NET__NETADDRESS_H_
#define _NET__NETADDRESS_H_

/* Genode */
#include <base/stdint.h>
#include <util/string.h>

namespace Net {

	/**
	 * Generic form of a network address.
	 */
	template <unsigned LEN>
	class Network_address
	{
		public:

			Genode::uint8_t addr[LEN];


			/******************
			 ** Constructors **
			 ******************/

			Network_address(Genode::uint8_t value = 0) {
				Genode::memset(&addr, value, LEN); }

			Network_address(void *src) {
				Genode::memcpy(&addr, src, LEN); }


			/***********************
			 ** Helper functions  **
			 ***********************/

			void copy(void *dst) { Genode::memcpy(dst, addr, LEN); }


			/***************
			 ** Operators **
			 ***************/

			bool operator==(const Network_address &other) const {

				/*
				 * We compare from lowest address segment to highest
				 * one, because in a local context, the higher segments
				 * of two addresses normally don't distinguish.
				 * (e.g. in an IPv4 local subnet)
				 */
				for (int i = LEN-1; i >= 0; --i) {
					if (addr[i] != other.addr[i])
						return false;
				}
				return true;
			}
	};
}

#endif /* _NET__NETADDRESS_H_ */
