#ifndef _H_CRYPT_CONFIG 
#define _H_CRYPT_CONFIG
#define USE_ASM 1
#define __x86_64__ 1
#define USE_AVX2 1

#define HAVE_SHA256_8WAY 1
#define HAVE_SHA256_4WAY 1

#define SCRYPT_MAX_WAYS 24
#define HAVE_SCRYPT_6WAY 1
#define HAVE_SCRYPT_3WAY 1
#ifdef __cplusplus
extern "C" {
#endif
	static __inline uint32_t be32dec(const void *pp)
	{
		const uint8_t *p = (uint8_t const *)pp;
		return ((uint32_t)(p[3]) + ((uint32_t)(p[2]) << 8) +
			((uint32_t)(p[1]) << 16) + ((uint32_t)(p[0]) << 24));
	}
	static __inline uint32_t le32dec(const void *pp)
	{
		const uint8_t *p = (uint8_t const *)pp;
		return ((uint32_t)(p[0]) + ((uint32_t)(p[1]) << 8) +
			((uint32_t)(p[2]) << 16) + ((uint32_t)(p[3]) << 24));
	}
	static __inline void be32enc(void *pp, uint32_t x)
	{
		uint8_t *p = (uint8_t *)pp;
		p[3] = x & 0xff;
		p[2] = (x >> 8) & 0xff;
		p[1] = (x >> 16) & 0xff;
		p[0] = (x >> 24) & 0xff;
	}
	static __inline void le32enc(void *pp, uint32_t x)
	{
		uint8_t *p = (uint8_t *)pp;
		p[0] = x & 0xff;
		p[1] = (x >> 8) & 0xff;
		p[2] = (x >> 16) & 0xff;
		p[3] = (x >> 24) & 0xff;
	}
	static __inline uint32_t swab32(uint32_t x)
	{
		return  ((x & (uint32_t)0x000000ffUL) << 24) |
			((x & (uint32_t)0x0000ff00UL) << 8) |
			((x & (uint32_t)0x00ff0000UL) >> 8) |
			((x & (uint32_t)0xff000000UL) >> 24);
	}

	void sha256_init(uint32_t *state);
	void sha256_transform(uint32_t *state, const uint32_t *block, int swap);
	void sha256d(unsigned char *hash, const unsigned char *data, int len);

	int sha256_use_4way();
	void sha256_init_4way(uint32_t *state);
	void sha256_transform_4way(uint32_t *state, const uint32_t *block, int swap);
	int sha256_use_8way();
	void sha256_init_8way(uint32_t *state);
	void sha256_transform_8way(uint32_t *state, const uint32_t *block, int swap);

	int scrypt_best_throughput();
	 void scrypt_1024_1_1_256(const uint32_t *input, uint32_t *output, uint32_t *midstate, unsigned char *scratchpad, int N);
		 void scrypt_1024_1_1_256_3way(const uint32_t *input, uint32_t *output, uint32_t *midstate, unsigned char *scratchpad, int N);
		 void scrypt_1024_1_1_256_4way(const uint32_t *input, uint32_t *output, uint32_t *midstate, unsigned char *scratchpad, int N);
		 void scrypt_1024_1_1_256_12way(const uint32_t *input, uint32_t *output, uint32_t *midstate, unsigned char *scratchpad, int N);
		 void scrypt_1024_1_1_256_24way(const uint32_t *input, uint32_t *output, uint32_t *midstate, unsigned char *scratchpad, int N);
#ifdef __cplusplus
}
#endif
#endif

