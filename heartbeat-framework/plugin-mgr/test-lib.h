#ifndef PLUGIN_TEST_TEST_LIB_H
#define PLUGIN_TEST_TEST_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

    int test_add(int a, int b);

    int plug_init(void * data) ;

    int plug_stop(void ) ;

    int plug_run(void * data) ;

#ifdef __cplusplus
};
#endif

#endif //PLUGIN_TEST_TEST_LIB_H
