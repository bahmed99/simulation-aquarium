

utils: 
	gcc -c Controller/utils.c -o Build/utils.o

# test: utils
# 	gcc -c Controller/test.c -o Build/test.o
# 	gcc Build/test.o Build/utils.o -o Build/test
	
clean:
	rm -rf Build/*