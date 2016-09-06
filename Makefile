#this makefile is temply windows-only 
httpSpider.exe:httpSpider.c httpSpider.h avltree.h
	gcc httpSpider.c -o httpSpider.exe -lwin32k -lwsock32 -lm -g
	

    
