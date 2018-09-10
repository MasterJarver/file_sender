#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#define SIZE 1024
#define WHENCE 1024
using namespace std;
unsigned long long get_file_size(string filename); // сигнатура функции для получения размера файла
int main(int argc, char* argv[]) // принимаем аргументы argc - счетчик аргументов argv строки аргументов
{
	const char* ip = argv[1]; // строка из main с IP
	const char* portS = argv[2]; // строка с портом
	int portI = atoi(portS); // преобразование строки с портом в int
	const char* fileName = argv[3]; // строка с именем файла
	unsigned long long num;
	const unsigned long long size = get_file_size(fileName); // размер файла 
	unsigned long long i = 0;
	static char Buffer[SIZE];
	FILE* file_ptr = nullptr;
	file_ptr = fopen(fileName, "r+b"); // открываем файл на чтение
	int Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(portI);
	SockAddr.sin_addr.s_addr = inet_addr(ip);
	connect(Socket, (struct sockaddr*)(&SockAddr), sizeof(SockAddr));
	//char f;
	send(Socket, &size, 4, MSG_NOSIGNAL); // отправляем размер файла
	cout << "sending size file: " << size << endl;
	cout << "begin senging file..." << endl;
	sleep(3);
	//shutdown(Socket, SHUT_RDWR);
	//int con = connect(Socket, (struct sockaddr*)(&SockAddr), sizeof(SockAddr));
	//printf("con: %s\n", con);
	while(i != size) // пока i меньше размера файла
	{
		//cout << "begin sending..." << endl;
		fseek(file_ptr, i, SEEK_SET); // ставим указатель на i вправо от начала
		num = (unsigned long long)fread(Buffer, 1, WHENCE, file_ptr); // читаем в Buffer по 1 байту WHENCE байт
		//cout << "read bytes: " << num << endl;
		num = (unsigned long long)send(Socket, Buffer, num,  MSG_NOSIGNAL); // пишем в сокет из Buffer число записанных байт num
		//cout << "send bytes: " << num << endl;
		i += num; // записываем в счетчик количество отправленных байт байт 
		cout << "bytes: " << i << endl;
		if(i == size)
		{	
			cout << "all file send bytes: " << i << endl;
			fclose(file_ptr);
		}
	}
	cout << "end program." << endl;
	shutdown(Socket, SHUT_RDWR);//закрываем соединение и клиента и сервера
	close(Socket);
	return 0; // возвращаем 0 в случае успешного выполнения
}
unsigned long long get_file_size(string filename)
{
	FILE* p_file = nullptr; // указатель на файл на винте
	p_file = fopen(filename.c_str(), "r+b"); // открываем по указателю файл на чтение
	fseek(p_file, 0, SEEK_END); // помещаем указатель на начало файла
	unsigned long long size = ftell(p_file); // функцией ftell узнаем размер файла
	fclose(p_file); // закрываем файл 
	return size; // возврааем размер
}