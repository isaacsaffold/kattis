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

void quickSort(const struct Person** people, int numPeople)
{
    if (numPeople < 2)
        return;
    SWAP(people[0], people[numPeople >> 1], const struct Person*);
    const struct Person* pivot = people[0];
    int i = 1, j = numPeople - 1;
    loopStart:
    {
        for (; i < numPeople && comparePeople(people[i], pivot) < 0; ++i);
        for (; comparePeople(people[j], pivot) > 0; --j);
        if (i < j)
        {
            SWAP(people[i], people[j], const struct Person*);
            goto loopStart;
        }
    }
    SWAP(people[0], people[j], const struct Person*);
    quickSort(people, j);
    quickSort(people + j + 1, numPeople - j - 1);
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
