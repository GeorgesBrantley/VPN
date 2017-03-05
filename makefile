makeever: tunneld.c mytunnel.c server.c send.c
	g++ -pthread -o tunneld tunneld.c 
	g++ -pthread -o mytunnel mytunnel.c
	g++ -o server server.c
	g++ -o send send.c
clean:
	rm -f tunneld mytunnel 
