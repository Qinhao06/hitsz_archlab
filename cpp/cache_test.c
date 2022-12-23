	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <sys/time.h>
	#include <sys/types.h>

	#define ARRAY_SIZE (1 << 30)                                    // test array size is 2^28

	#define Test_Size 50000000

	#define Single_Test_size 10

	typedef unsigned char BYTE;										// define BYTE as 4-byte type

	BYTE array[ARRAY_SIZE];											// test array
	const int L2_cache_size = 1 << 18;

	double get_usec(const struct timeval tp0, const struct timeval tp1)
	{
		return 1000000 * (tp1.tv_sec - tp0.tv_sec) + tp1.tv_usec - tp0.tv_usec;
	}

	// have an access to arrays with L2 Data Cache'size to clear the L1 cache
	void Clear_L1_Cache()
	{
		memset(array, 0, L2_cache_size);
	}

	// have an access to arrays with ARRAY_SIZE to clear the L2 cache
	void Clear_L2_Cache()
	{
		memset(array, 0, ARRAY_SIZE);
	}

	double test_size_rand(int size){
		struct timeval tp[2];
		struct timeval tp_rand[2];
		double time_all, time_con;
		double time_result;
		time_all = 0;
		time_con = 0;
		time_result = 0;
		register int j;
		int i;
		int rand1, rand2;
		int rank;
		BYTE filled_num = 16;
	
		for( i = 1;i <= Single_Test_size; i++){
			
			Clear_L2_Cache();
			for(j = 0; j < size; j++){
				array[j] = filled_num;
			}
			gettimeofday(&tp[0], NULL);
			//此处要注意，直接使用rand（）*rand（）%size会出现段错误有可能是数太大越界
			for(j = 1; j <= Test_Size; j++){
				rand1 = rand() % 10000;
				rand2 = rand() % 10000;
				rank = (rand1 * 10000 + rand2) % size;
				array[rank] = filled_num;
				array[rank] = filled_num;
			}
			gettimeofday(&tp[1], NULL);
			// avg_time[i] = (get_usec(tp[0], tp[1]) - used_time)/ Test_Size;
			time_all += get_usec(tp[0], tp[1]);
		}
		for( i=1;i <= Single_Test_size; i++){
			
			Clear_L2_Cache();
			for(j = 0; j < size; j++){
				array[j] = filled_num;
			}
			gettimeofday(&tp_rand[0], NULL);

			for(j = 1; j <= Test_Size; j++){
				rand1 = rand() % 10000;
				rand2 = rand() % 10000;
				rank = (rand1 * 10000 + rand2) % size;
			}
			gettimeofday(&tp_rand[1], NULL);
			// avg_time[i] = (get_usec(tp[0], tp[1]) - used_time)/ Test_Size;
			time_con += get_usec(tp_rand[0], tp_rand[1]);

		}
		return (time_all )/ Single_Test_size;
	}



	void Test_Cache_Size()
	{
		printf("**************************************************************\n");
		printf("Cache Size Test\n");
		// TODO
	
		for(int i = 12; i <= 21; i++){
		    int	size = 1 << i;
			double time = test_size_rand(size);
			printf("[Test_Array_Size = %4dKB]  total access time: %lf\n", 1 << (i - 10), time);
		}

		/**
		* struct timeval tp[2];
		* gettimeofday(&tp[0], NULL);
		* func();
		* gettimeofday(&tp[1], NULL);
		* time_used = getusec(tp[0], tp[1]);
		*/
	}


	double test_size_step(int step){
		struct timeval tp[2];
		struct timeval tp_rand[2];
		unsigned int size = 1; 
		double time_all, time_con;
		double time_result;
		time_all = 0;
		time_con = 0;
		time_result = 0;
		register int j;
		int i;
		int rank = 0;
		for( i=0;i <= Single_Test_size; i++){
			
			Clear_L1_Cache();
			
			gettimeofday(&tp[0], NULL);

			for(j = 0; j <= Test_Size; j++){
				rank = (rank + step) % ARRAY_SIZE;
				array[rank] = 30;
			}
			gettimeofday(&tp[1], NULL);
			// avg_time[i] = (get_usec(tp[0], tp[1]) - used_time)/ Test_Size;
			time_all += get_usec(tp[0], tp[1]);

		}
		for( i=0;i <=Single_Test_size; i++){
			
			Clear_L1_Cache();
			gettimeofday(&tp_rand[0], NULL);

			for(j = 0; j <= Test_Size; j++){
				rank = (rank + step) % ARRAY_SIZE;
			}
			gettimeofday(&tp_rand[1], NULL);
			// avg_time[i] = (get_usec(tp[0], tp[1]) - used_time)/ Test_Size;
			time_con += get_usec(tp_rand[0], tp_rand[1]);

		}
		//可以减去对照时间，实现纯访问时间的计算，不过这里直接返回的总时间，因为其他时间开销消耗相同
		return (time_all ) / Single_Test_size;
	}

	void Test_L1C_Block_Size()
	{
		printf("**************************************************************\n");
		printf("L1 DCache Block Size Test\n");

		Clear_L1_Cache();											// Clear L1 Cache
		for(int i= 3; i <= 9; i++){
			int step = 1 << i;
			double time = test_size_step(step);
			printf("[Test_Array_Jump = %4dB]  total access time: %lf\n", step, time);
		}
		// TODO
	}

	void Test_L2C_Block_Size()
	{
		printf("**************************************************************\n");
		printf("L2 Cache Block Size Test\n");
		
		Clear_L2_Cache();											// Clear L2 Cache
		
		// TODO
	}

	double test_way_count1(int n, BYTE test[]){
		struct timeval tp[2];
		struct timeval tp_rand[2];
		int size = (1 << 16); 
		int step = size / (1 << n);
		double time_all, time_con;
		double time_result;
		time_all = 0;
		time_con = 0;
		time_result = 0;
		register int j;
		int rank = 0;
		
		gettimeofday(&tp[0], NULL);
		rank = ((rand() * rand()) % 90000 )% size;
		if((rank / step) % 2 == 0){
			rank = (rank + step) % size;
		}  
		for(j = 0; j <= Test_Size; j++){
			test[rank] = 50;
			rank = (rank + 2 * step) % size;
		}
		gettimeofday(&tp[1], NULL);

		time_all = get_usec(tp[0], tp[1]);

		gettimeofday(&tp_rand[0], NULL);
		rank = ((rand() * rand()) % 90000 )% size;
		if((rank / step) % 2 == 0){
			rank = (rank + step) % size;
		}  
		for(j = 0; j <= Test_Size; j++){
			rank = (rank + 2 * step) % size;
		}
		gettimeofday(&tp_rand[1], NULL);

		time_con = get_usec(tp_rand[0], tp_rand[1]);

		return time_all ; 
	}


	void Test_L1C_Way_Count()
	{
		printf("**************************************************************\n");
		printf("L1 DCache Way Count Test\n");
		int size = 1 << 15;
		BYTE test[size * 2];
		for(int n = 2; n <= 10; n++){
		memset(test, 0, sizeof(test));
		double time = test_way_count1(n, test);
		printf("[Test_Split_Groups = %4d]  total access time: %lf\n", 1 << n, time);
	}
		// TODO
	}


	double test_way_count2(int n, BYTE test[]){
		struct timeval tp[2];
		struct timeval tp_rand[2];
		int size = (1 << 20); 
		int step = size / (1 << n);
		double time_all, time_con;
		double time_result;
		time_all = 0;
		time_con = 0;
		time_result = 0;
		register int j;
		int rank = 0;
		
		gettimeofday(&tp[0], NULL);
		rank = ((rand() * rand()) % 90000 )% size;
		if((rank / step) % 2 == 0){
			rank = (rank + step) % size;
		}  
		for(j = 0; j <= Test_Size; j++){
			test[rank] = 50;
			rank = (rank + 2 * step) % size;
		}
		gettimeofday(&tp[1], NULL);

		time_all = get_usec(tp[0], tp[1]);

		gettimeofday(&tp_rand[0], NULL);
		rank = ((rand() * rand()) % 90000 )% size;
		if((rank / step) % 2 == 0){
			rank = (rank + step) % size;
		}  
		for(j = 0; j <= Test_Size; j++){
			rank = (rank + 2 * step) % size;
		}
		gettimeofday(&tp_rand[1], NULL);

		time_con = get_usec(tp_rand[0], tp_rand[1]);

		return time_all ; 
	}


	void Test_L2C_Way_Count()
	{
		printf("**************************************************************\n");
		printf("L2 Cache Way Count Test\n");
		int size = 1 << 15;
		BYTE test[size * 2];
		for(int n = 2; n <= 10; n++){
			memset(test, 0, sizeof(test));
			double time = test_way_count2(n, test);
			printf("[Test_Split_Groups = %4d]  total access time: %lf\n", 1 << n, time);
		}
	}

	void Test_Cache_Write_Policy()
	{
		printf("**************************************************************\n");
		printf("Cache Write Policy Test\n");

		// TODO
	}

	void Test_Cache_Swap_Method()
	{
		printf("**************************************************************\n");
		printf("Cache Replace Method Test\n");

		// TODO
	}
	double test_tlb(int entry_log){
		int pageSize = 4096;
		int step = pageSize;
		struct timeval tp[2];
		struct timeval tp_rand[2];
		int size = (1 << entry_log) * pageSize; 
		double time_all, time_con;
		double time_result;
		time_all = 0;
		time_con = 0;
		time_result = 0;
		register int j;
		int i;
		int rand1,rand2;
		int rank = 0;
		//测试数组
		BYTE test[size];

		for(i = 0; i < size; i++){
			test[i] = 90;
		}

		for( i=0;i <= Single_Test_size; i++){
			//将测试数组装入cache
			for( j = 0; j < size; j++){
				test[j] = 90;
			}
			
			gettimeofday(&tp[0], NULL);

			rank = (rand() % 1000) % size;
			for(j = 0; j <= Test_Size; j++){
				rank = (rank + step) % size;
				test[rank] = 90;
			}
			gettimeofday(&tp[1], NULL);
			// avg_time[i] = (get_usec(tp[0], tp[1]) - used_time)/ Test_Size;
			time_all += get_usec(tp[0], tp[1]);

		}
		for( i=0;i <= Single_Test_size; i++){
			
			for( j = 0; j < size; j++){
				test[j] = 90;
			}
			
			gettimeofday(&tp_rand[0], NULL);

			rank = (rand()  % 1000) % size;
			for(j = 0; j <= Test_Size; j++){
				rank = (rank + step) % size;
			}
			gettimeofday(&tp_rand[1], NULL);
			// avg_time[i] = (get_usec(tp[0], tp[1]) - used_time)/ Test_Size;
			time_con += get_usec(tp[0], tp[1]);
		}
		return (time_all ) / Single_Test_size;
	}

	void Test_TLB_Size()
	{
		printf("**************************************************************\n");
		printf("TLB Size Test\n");

		const int page_size = 1 << 12;								// Execute "getconf PAGE_SIZE" under linux terminal

		for(int i = 1; i <= 8;i++){
			double time = test_tlb(i);
			printf("[Test_TLB_entries = %4d]  total access time: %lf\n", 1 << i, time);
		}
	}

	int main()
	{
		Test_Cache_Size();
		Test_L1C_Block_Size();
		// Test_L2C_Block_Size();
		Test_L1C_Way_Count();
		// Test_L2C_Way_Count();
		// Test_Cache_Write_Policy();
		// Test_Cache_Swap_Method();
		Test_TLB_Size();
		
		return 0;
	}
