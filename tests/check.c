#include "../src/utils/parsing.h"

int main(void)
{
    int actual = char_matching_pattern("amine123", "^[a-zA-Z0-9]*$");
    int expected = 1;
    return actual == expected;
}
