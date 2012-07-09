/*
 * \brief  IPC implementation for OKL4
 * \author Norman Feske
 * \date   2009-03-25
 */

/*
 * Copyright (C) 2009-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#include <base/printf.h>
#include <base/ipc.h>
#include <base/native_types.h>
#include <base/ipc_call.h>
#include <base/ipc_manager.h>

#include <spartan/syscalls.h>

using namespace Genode;

enum {
	PHONE_CORE = 0,
};

/*****************
 ** Ipc_ostream **
 *****************/

void Ipc_ostream::_send()
{
	/* perform IPC send operation
	 *
	 * Check whether the phone_id is valid and send the message
	 */
	if(_dst.dst().snd_phone < 1 ||
		Spartan::ipc_data_write_start_synch(_dst.dst().snd_phone,
			_dst.dst().rcv_task_id, _dst.dst().rcv_thread_id,
			_snd_msg->buf, _snd_msg->size()) != 0 ) {
		PERR("ipc error in _send.");
		throw Genode::Ipc_error();
	}

	_write_offset = sizeof(umword_t);
}


Ipc_ostream::Ipc_ostream(Native_capability dst, Msgbuf_base *snd_msg)
:
	Ipc_marshaller(&snd_msg->buf[0], snd_msg->size()),
	_snd_msg(snd_msg), _dst(dst)
{
	printf("Ipc_ostream:\t CONTRUCTOR callcount=%lu, task=%lu, thread=%lu\n", Ipc_manager::singleton()->get_call_count(Spartan::thread_get_id()), Spartan::task_get_id(), Spartan::thread_get_id());
	/*
	 * Establish connection to the other task before communicating
	 */
	int snd_phone = Spartan::ipc_connect_me_to(PHONE_CORE, dst.dst().rcv_task_id,
		dst.dst().rcv_thread_id, Spartan::thread_get_id());

	/*
	 * Overwrite current Destination with new one where the phone_id
	 * for the connection is added (there is no possibility to simply add
	 * the phone_id to the existing _dst)
	 */
	Ipc_destination dest = {dst.dst().rcv_task_id, dst.dst().rcv_thread_id,
		Spartan::task_get_id(), Spartan::thread_get_id(), snd_phone};
	_dst = Native_capability( dest, dst.local_name() );

	_write_offset = sizeof(umword_t);
}


/*****************
 ** Ipc_istream **
 *****************/

void Ipc_istream::_wait()
{
	static bool		connected = false;
	Ipc_call		call;
	addr_t			size;

	/* 
	 * Wait for an incomming connection request if strea, is not connected
	 */
	while(!connected) {
		addr_t retval;
		call = Ipc_manager::singleton()->wait_for_call(Spartan::task_get_id(),
				Spartan::thread_get_id());
		printf("Ipc_istream:\tUNCONNECTED got call with callid %lu\n", call.callid());
		switch(call.call_method()) {
			// TODO replace IPC_M_CONNECT_ME_TO call with cloning of connection
			case IPC_M_CONNECT_ME_TO:
				/* Call is not for me -> reject request */
				if(call.dest_task_id() != Spartan::task_get_id()
					|| call.dest_thread_id() != Spartan::thread_get_id()) {
						Spartan::ipc_answer_0(call.callid(), -1);
						break;
				}

				/* Accept the connection
				 * TODO: make shure the sender has the right Capability ?
				 */
				retval = Spartan::ipc_answer_0(call.callid(), 0);
				printf("Ipc_istream:\t asnwering the connection request gave %lu\n", retval);
				if(retval == 0) {
					_rcv_msg->callid = call.callid();
					_dst.snd_task_id = call.snd_task_id();
					_dst.snd_thread_id = call.snd_thread_id();
					_dst.snd_phonehash = 0;

					connected = true;
				}
				break;
			default:
				/* no connection call -> reject call */
				 Spartan::ipc_answer_0(_rcv_msg->callid, -1);
		}
	}

	/*
	 * Wait for IPC message
	 */
	call = Ipc_manager::singleton()->wait_for_call(Spartan::task_get_id(),
			Spartan::thread_get_id());
	printf("Ipc_istream:\tCONNECTED got call with callid %lu\n", call.callid());
	if(call.call_method() != IPC_M_DATA_WRITE
		|| (call.snd_task_id() != _dst.snd_task_id)
		|| (call.snd_thread_id() != _dst.snd_thread_id)
		|| ((_dst.snd_phonehash != 0) &&
			(call.snd_phonehash() != _dst.snd_phonehash))) {
		/* unknown sender */
		printf("Ipc_istream:\tCONNECTED unkown sender!\n");
		Spartan::ipc_answer_0(_rcv_msg->callid, -1);
		return;
	}
	_rcv_msg->callid = call.callid();
	size = call.call_arg2();
	if(_dst.snd_phonehash == 0)
		_dst.snd_phonehash = call.snd_phonehash();

	/* Retrieve the message */
	int ret = Spartan::ipc_data_write_finalize(_rcv_msg->callid, _rcv_msg->buf, size);
	printf("Ipc_istream:\tCONNECTED wite finalize returned %i\n", ret);

	/* reset unmarshaller */
	_read_offset = sizeof(umword_t);
}


Ipc_istream::Ipc_istream(Msgbuf_base *rcv_msg)
:
	Ipc_unmarshaller(&rcv_msg->buf[0], rcv_msg->size()),
	Native_capability(),
	_rcv_msg(rcv_msg)
{
	_rcv_cs = 0;
	_dst.rcv_task_id = Spartan::task_get_id();
	_dst.rcv_thread_id = Spartan::thread_get_id();
	_dst.snd_phonehash = 0;
	_read_offset = sizeof(umword_t);
}

Ipc_istream::~Ipc_istream() { }

Ipc_server::Ipc_server(Genode::Msgbuf_base *snd_msg, Genode::Msgbuf_base *rcv_msg)
:
	Ipc_istream(rcv_msg),
	Ipc_ostream(Native_capability(), snd_msg)
{}

void Ipc_server::_wait()
{}