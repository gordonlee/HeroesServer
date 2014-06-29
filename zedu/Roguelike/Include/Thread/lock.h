#pragma once
#include <string>
#include "Core/types.h"
#include "Util/w32_util.h"
#include <windows.h>

namespace zedu {

	enum { Lockable_CS=1, Lockable_Spin=2, Lockable_ILock=3 };

	class ILock
	{
	public:
		enum { TypeValue = Lockable_ILock };

		ILock( const char* name="NoName", int lockOrder=-1 )
			: m_name(name), m_pFileName(""), m_lineNumber(0), m_acqureCount(0), m_lockOrder(lockOrder)
		{
		}

		virtual ~ILock() {}

		virtual void Lock( const char* fileName="", int lineNumber=0 ) = 0;
		virtual void Unlock() = 0;
		virtual bool IsLocked() = 0;
		virtual bool TryLock( const char* fileName="", int lineNumber=0 ) { return false; }

		void OnLock( const char *fileName, int lineNumber ) {}
		void OnUnlock()										{}
		void OnAcquire()									{ ++m_acqureCount; }

	protected:
		std::string m_name;
		const char* m_pFileName;
		int m_lineNumber;
		int m_acqureCount;
		int m_lockOrder;
	};

	template<typename T>
	struct Lockable
	{
	public:
		typedef T Type_t;

		void Lock()				{}
		void Unlock()			{}
		bool IsLocked() const	{ return false; }
	};

	template<>
	struct Lockable<CRITICAL_SECTION>
	{
		enum { TypeValue = Lockable_CS };

		CRITICAL_SECTION m_cs;
		volatile int m_lockCount;

		Lockable()
		{
			InitializeCriticalSection( &m_cs );
		}

		~Lockable()
		{
			DeleteCriticalSection( &m_cs );
		}

		void Lock()
		{
			EnterCriticalSection( &m_cs );
			interlocked_increment( &m_lockCount );
		}

		void Unlock()
		{
			LeaveCriticalSection( &m_cs );
			interlocked_decrement( &m_lockCount );
		}

		bool IsLocked() { return m_lockCount > 0; }
	};
	typedef Lockable<CRITICAL_SECTION> CriticalSection;

	struct Spin {};
	template<>
	struct Lockable<Spin>
	{
		enum { TypeValue =  Lockable_Spin };

		volatile int m_lockCount;
		volatile bool m_bLocked;

		Lockable()
		{
			spinlock_init( &m_lockCount );
		}
		void Lock()
		{
			spinlock_enter( &m_lockCount );
			m_bLocked = true;
		}

		void Unlock()
		{
			spinlock_leave( &m_lockCount );
			m_bLocked = false;
		}

		bool IsLocked() const { return m_bLocked; }
	};
	typedef Lockable<Spin> SpinLock;

	template<typename Lockable>
	struct ScopedLock
	{
	private:
		Lockable& m_lock;

	public:
		ScopedLock( Lockable& lock ) : m_lock(lock)
		{
			m_lock.Lock();
		}
		~ScopedLock()
		{
			m_lock.Unlock();
		}
	};

	template<int N> struct LockableDeducer {};
	template<> struct LockableDeducer<Lockable_CS>		{ typedef CriticalSection Type_t; };
	template<> struct LockableDeducer<Lockable_Spin>	{ typedef SpinLock Type_t; };
}

#define THREAD_SYNC( obj )		ScopedLock<LockableDeducer<obj.TypeValue>::Type_t> __sync_obj( obj );