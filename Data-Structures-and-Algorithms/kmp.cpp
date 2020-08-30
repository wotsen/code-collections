#include <stdio.h>

void prefix_t(char *p, int *prefix, int n)
{
	prefix[0] = 0;
	int len = 0;
	int i = 1;
	while (i < n)
	{
		if (p[i] == p[len])
		{
			len++;
			prefix[i] = len;
			i++;
		}
		else
		{
			if (len > 0)
			{
				len = prefix[len - 1];
			}
			else
			{
				prefix[i] = len;
				i++;
			}
		}
	}
}

int main(void)
{
	char*p = "ABABBABAA";
	int prefix[9];

	prefix_t(p, prefix, 9);

	for (int i = 0; i < 9; i++)
	{
		printf("%d\n", prefix[i]);
	}
	return 0;
}