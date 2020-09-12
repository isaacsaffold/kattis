#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct Bounds
{
    unsigned char firstRow, lastRow;
    unsigned char firstColumn, lastColumn;
};

struct Window
{
    char* pixelBuffer;
    char** pixels;
    unsigned char* jumpLengthBuffer;
    unsigned char** jumpLengths;
    int width, height;
};

enum PixelType {FORBIDDEN, ALLOWED, BORDER};

enum PixelType classifyPixel(char pixel)
{
    switch (pixel)
    {
        case '+':
            return BORDER;
        case '?':
        case '!':
        case ',':
        case '.':
        case ' ':
            return ALLOWED;
        default:
            return isalnum(pixel) ? ALLOWED : FORBIDDEN;
    }
}

void initWindow(struct Window* window, int width, int height)
{
    int area = width * height, rowArrSize = height * sizeof(char*);
    char* buf = malloc(2 * (area + rowArrSize));
    window->pixelBuffer = buf, buf += area;
    window->pixels = buf, buf += rowArrSize;
    window->jumpLengthBuffer = buf, buf += area;
    window->jumpLengths = buf;
    window->width = width, window->height = height;
    memset(window->jumpLengthBuffer, 0, area);
    for (int i = 0, offset = 0; i < height; ++i, offset += width)
    {
        window->pixels[i] = window->pixelBuffer + offset;
        window->jumpLengths[i] = window->jumpLengthBuffer + offset;
        fread(window->pixels[i], 1, width, stdin);
        getchar();
    }
}

void destroyWindow(const struct Window* window)
{
    free(window->pixelBuffer);
}

struct Bounds findImageBounds(const struct Window* window, int startRow, int startColumn)
{
    int col = startColumn;
    while (++col < window->width && classifyPixel(window->pixels[startRow][col]) == BORDER);
    int jump = col - startColumn;
    window->jumpLengths[startRow][startColumn + 1] = jump - 1;
    int row = startRow;
    while (++row < window->height && classifyPixel(window->pixels[row][startColumn]) == BORDER)
        window->jumpLengths[row][startColumn] = jump;
    return (struct Bounds){startRow, row - 1, startColumn, col - 1};
}

void eraseImage(const struct Window* window, struct Bounds bounds)
{
    int n = bounds.lastColumn - bounds.firstColumn + 1;
    for (int i = bounds.firstRow; i <= bounds.lastRow; ++i)
        memset(window->pixels[i] + bounds.firstColumn, ' ', n);
}

void eraseAdsFromImage(const struct Window* window, struct Bounds bounds)
{
    for (int i = bounds.firstRow + 1; i < bounds.lastRow; ++i)
    {
        for (int j = bounds.firstColumn + 1; ; ++j)
        {
            j += window->jumpLengths[i][j];
            if (j >= bounds.lastColumn)
                break;
            switch (classifyPixel(window->pixels[i][j]))
            {
                case FORBIDDEN:
                    eraseImage(window, bounds);
                    return;
                case BORDER:
                    eraseAdsFromImage(window, findImageBounds(window, i, j));
                    break;
            }
        }
    }
}

void eraseAdsFromWindow(const struct Window* window)
{
    for (int i = 0; i < window->height; ++i)
    {
        for (int j = 0; ; ++j)
        {
            j += window->jumpLengths[i][j];
            if (j >= window->width)
                break;
            if (classifyPixel(window->pixels[i][j]) == BORDER)
                eraseAdsFromImage(window, findImageBounds(window, i, j));
        }
    }
}

int main(void)
{
    struct Window window;
    int width, height;
    scanf("%d %d", &height, &width);
    getchar();
    initWindow(&window, width, height);
    eraseAdsFromWindow(&window);
    for (int i = 0; i < height; ++i)
    {
        fwrite(window.pixels[i], 1, width, stdout);
        putchar('\n');
    }
    destroyWindow(&window);
    return 0;
}
