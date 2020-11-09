application: app cable
app: app.c
	gcc app.c receiver.c transmitter.c alarm.c protocol.c utils.c -o app

cable: cable.c
	gcc -w cable.c -o cable

clean:
	rm -f app.o app cable received_*
