run: lab4.cpp
	g++ lab4.cpp MathHelper.cpp -lglut -lGLU -lGL -lGLEW -g
	
clean: 
	rm -f *.out *~ run
	
