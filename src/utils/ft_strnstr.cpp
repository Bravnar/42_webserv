#include "./ft_strnstr.hpp"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i, j = 0;
	size_t	size = strlen(needle);

	if (size == 0)
		return ((char *)haystack);
	i = 0;
	while (haystack[i] && i < len) {
		j = 0;
		while (needle[j] && haystack[i + j] == needle[j] && i + j < len)
			j++;
		if (j == size)
			return ((char *)haystack + i);
		i++;
	}
	return (0);
}