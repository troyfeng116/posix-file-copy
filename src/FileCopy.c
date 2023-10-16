#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

const static int MAX_FILE_NAME_LEN = 1024;

bool isFile(const char *path)
{
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISREG(pathStat.st_mode);
}

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
        fprintf(stderr, "[FileCopy] file name length cannot exceed %lu bytes\n", maxLen);
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

/**
 * @brief Write prompt to `stdout`, read response from `stdin`.
 *
 * @param prompt Input prompt to display in `stdout`.
 * @param maxLen Max length of response to read from `stdin`.
 * @return char* Allocated buffer to be freed, as returned from `readOneLineStdin`.
 */
char *promptAndReadStdin(char *prompt, int maxLen)
{
    printf("%s", prompt);
    fflush(stdout);
    return readOneLineStdin(maxLen);
}

/**
 * @brief Copy contents of src file to dest file line-by-line.
 *
 * @param srcFile Src file object.
 * @param destFile Dest file object.
 * @return ssize_t Total number of bytes copied.
 */
ssize_t copySrcToDestFile(FILE *srcFile, FILE *destFile)
{
    size_t bufferSize = 0;
    char *line = NULL;
    ssize_t nRead, totalRead;
    while ((nRead = getline(&line, &bufferSize, srcFile)) > 0)
    {
        totalRead += nRead;
        fwrite(line, sizeof(char), nRead, destFile);
    }

    free(line);
    return totalRead;
}

int main()
{
    char *srcFileName = promptAndReadStdin("Enter src file: ", MAX_FILE_NAME_LEN);
    if (srcFileName == NULL)
    {
        exit(1);
    }

    char *destFileName = promptAndReadStdin("Enter dest file: ", MAX_FILE_NAME_LEN);
    if (destFileName == NULL)
    {
        free(srcFileName);
        exit(1);
    }

    FILE *srcFile = fopen(srcFileName, "r");
    if (srcFile == NULL || !isFile(srcFileName))
    {
        fprintf(stderr, "[FileCopy] unable to open src file `%s`\n", srcFileName);
        free(srcFileName);
        free(destFileName);
        exit(1);
    }

    FILE *destFile = fopen(destFileName, "w");
    if (destFile == NULL || !isFile(destFileName))
    {
        fprintf(stderr, "[FileCopy] unable to open dest file `%s`\n", destFileName);
        fclose(srcFile);
        free(srcFileName);
        free(destFileName);
        exit(1);
    }

    printf("Copying src file %s to dest file %s...\n", srcFileName, destFileName);
    ssize_t totalCopied = copySrcToDestFile(srcFile, destFile);
    printf("Copied %ld bytes from src file %s to dest file %s\n", totalCopied, srcFileName, destFileName);

    fclose(srcFile);
    fclose(destFile);
    free(srcFileName);
    free(destFileName);
    return 0;
}
