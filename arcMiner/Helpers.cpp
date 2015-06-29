#include "Helpers.h"


bool Helpers::PrintBinaryInHex(char* binary, size_t len)
{
	char* hex = (char *)malloc(len * 2 + 1);
	Helpers::BinaryToHex(hex, binary, len);
	printf("%s\n",hex);
	free(hex);
	return true;
}

bool Helpers::HexToBinary(char* hex, char* binary, size_t len)
{
	char hex_byte[3];
	char *ep;

	hex_byte[2] = '\0';

	while (*hex && len) {
		if (!hex[1]) {
			cout << "Helpers::HexToBinary received incomplete hex string";
			return false;
		}
		hex_byte[0] = hex[0];
		hex_byte[1] = hex[1];
		*binary = (unsigned char) strtol(hex_byte, &ep, 16);
		if (*ep) {
			cout << "Helpers::HexToBinary failed on '" << hex_byte << "'";
			return false;
		}
		binary++;
		hex += 2;
		len--;
	}

	return (len == 0 && *hex == 0) ? true : false;
}


bool Helpers::BinaryToHex(char* hex, char* binary, size_t len)
{
	char hex_byte[3];

	while (len) {
		uint8_t a = *((uint8_t*)binary);
		


		if (snprintf(hex_byte, 3, "%x", a) > 1)
		{
			hex[0] = hex_byte[0];
			hex[1] = hex_byte[1];
		}
		else
		{
			hex[0] = '0';
			hex[1] = hex_byte[0];
		}

		binary++;
		hex += 2;
		len--;
	}

	hex[0] = '\0';

	return true;
}


uint32_t Helpers::BigEndian32Decode(const void *buf)
{
	const uint8_t *p = (const uint8_t *)buf;

	return ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

void Helpers::BigEndian32Encode( char  *pp, uint32_t x)
{
	uint8_t *p = (uint8_t *)pp;
	p[3] = x & 0xff;
	p[2] = (x >> 8) & 0xff;
	p[1] = (x >> 16) & 0xff;
	p[0] = (x >> 24) & 0xff;
}
bool Helpers::GenerateExtraNonce(char* nonce, size_t len)
{
	if(4-len < 0 )
		return false;

	memset(nonce, 0, len);

	uint32_t a = GlobalUniqueGenerator;
	GlobalUniqueGenerator++;
	memcpy(nonce + len - 4, &a, 4);

	return true;

}
