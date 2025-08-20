#	- 继续执行
#	@ 隐藏命令
#	$^ 引用 目标 全部
#	$< 引用第一个
CC:=gcc
log := thirdparty/log.c thirdparty/log.h
cjson := thirdparty/cJSON.c thirdparty/cJSON.h

logtest: test/log_test.c $(log)
	-@$(CC) $^ -o $@ -Ithirdparty
	-@./$@
	-@rm -rf $@


mqtt := app/App_mqtt.c app/App_mqtt.h
mqtt_test: test/testmqtt.c $(log) $(mqtt) $(cjson)
	-@$(CC) $^ -o $@ -Ithirdparty -Iapp -lpaho-mqtt3c
	-@./$@
	-@rm -rf $@


queue_test: test/test_queue.c $(log) 
	-@$(CC) $^ -o $@ -Ithirdparty -Iapp
	-@./$@
	-@rm -rf $@