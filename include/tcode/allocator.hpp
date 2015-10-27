#ifndef __tcode_allocator_h__
#define __tcode_allocator_h__

namespace tcode {

	/*!
		@class	allocator
		@brief	�Ҵ��� �������̽�
		@detail	������ �ּ�ȭ �ϱ� ���� custom allocator �� �ʿ��� object ����
		�켱 �� interface �� ����ϰ� ���Ŀ� �����Ͽ� ����� �뵵
		����� std::xxx �⺻�Լ��� ����ó���Ǿ� ����
	*/
    struct allocator{
        allocator( void );

        void* (*alloc)( const std::size_t );
		void* (*realloc)(void* p, const std::size_t sz);
        void  (*free)( void* );
    };

}

#endif
