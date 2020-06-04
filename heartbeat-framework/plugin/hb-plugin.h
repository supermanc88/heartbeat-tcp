#ifndef PLUGIN_TEST_HB_PLUGIN_H
#define PLUGIN_TEST_HB_PLUGIN_H

#ifdef __cplusplus
extern "C"
{
#endif

int plug_init(void * data) ;

int plug_stop(void ) ;

int plug_run(void * data) ;


#ifdef __cplusplus
}
#endif

#endif //PLUGIN_TEST_HB_PLUGIN_H
