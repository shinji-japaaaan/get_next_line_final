#include "get_next_line.h"

char        *ft_strncpy(char *dest, const char *src, int n)
{
        int        i;

        if (dest == NULL || src == NULL || n < 0)
                return (NULL); 

        i = 0;
        while (i < n && src[i] != '\0')
        {
                dest[i] = src[i];
                i++;
        }
        while (i < n)
        {
                dest[i] = '\0';
                i++;
        }
        return (dest);
}

char        *ft_strcpy(char *dest, const char *src)
{
        int        i;

        if (dest == NULL || src == NULL)  // NULLポインタチェック
                return (NULL);

        i = 0;
        while (src[i] != '\0')
        {
                dest[i] = src[i];
                i++;
        }
        dest[i] = '\0';
        return (dest);
}

char        *ft_strnjoin(char const *s1, char const *s2, int n)
{
        size_t                s1_len;
        char                *result;
        size_t                i;
        size_t                j;

        if (s1 == NULL || s2 == NULL || n < 0)  // NULLポインタとnのチェック
                return (NULL);
        s1_len = ft_strlen(s1);
        result = (char *)malloc(s1_len + n + 1);  // 確保するメモリのサイズを正しく計算
        if (result == NULL)
                return (NULL);  // malloc失敗時にNULLを返す
        i = 0;
        while (s1[i] != '\0')
        {
                result[i] = s1[i];
                i++;
        }
        j = 0;
        while (j < n && s2[j] != '\0')  // nに制限を加える
        {
                result[i++] = s2[j++];
        }
        result[i] = '\0';
        return (result);
}


size_t        ft_strlen(const char *str)
{
        size_t        length;

        length = 0;
        while (*str != '\0')
        {
                length++;
                str++;
        }
        return (length);
}

char        *ft_strchr_len(const char *str, int c)
{
        int        i;

        if (str == NULL)  // NULLポインタチェック
                return (NULL);

        i = 0;
        while (*str != '\0')
        {
                if (*str == (char)c)
                        return ((char *)str);
                str++;
                i++;
        }
        return (NULL);
}


char        *ft_strdup(const char *src)
{
        size_t        len;
        char        *dest;
        char        *ptr;

        if (src == NULL)  // NULLポインタチェック
                return (NULL);

        len = ft_strlen(src);
        dest = (char *)malloc(len + 1);
        if (dest == NULL)
                return (NULL);  // malloc失敗時にNULLを返す

        ptr = dest;
        while (*src != '\0')
        {
                *ptr = *src;
                ptr++;
                src++;
        }
        *ptr = '\0';
        return (dest);
}

static void        safe_free(char **ptr)
{
        if (ptr != NULL && *ptr != NULL)
        {
                free(*ptr);
                *ptr = NULL;
        }
}

static char        *find_newline(char **st_arr)
{
        char        *newline_pos;
        char        *line;
        int                len;

        newline_pos = ft_strchr_len(*st_arr, '\n');
        if (newline_pos)
                len = newline_pos - *st_arr + 1;
        else
                len = ft_strlen(*st_arr);
        line = malloc(sizeof(char) * (len + 1));
        if (!line)
                return (NULL);
        ft_strncpy(line, *st_arr, len);
        line[len] = '\0';
        if (newline_pos)
                ft_strcpy(*st_arr, newline_pos + 1);
        else
                safe_free(st_arr);
        return (line);
}

static int        append_buffer(char **st_arr, char *buf, int count_bytes)
{
        char        *tmp;

        buf[count_bytes] = '\0';
        tmp = *st_arr;
        *st_arr = ft_strnjoin(*st_arr, buf, count_bytes);
        if (!*st_arr)
        {
                free(tmp);
                return (-1);
        }
        free(tmp);
        return (0);
}

static int        read_fd(int fd, char **st_arr)
{
        char        *buf;
        int                count_bytes;

        buf = malloc(BUFFER_SIZE + 1);
        if (!buf)
                return (-1);
        while (1)
        {
                count_bytes = read(fd, buf, BUFFER_SIZE);
                if (count_bytes <= 0 || append_buffer(st_arr, buf, count_bytes) < 0)
                        break ;
                if (ft_strchr_len(*st_arr, '\n'))
                        break ;
        }
        free(buf);
        if (count_bytes < 0)
                return (-1);
        return (0);
}

char        *get_next_line(int fd)
{
        static char        *st_arr[FD_MAX];
        char                *line;
        int                        ret;

        if (fd < 0 || fd >= FD_MAX || BUFFER_SIZE <= 0)
                return (NULL);
        if (st_arr[fd] == NULL && (st_arr[fd] = ft_strdup("")) == NULL)
                return (NULL);
        ret = read_fd(fd, &st_arr[fd]);
        if (ret < 0 || (ret == 0 && *st_arr[fd] == '\0'))
        {
                safe_free(&st_arr[fd]);
                return (NULL);
        }
        line = find_newline(&st_arr[fd]);
        if (line == NULL)
                safe_free(&st_arr[fd]);
        return (line);
}
