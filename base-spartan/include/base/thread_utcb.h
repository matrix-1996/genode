#ifndef _INCLUDE__BASE__THREAD_UTCB_H_
#define _INCLUDE__BASE__THREAD_UTCB_H_

#include <base/native_types.h>
#include <base/ipc_call.h>
#include <base/ipc_call_queue.h>
#include <base/lock.h>

namespace Genode {
	enum {
		THREAD_KILLED = -30,
	};

	class Thread_utcb
	{
		private:
			Native_task		_task_id;
			Native_thread_id	_thread_id;
			Ipc_call_queue		_call_queue;

			Ipc_call		_ipc_answer;
			Lock			_answer_lock;
			bool			_answer_used;

		public:
			explicit Thread_utcb()
			: _task_id(Spartan::task_get_id()),
			  _thread_id(Spartan::thread_get_id()) {}
			~Thread_utcb();

			Native_task	task_id() { return _task_id; }
			Native_thread_id thread_id() { return _thread_id; }
			void		insert_call(Ipc_call call);
			Ipc_call	get_next_call(addr_t imethod=0);
			Ipc_call	wait_for_call(addr_t imethod=0);

			bool		insert_reply(Ipc_call call);
			Ipc_call	get_reply();
	};

	typedef Thread_utcb Native_utcb;
}

#endif /* _INCLUDE__BASE__THREAD_UTCB_H_ */
