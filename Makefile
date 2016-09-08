#this makefile is temply windows-only 
httpSpider.exe:httpSpider.c httpSpider.h basedef.h linkqueue.h trie.h
	gcc httpSpider.c -std=c99 -o httpSpider.exe -lwsock32 -lws2_32 -lm -g
	
plugin.dll:plugin.c httpSpider.h basedef.h linkqueue.h trie.h 
	gcc plugin.c -shared -o plugin.dll