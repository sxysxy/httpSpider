#this makefile is temply windows-only 
httpSpider.exe:httpSpider.c httpSpider.h basedef.h linkqueue.h
	gcc httpSpider.c -std=c99 -o httpSpider.exe -lwsock32 -lm -g
	

    
