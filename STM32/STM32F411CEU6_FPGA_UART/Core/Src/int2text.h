static inline int dec_unsigned_to_str(unsigned int value, char *dst)
{
	int c = 0;
	char *h = dst, buf[32];

	if(!value)
	{
		dst[0] = '0';
		dst[1] = 0;
		return 2;
	};

	while(value)
	{
		buf[c++] = 0x30 + (value % 10);
		value /= 10;
	}

	while(c > 0)
	{
		*h = buf[c - 1];
		h++;
		c--;
	}

	*h = 0;

	return h - dst;
}

static inline int dec_signed_to_str(int value, char *dst)
{
	if(!value)
	{
		dst[0] = '0';
		dst[1] = 0;
		return 2;
	};

	if(value < 0)
	{
		value = -value;
		dst[0] = '-';
	}
	else
		dst[0] = '+';

	return 1 + dec_unsigned_to_str(value, dst + 1);
}

static inline int to_dec(int value, char *dst)
{
	int c = 0;
	char *h = dst, buf[32], sign = 0;

	if(!value)
	{
		dst[0] = '0';
		dst[1] = 0;
		return 2;
	};

	if(value < 0)
	{
		value = -value;
		sign = '-';
	}
	else
		sign = '+';

	while(value)
	{
		buf[c++] = 0x30 + (value % 10);
		value /= 10;
	}

	if(sign)
	{
		*h = sign;
		h++;
	}

	while(c > 0)
	{
		*h = buf[c - 1];
		h++;
		c--;
	}

	*h = 0;

	return h - dst;
}
