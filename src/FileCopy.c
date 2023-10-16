#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const static int MAX_FILE_NAME_LEN = 1024;

/**
 * @brief `true` iff if given character ends `stdin` buffer.
 */
bool isTerminal(char ch)
{
    return ch == '\n' || ch == EOF;
}

/**
 * @brief Flushes `stdin` in case `read` buffer size unable to hold buffered `stdin`.
 */
void flushStdin()
{
    while (!isTerminal(getchar()))
    {
        ;
    }
}

/**
 * @brief Allocates and returns newline-trimmed, null-terminated line read from `stdin`.
 *
 * @param maxLen Maximum length of trimmed string to read from `stdin`.
 * @return char* Newly allocated string to be freed, or `NULL` if error or more than `maxBytes` buffered in `stdin`.
 */
char *readOneLineStdin(size_t maxLen)
{
    size_t bufferLen = maxLen + 1;
    char *line = (char *)malloc(bufferLen);
    if (line == NULL)
    {
        return NULL;
    }

    size_t nRead = read(0, line, maxLen + 1);
    if (nRead == -1)
    {
        free(line);
        return NULL;
    }

    if ((nRead >= bufferLen && !isTerminal(line[nRead - 1])))
    {
        fprintf(stderr, "file name length cannot exceed %lu bytes\n", maxLen);
        flushStdin();
        free(line);
        return NULL;
    }

    if (line[nRead - 1] == '\n')
    {
        nRead--;
    }
    line[nRead] = '\0';
    return line;
}

int main()
{
    printf("Enter src file: ");
    fflush(stdout);
    char *srcFileName = readOneLineStdin(MAX_FILE_NAME_LEN);
    if (srcFileName == NULL)
    {
        exit(1);
    }
    printf("%s\n", srcFileName);

    printf("Enter dest file: ");
    fflush(stdout);
    char *destFileName = readOneLineStdin(MAX_FILE_NAME_LEN);
    if (destFileName == NULL)
    {
        free(srcFileName);
        exit(1);
    }
    printf("%s\n", destFileName);

    free(srcFileName);
    free(destFileName);
    return 0;
}
