/*
 *  Cache simulation project
 *  Class UCR IE-521
 */

#include <gtest/gtest.h>
#include <time.h>
#include <stdlib.h>
#include <debug_utilities.h>

/* 
 * Gtest main function: Generates random seed, if not provided,
 * parses DEBUG flag, and execute the test suite
 */
int main(int argc, char **argv) {
  int argc_to_pass = 0;
  char **argv_to_pass = NULL; 
  int seed = 0;

  /* Generate seed */
  seed = time(NULL) & 0xffff;

  /* Parse arguments looking if random seed was provided */
  argv_to_pass = (char **)calloc(argc + 1, sizeof(char *));
  
  for (int i = 0; i < argc; i++){
    std::string arg = std::string(argv[i]);

    if (!arg.compare(0, 20, "--gtest_random_seed=")){
      seed = atoi(arg.substr(20).c_str());
      continue;
    }
    argv_to_pass[argc_to_pass] = strdup(arg.c_str());
    argc_to_pass++;
  }

  /* Init Gtest */
  ::testing::GTEST_FLAG(random_seed) = seed;
  testing::InitGoogleTest(&argc, argv_to_pass);

  /* Print seed for debug */
  printf(YEL "Random seed %d \n",seed);
  srand(seed);

  /* Execute test */
  return RUN_ALL_TESTS();
  
  /* Free memory */
  free(argv_to_pass);

  return 0;
}
