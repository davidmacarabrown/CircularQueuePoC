// CircularQueue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <Windows.h>
#include <mutex>

typedef struct cq_struct
{
	unsigned int* data_buffer;
	volatile unsigned int write_head;
	volatile unsigned int read_head;
	volatile unsigned int free = 16;
	unsigned int queue_size;
	volatile char lock = 0;
	
} CQ_STRUCT;

DWORD WINAPI queue_writer(LPVOID lpParameter)
{
	CQ_STRUCT* cq = (CQ_STRUCT*)lpParameter;

	unsigned int current_val = 0;
	unsigned int read_head = 0;
	unsigned int write_head = 0;
	unsigned int bytes_free = 0;

	while (1)
	{	
		printf("Writer\n");
		if (cq->lock == 0)
		{
			cq->lock = 1;
			if (cq->free > 0)
			{
				cq->data_buffer[cq->write_head] = current_val;

				cq->write_head++;

				if (cq->write_head >= (cq->queue_size))
				{
					cq->write_head = 0;
				}

				current_val++;
				cq->free--;
			}
			cq->lock = 0;
		}
			Sleep(1);
	}
		
	
	return 0;
}

DWORD WINAPI queue_reader(LPVOID lpParameter)
{
	CQ_STRUCT* cq = (CQ_STRUCT*)lpParameter;

	unsigned int previous = 0;

	while (1)
	{
		printf("Reader\n");
		if (cq->lock == 0)
		{
			cq->lock = 1;
			if (cq->free < 16)
			{
				if ( (cq->data_buffer[cq->read_head] != 0) && (cq->data_buffer[cq->read_head] != (previous + 1)) )
				{
					printf("MISMATCH %lu @ Read Head: %lu!\n", cq->data_buffer[cq->read_head], cq->read_head);
				}
				else
				{
					printf("%lu\n", cq->data_buffer[cq->read_head]);
				}

				previous = cq->data_buffer[cq->read_head];
				cq->read_head++;

				if (cq->read_head >= cq->queue_size)
				{
					cq->read_head = 0;
				}

				cq->free++;
			}
			cq->lock = 0;
		}
		Sleep(1);
	}
	return 0;
}

CQ_STRUCT a_cq_struct;

int main()
{
	const unsigned int BUFFER_SIZE = 16;
	unsigned int* buffer = new unsigned int[BUFFER_SIZE];

	memset(buffer, 0, 64);

	HANDLE write_thread;
	LPDWORD write_thread_id = 0;

	HANDLE read_thread;
	LPDWORD read_thread_id = 0;

	a_cq_struct.data_buffer = buffer;
	a_cq_struct.write_head = 0;
	a_cq_struct.read_head = 0;
	a_cq_struct.queue_size = BUFFER_SIZE;

	write_thread = CreateThread(NULL, 0, queue_writer, &a_cq_struct, 0, write_thread_id);
	read_thread = CreateThread(NULL, 0, queue_reader, &a_cq_struct, 0, read_thread_id);

	WaitForSingleObject(write_thread, -1);
	WaitForSingleObject(read_thread, -1);

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
