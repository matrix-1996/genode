/*
 * \brief  Spartan syscalls
 * \author Tobias Börtitz
 * \date   2012-08-14
 */

/*
 * Copyright (C) 2010-2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__SPARTAN__SYSCALLS_H_
#define _INCLUDE__SPARTAN__SYSCALLS_H_

#include <base/stdint.h>
#include <base/native_types.h>

namespace Spartan
{

#include <sys/types.h>
#include <abi/ipc/ipc.h>
#include <abi/ipc/methods.h>
#include <abi/mm/as.h>
#include <abi/errno.h>

#define __SYSCALL0(id) \
	__syscall(0, 0, 0, 0, 0, 0, id)
#define __SYSCALL1(id, p1) \
	__syscall(p1, 0, 0, 0, 0, 0, id)
#define __SYSCALL2(id, p1, p2) \
	__syscall(p1, p2, 0, 0, 0, 0, id)
#define __SYSCALL3(id, p1, p2, p3) \
	__syscall(p1, p2, p3, 0, 0, 0, id)
#define __SYSCALL4(id, p1, p2, p3, p4) \
	__syscall(p1, p2, p3, p4, 0, 0, id)
#define __SYSCALL5(id, p1, p2, p3, p4, p5) \
	__syscall(p1, p2, p3, p4, p5, 0, id)
#define __SYSCALL6(id, p1, p2, p3, p4, p5, p6) \
	__syscall(p1, p2, p3, p4, p5, p6, id)

#define ipc_call_sync_2_0(phoneid, method, arg1, arg2) \
	ipc_call_sync_fast((phoneid), (method), (arg1), (arg2), 0, 0, 0, 0, \
		0, 0)
#define ipc_call_sync_3_0(phoneid, method, arg1, arg2, arg3) \
	ipc_call_sync_fast((phoneid), (method), (arg1), (arg2), (arg3), 0, 0, 0, \
		0, 0)
#define ipc_call_sync_2_5(phoneid, method, arg1, arg2, res1, res2, \
		res3, res4, res5) \
	ipc_call_sync_fast((phoneid), (method), (arg1), (arg2), 0, \
		(res1), (res2), (res3), (res4), (res5))
#define ipc_call_sync_3_5(phoneid, method, arg1, arg2, arg3, res1, res2, \
		res3, res4, res5) \
	ipc_call_sync_fast((phoneid), (method), (arg1), (arg2), (arg3), \
		(res1), (res2), (res3), (res4), (res5))
#define ipc_call_sync_4_0(phoneid, method, arg1, arg2, arg3, arg4) \
	ipc_call_sync_slow((phoneid), (method), (arg1), (arg2), (arg3), (arg4), \
		0, 0, 0, 0, 0, 0)
#define ipc_call_sync_5_0(phoneid, method, arg1, arg2, arg3, arg4, arg5) \
	ipc_call_sync_slow((phoneid), (method), (arg1), (arg2), (arg3), (arg4), \
		(arg5), 0, 0, 0, 0, 0)

#define ipc_call_async_2(phoneid, method, arg1, arg2) \
	ipc_call_async_fast((phoneid), (method), (arg1), (arg2), 0, 0)
#define ipc_call_async_5_0(phoneid, method, arg1, arg2, arg3, arg4, arg5) \
	ipc_call_async_slow((phoneid), (method), (arg1), (arg2), (arg3), (arg4), \
		(arg5))

/*
 * User-friendly wrappers for ipc_answer_fast() and ipc_answer_slow().
 * They are in the form of ipc_answer_m(), where m is the number of return
 * arguments. The macros decide between the fast and the slow version according
 * to m.
 */
#define ipc_answer_0(callid, retval) \
	ipc_answer_fast((callid), (retval), 0, 0, 0, 0)
#define ipc_answer_1(callid, retval, arg1) \
	ipc_answer_fast((callid), (retval), (arg1), 0, 0, 0)
#define ipc_answer_2(callid, retval, arg1, arg2) \
	ipc_answer_fast((callid), (retval), (arg1), (arg2), 0, 0)
#define ipc_answer_3(callid, retval, arg1, arg2, arg3) \
	ipc_answer_fast((callid), (retval), (arg1), (arg2), (arg3), 0)
#define ipc_answer_4(callid, retval, arg1, arg2, arg3, arg4) \
	ipc_answer_fast((callid), (retval), (arg1), (arg2), (arg3), (arg4))
#define ipc_answer_5(callid, retval, arg1, arg2, arg3, arg4, arg5) \
	ipc_answer_slow((callid), (retval), (arg1), (arg2), (arg3), (arg4), (arg5))


	void io_port_enable(Genode::addr_t pio_addr, Genode::size_t size);
	void exit(int status);

	Genode::Native_task task_get_id(void);
	Genode::Native_thread_id thread_get_id(void);
	Genode::Native_thread thread_create(void* ip, void* sp, 
			Genode::addr_t stack_size, const char* name);

	/** Wait for first IPC call to come. */
	Genode::Native_ipc_callid ipc_wait_cycle(Genode::Native_ipc_call *call,
			Genode::addr_t usec, unsigned int flags);
	/** Wait for first IPC call to come. */
	Genode::Native_ipc_callid 
		ipc_wait_for_call_timeout(Genode::Native_ipc_call *call, 
				Genode::addr_t usec);
	/** Check if there is an IPC call waiting to be picked up. */
	Genode::Native_ipc_callid 
		ipc_trywait_for_call(Genode::Native_ipc_call *call);

	/** Request callback connection. */
	int ipc_connect_to_me(int phoneid, Genode::Native_thread_id my_threadid,
			Genode::addr_t arg2, Genode::addr_t arg3,
			Genode::Native_task *task_id,
			Genode::addr_t *phonehash);
	/** Request new connection. */
	int ipc_connect_me_to(int phoneid, Genode::Native_thread_id dest_threadid,
			Genode::Native_thread_id my_threadid);

	int ipc_clone_connection(int phoneid, Genode::Native_thread_id dst_thread_id,
			long cap_id, Genode::Native_thread_id phone_target_thread,
			int clone_phone);

	/**************************
	 * Synchronoous Framework *
	 **************************/
	/* Fast synchronous call */
	int ipc_call_sync_fast(int phoneid, Genode::addr_t method,
			Genode::addr_t arg1, Genode::addr_t arg2,
			Genode::addr_t arg3, Genode::addr_t *result1,
			Genode::addr_t *result2, Genode::addr_t *result3,
			Genode::addr_t *result4, Genode::addr_t *result5);
	int ipc_call_sync_slow(int phoneid, Genode::addr_t method, 
			Genode::addr_t arg1, Genode::addr_t arg2,
			Genode::addr_t arg3, Genode::addr_t arg4,
			Genode::addr_t arg5, Genode::addr_t *result1,
			Genode::addr_t *result2, Genode::addr_t *result3,
			Genode::addr_t *result4, Genode::addr_t *result5);


	/*************************
	 * Asynchronous Framwork *
	 *************************/
	/** Fast asynchronous call. */
	Genode::Native_ipc_callid ipc_call_async_fast(int phoneid,
		Genode::addr_t imethod, Genode::addr_t arg1,
		Genode::addr_t arg2, Genode::addr_t arg3, Genode::addr_t arg4);
	Genode::Native_ipc_callid ipc_call_async_slow(int phoneid, 
		Genode::addr_t imethod, Genode::addr_t arg1, 
		Genode::addr_t arg2, Genode::addr_t arg3, Genode::addr_t arg4,
		Genode::addr_t arg5);

	/***************
	 * IPC answers *
	 ***************/
	/** Answer received call (fast version).
	 * @return Zero on success.
	 * @return Value from @ref errno.h on failure.
	 */
	Genode::addr_t ipc_answer_fast(Genode::Native_ipc_callid callid,
			Genode::addr_t retval, Genode::addr_t arg1,
			Genode::addr_t arg2, Genode::addr_t arg3,
			Genode::addr_t arg4);
	/** Answer received call (entire payload).
	 * @return Zero on success.
	 * @return Value from @ref errno.h on failure.
	 */
	Genode::addr_t ipc_answer_slow(Genode::Native_ipc_callid callid,
			Genode::addr_t retval, Genode::addr_t arg1,
			Genode::addr_t arg2, Genode::addr_t arg3,
			Genode::addr_t arg4, Genode::addr_t arg5);

	int ipc_forward_fast(Genode::Native_ipc_callid callid, int phoneid,
			Genode::addr_t imethod, Genode::addr_t arg1,
			Genode::addr_t arg2, unsigned int mode);
	int ipc_forward_slow(Genode::Native_ipc_callid callid, int phoneid,
			Genode::addr_t imethod, Genode::addr_t arg1,
			Genode::addr_t arg2, Genode::addr_t arg3,
			Genode::addr_t arg4, Genode::addr_t arg5,
			unsigned int mode);

	/** Wrappers for IPC_M_DATA_WRITE calls. */
	int ipc_data_write_start_synch(int phoneid, 
			Genode::Native_thread_id dst_thread, const void *src,
			size_t size, bool is_reply=false);
	int ipc_data_write_start_asynch(int phoneid, 
			Genode::Native_thread_id dst_thread, const void *src,
			size_t size, bool is_reply=false);
	/** Wrapper for answering the IPC_M_DATA_WRITE calls */
	int ipc_data_write_finalize(Genode::Native_ipc_callid callid,
			void *dst, Genode::addr_t size);

	/** Wrapper for receiving the IPC_M_DATA_READ calls */
	int ipc_data_read_start_synch(int phoneid,
			Genode::Native_thread_id dst_thread, void *dst, size_t size);
	int ipc_data_read_finalize(Genode::Native_ipc_callid callid,
			const void *src, size_t size);

	/** Hang up a phone. */
	int ipc_hangup(int phoneid);

	/***************
	 * Futex calls *
	 ***************/
	int futex_sleep(volatile int *futex);
	int futex_wakeup(volatile int *futex);

	/***************
	 * Timer calls *
	 ***************/
	int usleep(Genode::addr_t usec);
	void udelay(Genode::addr_t time);

	/***************************
	 * Address spaces handling *
	 ***************************/
	enum {
		PROT_READ = AS_AREA_READ,
		PROT_WRITE = AS_AREA_WRITE,
	};
	void *as_area_create(void *base, size_t size, unsigned int flags);
	int as_area_resize(void *address, size_t size, unsigned int flags);
	int as_area_destroy(void *address);
	int as_area_change_flags(void *address, unsigned int flags);
}

#endif /* _INCLUDE__SPARTAN__SYSCALLS_H_ */