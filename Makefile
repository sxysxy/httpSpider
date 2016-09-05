#this makefile is temply windows-only 
httpSpider.exe:httpSpider.c httpSpider.h
	gcc httpSpider.c -o httpSpider.exe -lwin32k -lwsock32 -lm -g
	

    
