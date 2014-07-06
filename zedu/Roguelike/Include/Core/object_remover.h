#pragma once

#include <assert.h>
#include "Thread/lock.h"
#include <list>
#include <algorithm>

namespace zedu {

	template<typename T>
	struct ObjectRemover
	{
		CriticalSection m_lock;
		std::list<T> m_objList;

		void AddObject( T pObj )
		{
			THREAD_SYNC( m_lock );

			std::list<T>::iterator it = std::find( m_objList.begin(), m_objList.end(), pObj );
			if( it == m_objList.end() )
			{
				m_objList.push_back( pObj );
			}
			else
			{
				assert( 0 );
			}
		}

		void DelObject()
		{
			THREAD_SYNC( m_lock );

			std::list<T>::iterator it = m_objList.begin();
			for( ; it != m_objList.end(); it++ )
			{
				delete (*it);
			}

			m_objList.clear();
		}

		int Count() { return m_objList.size(); }
	};
}