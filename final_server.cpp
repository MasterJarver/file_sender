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
unsigned long long get_file_size(string filename);
int main(int argc, char* argv[])
{
	const char* portS = argv[1]; // строка из main с номером порта
	int portI = atoi(portS); // преобразование строки в int
	const char* nameS = argv[2]; // строка из main с именем файла для сохранения
	unsigned long long i = 0;
	static char Buffer[SIZE]; // создаем буфер
	unsigned long long waitSizeFile = 0; // ожидаемый размер файла
	unsigned long long fileSize = 0; // реальный размер файла
	unsigned long long recvSize = 0;
	unsigned long long num;
	cout << portI << endl;
	cout << nameS << endl;
	int MasterSocket = socket(AF_INET, 	SOCK_STREAM, IPPROTO_TCP); // Декларируем серверный сокет
	struct sockaddr_in SockAddr; // структура для задания пары IP port
	SockAddr.sin_family = AF_INET; // указываем что используем версию IPv4
	SockAddr.sin_port = htons(portI); // приводим порт 12345 к сетевому порядку байт
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // открываем сокет на все IP аналог 0.0.0.0 inet_addr 127.0.0.1
	bind(MasterSocket, (struct sockaddr*)(&SockAddr), sizeof(SockAddr)); // биндим сокет(привязка сокета к порту) 
	listen(MasterSocket, SOMAXCONN); // включаем прием соединений, создаем очередь
	int ClientSocket = accept(MasterSocket, 0, 0); // создаем клиентский сокет
	while(recvSize == 0 || recvSize == -1)
	{
		recvSize = (unsigned long long)recv(ClientSocket, &waitSizeFile, 4, MSG_NOSIGNAL); // читаем в переменную из сокета
		cout << "RecvSize: " << recvSize << endl; 
		cout << "wait_size_file: " << waitSizeFile << endl;
	}
	cout << "begin accepting file..." << endl;
	sleep(3);
	FILE* copy_ptr = nullptr;
	copy_ptr = fopen(nameS, "a+"); // создали файл
	while(i != waitSizeFile)
	{	
		//cout << "begin listen: " << endl;
		recvSize = (unsigned long long)recv(ClientSocket, Buffer, WHENCE, MSG_NOSIGNAL); // читаем в буфер из сокета
		//cout << "RecvSize: " << RecvSize << endl;
		if(recvSize != 0 || recvSize != -1) // если есть инфа для чтения
		{	
			//fseek(copy_ptr, i, SEEK_SET);
			num = (unsigned long long)fwrite(Buffer, 1, recvSize, copy_ptr);
			//cout << "write bytes" << num << endl;
			i += num;
			cout << "bytes: " << i << endl;
		}	
		else if(i == waitSizeFile)
		{
			//cout << "i: " << i << endl;
			//cout << "i == wait_size_file: " << i << endl;
			fclose(copy_ptr); // закрываем указатель на кипируемый файл
			shutdown(ClientSocket, SHUT_RDWR);
			close(ClientSocket);
		}
	}
	shutdown(MasterSocket, SHUT_RDWR); // закрываем соединение
	close(MasterSocket); // закрываем дескриптор
	cout << "final bytes: " << i << endl;
	cout << "end program." << endl;
	return 0;
}
unsigned long long get_file_size(string filename)
{
	FILE* p_file = nullptr;
	p_file = fopen(filename.c_str(), "r+b");
	fseek(p_file, 0, SEEK_END);
	unsigned long long size = ftell(p_file);
	fclose(p_file);
	return size;
}