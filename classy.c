#include <stdio.h>
#include <string.h>

#define NAME_MAX_LEN 30

struct Person
{
    char name[NAME_MAX_LEN + 1];
    int class;
};

int comparePeople(const struct Person* a, const struct Person* b)
{
    int n = b->class - a->class;
    return n ? n : strcmp(a->name, b->name);
}

#define SWAP(a, b, type)        \
{                               \
    type SWAP_temp = (a);       \
    (a) = (b), (b) = SWAP_temp; \
}

void insertionSort(const struct Person** people, int numPeople)
{
    for (int i = 1; i < numPeople; ++i)
    {
        int j = i - 1;
        for (; j >= 0 && comparePeople(people[i], people[j]) < 0; --j);
        const struct Person* temp = people[i];
        for (int k = i - 1; k > j; --k)
            people[k + 1] = people[k];
        people[j + 1] = temp;
    }
}

const struct Person* medianOfThree(const struct Person* a, const struct Person* b, const struct Person* c)
{
    if (comparePeople(b, a) < 0)
        SWAP(a, b, const struct Person*);
    if (comparePeople(c, a) < 0)
        return a;
    else if (comparePeople(c, b) < 0)
        return c;
    else
        return b;
}

#define INSERTION_SORT_MAX_LEN 8

void quickSort(const struct Person** people, int numPeople)
{
    if (numPeople < 2)
        return;
    else if (numPeople <= INSERTION_SORT_MAX_LEN)
        insertionSort(people, numPeople);
    else
    {
        const struct Person* pivot = medianOfThree(people[0], people[numPeople >> 1], people[numPeople - 1]);
        int i = -1, j = numPeople;
        loopStart:
        {
            while (comparePeople(people[++i], pivot) < 0);
            while (comparePeople(people[--j], pivot) > 0);
            if (i < j)
            {
                SWAP(people[i], people[j], const struct Person*);
                goto loopStart;
            }
        }
        quickSort(people, i);
        quickSort(people + i, numPeople - i);
    }
}

int evalTernary(const unsigned char* digits, int numDigits)
{
    return numDigits == 1 ? digits[0] : digits[--numDigits] + 3*evalTernary(digits, numDigits);
}

void reverse(unsigned char* arr, int len)
{
    for (int i = 0, mid = len >> 1; i < mid; ++i)
        SWAP(arr[i], arr[len - i - 1], unsigned char);
}

#define CLASS_CHAIN_MAX_LEN 10

void parseLine(char* restrict line, struct Person* restrict person)
{
    static unsigned char digits[CLASS_CHAIN_MAX_LEN];
    strcpy(person->name, strtok(line, ":"));
    int i = 0;
    while (line = strtok(NULL, "- "))
    {
        switch (*line)
        {
            case 'l':
                digits[i++] = 0;
                break;
            case 'm':
                digits[i++] = 1;
                break;
            case 'u':
                digits[i++] = 2;
                break;
        }
    }
    reverse(digits, i);
    for (; i < CLASS_CHAIN_MAX_LEN; ++i)
        digits[i] = 1;
    person->class = evalTernary(digits, CLASS_CHAIN_MAX_LEN);
}

#define MAX_PEOPLE 100
#define MAX_LINE_LEN 108
#define CASE_SEPARATOR "=============================="

int main(void)
{
    char buf[MAX_LINE_LEN + 1];
    struct Person people[MAX_PEOPLE];
    struct Person* personPointers[MAX_PEOPLE];
    int numCases;
    scanf("%d\n", &numCases);
    for (int i = 0; i < numCases; ++i)
    {
        int numPeople;
        scanf("%d\n", &numPeople);
        for (int j = 0; j < numPeople; ++j)
        {
            parseLine(fgets(buf, MAX_LINE_LEN + 1, stdin), people + j);
            personPointers[j] = people + j;
        }
        quickSort((const struct Person**)personPointers, numPeople);
        for (int j = 0; j < numPeople; ++j)
            puts(personPointers[j]->name);
        puts(CASE_SEPARATOR);
    }
    return 0;
}
