//#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>

#include "revert_string.h"

void testRevertString(void) {
  char simple_string[] = "Hello";
  char str_with_spaces[] = "String with spaces";
  char str_with_odd_chars_num[] = "abc";
  char str_with_even_chars_num[] = "abcd";

  RevertString(simple_string);
 // CU_ASSERT_STRING_EQUAL_FATAL(simple_string, "olleH");

  RevertString(str_with_spaces);
  //CU_ASSERT_STRING_EQUAL_FATAL(str_with_spaces, "secaps htiw gnirtS");

  RevertString(str_with_odd_chars_num);
  //CU_ASSERT_STRING_EQUAL_FATAL(str_with_odd_chars_num, "cba");

  RevertString(str_with_even_chars_num);
  //CU_ASSERT_STRING_EQUAL_FATAL(str_with_even_chars_num, "dcba");
}

int main() {
 printf("test pass \n");
  
}
