#ifndef __tcode__crc32_h__
#define __tcode__crc32_h__

namespace tcode {

class crc32 {
public:
	crc32(void);
	~crc32(void);

	void update( void* ptr , int size );
    uint32_t value( void );
    void reset( void );
private:
	uint32_t _crc32;
};


}

#endif
