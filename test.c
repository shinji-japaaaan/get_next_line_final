#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#ifndef FD_MAX
# define FD_MAX 1024
#endif

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 1024
#endif

int ft_strlen(char *str)
{
    int len = 0;

    while (*str)
    {
        str++;
        len++;
    }
    return (len);
}

char *ft_strdup(char *str)
{
    int len = 0;
    char *new_str;
    int i;

    if (str == NULL)
        return (NULL);
    len = ft_strlen(str);
    new_str = malloc(len + 1);
    if (new_str == NULL)
        return (NULL);
    i = 0;
    while (str[i])
    {
        new_str[i] = str[i];
        i++;
    }
    new_str[i] = '\0';
    return (new_str);
}

char *ft_strnjoin(char *str, char *buf, int count_bytes)
{
    int len = 0;
    char *new_str;

    len = ft_strlen(str);
    new_str = malloc(len + count_bytes + 1);
    if (!new_str)
        return(NULL);
    int i = 0;
    while (str[i])
    {
        new_str[i] = str[i];
        i++;
    }
    int j = 0;
    while (j < count_bytes && buf[j])
    {
        new_str[i] = buf[j];
        i++;
        j++;
    }
    new_str[i] = '\0';
    return (new_str);
}

int append_buffer(char **str, char *buf, int count_bytes)//引数を*strとしてしまい、戻り値をどうするか悩んだ
{
    char *tmp;

    buf[count_bytes] = '\0';
    tmp = *str;//tmpに代入して、tmpを後で解放することができなかった
    *str = ft_strnjoin(*str, buf, count_bytes);
    if (*str == NULL)
    {
        free(tmp);//エラーの時にtmpをfree
        return (-1);
    }
    free(tmp); //buf,strを結合したら、もともとあった*strは解放
    return (0);
}

char *ft_strchr_len(char *str, char c)//ここはintではなくてはいけないか？
{
    if (str == NULL)
        return (NULL);//ここのエラー処理ができていなかった
    while (*str)
    {
        if (*str == c)
            return (str);
        str++;
    }
    return (NULL);//最後のreturnを記載できていなかった
}

int read_fd(int fd, char **str)
{
    char *buf;
    int count_bytes = 1;

    buf = malloc(BUFFER_SIZE + 1);
    if (buf == NULL)
        return (-1);
    while (count_bytes > 0)//count_bytes = 0;としてしまっていたのでwhileに入らなかった
    {
        count_bytes = read(fd, buf, BUFFER_SIZE);
        if (count_bytes <= 0)
            break;
        if (append_buffer(str, buf, count_bytes) < 0)
            break;
        if (ft_strchr_len(*str, '\n'))//戻り値をどうすればいいかわからなくなってしまった
            break;
    }
    free(buf);
    return (count_bytes);
}

void safe_free(char **str)
{
    if (str != NULL && *str != NULL)//コード書いている中で、このifの役割が二重freeを防ぐことであるとわかった,if文の中は||ではなく＆＆
    {
        free(*str);
        *str = NULL; //*str == NULLとしてしまっていた!!!
    }
}

char *ft_strncpy(char *line, char *str, int len)
{
    if (line == NULL || str == NULL || len <= 0)
    {
        return (NULL);
    }
    int i = 0;
    while (str[i] && i < len)//i < lenの書き方ができなかった
    {
        line[i] = str[i];
        i++;
    }
    while (i < len)//ヌルでうめるここのコードが書けなかった
    {
        line[i] = '\0';
        i++; //iがぬけていた
    }
    line[len] = '\0';//関数の中でこれをするようにした
    return (line);
}

char *find_newline(char **str)//ここでエラーがおきたらstrをフリーしていい
{
    char *new_pos;
    int len = 0;
    char *line;
    char *tmp;

    new_pos = ft_strchr_len(*str, '\n');
    if (new_pos)
        len = new_pos - *str + 1;
    else
        len = ft_strlen(*str);
    line = malloc(len + 1);
    if (!line)
        return (NULL);
    if (ft_strncpy(line, *str, len) == NULL)//ここのエラー処理を追加した。まだ、戻ってきて処理再開できるようにstrは解放しない
    {
        free(line);
        return (NULL);
    }
    if (new_pos)
    {
        tmp = ft_strdup(new_pos + 1);//ここでtmpではなく、直接*strにいれてしまっていた、引数として*str + lenとしたが、new_pos + 1の方がシンプル
        if (!tmp)//*strではなくtmp
        {
            safe_free(str);
            free(line);//ここのfreeができていなかった
            return (NULL);
        }
        safe_free(str);
        *str = tmp;//上で*strをfreeしてから、tmpを*strにいれることができていなかった
    }
    else
        safe_free(str);//ここでもfreeできていなかった。strdupを使うことにしたので、ft_strcpyは使わない。ただ、lineにただコピーした後、もとのstrはfreeしてあげる必要がある
    return (line);
}

char *get_next_line(int fd)
{
    static char *str[FD_MAX]; //!!staticをつけていなかった
    char *line;
    int res = 0;

    if (fd < 0 || FD_MAX <= 0 || BUFFER_SIZE <= 0)
    {
        return (NULL);
    }
    if (str[fd] == NULL)
        str[fd] = ft_strdup("");
    if (str[fd] == NULL)
        return (NULL);
    res = read_fd(fd, &str[fd]);
    if (res < 0 || *str[fd] == '\0')//*str[fd] *をつけるのわすれないこと
    {
        safe_free(&str[fd]);
        return (NULL);
    }
    line = find_newline(&str[fd]);
    if (line == NULL)
    {
        safe_free(&str[fd]);
        return (NULL);
    }
    // safe_free(&str[fd]); //なぜここでsafe_freeしているのか
    return (line);
}

int main()
{
    int fd = 0;
    char *line;

    printf("BUFFER_SIZE = %d\n", BUFFER_SIZE);
    if ((fd = open("test.txt", O_RDONLY)) == -1)//ここのO_RDONLYの書き方も注意
    {
        perror("failed to open");
        return (-1);
    }
    while ((line = get_next_line(fd)) != NULL)//if文にしてしまっていた
    {
        printf("%s\n", line);
        free(line);
        line = NULL; //ここでNULLをいれていなかった
        // close(fd); //ここでcloseはしてはいけない
        // return (-1); //ここはいらない
    }
    close(fd);
    return (0);
}
